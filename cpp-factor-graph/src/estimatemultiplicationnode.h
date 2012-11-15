#ifndef ESTIMATEDMULTIPLICATIONNODE_H
#define ESTIMATEDMULTIPLICATIONNODE_H

#include "multiplicationnode.h"

/**
 * @brief The EstimateMultiplicationNode class
 * Table 5.1 [Korl2005]
 */
class EstimateMultiplicationNode : public MultiplicationNode
{
public:
    static const char *ESTIMATED_TAG;

    virtual ~EstimateMultiplicationNode() {}

    inline void setParam(const double *v, int nSize)
    {
        m_matrix = Matrix(nSize, nSize);

        for (size_t i = 1; i < size() * size() - size(); i += (size()+1))
            m_matrix.data()[i] = 1;
        for (size_t i = 0; i < size(); i++)
            m_matrix.data()[size() * i] = v[i];
    }


    inline size_t size() const
    {
        return m_matrix.rows();
    }

    //! @overload
    void receive(int from, const GaussianMessage &msg);

    //! @overload
    bool isSupported(Message::Type type);


protected:
    //! @overload
    GaussianMessage function(int to, const MessageBox &msgs);

};

#endif // ESTIMATEDMULTIPLICATIONNODE_H
