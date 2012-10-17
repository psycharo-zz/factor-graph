#ifndef CORETESTS_H
#define CORETESTS_H

#include "util_test.h"

#include <factorgraph.h>




TEST(KalmanFilter, Scalar) {

    normal_distribution<double> randn(0, 1);
    default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());

    auto xin = new EvidenceNode(1);
    auto xout = new EvidenceNode(2);
    auto n = new EvidenceNode(3);
    auto y = new EvidenceNode(4);
    auto e = new EqualityNode(5);
    auto a = new AddNode(6);
    auto u = new EvidenceNode(7);
    auto b = new AddNode(8);

    xin->setDest(e);
    b->setConnections(e, u, xout);
    xout->setDest(b);
    n->setDest(a);
    y->setDest(a);
    e->setConnections(xin, a, b);
    a->setConnections(e, n, y);
    u->setDest(b);

    double sd = 10;
    double sd2 = sd*sd;
    double u_const = 1;

    GaussianMessage msg = makeGaussian({1 +  randn(generator)*sd},
                                        {sd2});

    xout->setInitital(msg);

    n->setInitital(makeGaussian({0},
                                 {sd2}));

    u->setInitital(makeGaussian({u_const},
                                 {0}));


    const int N_ITERATIONS = 1000;

    for (int i = 1; i <= N_ITERATIONS; i++)
    {
        // signal to xin
        xin->setInitital(msg);

        //  observable value to y
        y->setInitital(makeGaussian({i+randn(generator)},
                                     {0}));
        msg = xout->evidence();
    }


    ASSERT_NEAR(msg.mean()[0], 1000, 2);
    ASSERT_NEAR(msg.variance()[0], 0.1, 0.1);
}



/**
 * @brief TEST kalman filtering for multivariate gaussians:
 * TODO: create fixture for generated values ?
 */
TEST(KalmanFilter, DISABLE_Vector) {

    normal_distribution<double> randn(0, 1);
    default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());

    auto xin = new EvidenceNode(1);
    auto xout = new EvidenceNode(2);
    auto n = new EvidenceNode(3);
    auto y = new EvidenceNode(4);
    auto e = new EqualityNode(5);
    auto a = new AddNode(6);
    auto u = new EvidenceNode(7);
    auto b = new AddNode(8);

    xin->setDest(e);
    b->setConnections(e, u, xout);
    xout->setDest(b);
    n->setDest(a);
    y->setDest(a);
    e->setConnections(xin, a, b);
    a->setConnections(e, n, y);
    u->setDest(b);

    double sd = 10;
    double sd2 = sd*sd;
    double u_const = 1;


    GaussianMessage msg = makeGaussian({1 +  randn(generator)*sd, 1 +  randn(generator)*sd},
                                        {sd2, 0,
                                         0, sd2});

    xout->setInitital(msg);

    n->setInitital(makeGaussian({0, 0},
                                 {sd2, 0,
                                  0, sd2}));

    u->setInitital(makeGaussian({u_const, u_const},
                                 {0, 0,
                                  0, 0}));


    const int N_ITERATIONS = 1000;

    for (int i = 1; i <= N_ITERATIONS; i++)
    {
        // signal to xin
        xin->setInitital(msg);

        //  observable value to y
        y->setInitital(makeGaussian({i+randn(generator), i+randn(generator)},
                                     {0, 0,
                                      0, 0}));
        msg = xout->evidence();
    }



}






#endif // CORETESTS_H
