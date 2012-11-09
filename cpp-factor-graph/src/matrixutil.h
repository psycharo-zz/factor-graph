#ifndef UTIL_H
#define UTIL_H

/**
 * contains utility functions for matrix manipulation etc
 * NOTE: these use column major matrix representation
 */


#include <algorithm>
#include <vector>


// macroses for compilation
#if !defined (WIN32)  && !defined (_WIN32) || !defined (_WIN64)
    #define dgetrf dgetrf_
    #define dgetri dgetri_
    #define dgemm dgemm_
    #define dgemv dgemv_
    #define ddot ddot_
    #define dscal dscal_
#endif


extern "C" {

    // TODO: vary namings depending on the platform

    // LU decomoposition of a general matrix
    void dgetrf(size_t * M, size_t *N, double* A, size_t * lda, size_t* IPIV, size_t* INFO);

    // generate inverse of a matrix given its LU decomposition
    void dgetri(size_t* N, double* A, size_t* lda, size_t* IPIV, double* WORK, size_t* lwork, size_t* INFO);


    // blas matrix-matrix multiplication
    void dgemm(const char *transa, const char *transb,
               size_t *N, size_t *M, size_t *K, // TODO: check the order here
                double *alpha,
                const double *A, size_t *lda,
                const double *B, size_t *ldb,
                double *beta,
                double *C, size_t *ldc);

    // blas matrix-vector multiplication
    void dgemv(const char *trans, size_t *N_rowsA, size_t *M_colsA,
                double *alpha,
                const double *A, size_t *lda,
                const double *X, size_t *incx,
                double *beta,
                void *y, size_t *incy);

    // dot product
    double ddot(size_t *N, const double *X, size_t *incx, const double *Y, size_t *incy);

    // vector
    void dscal(size_t *N, const double *alpha, double *X, size_t *incx);
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
inline void matrix_mult(size_t M_rowsA, size_t N_colsB, size_t K_colsA, const double *A, const double *B, double *C,
                        bool transA = false, bool transB = false, double alpha = 1.0)
{
    char TN = 'N';
    char TT = 'T';
    double beta = 0.0;

    // TODO: check if this is working for *nix
    dgemm(transA ? &TT : &TN,
          transB ? &TT : &TN,
          &M_rowsA, &N_colsB, &K_colsA,
          &alpha,
          A, transA ? &K_colsA : &M_rowsA,
          B, transB ? &N_colsB : &K_colsA,
          &beta,
          C, &M_rowsA);
}


/**
 * @brief mulitply_matrix_by_vector
 * @param M - # of rows in A
 * @param N - # of columns in A
 * @param A - the matrix
 * @param X - the vector
 * @param out - the output matrix
 */
inline void matrix_vector_mult(size_t M_rowsA, size_t N_colsA, const double *A, const double *X, double *out, bool transA = false, double alpha = 1.0)
{
    char TN = 'N';
    char TT = 'T';
    double beta = 0.0;
    size_t inc = 1;

    dgemv(transA ? &TT : &TN,
          &M_rowsA, &N_colsA,
          &alpha,
          A, transA ? &N_colsA : &M_rowsA,
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


/**
 * @brief compute the dot product of two vectors
 */
inline double vector_dot(const double *A, const double *B, size_t NA)
{
    size_t one = 1;
    return ddot(&NA, A, &one, B, &one);
}


/**
 * @brief multiply vector by a scalar
 */
inline void vector_scalar(double *A, size_t NA, double scalar)
{
    size_t one = 1.0;
    dscal(&NA, &scalar, A, &one);
}





#endif // UTIL_H
