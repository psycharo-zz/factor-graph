#ifndef MATRIX_H
#define MATRIX_H

#include <matrixutil.h>

#include <algorithm>
#include <vector>
#include <ostream>
#include <cstring>

/**
 * VERY BASIC blas/lapack-based matrix class
 */
class Matrix
{
private:
    std::vector<double> m_data;
    size_t m_rows;
    size_t m_cols;
    bool m_transposed;


public:
    Matrix():
        m_transposed(false)
    {}

    Matrix(const double *_data, size_t _rows, size_t _cols, bool _transposed = false);

    Matrix(size_t _rows, size_t _cols, bool _transposed = false);

    //! slow but very nice for testing
    Matrix(std::initializer_list<std::vector<double> > l);

    ~Matrix() {}

    inline double *data() { return m_data.data(); }
    inline const double *data() const { return m_data.data(); }

    inline size_t rows() const { return m_rows; }
    inline size_t cols() const { return m_cols; }
    inline size_t size() const { return m_rows * m_cols; }

    inline bool transposed() const { return m_transposed; }

    //! multiplication
    // TODO: move everything to classes?
    inline Matrix operator *(const Matrix &other) const
    {
        Matrix result(rows(), other.cols());
        matrix_mult(rows(), other.cols(), cols(), data(), other.data(), result.data(), m_transposed, other.transposed());
        return result;
    }

    inline double operator()(size_t i, size_t j) const
    {
        // column-major
        return m_transposed ? m_data[i * m_cols + j] : m_data[j * m_rows + i];
    }

    inline double &operator()(size_t i, size_t j)
    {
        // column-major
        return m_transposed ? m_data[i * m_cols + j] : m_data[j * m_rows + i];
    }


    //! find inverse matrix
    inline void inv()
    {
        assert(m_rows = m_cols);
        matrix_inverse(data(), m_rows);
    }

    //! pseudo inverse
    inline Matrix pinv() const
    {
        Matrix result(cols(), rows());
        matrix_pseudo_inverse(data(), m_rows, m_cols, result.data());
        return result;
    }

    //! transpose
    inline Matrix T() const
    {
        return Matrix(m_data.data(), m_rows, m_cols, !m_transposed);
    }

    //! summation
    inline Matrix operator +(const Matrix &other) const
    {
        Matrix result(rows(), cols());

        if ((m_transposed && other.m_transposed) ||
            (!m_transposed && !other.m_transposed))
        {
            std::transform(m_data.begin(), m_data.end(), other.m_data.begin(), result.m_data.begin(), std::plus<double>());
        }
        else if ((m_transposed && !other.m_transposed ||
                 !m_transposed && other.m_transposed))
        {
            // TODO: make it more efficient?
            for (size_t i = 0; i < rows(); i++)
                for (size_t j = 0; j < cols(); j++)
                    result(i, j) = ((*this)(i, j) + other(i, j));
        }

        return result;
    }

    //! summation
    inline Matrix operator -(const Matrix &other) const
    {
        Matrix result(rows(), cols());

        if ((m_transposed && other.m_transposed) ||
            (!m_transposed && !other.m_transposed))
        {
            std::transform(m_data.begin(), m_data.end(), other.m_data.begin(), result.m_data.begin(), std::minus<double>());
        }
        else if ((m_transposed && !other.m_transposed ||
                 !m_transposed && other.m_transposed))
        {
            // TODO: make it more efficient?
            for (size_t i = 0; i < rows(); i++)
                for (size_t j = 0; j < cols(); j++)
                    result(i, j) = ((*this)(i, j) - other(i, j));
        }
        return result;
    }

};



inline Matrix::Matrix(std::initializer_list<std::vector<double> > l):
    m_transposed(false)
{
    assert(l.size() > 0);
    m_rows = l.size();
    m_cols = l.begin()->size();
    m_data.resize(m_rows * m_cols);

    size_t i = 0;
    for (const std::vector<double> &vec : l)
    {
        assert(vec.size() == m_cols);
        for (size_t j = 0; j < m_cols; j++)
            this->operator ()(i, j) = vec.at(j);
        ++i;
    }
}


inline Matrix::Matrix(const double *_data, size_t _rows, size_t _cols, bool _transposed):
    m_rows(_rows),
    m_cols(_cols),
    m_transposed(_transposed)
{
    if (m_transposed)
        std::swap(m_rows, m_cols);
    m_data.assign(_data, _data + _rows * _cols);
}

inline Matrix::Matrix(size_t _rows, size_t _cols, bool _transposed):
    m_rows(_rows),
    m_cols(_cols),
    m_transposed(_transposed)
{
    if (m_transposed)
        std::swap(m_rows, m_cols);
    m_data.resize(_rows * _cols, 0.0);
}



inline Matrix eye(int M, int N)
{
    Matrix result(M, N);
    for (size_t i = 0; i < std::min(M, N); i++)
        result(i, i) = 1.0;
    return result;
}



inline std::ostream& operator <<(std::ostream &os, const Matrix &a)
{
    for (int i = 0; i < a.rows(); i++)
    {
        for (int j = 0; j < a.cols(); j++)
            os << a(i, j) << "\t";
        os << "\n";
    }
    return os;
}



#endif // MATRIX_H
