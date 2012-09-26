#include <iostream>

#include "factorgraph.h"


#include <random>
#include <chrono>


#include <cstdlib>
#include <climits>
#include <cmath>




int main()
{
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

    Message msg(1 +  randn(generator)*sd, sd2);

    xout->receive(msg);
    n->receive(Message(0, sd2));
    u->receive(Message(u_const, 0));


    const int N_ITERATIONS = 20;

    for (int i = 1; i <= N_ITERATIONS; i++)
    {
        // signal to xin
        xin->receive(msg);
        //  observable value to y
        y->receive(Message(i+randn(generator), 0));
        msg = xout->evidence();
    }

    FactorNode *test = u;


    return 0;
}

