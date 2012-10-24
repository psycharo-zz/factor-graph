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
    {
        m_nodes.resize(2, NULL);
    }


    void addIncoming(FactorNode *node)
    {
        m_in = node;
        m_nodes[0] = m_in;
    }

    void addOutgoing(FactorNode *node)
    {
        m_out = node;
        m_nodes[1] = m_out;
    }



    /**
     * @brief setConnections
     * @param _in before multiplication
     * @param _out after multiplication
     */
    void setConnections(FactorNode *_in, FactorNode *_out)
    {
        m_in = _in;
        m_out = _out;

        m_nodes.clear();
        m_nodes.push_back(m_in);
        m_nodes.push_back(m_out);
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

        // outgoing message
        if (to == m_out->id())
        {
            const GaussianMessage &msg = msgs.at(m_in->id());
            matrix_mult(size, size, size, m_matrix.data(), msg.variance(), tmp_variance.data());
            matrix_mult(size, size, size, tmp_variance.data(), m_matrix.data(), result.variance(), false, true);
            matrix_vector_mult(size, size, m_matrix.data(), msg.mean(), result.mean());
        }
        else
        {
            const GaussianMessage &msg = msgs.at(m_out->id());

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

        return result;
    }

private:




};

#endif // MULTIPLICATIONNODE_H
