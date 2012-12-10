#ifndef CORETESTS_H
#define CORETESTS_H

#include "util_test.h"

#include <factorgraph.h>
#include <network.h>


TEST(KalmanFilter, Scalar) {

    normal_distribution<double> randn(0, 1);
    default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());


    // TODO: delete/smartpointers
    auto xin = new EvidenceNode();
    auto xout = new EvidenceNode();
    auto n = new EvidenceNode();
    auto y = new EvidenceNode();
    auto e = new EqualityNode();
    auto a = new AddNode();
    auto u = new EvidenceNode();
    auto b = new AddNode();


//                    u
//             (e)    |
//      xin --> = --> + --> xout
//              |    (b)
//         n--> +(a)
//              |
//              y
    Network nwk;
    // xin --> e
    nwk.addEdge(xin, e);

    // b connections
    nwk.addEdge(e, b);
    nwk.addEdge(u, b);
    nwk.addEdge(b, xout);

    // a connections
    nwk.addEdge(e, a);
    nwk.addEdge(a, y);
    nwk.addEdge(n, a);

    double sd = 10;
    double sd2 = sd*sd;
    double u_const = 1;

    GaussianMessage msg = makeGaussian({1 +  randn(generator)*sd},
                                        {sd2});

    xout->propagate(msg);

    n->propagate(makeGaussian({0},
                                 {sd2}));

    u->propagate(makeGaussian({u_const},
                                 {0}));


    const int N_ITERATIONS = 2000;

    for (int i = 1; i <= N_ITERATIONS; i++)
    {
        // signal to xin
        xin->propagate(msg);

        //  observable value to y
        y->propagate(makeGaussian({i+randn(generator)},
                                     {0}));
        msg = xout->evidence();
    }


    ASSERT_NEAR(msg.mean()[0], N_ITERATIONS, 2);
    ASSERT_NEAR(msg.variance()[0], 0.1, 0.1);

    delete xin;
    delete xout;
    delete n;
    delete y;
    delete e;
    delete a;
    delete u;
    delete b;
}



/**
 * @brief TEST kalman filtering for multivariate gaussians:
 * TODO: create fixture for generated values ?
 */
TEST(KalmanFilter, Vector) {

    normal_distribution<double> randn(0, 1);
    default_random_engine generator(chrono::system_clock::now().time_since_epoch().count());

    auto xin = new EvidenceNode();
    auto xout = new EvidenceNode();
    auto n = new EvidenceNode();
    auto y = new EvidenceNode();
    auto e = new EqualityNode();
    auto a = new AddNode();
    auto u = new EvidenceNode();
    auto b = new AddNode();


    //                    u
    //             (e)    |
    //      xin --> = --> + --> xout
    //              |    (b)
    //         n--> +(a)
    //              |
    //              y
    Network nwk;
    // xin --> e
    nwk.addEdge(xin, e);

    // b connections
    nwk.addEdge(e, b);
    nwk.addEdge(u, b);
    nwk.addEdge(b, xout);

    // a connections
    nwk.addEdge(e, a);
    nwk.addEdge(a, y);
    nwk.addEdge(n, a);


    double sd = 10;
    double sd2 = sd*sd;
    double u_const = 1;


    GaussianMessage msg = makeGaussian({1 +  randn(generator)*sd, 1 +  randn(generator)*sd},
                                        {sd2, 0,
                                         0, sd2});

    xout->propagate(msg);

    n->propagate(makeGaussian({0, 0},
                                 {sd2, 0,
                                  0, sd2}));

    u->propagate(makeGaussian({u_const, u_const},
                                 {0, 0,
                                  0, 0}));


    const int N_ITERATIONS = 1000;

    for (int i = 1; i <= N_ITERATIONS; i++)
    {
        // signal to xin
        xin->propagate(msg);

        //  observable value to y
        y->propagate(makeGaussian({i+randn(generator), i+randn(generator)},
                                     {0, 0,
                                      0, 0}));
        msg = xout->evidence();
    }

    delete xin;
    delete xout;
    delete n;
    delete y;
    delete e;
    delete a;
    delete u;
    delete b;

}






#endif // CORETESTS_H
