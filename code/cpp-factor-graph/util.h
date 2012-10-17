#ifndef UTIL_H
#define UTIL_H

/**
 * contains utility functions for matrix manipulation etc
 * NOTE: these use column major matrix representation
 */


#include <gsl/gsl_cblas.h>

#include <cassert>
#include <algorithm>
#include <vector>


extern "C" {
    // LU decomoposition of a general matrix
    void dgetrf_(int* M, int *N, double* A, int* lda, int* IPIV, int* INFO);

    // generate inverse of a matrix given its LU decomposition
    void dgetri_(int* N, double* A, int* lda, int* IPIV, double* WORK, int* lwork, int* INFO);

}




/**
 * @brief multiply_matrices
 * @param M - # of rows in A
 * @param N - # of columns in B
 * @param K - # of columns in A
 * @param A - input MxK matrix
 * @param B - input KxN matrix
 * @param out - output matrix MxN
 */
inline void matrix_mult(size_t M, size_t N, size_t K, const double *A, const double *B, double *out, bool transA = false, bool transB = false)
{
    cblas_dgemm(CblasColMajor,
                transA ? CblasTrans : CblasNoTrans,
                transB ? CblasTrans : CblasNoTrans,
                M, N, K,
                1.0, A, M,
                B, K, 1.0,
                out, M);
}


/**
 * @brief mulitply_matrix_by_vector
 * @param M - # of rows in A
 * @param N - # of columns in A
 * @param A - the matrix
 * @param X - the vector
 * @param out - the output matrix
 */
inline void matrix_vector_mult(size_t M, size_t N, const double *A, const double *X, double *out, bool transA = false)
{
    cblas_dgemv(CblasColMajor,
                transA ? CblasTrans : CblasNoTrans,
                M, N,
                1.0, A, M,
                X, 1,
                0, out, 1);
}



/**
 * @brief inverse_matrix inverts in-place a square matrix
 * @param A
 * @param N
 */
inline void matrix_inverse(double* A, int N)
{
    int *IPIV = new int[N+1];
    int LWORK = N*N;
    double *WORK = new double[LWORK];
    int INFO;
    dgetrf_(&N,&N,A,&N,IPIV,&INFO);
    dgetri_(&N,A,&N,IPIV,WORK,&LWORK,&INFO);
    delete [] IPIV;
    delete [] WORK;
}





#endif // UTIL_H
