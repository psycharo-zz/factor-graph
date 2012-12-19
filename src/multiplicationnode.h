#ifndef MULTIPLICATIONNODE_H
#define MULTIPLICATIONNODE_H

/**
 * @brief class MultiplicationNode represents multiplication of a message by a square(??) matrix (scalar?)
 * TODO: finish me
 */


#include "factornode.h"
#include "matrix.h"

class MultiplicationNode : public FactorNode
{
public:
    MultiplicationNode()
    {}

    MultiplicationNode(const double *matrix, int nRows, int nCols):
        m_matrix(matrix, nRows, nCols)
    {
    }


    virtual ~MultiplicationNode() {}


    /**
     * @brief setMatrix set the matrix to use
     * @param matrix
     * @param nRows
     * @param nCols
     */
    void setMatrix(const double *matrix, int nRows, int nCols)
    {
        m_matrix = Matrix(matrix, nRows, nCols);
    }


    void setMatrix(const Matrix &mx)
    {
        m_matrix = mx;
    }

    /**
     * get the data pointer to node's matrix
     */
    const double *matrix() const
    {
        return m_matrix.data();
    }


    //! @overload
    void addIncoming(FactorNode *node)
    {
        if (m_incoming.size() != 0)
            throw std::runtime_error("MultiplicationNode(" + id_to_string(id()) + ")::addIncoming: already have an incoming node");
        FactorNode::addIncoming(node);
    }

    //! @overload
    void addOutgoing(FactorNode *node)
    {
        if (m_outgoing.size() != 0)
            throw std::runtime_error("MultiplicationNode(" + id_to_string(id()) + ")::addOutgoing: already have an outgoing node");
        FactorNode::addOutgoing(node);
    }

    //! @overload
    virtual bool isSupported(Message::Type type);

protected:
    virtual GaussianMessage function(int to, const MessageBox &msgs);

    virtual GaussianMessage forwardFunction(int to, const MessageBox &msgs);
    virtual GaussianMessage backwardFunction(int to, const MessageBox &msgs);


    //! the matrix to multiply on
    Matrix m_matrix;

};

#endif // MULTIPLICATIONNODE_H
