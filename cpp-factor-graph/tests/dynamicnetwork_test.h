#ifndef DYNAMICNETWORK_TEST_H
#define DYNAMICNETWORK_TEST_H

#include "util_test.h"

#include <factorgraph.h>
#include <dynamicnetwork.h>


/**
 * @brief testing the dynamic network
 */
TEST(DynamicNetwork, Vector) {

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
    DynamicNetwork nwk;
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

    nwk.addTemporalEdge(xout, xin);

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



    double sd = 10;
    double sd2 = sd*sd;
    double u_const = 1;


    GaussianMessage msg = makeGaussian({1 +  randn(generator)*sd, 1 +  randn(generator)*sd},
                                        {sd2, 0,
                                         0, sd2});

    xout->receive(msg);

    n->receive(makeGaussian({0, 0}, {sd2, 0, 0, sd2}));
    u->receive(makeGaussian({u_const, u_const}, {0, 0, 0, 0}));


    const int N_ITERATIONS = 1000;

    xin->receive(msg);

    // filling the data
    vector<map<int, GaussianMessage> > data;
    for (int i = 1; i <= N_ITERATIONS; i++)
    {
        auto evd = makeGaussian({i+randn(generator), i+randn(generator)}, {0, 0, 0, 0});
        data.push_back({{y->id(), evd}});
    }

    nwk.step(data);


    delete xin;
    delete xout;
    delete n;
    delete y;
    delete e;
    delete a;
    delete u;
    delete b;

}





#endif // DYNAMICNETWORK_TEST_H
