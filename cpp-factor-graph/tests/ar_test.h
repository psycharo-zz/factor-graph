#ifndef AR_TEST_H
#define AR_TEST_H

#include "util_test.h"
#include <factorgraph.h>



TEST(AutoRegressive, One) {

    normal_distribution<double> randn(0, 1);
    default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());

    auto x_prev = new EvidenceNode;
    auto x_next = new EvidenceNode;
    auto y = new EvidenceNode;
    auto noise_U = new EvidenceNode;
    auto b = new MultiplicationNode;
    auto A = new EstimateMultiplicationNode;
    auto add_A_b = new AddNode;
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
    nwk.addEdge(equMult, y);

    nwk.addEdge(A, a_equ, EstimateMultiplicationNode::ESTIMATED_TAG);
    nwk.addEdge(a_prev, a_equ);
    nwk.addEdge(a_equ, a_next);


    Network::Schedule schedule = { {x_prev, A}, {A, add_A_b}, {noise_U, b}, {b, add_A_b},
                                   {add_A_b, equMult}, {y, equMult},
                                   {equMult, x_next}, {x_next, equMult},
                                   {equMult, add_A_b},  {add_A_b, A},
                                   {A, a_equ}, {a_prev, a_equ}, {a_equ, a_next} };
    nwk.setSchedule(schedule);

    double REAL_varU = 0.1;
    // the real coefficients
    Matrix REAL_A = {{1.51, -1.08, 0.47, -0.23, 0.91, -1.30, 0.86, -0.32}};
    size_t M = REAL_A.cols();

    A->setParam(REAL_A.data(), REAL_A.size());

    a_prev->receive(makeGaussianMx(eye(1,M), eye(M, M), GaussianMessage::GAUSSIAN_PRECISION));


    int N_OBSERVATIONS = 10000;

    b->setMatrix(eye(M, 1));
    equMult->setMatrix(eye(1, M));


    Matrix REAL_x = eye(1,M);
    Matrix var = eye(M, M);

    GaussianMessage msg = makeGaussian(vector<double>(REAL_x.data(), REAL_x.data() + REAL_x.size()),
                                       vector<double>(var.data(), var.data() + var.size() * var.size()),
                                       GaussianMessage::GAUSSIAN_VARIANCE);


    noise_U->receive(makeGaussian(0, REAL_varU, GaussianMessage::GAUSSIAN_VARIANCE));
    for (int i = 0; i < N_OBSERVATIONS; i++)
    {
        double x_new = (REAL_A * REAL_x.T())(0,0) + randn(generator) * REAL_varU;
        Matrix tmp = REAL_x;
        std::copy(tmp.data(), tmp.data()+tmp.size()-1, REAL_x.data()+1);
        REAL_x(0, 0) = x_new;

        double observation = x_new;

        // data from the previous observation
        x_prev->receive(msg);
        // observing the data
        y->receive(makeGaussian(observation, 0));

        // doing the inference
        nwk.step();

        msg = x_next->evidence();

        a_prev->receive(a_next->evidence());
    }

    double *ESTIMATED_A = a_next->evidence().mean();
    // computing the average absolute error
    double absError = 0.0;
    for (int i = 0; i < REAL_A.size(); i++)
        absError += fabs(REAL_A(0, i) - ESTIMATED_A[i]);
    absError /= REAL_A.size();

    ASSERT_NEAR(absError, 0.19, 1e-1);
}


#endif // AR_TEST_H
