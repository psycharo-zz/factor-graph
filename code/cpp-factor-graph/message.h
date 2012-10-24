#ifndef MESSAGE_H
#define MESSAGE_H


#include <vector>
#include <map>
#include <cstring>


using namespace std;


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
        DISCRETE = 0,
        GAUSSIAN = 1,
        CUSTOM, // for possible extensions without cpp code modifications
        UNKNOWN
    };


    /**
     * @brief UNDEFINED_ID for sources with undefined identifier
     */
    static const int UNDEFINED_ID = -1;

    /**
     * @brief Message
     * @param type
     * @param from_id
     * @param to_id
     * @param data
     * @param n
     */
    Message(Type _type, int _from, int _to):
        m_from(_from),
        m_to(_to),
        m_type(_type)

    {}

    //! get the type of the message
    inline Type type() const
    {
        return m_type;
    }


    //! @return the sender of the message
    inline int from() const
    {
        return m_from;
    }

    //! @return the receipient of the message
    inline int to() const
    {
        return m_to;
    }

    inline bool operator<(const Message &other) const
    {
        return m_from < other.m_from;
    }


protected:
    //! the source node of the message
    int m_from;

    //! the destination node of the message
    int m_to;

private:
    //! the type of the message
    Type m_type;
};






/**
 * @brief The GaussianMessage class, represent multivariate gaussian,
 * consists of mean vector and precision matrix
 */
class GaussianMessage : public Message
{
private:
    size_t m_size;

    vector<double> m_mean;

    vector<double> m_variance;

public:
    /**
     * @brief GaussianMessage
     * @param _median - mean vector
     * @param _precision W - precision matrix (NxN)
     * @param _size N - # of dimensions
     */
    GaussianMessage(int from, int to, const double *_median, const double *_variance, size_t _size):
        Message(GAUSSIAN, from, to),
        m_size(_size)
    {
        m_mean.assign(_median, _median + _size);
        m_variance.assign(_variance, _variance + _size * _size);
    }


    /**
     * @brief construct an empty GaussianMessage of _size
     * @param from
     * @param to
     * @param _size
     */
    GaussianMessage(int from, int to, size_t _size):
        Message(GAUSSIAN, from, to),
        m_size(_size)
    {
        m_mean.resize(size(), 0.0);
        m_variance.resize(size2(), 0.0);
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
     * @return pointer to the precision matrix
     */
    inline const double *variance() const
    {
        return m_variance.data();
    }

    /**
     * @return pointer to the precision matrix
     */
    inline double *variance()
    {
        return m_variance.data();
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




typedef map<int, GaussianMessage> MessageBox;


#include <ostream>
/**
 * ostream for GaussianMessage
 */
ostream& operator <<(ostream &os, const GaussianMessage &msg);




#endif // MESSAGE_H
