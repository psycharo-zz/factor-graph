#ifndef UTIL_TEST_H
#define UTIL_TEST_H


#include <gtest/gtest.h>

#include <message.h>
#include <matrix.h>

#include <random>
#include <chrono>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <set>
using namespace std;


// just for debug
GaussianMessage makeGaussian(double mean, double variance, GaussianMessage::Type type = GaussianMessage::GAUSSIAN_VARIANCE)
{
    return GaussianMessage(&mean, &variance, 1, type);
}


GaussianMessage makeGaussian(const vector<double> &mean, const vector<double> &var, GaussianMessage::Type type = GaussianMessage::GAUSSIAN_VARIANCE)
{
    return GaussianMessage(mean.data(), var.data(), mean.size(), type);
}

GaussianMessage makeGaussianMx(const Matrix &mean, const Matrix &var, GaussianMessage::Type type = GaussianMessage::GAUSSIAN_VARIANCE)
{
    return GaussianMessage(mean.data(), var.data(), mean.size(), type);
}


class MultivariateGaussianTest : public ::testing::Test {
protected:

    GaussianMessage MESSAGE1;
    GaussianMessage MESSAGE2;


    MultivariateGaussianTest():
        MESSAGE1(makeGaussian({6.294473727863579,8.115838741512384,-7.460263674129878,8.267517122780387},
                              {1.830027341737190,1.859554140797106,-1.369547673289807,1.882371127042463,
                                1.828667792971782,-0.058497405108635,1.201121875555200,-1.432454645491139,
                              -0.312954869494900,1.662942100756268,1.168829318238218,1.837969705571612,
                                0.622962796626347,-1.857153285703242,1.396517223475108,1.735972991030202})),
        MESSAGE2(makeGaussian({2.647184924508190,-8.049191900011810,-4.430035622659032,0.937630384099677},
                              {0.714940619431094,1.030960522313334,0.972529872499665,-0.431091921863327,
                               0.621911560710227,-1.315253248753753,0.824184352078435,-1.872668614490317,
                               -0.892308060156440,-1.815314437475384,-1.611472875056610,1.293831313309171,
                               0.779314491903268,-0.731602079756558,1.800888195353420,-1.862215677988365}))
    {}

};









#endif // UTIL_TEST_H
