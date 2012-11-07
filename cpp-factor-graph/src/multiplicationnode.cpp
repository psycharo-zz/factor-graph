#include "multiplicationnode.h"

#include <iostream>

//! the action that this node is actually doing
GaussianMessage MultiplicationNode::function(int to, const MessageBox &msgs)
{
    assert(!msgs.empty());
    assert(m_nodes.count(to));

    // the dimensionality of the message
    size_t msg_size = msgs.begin()->second.size();

    assert(m_cols == msg_size);


    // forward
    if (isForward(to))
    {
        const GaussianMessage &msg = msgs.at(*m_incoming.begin());

        GaussianMessage result(m_rows);

        vector<double> tmp_variance(m_rows * msg_size, 0.0);

        // A * V = m_rows * m_cols
        matrix_mult(m_rows, msg_size, m_cols, m_matrix.data(), msg.variance(), tmp_variance.data());
        // A * V * A^T
        matrix_mult(m_rows, m_rows, m_cols, tmp_variance.data(), m_matrix.data(), result.variance(), false, true);

        // m_y = A * m_x
        matrix_vector_mult(m_rows, m_cols, m_matrix.data(), msg.mean(), result.mean());
        return result;
    }
    else
    {
        size_t msg_size2 = msg_size * msg_size;

        // the output size of the message
        size_t size = m_cols;
        // variance
        size_t size2 = m_cols * m_cols;

        GaussianMessage result(m_cols);

        vector<double> tmp_variance(m_cols * m_cols, 0.0);
        vector<double> tmp_mean(m_cols, 0.0);

        const GaussianMessage &msg = msgs.at(*m_outgoing.begin());

        // W_y = V_y^-1
        vector<double> tmp_inverse(msg.variance(), msg.variance() + msg_size2);
        matrix_inverse(tmp_inverse.data(), msg_size);

        // tmp = A^T W_y
        matrix_mult(m_cols, msg_size, m_rows, m_matrix.data(), tmp_inverse.data(), tmp_variance.data(), true, false);
        // W_x = A^T W_y A = tmp A
        matrix_mult(m_cols, m_cols, msg_size, tmp_variance.data(), m_matrix.data(), result.variance());
        // V_x = W_x^-1
        matrix_inverse(result.variance(), m_cols);

        // (tmp m_y) = A^T W_y m_y
        matrix_vector_mult(m_cols, msg_size, tmp_variance.data(), msg.mean(), tmp_mean.data());
        // m_x = W^-1 (A^T W_y m_y) = V (tmp m_y)
        matrix_vector_mult(m_cols, m_cols, result.variance(), tmp_mean.data(), result.mean());

        return result;
    }

}
