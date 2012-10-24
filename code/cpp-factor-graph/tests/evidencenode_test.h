#ifndef EVIDENCENODE_TEST_H
#define EVIDENCENODE_TEST_H

#include <gtest/gtest.h>
#include "../evidencenode.h"
#include "util_test.h"
#include "../network.h"



/**
 * @brief TEST basic test for scalar gaussians
 */
TEST(Scalar, EvidenceFirst) {
    EvidenceNode node;
    EvidenceNode dest;

    // node <---> dest
    node.setDest(&dest);
    dest.setDest(&node);

    GaussianMessage msg = makeGaussian(2, 3);
    node.setInitital(msg);

    EXPECT_EQ(msg.mean()[0], dest.evidence().mean()[0]);
    EXPECT_EQ(msg.variance()[0], dest.evidence().variance()[0]);
}


#endif // EVIDENCENODE_TEST_H
