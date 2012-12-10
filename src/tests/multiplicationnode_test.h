#ifndef MULTIPLICATIONNODE_TEST_H
#define MULTIPLICATIONNODE_TEST_H


#include "util_test.h"
#include <factorgraph.h>
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

    EXPECT_EQ(evidence.size(), msg.size());


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

    GaussianMessage msg = makeGaussian({1, 2, 3}, {1, 1, 0.5, 0.5, 1, 0, 0, 0, 1}, GaussianMessage::GAUSSIAN_PRECISION);

    out.propagate(msg);

    GaussianMessage evidence = in.evidence();

    EXPECT_EQ(evidence.size(), msg.size());

    for (size_t i = 0; i < msg.size(); i++)
        EXPECT_FLOAT_EQ(evidence.mean()[i], msg.mean()[i]);
    for (size_t i = 0; i < msg.size2(); i++)
        EXPECT_FLOAT_EQ(evidence.precision()[i], msg.precision()[i]);

}



TEST(MultiplicationNode, ScalarVectorForward) {

    EvidenceNode in;
    MultiplicationNode b;
    EvidenceNode out;

    vector<double> matrix = {1,0,0,0,0};
    b.setMatrix(matrix.data(), 5, 1);

    Network nwk;

    nwk.addEdge(&in, &b);
    nwk.addEdge(&b, &out);

    in.propagate(makeGaussian(20, 0.5));

    auto msg = out.evidence();

    EXPECT_EQ(msg.size(), matrix.size());

    vector<double> EXPECTED_VAR(msg.size2(), 0);
    EXPECTED_VAR[0] = 0.5;

    for (size_t i = 0; i < msg.size2(); i++)
        EXPECT_FLOAT_EQ(msg.variance()[i], EXPECTED_VAR[i]);


}



TEST(MultiplicationNode, ScalarVectorBackward) {

    EvidenceNode in;
    MultiplicationNode b;
    EvidenceNode out;

    vector<double> matrix = {1,0,0,0,0};
    b.setMatrix(matrix.data(), 1, 5);

    Network nwk;

    nwk.addEdge(&in, &b);
    nwk.addEdge(&b, &out);

    out.propagate(makeGaussian(20, 2, GaussianMessage::GAUSSIAN_PRECISION));

    auto msg = in.evidence();

    EXPECT_EQ(msg.size(), matrix.size());

    vector<double> EXPECTED_PRECISION(msg.size2(), 0);
    EXPECTED_PRECISION[0] = 2;

    for (size_t i = 0; i < msg.size2(); i++)
        EXPECT_FLOAT_EQ(msg.precision()[i], EXPECTED_PRECISION[i]);
}










#endif // MULTIPLICATIONNODE_TEST_H
