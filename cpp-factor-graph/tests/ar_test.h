#ifndef AR_TEST_H
#define AR_TEST_H

#include "util_test.h"
#include <factorgraph.h>
#include <memory>

TEST(AutoRegressive, RLS) {

    normal_distribution<double> randn(0, 1);
    default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());

    auto x_prev = new EvidenceNode;
    auto x_next = new EvidenceNode;
    auto y = new EvidenceNode;
    auto noise_U = new EvidenceNode;
    auto noise_W = new EvidenceNode;
    auto b = new MultiplicationNode;
    auto A = new EstimateMultiplicationNode;
    auto add_A_b = new AddNode;

    auto add_W_y = new AddNode;
    auto equMult = new EquMultNode;
    auto a_equ = new EqualityNode(GaussianMessage::GAUSSIAN_PRECISION);
    auto a_prev = new EvidenceNode;
    auto a_next = new EvidenceNode;


    Network nwk;
    nwk.addEdge(x_prev, A);
    nwk.addEdge(A, add_A_b);
    nwk.addEdge(noise_U, b);
    nwk.addEdge(b, add_A_b);
    nwk.addEdge(add_A_b, equMult);
    nwk.addEdge(x_next, equMult);
    nwk.addEdge(equMult, add_W_y);
    nwk.addEdge(noise_W, add_W_y);
    nwk.addEdge(add_W_y, y);
    nwk.addEdge(A, a_equ, EstimateMultiplicationNode::ESTIMATED_TAG);
    nwk.addEdge(a_prev, a_equ);
    nwk.addEdge(a_equ, a_next);

    Network::Schedule schedule = { {a_next, a_equ}, {a_prev, a_equ}, {a_equ, A},
                                   {x_prev, A}, {A, add_A_b}, {noise_U, b}, {b, add_A_b},
                                   {add_A_b, equMult},
                                   {noise_W, add_W_y}, {y, add_W_y}, {add_W_y, equMult},
                                   {equMult, x_next},
                                   {x_next, equMult}, {equMult, add_A_b},  {add_A_b, A}, {A, a_equ},
                                   {a_prev, a_equ}, {a_equ, a_next} };
    nwk.setSchedule(schedule);

    double REAL_varU = 0.1;
    double REAL_varW = 0.001;
    // the real coefficients
    Matrix REAL_A = {{1.51, -1.08, 0.47, -0.23, 0.91, -1.30, 0.86, -0.32}};

    size_t M = REAL_A.cols();

    // dummy zero precision message
    const GaussianMessage NO_INFORMATION = makeGaussian(vector<double>(M, 0.0),
                                                        vector<double>(M * M, 0.0),
                                                        GaussianMessage::GAUSSIAN_PRECISION);

    a_next->receive(NO_INFORMATION);
    a_prev->receive(NO_INFORMATION);

    int N_OBSERVATIONS = 10000;

    b->setMatrix(eye(M, 1));
    equMult->setMatrix(eye(1, M));


    Matrix REAL_x = eye(1,M);
    Matrix var = eye(M, M);

    GaussianMessage msg = makeGaussianMx(REAL_x, var);

    noise_U->receive(makeGaussian(0, REAL_varU));
    noise_W->receive(makeGaussian(0, REAL_varW));

    for (int i = 0; i < N_OBSERVATIONS; i++)
    {
        double x_new = (REAL_A * REAL_x.T())(0,0) + randn(generator) * REAL_varU;
        Matrix tmp = REAL_x;
        std::copy(tmp.data(), tmp.data()+tmp.size()-1, REAL_x.data()+1);
        REAL_x(0, 0) = x_new;

        double observation = x_new + randn(generator) * REAL_varW;

        // data from the previous observation
        x_prev->receive(msg);
        // observing the data
        y->receive(makeGaussian(observation, 0));

        // doing the inference
        nwk.step();

        msg = x_next->evidence();

        a_prev->receive(a_next->evidence());
    }

    const double *ESTIMATED_A = a_next->evidence().mean();

    // computing the average errors
    double absError = 0.0;
    double relError = 0.0;
    for (size_t i = 0; i < REAL_A.size(); i++)
    {
        double delta = REAL_A(0, i) - ESTIMATED_A[i];
        absError += fabs(delta);
        relError += fabs(delta / ESTIMATED_A[i]);
    }
    absError /= REAL_A.size();
    relError /= REAL_A.size();

    cout << relError << endl;

    ASSERT_LE(relError, 0.3);
    ASSERT_LE(absError, 0.5);

}



#endif // AR_TEST_H
