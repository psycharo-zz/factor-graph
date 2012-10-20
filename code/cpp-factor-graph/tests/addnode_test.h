#ifndef ADDNODE_TEST_H
#define ADDNODE_TEST_H

#include <gtest/gtest.h>
#include "../addnode.h"
#include "../evidencenode.h"
#include "util_test.h"


TEST(Scalar, AddForward) {
    AddNode node(1);
    EvidenceNode a(2);
    EvidenceNode b(3);
    EvidenceNode c(4);

    node.setConnections(&a, &b, &c);
    a.setDest(&node);
    b.setDest(&node);
    c.setDest(&node);

    a.setInitital(makeGaussian(10, 5));
    b.setInitital(makeGaussian(20, 4));

    double EXPECTED_MEAN_C = 30;
    double EXPECTED_VAR_C = 9;

    GaussianMessage msg = c.evidence();
    EXPECT_EQ(msg.size(), 1);
    EXPECT_DOUBLE_EQ(msg.mean()[0], EXPECTED_MEAN_C);
    EXPECT_DOUBLE_EQ(msg.variance()[0], EXPECTED_VAR_C);
}


TEST(Scalar, AddBackward) {
    AddNode node(1);
    EvidenceNode a(2);
    EvidenceNode b(3);
    EvidenceNode c(4);

    node.setConnections(&a, &b, &c);
    a.setDest(&node);
    b.setDest(&node);
    c.setDest(&node);

    c.setInitital(makeGaussian(10, 5));
    b.setInitital(makeGaussian(20, 4));

    double EXPECTED_MEAN_A = -10;
    double EXPECTED_VAR_A = 9;
    GaussianMessage msg = a.evidence();
    EXPECT_EQ(msg.size(), 1);
    EXPECT_DOUBLE_EQ(msg.mean()[0], EXPECTED_MEAN_A);
    EXPECT_DOUBLE_EQ(msg.variance()[0], EXPECTED_VAR_A);
}


// TODO:
TEST_F(MultivariateGaussianTest, DISABLED_AddFirst) {
}



#endif // ADDNODE_TEST_H
