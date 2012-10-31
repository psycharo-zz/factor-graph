#ifndef ADDNODE_TEST_H
#define ADDNODE_TEST_H

#include <gtest/gtest.h>
#include "../addnode.h"
#include "../evidencenode.h"
#include "util_test.h"
#include "../network.h"


TEST(Scalar, AddForward) {
    AddNode node;
    EvidenceNode a;
    EvidenceNode b;
    EvidenceNode c;

    Network nwk;
    nwk.addEdge(&a, &node);
    nwk.addEdge(&b, &node);
    nwk.addEdge(&node, &c);

    a.setInitial(makeGaussian(10, 5));
    b.setInitial(makeGaussian(20, 4));

    double EXPECTED_MEAN_C = 30;
    double EXPECTED_VAR_C = 9;

    GaussianMessage msg = c.evidence();
    EXPECT_EQ(msg.size(), 1);
    EXPECT_DOUBLE_EQ(msg.mean()[0], EXPECTED_MEAN_C);
    EXPECT_DOUBLE_EQ(msg.variance()[0], EXPECTED_VAR_C);
}


TEST(Scalar, AddBackward) {
    AddNode node;
    EvidenceNode a;
    EvidenceNode b;
    EvidenceNode c;

    Network nwk;
    nwk.addEdge(&a, &node);
    nwk.addEdge(&b, &node);
    nwk.addEdge(&node, &c);

    c.setInitial(makeGaussian(10, 5));
    b.setInitial(makeGaussian(20, 4));

    double EXPECTED_MEAN_A = -10;
    double EXPECTED_VAR_A = 9;
    GaussianMessage msg = a.evidence();
    EXPECT_EQ(msg.size(), 1);
    EXPECT_DOUBLE_EQ(msg.mean()[0], EXPECTED_MEAN_A);
    EXPECT_DOUBLE_EQ(msg.variance()[0], EXPECTED_VAR_A);
}


// TODO:
//TEST_F(MultivariateGaussianTest, DISABLED_AddFirst) {
//}



#endif // ADDNODE_TEST_H
