#ifndef MATRIXUTIL_TEST_H
#define MATRIXUTIL_TEST_H

#include "util_test.h"
#include <matrixutil.h>


TEST(MatrixUtil, SingularValueDecomposition)
{
    double INPUT_MATRIX[] = {
        1,2,
        2,3,
        3,4
    };

    double *U;
    double *S;
    double *VT;

    matrix_svd(INPUT_MATRIX, 2, 3, U, S, VT);

    double EXPECTED_U[] = {
        -0.569594837762602,  -0.821925617555625,
        -0.821925617555625,   0.569594837762601
    };

    double EXPECTED_S[] = {
        6.546755636442665, 0.374153226240496
    };

    double EXPECTED_VT[] = {
        -0.338098165838458,   0.847952217751686,   0.408248290463863,
        -0.550649318285984,   0.173547289245654,  -0.816496580927726,
        -0.763200470733510,  -0.500857639260381,   0.408248290463863
    };

    for (int i = 0; i < 2; i++)
        EXPECT_FLOAT_EQ(EXPECTED_S[i], S[i]) << "at index " << i;

    for (int i = 0; i < 2*2; i++)
        EXPECT_FLOAT_EQ(EXPECTED_U[i], U[i]) << "at index " << i;

    for (int i = 0; i < 3*3; i++)
        EXPECT_FLOAT_EQ(EXPECTED_VT[i], VT[i]) << "at index " << i;

}



TEST(MatrixUtil, PseudoInverseNormal)
{
    double INPUT_MATRIX[] = {
        1,2,
        2,3,
        3,4
    };

    double EXPECTED_PINV[] = {
        -1.833333333333333,  -0.333333333333335,   1.166666666666668,
         1.333333333333333,   0.333333333333335,  -0.666666666666668
    };

    double RESULT[2*3];
    matrix_pseudo_inverse(INPUT_MATRIX, 2, 3, RESULT);

    for (int i = 0; i < 3*2; i++)
        EXPECT_FLOAT_EQ(EXPECTED_PINV[i], RESULT[i]) << "at index " << i;
}


TEST(MatrixUtil, PseudoInverseVector)
{
    double INPUT_MATRIX[] = {
        1,0,0
    };

    double RESULT[3];
    matrix_pseudo_inverse(INPUT_MATRIX, 1, 3, RESULT);
    for (int i = 0; i < 3; i++)
        EXPECT_FLOAT_EQ(INPUT_MATRIX[i], RESULT[i]) << "at index " << i;

    matrix_pseudo_inverse(INPUT_MATRIX, 3, 1, RESULT);
    for (int i = 0; i < 3; i++)
        EXPECT_FLOAT_EQ(INPUT_MATRIX[i], RESULT[i]) << "at index " << i;

}


TEST(MatrixUtil, PseudoInverseZeros) {

    double INPUT_MATRIX[] = {
        100, 0, 0,
        0, 0, 0,
        0, 0, 0
    };

    double EXPECTED_PINV[] = {
        1./100, 0, 0,
        0, 0, 0,
        0, 0, 0
    };

    double RESULT[3*3];
    matrix_pseudo_inverse(INPUT_MATRIX, 3, 3, RESULT);

    for (int i = 0; i < 3*3; i++)
        EXPECT_FLOAT_EQ(EXPECTED_PINV[i], RESULT[i]) << "at index " << i;
}

TEST(Matrix, Add) {




//    Matrix a();
}



#endif // MATRIXUTIL_TEST_H
