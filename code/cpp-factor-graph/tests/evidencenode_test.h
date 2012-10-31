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

    Network nwk;
    EvidenceNode node;
    EvidenceNode dest;

    // node <---> dest
    nwk.addEdge(&node, &dest);
    nwk.addEdge(&dest, &node);

    GaussianMessage msg = makeGaussian(2, 3);
    node.setInitial(msg);

    EXPECT_EQ(msg.mean()[0], dest.evidence().mean()[0]);
    EXPECT_EQ(msg.variance()[0], dest.evidence().variance()[0]);
}


#endif // EVIDENCENODE_TEST_H
