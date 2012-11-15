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
    #define dgesvd dgesvd_
#endif



const int EPSILON = 1e-5;


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
    void dgemv(const char *trans, const size_t *N_rowsA, const size_t *M_colsA,
               double *alpha,
               const double *A, size_t *lda,
               const double *X, size_t *incx,
               double *beta,
               void *y, size_t *incy);

    // dot product
    double ddot(size_t *N, const double *X, size_t *incx, const double *Y, size_t *incy);

    // vector
    void dscal(size_t *N, const double *alpha, double *X, size_t *incx);

//    SUBROUTINE DGESVD( JOBU, JOBVT, M, N, A, LDA, S, U, LDU, VT, LDVT,
//                      WORK, LWORK, INFO )
    // SVD decomposition
    void dgesvd(char *jobu, char *jobvt,
                size_t *m, size_t *n,
                const double *a, size_t *lda,
                double *s,
                double *u, size_t *ldu,
                double *vt, size_t *ldvt,
                double *work, int *lwork, int *info);

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
inline void matrix_mult(size_t M_rowsA, size_t N_colsB, size_t K_colsA,
                        const double *A, const double *B, double *C,
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
 * @brief matrix_add
 * @param M_rows
 * @param N_cols
 * @param A
 * @param B
 * @param C
 */
inline void matrix_add(size_t M_rows, size_t N_cols,
                       const double *A, const double *B, double *C)
{
    std::transform(A, A + M_rows * N_cols, B, C, std::plus<double>());
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


/**
 * @brief compute matrix SVD decomposition A = U S V'
 * @param A the input matrix
 * @param M_rows the # of rows
 * @param N_cols the # of columns
 * @param U matrix sized M*M
 * @param S singular values - at most min(M_rows,N_cols)
 * @param V' transposed left singular values
 * @param workMult space to use for work matrix
 * @return true if everything went OK, false otherwise
 */
inline void matrix_svd(const double *A, size_t M_rows, size_t N_cols, double* &U, double* &S, double * &VT)
{
    char jobu = 'A';
    char jobvt = 'A';

    size_t min_NM = std::min(M_rows, N_cols);
    size_t max_NM = std::max(M_rows, N_cols);

    S = new double[min_NM];

    size_t ldu = M_rows;
    U = new double[ldu * M_rows];
    size_t ldvt = N_cols;
    VT = new double[ldvt * N_cols];

    // this is not a magic number, see fortran docs
    int lwork = 5 * max_NM;
    double *work = new double[lwork];

    int info = 0;

    dgesvd(&jobu, &jobvt,
           &M_rows, &N_cols,
           A, &M_rows,
           S,
           U, &ldu,
           VT, &ldvt,
           work, &lwork,
           &info);
    // TODO: process infO?
}



/**
 * compute the Moore-Penrouse pseudoinverse using SVD
 * @param A input matrix
 * @param M_rows # of rows in A and columns in out
 * @param N_cols # of cols in A and rows in out
 * @param out the pseudoinverse matrix of A
 */
inline void matrix_pseudo_inverse(const double *A, size_t M_rows, size_t N_cols, double *out)
{
    // M x M
    double *U;
    // min(M, N)
    double *S;
    // N x N
    double *VT;

    // the number of singular values
    size_t min_NM = std::min(M_rows, N_cols);

    // the matrix in svd is overwritten for some reason
    std::vector<double> tmp(A, A + M_rows * N_cols);

    matrix_svd(tmp.data(), M_rows, N_cols, U, S, VT);

    tmp.assign(tmp.size(), 0.0);

    // transposed S matrix
    for (size_t i = 0; i < min_NM; ++i)
        if (std::fabs(S[i]) > EPSILON)
            tmp[(N_cols+1) * i] =  1.0 / S[i];

    std::vector<double> VS(M_rows * N_cols);

    matrix_mult(N_cols, M_rows, N_cols, VT, tmp.data(), VS.data(), true);
    matrix_mult(N_cols, M_rows, M_rows, VS.data(), U, out, false, true);
}










#endif // UTIL_H
