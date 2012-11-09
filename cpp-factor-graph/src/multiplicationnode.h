#ifndef MULTIPLICATIONNODE_H
#define MULTIPLICATIONNODE_H

/**
 * @brief class MultiplicationNode represents multiplication of a message by a square(??) matrix (scalar?)
 * TODO: finish me
 */


#include "factornode.h"
#include "matrixutil.h"

class MultiplicationNode : public FactorNode
{
public:
    MultiplicationNode(const double *matrix, int nRows, int nCols):
        m_matrix(matrix, matrix + nRows * nCols),
        m_rows(nRows),
        m_cols(nCols)
    {
        // for now only square matrices are supported
        assert(m_rows == m_cols);
    }

    MultiplicationNode():
        m_rows(0),
        m_cols(0)
    {}

    virtual ~MultiplicationNode() {}


    /**
     * @brief setMatrix set the matrix to use
     * @param matrix
     * @param nRows
     * @param nCols
     */
    void setMatrix(const double *matrix, int nRows, int nCols)
    {
        m_matrix.assign(matrix, matrix + nRows * nCols);
        m_rows = nRows;
        m_cols = nCols;
    }


    //! @overload
    void addIncoming(FactorNode *node)
    {
        assert(m_incoming.size() == 0);
        FactorNode::addIncoming(node);
    }

    //! @overload
    void addOutgoing(FactorNode *node)
    {
        assert(m_outgoing.size() == 0);
        FactorNode::addOutgoing(node);
    }

    //! @overload
    bool isSupported(Message::Type type)
    {
        return type == GaussianMessage::GAUSSIAN_VARIANCE;
    }


protected:
    GaussianMessage function(int to, const MessageBox &msgs);

    //! the matrix to multiply on
    vector<double> m_matrix;

    //!
    size_t m_rows;
    size_t m_cols;

};

#endif // MULTIPLICATIONNODE_H
