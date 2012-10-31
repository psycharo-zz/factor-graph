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
    FactorNode *m_in;
    FactorNode *m_out;

    //! the matrix to multiply on
    vector<double> m_matrix;

public:
    MultiplicationNode(const double *_matrix, int _size):
        m_matrix(_matrix, _matrix + _size * _size)
    {}


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
    //! the action that this node is actually doing
    GaussianMessage function(int to, const MessageBox &msgs)
    {
        assert(!msgs.empty());

        size_t size = msgs.begin()->second.size();
        size_t size2 = size * size;

        GaussianMessage result(id(), to, size);


        vector<double> tmp_variance(size2, 0.0);
        vector<double> tmp_mean(size, 0.0);

        // forward
        if (isForward(to))
        {
            const GaussianMessage &msg = msgs.at(*m_incoming.begin());
            matrix_mult(size, size, size, m_matrix.data(), msg.variance(), tmp_variance.data());
            matrix_mult(size, size, size, tmp_variance.data(), m_matrix.data(), result.variance(), false, true);
            matrix_vector_mult(size, size, m_matrix.data(), msg.mean(), result.mean());
        }
        else if (isBackward(to))
        {
            const GaussianMessage &msg = msgs.at(*m_outgoing.begin());

            // W_y = V_y^-1
            vector<double> tmp_inverse(msg.variance(), msg.variance() + size2);
            matrix_inverse(tmp_inverse.data(), size2);

            // tmp = A^T W_y
            matrix_mult(size, size, size, m_matrix.data(), tmp_inverse.data(), tmp_variance.data(), true, false);
            // W_x = A^T W_y A = tmp A
            matrix_mult(size, size, size, tmp_variance.data(), m_matrix.data(), result.variance());
            // V_x = W_x^-1
            matrix_inverse(result.variance(), size);

            // (tmp m_y) = A^T W_y m_y
            matrix_vector_mult(size, size, tmp_variance.data(), msg.mean(), tmp_mean.data());
            // m_x = W^-1 (A^T W_y m_y) = V (tmp m_y)
            matrix_vector_mult(size, size, result.variance(), tmp_mean.data(), result.mean(), true);
        }
        else
            throw "MultiplicatioNode: unknown id";

        return result;
    }


};

#endif // MULTIPLICATIONNODE_H
