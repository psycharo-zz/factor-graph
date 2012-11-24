#ifndef MESSAGE_H
#define MESSAGE_H


#include <vector>
#include <map>
#include <cstring>
#include <cassert>
#include <exception>
#include <string>
#include <matrix.h>
#include <stdexcept>


class Exception : public std::runtime_error
{
public:
    Exception(const std::string &msg): std::runtime_error(msg)
    {}
};




/**
 * @brief The Message class
 * for now it is a 1-dim gaussian
 */
class Message
{
public:
    /**
     * @brief The Type enum describes all possible message type
     */
    enum Type
    {
        GAUSSIAN_VARIANCE = 1,
        GAUSSIAN_PRECISION = 2,
        CUSTOM, // for possible extensions without cpp code modifications
        UNKNOWN
    };


    static std::string typeName(Type type)
    {
        if (type == GAUSSIAN_VARIANCE)
            return "VARIANCE";
        else if (type == GAUSSIAN_PRECISION)
            return "PRECISION";
        else
            return "UNKNOWN";
    }

    static Type typeByName(const std::string &name)
    {
        // TODO
        if (name == "VARIANCE")
            return GAUSSIAN_VARIANCE;
        else if (name == "PRECISION")
            return GAUSSIAN_PRECISION;
        else
            return UNKNOWN;
    }


    /**
     * @brief UNDEFINED_ID for sources with undefined identifier
     */
    static const int UNDEFINED_ID;

    /**
     * @brief Message
     * @param type
     * @param from_id
     * @param to_id
     * @param data
     * @param n
     */
    Message(Type _type):
        m_type(_type)

    {}

    //! get the type of the message
    inline Type type() const
    {
        return m_type;
    }



private:
    //! the type of the message
    Type m_type;
};




/**
 * @brief gaussian message with variance
 */
class VarianceMessage : public Message
{
public:
    VarianceMessage():
        Message(GAUSSIAN_VARIANCE)
    {}

    VarianceMessage(const double *_mean, const double *_variance, size_t _size):
        Message(GAUSSIAN_VARIANCE),
        m_mean(_mean, _size, 1),
        m_variance(_variance, _size, _size),
        m_size(_size)
    {}

    inline Matrix &mean()
    {
        return m_mean;
    }

    inline Matrix &variance()
    {
        return m_variance;
    }


    inline size_t size() const
    {
        return m_size;
    }

    inline size_t size2() const
    {
        return m_size * m_size;
    }

private:
    Matrix m_mean;
    Matrix m_variance;
    size_t m_size;
};


/**
 * @brief gaussian message with precision matrix
 */
class PrecisionMessage : public Message
{
public:
    PrecisionMessage():
        Message(GAUSSIAN_PRECISION)
    {}

    PrecisionMessage(const double *_mean, const double *_precision, size_t _size):
        Message(GAUSSIAN_PRECISION),
        m_mean(_mean, _size, 1),
        m_precision(_precision, _size, _size),
        m_size(_size)
    {}

    inline Matrix &mean()
    {
        return m_mean;
    }

    inline Matrix &precision()
    {
        return m_precision;
    }


    inline size_t size() const
    {
        return m_size;
    }

    inline size_t size2() const
    {
        return m_size * m_size;
    }

private:
    Matrix m_mean;
    Matrix m_precision;
    size_t m_size;
};




/**
 * TODO: eliminate this completely?
 * @brief The GaussianMessage class, represent multivariate gaussian,
 * consists of mean vector and precision matrix
 */
class GaussianMessage : public Message
{
private:
    size_t m_size;

    std::vector<double> m_mean;

    //! data for variance/precision
    std::vector<double> m_data;

public:
    /**
     * @brief construct a gaussian (mean, variance) message
     * @param _median - mean vector
     * @param _variance V - variance matrix (NxN)
     * @param _size N - # of dimensions
     */
    GaussianMessage(const double *_mean, const double *_variance, size_t _size, Message::Type _type = GAUSSIAN_VARIANCE):
        Message(_type),
        m_size(_size)
    {
        m_mean.assign(_mean, _mean + _size);
        m_data.assign(_variance, _variance + _size * _size);
    }


    /**
     * @brief construct an empty GaussianMessage of _size
     * @param _size the dimensionality
     * @param _type the type of the message, can be either of GAUSSIAN_VARIANCE or GAUSSIAN_PRECISION
     */
    GaussianMessage(size_t _size, Message::Type _type = GAUSSIAN_VARIANCE):
        Message(_type),
        m_size(_size)
    {
        assert(_type == GAUSSIAN_VARIANCE || _type == GAUSSIAN_PRECISION);
        m_mean.resize(size(), 0.0);
        m_data.resize(size2(), 0.0);
    }



    /**
     * @return pointer to the median vector
     */
    inline const double *mean() const
    {
        return m_mean.data();
    }

    /**
     * @return pointer to the median vector
     */
    inline double *mean()
    {
        return m_mean.data();
    }


    /**
     * @brief pointer to the precision matrix
     */
    inline const double *variance() const
    {
        if (type() != GAUSSIAN_VARIANCE)
            throw Exception("GaussianMessage::variance: not defined for this type");
        return m_data.data();
    }

    /**
     * @brief pointer to the variance matrix
     * NOTE: only works for type() == GAUSSIAN_VARIANCE
     */
    inline double *variance()
    {
        if (type() != GAUSSIAN_VARIANCE)
            throw Exception("GaussianMessage::variance: not defined for this type");
        return m_data.data();
    }

    /**
     * @return pointer to the precision matrix
     * NOTE: only works for type() == GAUSSIAN_PRECISION
     * TODO: INHERITANCE
     */
    inline double *precision()
    {
        if (type() != GAUSSIAN_PRECISION)
            throw Exception("GaussianMessage::precision: not defined for this type");
        return m_data.data();
    }

    /**
     * @return pointer to the precision matrix
     * NOTE: only works for type() == GAUSSIAN_PRECISION
     * TODO: INHERITANCE
     */
    inline const double *precision() const
    {
        if (type() != GAUSSIAN_PRECISION)
            throw Exception("GaussianMessage::precision: not defined for this type");
        return m_data.data();
    }



    /**
     * @return # of dimensions of the message
     */
    inline size_t size() const
    {
        return m_size;
    }

    /**
     * @return the size of variance matrix
     */
    inline size_t size2() const
    {
        return m_size * m_size;
    }


};




typedef std::map<int, GaussianMessage> MessageBox;

#include <ostream>

/**
 * output for GaussianMessage
 */
std::ostream& operator <<(std::ostream &os, const GaussianMessage &msg);




#endif // MESSAGE_H
