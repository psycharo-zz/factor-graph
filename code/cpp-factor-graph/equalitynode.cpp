#include "equalitynode.h"


GaussianMessage EqualityNode::function(int to, const MessageBox &msgs)
{
    assert(!msgs.empty());

    // TODO: assert on various sizes of messages
    size_t size = msgs.begin()->second.size();

    size_t size2 = size * size;

    GaussianMessage result(id(), to, size);

    // m_j size
    double *mean = result.mean();
    // V_j size2
    double *variance = result.variance();


    // W_j m_j = W_1 m_1 + ... + W_n m_n
    vector<double> tmp_sum(size, 0.0);

    // to store temporary W_i
    vector<double> tmp_precision(size2, 0.0);

    // to store temporary W_i * m_i
    vector<double> tmp_mul(size, 0.0);

    for (MessageBox::const_iterator it = msgs.begin(); it != msgs.end(); ++it)
    {
        const GaussianMessage &msg = it->second;

        // skipping the message itself
        if (msg.from() == to)
            continue;

        // tmp = V
        tmp_precision.assign(msg.variance(), msg.variance() + size2);
        // W_i = V^-1 = tmp^-1
        matrix_inverse(tmp_precision.data(), size);

        // W_j += W_i
        transform(variance, variance + size2, tmp_precision.data(),
                  variance, std::plus<double>());

        // tmp = W * m
        matrix_vector_mult(size, size, tmp_precision.data(), msg.mean(), tmp_mul.data());

        // tmp_sum += W_i m_i
        transform(tmp_sum.begin(), tmp_sum.end(), tmp_mul.begin(),
                  tmp_sum.begin(), std::plus<double>());
    }

    // V_j = W_j^-1
    matrix_inverse(variance, size);

    // m_j = V_j (W_1 m_1 + ... W_n m_n)
    matrix_vector_mult(size, size, variance, tmp_sum.data(), mean);

    // FIXME: no copying??
    return result;
}




void EqualityNode::setConnections(FactorNode *a, FactorNode *b, FactorNode *c)
{
    m_nodes.clear();
    m_nodes.push_back(a);
    m_nodes.push_back(b);
    m_nodes.push_back(c);
}





