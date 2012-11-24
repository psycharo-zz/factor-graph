#ifndef MATRIX_H
#define MATRIX_H

#include <matrixutil.h>

#include <algorithm>
#include <vector>
#include <ostream>
#include <cstring>
#include <stdexcept>


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

    /**
     * @brief constructor
     * @param _data
     * @param _rows
     * @param _cols
     * @param _transposed whether the matrix is a transposed one or not
     */
    inline Matrix(const double *_data, size_t _rows, size_t _cols, bool _transposed = false):
        m_rows(_rows),
        m_cols(_cols),
        m_transposed(_transposed)
    {
        m_data.assign(_data, _data + _rows * _cols);
    }

    /**
     * @brief Matrix
     * @param _rows
     * @param _cols
     * @param _transposed
     */
    inline Matrix(size_t _rows, size_t _cols, bool _transposed = false):
        m_rows(_rows),
        m_cols(_cols),
        m_transposed(_transposed)
    {
        m_data.resize(_rows * _cols, 0.0);
    }

#if __cplusplus >= 201103L
    /**
     * @brief
     * @param l
     */
    inline Matrix(std::initializer_list<std::vector<double> > l):
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
#endif


    inline double *data() { return m_data.data(); }
    inline const double *data() const { return m_data.data(); }

    inline size_t rows() const { return m_rows; }
    inline size_t cols() const { return m_cols; }
    inline size_t size() const { return m_rows * m_cols; }

    inline bool transposed() const { return m_transposed; }

    /**
     * @brief matrix multiplication
     * @param other
     * @return
     */
    inline Matrix operator *(const Matrix &other) const
    {
        if (m_cols != other.m_rows)
            throw std::runtime_error("Matrix::operator*(): dimensionalities do not conform");


        Matrix result(rows(), other.cols());
        matrix_mult(rows(), other.cols(), cols(), data(), other.data(), result.data(), m_transposed, other.transposed());
        return result;
    }

    /**
     * @brief element access
     */
    inline double operator()(size_t i, size_t j) const
    {
        // column-major
        return m_transposed ? data()[i * m_cols + j] : data()[j * m_rows + i];
    }

    /**
     * @brief element access, non-constant
     */
    inline double &operator()(size_t i, size_t j)
    {
        // column-major
        return m_transposed ? data()[i * m_cols + j] : data()[j * m_rows + i];
    }


    /**
     * @brief inv get an inversed matrix
     */
    inline void inv()
    {
        if (m_rows != m_cols)
            throw std::runtime_error("Matrix::inv(): dimensionalities do not conform.");
        matrix_inverse(data(), m_rows);
    }

    /**
     * @brief pinv Moore-Penrouse pseudoinverse
     */
    inline void pinv()
    {
        matrix_pseudo_inverse(data(), m_rows, m_cols, data());
        std::swap(m_rows, m_cols);
    }

    /**
     * @brief get a transposed matrix
     */
    inline Matrix T() const
    {
        return Matrix(data(), cols(), rows(), !transposed());
    }

    /**
     * @brief matrix addition
     */
    inline Matrix operator +(const Matrix &other) const
    {
        if (m_rows != other.m_rows || m_cols != other.cols())
            throw std::runtime_error("Matrix::operator+: dimensionalities do not conform");

        Matrix result(rows(), cols(), (m_transposed && other.m_transposed));

        if ((m_transposed && other.m_transposed) ||
            (!m_transposed && !other.m_transposed))
        {
            std::transform(data(), data() + size(), other.data(), result.data(), std::plus<double>());
        }
        else if (m_transposed && !other.m_transposed)
        {
            // TODO: make it more efficient?
            for (size_t i = 0; i < m_rows; i++)
                for (size_t j = 0; j < m_cols; j++)
                    result.data()[j * m_rows + i] = data()[i * m_cols + j] + other.data()[j * m_rows + i];
        }
        else // if (!m_transposed && other.m_transposed)
        {
            for (size_t i = 0; i < m_rows; i++)
                for (size_t j = 0; j < m_cols; j++)
                    result.data()[j * m_rows + i] = other.data()[i * m_cols + j] + data()[j * m_rows + i];
        }
        return result;
    }

    /**
     * @brief matrix addition, no new matrix created
     */
    inline Matrix &operator +=(const Matrix &other)
    {
        if (m_rows != other.m_rows || m_cols != other.cols())
            throw std::runtime_error("Matrix::operator+(): dimensionalities do not conform");

        if ((m_transposed && other.m_transposed) || (!m_transposed && !other.m_transposed))
            std::transform(data(), data() + size(), other.data(), data(), std::plus<double>());
        else if (m_transposed && !other.m_transposed)
        {
            for (size_t i = 0; i < m_rows; i++)
                for (size_t j = 0; j < m_cols; j++)
                    data()[j * m_rows + i] += other.data()[j * m_rows + i];
        }
        else // if (!m_transposed && other.m_transposed)
        {
            for (size_t i = 0; i < m_rows; i++)
                for (size_t j = 0; j < m_cols; j++)
                    data()[j * m_rows + i] += other.data()[i * m_cols + j];
        }
        return *this;
    }



    /**
     * @brief matrix subtraction
     */
    inline Matrix operator -(const Matrix &other) const
    {
        if (m_rows != other.m_rows || m_cols != other.cols())
            throw std::runtime_error("Matrix::operator-: dimensionalities do not conform");


        Matrix result(m_rows, m_cols, m_transposed && other.m_transposed);

        if ((m_transposed && other.m_transposed) ||
            (!m_transposed && !other.m_transposed))
            std::transform(data(), data() + size(), other.data(), result.data(), std::minus<double>());
        else if (m_transposed && !other.m_transposed)
        {
            // TODO: make it more efficient?
            for (size_t i = 0; i < m_rows; i++)
                for (size_t j = 0; j < m_cols; j++)
                    result.data()[j * m_rows + i] = data()[i * m_cols + j] - other.data()[j * m_rows + i];
        }
        else // if (!m_transposed && other.m_transposed)
        {
            for (size_t i = 0; i < m_rows; i++)
                for (size_t j = 0; j < m_cols; j++)
                    result.data()[j * m_rows + i] = data()[j * m_rows + i] - other.data()[i * m_cols + j];
        }

        return result;
    }

    /**
     * @brief matrix subtruction, no new matrix created
     */
    inline Matrix &operator -=(const Matrix &other)
    {
        if (m_rows != other.m_rows || m_cols != other.cols())
            throw std::runtime_error("Matrix::operator-=(): dimensionalities do not conform");


        if ((m_transposed && other.m_transposed) || (!m_transposed && !other.m_transposed))
            std::transform(data(), data() + size(), other.data(), data(), std::minus<double>());
        else if (m_transposed && !other.m_transposed)
        {
            // TODO: make it more efficient?
            for (size_t i = 0; i < m_rows; i++)
                for (size_t j = 0; j < m_cols * m_rows; j += m_rows)
                    data()[j + i] -= other.data()[j + i];
        }
        else // if (!m_transposed && other.m_transposed)
        {
            for (size_t i = 0; i < m_rows; i++)
                for (size_t j = 0; j < m_cols; j++)
                    data()[j * m_rows + i] -= other.data()[i * m_cols + j];
        }

        return *this;
    }


};




inline Matrix eye(size_t M, size_t N)
{
    Matrix result(M, N);
    for (size_t i = 0; i < std::min(M, N); i++)
        result(i, i) = 1.0;
    return result;
}


inline Matrix inv(const Matrix &mx)
{
    // TODO: switch to exceptions?
    if (mx.cols() != mx.rows())
        throw std::runtime_error("inv(): dimensionalities do not conform");
    Matrix result(mx.data(), mx.rows(), mx.cols());
    matrix_inverse(result.data(), result.rows());
    return result;
}


inline Matrix pinv(const Matrix &mx)
{
    Matrix result(mx.data(), mx.cols(), mx.rows(), mx.transposed());
    matrix_pseudo_inverse(mx.data(), mx.rows(), mx.cols(), result.data());
    return result;
}




inline std::ostream& operator <<(std::ostream &os, const Matrix &a)
{
    for (size_t i = 0; i < a.rows(); i++)
    {
        for (size_t j = 0; j < a.cols(); j++)
            os << a(i, j) << "\t";
        os << "\n";
    }
    return os;
}



#endif // MATRIX_H
