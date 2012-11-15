#ifndef ESITMATEMULTIPLICATIONNODE_TEST_H
#define ESITMATEMULTIPLICATIONNODE_TEST_H

#include <factorgraph.h>
#include "util_test.h"

// sanity check for identity matrix multiplication
TEST(EstimateMultiplication, IdentityForward) {

    Network nwk;

    EvidenceNode in;
    EvidenceNode out;

    double matrix[] = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };

    EstimateMultiplicationNode A;

    A.setMatrix(matrix, 3, 3);

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
TEST(EstimateMultiplication, IdentityBackward) {


    Network nwk;

    EvidenceNode in;
    EvidenceNode out;

    double matrix[] = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };

    EstimateMultiplicationNode A;
    A.setMatrix(matrix, 3, 3);

    nwk.addEdge(&in, &A);
    nwk.addEdge(&A, &out);

    GaussianMessage msg = makeGaussian({1, 2, 3}, {1, 0, 0.5, 0.5, 1, 0, 0, 0, 1}, GaussianMessage::GAUSSIAN_PRECISION);

    out.propagate(msg);

    GaussianMessage evidence = in.evidence();

    for (size_t i = 0; i < msg.size(); i++)
        EXPECT_FLOAT_EQ(evidence.mean()[i], msg.mean()[i]);
    for (size_t i = 0; i < msg.size2(); i++)
        EXPECT_FLOAT_EQ(evidence.precision()[i], msg.precision()[i]);


}


TEST(EstimateMultiplication, IdentityEstimate) {
    Network nwk;

    EvidenceNode in;
    EvidenceNode out;
    EvidenceNode estmt;

    EstimateMultiplicationNode A;

    vector<double> EYE3 = {1, 0, 0, 0, 1, 0, 0, 0, 1};

    A.setMatrix(EYE3.data(), 3, 3);


    nwk.addEdge(&in, &A);
    nwk.addEdge(&A, &out);
    nwk.addEdge(&A, &estmt, "estimate");


    auto INPUT_MSG_X = makeGaussian({1,2,3}, EYE3);
    auto INPUT_MSG_Y = makeGaussian({1, 0, 0}, EYE3);

    in.propagate(INPUT_MSG_X);
    out.propagate(INPUT_MSG_Y);

    vector<double> EXPECTED_MEAN = {0.071428575, 0.14285715, 0.21428572};
    vector<double> EXPECTED_VAR = {1, 2, 3, 2, 4, 6, 3, 6, 9};

    auto RESULT = estmt.evidence();

    assert(RESULT.type() == GaussianMessage::GAUSSIAN_PRECISION);
    for (size_t i = 0; i < EXPECTED_MEAN.size(); i++)
        EXPECT_FLOAT_EQ(RESULT.mean()[i], EXPECTED_MEAN[i]);
    for (size_t i = 0; i < EXPECTED_VAR.size(); i++)
        EXPECT_FLOAT_EQ(RESULT.precision()[i], EXPECTED_VAR[i]);

}


// matrix update check
TEST(EstimateMultiplication, ReceiveEstimate) {
    Network nwk;

    EstimateMultiplicationNode A;
    EvidenceNode estmt;

    nwk.addEdge(&A, &estmt, EstimateMultiplicationNode::ESTIMATED_TAG);

    auto msg = makeGaussian({1,2,3}, {0,0,0,0,0,0,0,0,0});

    A.receive(estmt.id(), msg);

    vector<double> EXPECTED_MATRIX = {1, 1, 0, 2, 0, 1, 3, 0, 0};

    EXPECT_EQ(A.size(), 3);
    for (size_t i = 0; i < EXPECTED_MATRIX.size(); i++)
        EXPECT_FLOAT_EQ(A.matrix()[i], EXPECTED_MATRIX[i]);
}





#endif // ESITMATEMULTIPLICATIONNODE_TEST_H
