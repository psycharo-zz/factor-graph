#ifndef ESTIMATEDMULTIPLICATIONNODE_H
#define ESTIMATEDMULTIPLICATIONNODE_H

#include "factornode.h"

/**
 * @brief The EstimateMultiplicationNode class
 * Table 5.1 [Korl2005]
 */


class EstimateMultiplicationNode : public FactorNode
{
public:
    static const char *ESTIMATED_TAG;

    EstimateMultiplicationNode():
        m_size(0)
    {}

    virtual ~EstimateMultiplicationNode() {}

    /**
     * @brief setMatrix set the matrix to use directly
     * @param matrix
     * @param nSize
     */
    void setMatrix(const double *matrix, int nSize)
    {
        m_size = nSize;
        m_matrix.assign(matrix, matrix + nSize * nSize);
    }

    //! @overload
    void receive(int from, const GaussianMessage &msg);

    //! @overload
    bool isSupported(Message::Type type);


protected:
    //! @overload
    GaussianMessage function(int to, const MessageBox &msgs);

    //! the matrix to multiply on
    vector<double> m_matrix;

    //! matrix size
    size_t m_size;
};

#endif // ESTIMATEDMULTIPLICATIONNODE_H
