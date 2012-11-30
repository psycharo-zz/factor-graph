#ifndef UTIL_H
#define UTIL_H

/**
 * contains utility functions for matrix manipulation etc
 * NOTE: these use column major matrix representation
 */


#include <algorithm>
#include <vector>
#include <cmath>
#include <cstddef>



// macroses for compilation
#if !defined(MATLAB) || (!defined(_WIN32) && !defined(_WIN64))
    #define dgetrf dgetrf_
    #define dgetri dgetri_
    #define dgemm dgemm_
    #define dgemv dgemv_
    #define ddot ddot_
    #define dscal dscal_
    #define dgesvd dgesvd_
#endif

#ifdef MATLAB
#include <mex.h>
inline void printMatrix(const double *a, size_t m, size_t n)
{
    for (size_t i = 0; i < m; i++)
    {
        for (size_t j = 0; j < n; j++)
            mexPrintf("%f ", a[j * m + i]);
        mexPrintf("\n");
    }
    mexPrintf("\n");
}
#endif



const int EPSILON = 1e-6;


extern "C" {

    // LU decomoposition of a general matrix
    void dgetrf(ptrdiff_t * M,
                ptrdiff_t *N,
                double* A,
                ptrdiff_t *lda,
                ptrdiff_t *IPIV,
                ptrdiff_t *INFO);

    // generate inverse of a matrix given its LU decomposition
    void dgetri(ptrdiff_t* N,
                double* A,
                ptrdiff_t* lda,
                ptrdiff_t* IPIV,
                double* WORK,
                ptrdiff_t* lwork,
                ptrdiff_t* INFO);


    // blas matrix-matrix multiplication
    void dgemm(const char *transa, 
               const char *transb,
               ptrdiff_t *N, 
               ptrdiff_t *M, 
               ptrdiff_t *K, // TODO: check the order here
               double *alpha,
               const double *A,
               ptrdiff_t *lda,
               const double *B, 
               ptrdiff_t *ldb,
               double *beta,
               double *C, 
               ptrdiff_t *ldc);

    // blas matrix-vector multiplication
    void dgemv(const char *trans, 
               const ptrdiff_t *N_rowsA, 
               const ptrdiff_t *M_colsA,
               double *alpha,
               const double *A, 
               ptrdiff_t *lda,
               const double *X, 
               ptrdiff_t *incx,
               double *beta,
               void *y, 
               ptrdiff_t *incy);

    // dot product
    double ddot(ptrdiff_t *N, 
                const double *X, 
                ptrdiff_t *incx, 
                const double *Y, 
                ptrdiff_t *incy);

    // vector
    void dscal(ptrdiff_t *N,
               const double *alpha,
               double *X,
               ptrdiff_t *incx);

    // SVD decomposition
    void dgesvd(char *jobu, 
                char *jobvt,
                ptrdiff_t *m, 
                ptrdiff_t *n,
                const double *a, 
                ptrdiff_t *lda,
                double *s,
                double *u, 
                ptrdiff_t *ldu,
                double *vt, 
                ptrdiff_t *ldvt,
                double *work, 
                ptrdiff_t *lwork, 
                ptrdiff_t *info);

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
                        bool transA = false, bool transB = false, double alpha = 1.0, double beta = 0.0)
{
    char TN = 'N';
    char TT = 'T';
    ptrdiff_t M = M_rowsA;
    ptrdiff_t N = N_colsB;
    ptrdiff_t K = K_colsA;

    // TODO: check if this is working for *nix
    dgemm(transA ? &TT : &TN,
          transB ? &TT : &TN,
          &M, &N, &K,
          &alpha,
          A, transA ? &K : &M,
          B, transB ? &N : &K,
          &beta,
          C, &M);
}


/**
 * multiply the matrix by scalar
 */
inline void matrix_scalar_mult(size_t M_rows, size_t N_cols, double *MX, double val)
{
    matrix_mult(M_rows, N_cols, 1, NULL, NULL, MX, false, false, 0, val);
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
inline void matrix_vector_mult(ptrdiff_t M_rowsA, ptrdiff_t N_colsA, const double *A, const double *X, double *out, bool transA = false, double alpha = 1.0)
{
    char TN = 'N';
    char TT = 'T';
    double beta = 0.0;

    ptrdiff_t inc = 1;

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
inline void matrix_inverse(double* A, ptrdiff_t N)
{
    ptrdiff_t *IPIV = new ptrdiff_t[N+1];
    ptrdiff_t LWORK = N*N;
    double *WORK = new double[LWORK];
    ptrdiff_t INFO;
    dgetrf(&N,&N,A,&N,IPIV,&INFO);
    dgetri(&N,A,&N,IPIV,WORK,&LWORK,&INFO);
    delete [] IPIV;
    delete [] WORK;
}


/**
 * @brief compute the dot product of two vectors
 */
inline double vector_dot(const double *A, const double *B, ptrdiff_t NA)
{
    ptrdiff_t one = 1;
    return ddot(&NA, A, &one, B, &one);
}


/**
 * @brief multiply vector by a scalar
 */
inline void vector_scalar(double *A, ptrdiff_t NA, double scalar)
{
    ptrdiff_t one = 1.0;
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
inline bool matrix_svd(const double *A, ptrdiff_t M_rows, ptrdiff_t N_cols, double* U, double* S, double * VT)
{
    size_t min_NM = std::min(M_rows, N_cols);
    size_t max_NM = std::max(M_rows, N_cols);

    ptrdiff_t ldu = M_rows;
    ptrdiff_t ldvt = N_cols;

    // this is not a magic number, see fortran docs
    ptrdiff_t lwork = 5 * max_NM;

    double *work = new double[lwork];

    ptrdiff_t info = 0;

    dgesvd("A", "A",
           &M_rows, &N_cols,
           A, &M_rows,
           S,
           U, &ldu,
           VT, &ldvt,
           work, &lwork,
           &info);
    
    delete [] work;
    
    return info == 0;
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
    // the number of singular values
    size_t min_NM = std::min(M_rows, N_cols);

    // TODO: vectors?
    std::vector<double> U(M_rows * M_rows);
    std::vector<double> S(min_NM);
    std::vector<double> VT(N_cols * N_cols);

    // the matrix in svd is overwritten for some reason
    std::vector<double> tmp(A, A + M_rows * N_cols);

    matrix_svd(tmp.data(), M_rows, N_cols, U.data(), S.data(), VT.data());
    tmp.assign(tmp.size(), 0.0);

    // transposed S matrix
    for (size_t i = 0; i < min_NM; ++i)
        if (fabs(S[i]) > EPSILON)
            tmp[(N_cols+1) * i] =  1.0 / S[i];

    std::vector<double> VS(M_rows * N_cols);

    matrix_mult(N_cols, M_rows, N_cols, VT.data(), tmp.data(), VS.data(), true);
    matrix_mult(N_cols, M_rows, M_rows, VS.data(), U.data(), out, false, true);
    
}










#endif // UTIL_H
