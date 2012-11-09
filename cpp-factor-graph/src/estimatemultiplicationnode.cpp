#include "estimatemultiplicationnode.h"

#include "matrixutil.h"

#include <algorithm>
#include <functional>
using namespace std;
using namespace placeholders;


const char *EstimateMultiplicationNode::ESTIMATED_TAG = "estimated";


bool EstimateMultiplicationNode::isSupported(Message::Type type)
{
    // TODO: support for GAUSSIAN_PRECISION
    return type == GaussianMessage::GAUSSIAN_VARIANCE;
}


void EstimateMultiplicationNode::receive(int from, const GaussianMessage &msg)
{
    assert(isForward(from) || isBackward(from) || isConnection(from, ESTIMATED_TAG));

    if (isConnection(from, ESTIMATED_TAG))
    {
        // filling matrix
        m_size = msg.size();
        m_matrix.assign(msg.size2(), 0);

        // for (size_t i = 1; i < m_size * (m_size-1); i += m_size)
        // m_matrix[i] = 1;
        for (size_t i = 1; i < m_size-1; i++)
            m_matrix[(i-1)*m_size + (i+1)] = 1;
        // column-major :(
        for (size_t i = 0; i < m_size * m_size; i += m_size)
            m_matrix[i] = msg.mean()[0];
    }
    else
        FactorNode::receive(from, msg);
}



GaussianMessage EstimateMultiplicationNode::function(int to, const MessageBox &msgs)
{

    assert(!msgs.empty());
    assert(m_nodes.count(to));

    // the dimensionality of the message
    size_t msg_size = msgs.begin()->second.size();

    assert(m_size == msg_size);

    // forward
    if (isForward(to))
    {
        const GaussianMessage &msg = msgs.at(*m_incoming.begin());

        GaussianMessage result(m_size);

        vector<double> tmp_variance(m_size * m_size, 0.0);

        // A * V = m_rows * m_cols
        matrix_mult(m_size, m_size, m_size, m_matrix.data(), msg.variance(), tmp_variance.data());
        // A * V * A^T
        matrix_mult(m_size, m_size, m_size, tmp_variance.data(), m_matrix.data(), result.variance(), false, true);

        // m_y = A * m_x
        matrix_vector_mult(m_size, m_size, m_matrix.data(), msg.mean(), result.mean());
        return result;
    }
    else if (isBackward(to))
    {
        GaussianMessage result(m_size);

        vector<double> tmp_variance(m_size * m_size, 0.0);
        vector<double> tmp_mean(m_size, 0.0);

        const GaussianMessage &msg = msgs.at(*m_outgoing.begin());

        // W_y = V_y^-1
        vector<double> tmp_inverse(msg.variance(), msg.variance() + m_size * m_size);
        matrix_inverse(tmp_inverse.data(), m_size);

        // tmp = A^T W_y
        matrix_mult(m_size, m_size, m_size, m_matrix.data(), tmp_inverse.data(), tmp_variance.data(), true, false);
        // W_x = A^T W_y A = tmp A
        matrix_mult(m_size, m_size, m_size, tmp_variance.data(), m_matrix.data(), result.variance());
        // V_x = W_x^-1
        matrix_inverse(result.variance(), m_size);

        // (tmp m_y) = A^T W_y m_y
        matrix_vector_mult(m_size, m_size, tmp_variance.data(), msg.mean(), tmp_mean.data());
        // m_x = W^-1 (A^T W_y m_y) = V (tmp m_y)
        matrix_vector_mult(m_size, m_size, result.variance(), tmp_mean.data(), result.mean());

        return result;
    }
    else // if (isEstimated(to))
    {
        GaussianMessage result(m_size, GaussianMessage::GAUSSIAN_PRECISION);

        const GaussianMessage &inMsg = msgs.at(*m_incoming.begin());
        const GaussianMessage &outMsg = msgs.at(*m_outgoing.begin());

        // m_a = m_y[1] * m_x / ||m_x||^2
        double scalar = outMsg.mean()[0] / vector_dot(inMsg.mean(), inMsg.mean(), inMsg.size());
        transform(inMsg.mean(), inMsg.mean() + inMsg.size(), result.mean(),
                  bind(multiplies<double>(), scalar, _1));

        // W_a = m_x * m_x^T / sd_y2
        matrix_mult(inMsg.size(), inMsg.size(), 1,
                    inMsg.mean(), inMsg.mean(),
                    result.precision(),
                    false, false,
                    1.0 / outMsg.variance()[0]);
        return result;
    }


}
