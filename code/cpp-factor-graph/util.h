#ifndef UTIL_H
#define UTIL_H

/**
 * contains utility functions for matrix manipulation etc
 * NOTE: these use column major matrix representation
 */


#include <algorithm>
#include <vector>


// macroses for compilation
#ifndef MATLAB
    #define dgetrf dgetrf_
    #define dgetri dgetri_
    #define dgemm dgemm_
    #define dgemv dgemv_
#endif


extern "C" {

    // TODO: vary namings depending on the platform

    // LU decomoposition of a general matrix
    void dgetrf(size_t * M, size_t *N, double* A, size_t * lda, size_t* IPIV, size_t* INFO);

    // generate inverse of a matrix given its LU decomposition
    void dgetri(size_t* N, double* A, size_t* lda, size_t* IPIV, double* WORK, size_t* lwork, size_t* INFO);


    void dgemm(const char *transa, const char *transb,
               size_t *N, size_t *M, size_t *K, // TODO: check the order here
                double *alpha,
                const double *B, size_t *ldb,
                const double *A, size_t *lda,
                double *beta,
                double *C, size_t *ldc);

    void dgemv(const char *trans, size_t *N, size_t *M,
                double *alpha,
                const double *A, size_t *lda,
                const double *X, size_t *incx,
                double *beta,
                void *y, size_t *incy);
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
inline void matrix_mult(size_t M, size_t N, size_t K, const double *A, const double *B, double *C, bool transA = false, bool transB = false)
{
    char TN = 'N';
    char TT = 'T';
    double alpha = 1.0;
    double beta = 0.0;

    // TODO: check if this is working for *nix
    dgemm(transA ? &TT : &TN,
           transB ? &TT : &TN,
           &N, &M, &K,
           &alpha,
           B, &K,
           A, &M,
           &beta,
           C, &M);
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
    char TN = 'N';
    char TT = 'T';
    double alpha = 1.0;
    double beta = 0.0;
    size_t inc = 1;

    dgemv(transA ? &TT : &TN,
           &N, &M,
           &alpha,
           A, &M,
           X, &inc,
           &beta,
           out, &inc);
}



/**
 * @brief inverse_matrix inverts in-place a square matrix
 * @param A
 * @param N
 */
inline void matrix_inverse(double* A, size_t N)
{
    size_t *IPIV = new size_t[N+1];
    size_t LWORK = N*N;
    double *WORK = new double[LWORK];
    size_t INFO;
    dgetrf(&N,&N,A,&N,IPIV,&INFO);
    dgetri(&N,A,&N,IPIV,WORK,&LWORK,&INFO);
    delete [] IPIV;
    delete [] WORK;
}





#endif // UTIL_H
