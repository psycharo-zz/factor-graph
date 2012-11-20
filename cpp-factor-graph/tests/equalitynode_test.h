#ifndef EQUALITYNODE_TEST_H
#define EQUALITYNODE_TEST_H


#include <gtest/gtest.h>
#include <equalitynode.h>
#include <evidencenode.h>
#include "util_test.h"

#include <network.h>

/**
 * @brief TEST for scalar gaussian messages
 */
TEST(Equality, EqualityFirst) {
    EqualityNode node;

    EvidenceNode a;
    EvidenceNode b;
    EvidenceNode c;

    Network nwk;
    nwk.addEdge(&a, &node);
    nwk.addEdge(&b, &node);
    nwk.addEdge(&c, &node);

    a.propagate(makeGaussian(10, 5));
    b.propagate(makeGaussian(20, 4));

    double EXPECTED_MEAN = 15.555555555555557;
    double EXPECTED_VAR = 2.2222222222222223;

    GaussianMessage msg = c.evidence();
    EXPECT_EQ(msg.size(), 1);
    EXPECT_DOUBLE_EQ(msg.mean()[0], EXPECTED_MEAN);
    EXPECT_DOUBLE_EQ(msg.variance()[0], EXPECTED_VAR);
}





/**
 * @brief TEST a test for randomly generated multivariate gaussian messages
 */
TEST_F(MultivariateGaussianTest, EqualityFirst) {
    EqualityNode node;

    EvidenceNode a;
    EvidenceNode b;
    EvidenceNode c;

    Network nwk;
    nwk.addEdge(&a, &node);
    nwk.addEdge(&b, &node);
    nwk.addEdge(&c, &node);


    a.propagate(MESSAGE1);
    b.propagate(MESSAGE2);
    GaussianMessage msg = c.evidence();


    vector<double> EXPECTED_VAR={
    -1.646459154270367,-6.126513638189412,-4.389187806891512,1.592975300265311,
    0.878057249118697,0.787807839002381,1.591054549220516,-1.415269373111098,
    3.520025769180220,12.027423496056246,8.478920460635683,-2.423395475156308,
    -1.056083712272815,-5.252161783528413,-2.350108335279095,0.321750794414538
    };

    vector<double> EXPECTED_MEAN={13.094486059998646,29.331956236646377,18.750973326997514,-4.171817378026319};


    EXPECT_EQ(msg.size(), EXPECTED_MEAN.size());
    EXPECT_EQ(msg.size2(), EXPECTED_VAR.size());
    for (size_t i = 0; i < msg.size(); i++)
        EXPECT_FLOAT_EQ(msg.mean()[i], EXPECTED_MEAN[i]);
    for (size_t i = 0; i < msg.size2(); i++)
        EXPECT_FLOAT_EQ(msg.variance()[i], EXPECTED_VAR[i]);
}



TEST(Equality, Equal) {

    EvidenceNode x;
    EvidenceNode y;
    EvidenceNode z;
    EqualityNode equ(GaussianMessage::GAUSSIAN_PRECISION);


    Network nwk;

    nwk.addEdge(&x, &equ);
    nwk.addEdge(&y, &equ);
    nwk.addEdge(&z, &equ);

    Network::Schedule schedule = {
        {&x, &equ},
        {&y, &equ},
        {&equ, &z},
        {&z, &equ},
        {&equ, &x}
    };

    nwk.setSchedule(schedule);

    x.receive(makeGaussian({2, 2}, {10, 0, 0, 10}, GaussianMessage::GAUSSIAN_PRECISION));
    y.receive(makeGaussian({2, 2}, {10, 0, 0, 10}, GaussianMessage::GAUSSIAN_PRECISION));

    nwk.step();

    vector<double> EXPECTED_MEAN_Z = {2,2};
    vector<double> EXPECTED_PREC_Z = {20,0,0,20};
    for (size_t i = 0; i < EXPECTED_MEAN_Z.size(); i++)
        EXPECT_FLOAT_EQ(z.evidence().mean()[i], EXPECTED_MEAN_Z[i]);
    for (size_t i = 0; i < EXPECTED_PREC_Z.size(); i++)
        EXPECT_FLOAT_EQ(z.evidence().precision()[i], EXPECTED_PREC_Z[i]);


    vector<double> EXPECTED_MEAN_X = {2,2};
    vector<double> EXPECTED_PREC_X = {30,0,0,30};
    for (size_t i = 0; i < EXPECTED_MEAN_X.size(); i++)
        EXPECT_FLOAT_EQ(x.evidence().mean()[i], EXPECTED_MEAN_X[i]);
    for (size_t i = 0; i < EXPECTED_PREC_X.size(); i++)
        EXPECT_FLOAT_EQ(x.evidence().precision()[i], EXPECTED_PREC_X[i]);

}


/**
 * @brief testing propagation for the no-information (precision = 0) message
 */
TEST(EqualityNode, NoInformation) {

    EvidenceNode x;
    EvidenceNode y;
    EvidenceNode z;
    EqualityNode equ(GaussianMessage::GAUSSIAN_PRECISION);

    Network nwk;

    nwk.addEdge(&x, &equ);
    nwk.addEdge(&y, &equ);
    nwk.addEdge(&z, &equ);

    x.propagate(makeGaussian({2,3}, {1, 0, 0, 1}, GaussianMessage::GAUSSIAN_PRECISION));

    // dummy message for no
    z.propagate(makeGaussian({12313, 123131321}, {0, 0, 0, 0}, GaussianMessage::GAUSSIAN_PRECISION));


    vector<double> EXPECTED_MEAN = {2,3};
    vector<double> EXPECTED_PREC = {1, 0, 0, 1};
    for (size_t i = 0; i < EXPECTED_MEAN.size(); i++)
        EXPECT_FLOAT_EQ(y.evidence().mean()[i], EXPECTED_MEAN[i]);

    for (size_t i = 0; i < EXPECTED_PREC.size(); i++)
        EXPECT_FLOAT_EQ(y.evidence().precision()[i], EXPECTED_PREC[i]);


}




#endif // EQUALITYNODE_TEST_H
