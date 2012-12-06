#ifndef SCHEDULE_TEST_H
#define SCHEDULE_TEST_H

#include "util_test.h"

#include <factorgraph.h>
#include <network.h>



TEST(Schedule, Scalar) {

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

    GaussianMessage msg = makeGaussian({1 +  randn(generator)*sd},
                                        {sd2});

    xout->receive(msg);

    n->receive(makeGaussian({0}, {sd2}));

    u->receive(makeGaussian({u_const}, {0}));


    Network::Schedule schedule = {
        {xin, e},
        {n, a},
        {y, a},
        {a, e},
        {e, b},
        {u, b},
        {b, xout},
    };


    nwk.setSchedule(schedule);


    const int N_ITERATIONS = 2000;

    for (int i = 1; i <= N_ITERATIONS; i++)
    {
        // signal to xin
        xin->receive(msg);
        //  observable value to y
        y->receive(makeGaussian({i+randn(generator)}, {0}));

        nwk.step();

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




#endif // SCHEDULE_TEST_H
