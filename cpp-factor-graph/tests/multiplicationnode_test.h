#ifndef MULTIPLICATIONNODE_TEST_H
#define MULTIPLICATIONNODE_TEST_H


#include "util_test.h"
#include <multiplicationnode.h>

/**
 * NOTE: more involved tests are in MATLAB
 */


// sanity check for identity matrix multiplication
TEST(Multiplication, IdentityForward) {

    Network nwk;

    EvidenceNode in;
    EvidenceNode out;

    double matrix[] = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };

    MultiplicationNode A(matrix, 3, 3);

    nwk.addEdge(&in, &A);
    nwk.addEdge(&A, &out);

    GaussianMessage msg = makeGaussian({1, 2, 3}, {1, 0, 0.5, 0.5, 1, 0, 0, 0, 1});

    in.propagate(msg);

    GaussianMessage evidence = out.evidence();


    for (size_t i = 0; i < msg.size(); i++)
        EXPECT_FLOAT_EQ(evidence.mean()[i], msg.mean()[i]);
    for (size_t i = 0; i < msg.size2(); i++)
        EXPECT_FLOAT_EQ(evidence.variance()[i], msg.variance()[i]);

}


// sanity check for identity matrix multiplication
TEST(Multiplication, IdentityBackward) {

    Network nwk;

    EvidenceNode in;
    EvidenceNode out;

    double matrix[] = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };

    MultiplicationNode A(matrix, 3, 3);

    nwk.addEdge(&in, &A);
    nwk.addEdge(&A, &out);

    GaussianMessage msg = makeGaussian({1, 2, 3}, {1, 0, 0.5, 0.5, 1, 0, 0, 0, 1});

    out.propagate(msg);

    GaussianMessage evidence = in.evidence();

    for (size_t i = 0; i < msg.size(); i++)
        EXPECT_FLOAT_EQ(evidence.mean()[i], msg.mean()[i]);
    for (size_t i = 0; i < msg.size2(); i++)
        EXPECT_FLOAT_EQ(evidence.variance()[i], msg.variance()[i]);

}











#endif // MULTIPLICATIONNODE_TEST_H
