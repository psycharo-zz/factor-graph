#ifndef MULTIPLICATIONNODE_H
#define MULTIPLICATIONNODE_H

/**
 * @brief class MultiplicationNode represents multiplication of a message by a square(??) matrix (scalar?)
 * TODO: finish me
 */


#include "factornode.h"
#include "util.h"

class MultiplicationNode : public FactorNode
{
private:
    //! the matrix to multiply on
    vector<double> m_matrix;

    //!
    int m_rows;
    int m_cols;

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


    void setMatrix(const double *matrix, int nRows, int nCols)
    {
        m_matrix.assign(matrix, matrix + nRows * nCols);
        m_rows = nRows;
        m_cols = nCols;
    }


    void addIncoming(FactorNode *node)
    {
        assert(m_incoming.size() == 0);
        FactorNode::addIncoming(node);
    }

    void addOutgoing(FactorNode *node)
    {
        assert(m_outgoing.size() == 0);
        FactorNode::addOutgoing(node);
    }



protected:
    GaussianMessage function(int to, const MessageBox &msgs);

};

#endif // MULTIPLICATIONNODE_H
