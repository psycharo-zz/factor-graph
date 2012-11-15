#include "equalitynode.h"


bool EqualityNode::isSupported(Message::Type type)
{
    return m_type == type;

}


GaussianMessage EqualityNode::function(int to, const MessageBox &msgs)
{

    return (m_type == GaussianMessage::GAUSSIAN_VARIANCE) ? functionVariance(to, msgs)
                                                          : functionPrecision(to, msgs);
}


GaussianMessage EqualityNode::functionVariance(int to, const MessageBox &msgs)
{
    assert(!msgs.empty());

    // TODO: assert on various sizes of messages
    size_t size = msgs.begin()->second.size();
    size_t size2 = size * size;

    GaussianMessage result(size);

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
        const int from = it->first;
        const GaussianMessage &msg = it->second;

        // skipping the message itself
        if (from == to)
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

    return result;
}


GaussianMessage EqualityNode::functionPrecision(int to, const MessageBox &msgs)
{
    size_t size = msgs.begin()->second.size();
    size_t size2 = size * size;

    GaussianMessage result(size, GaussianMessage::GAUSSIAN_PRECISION);

    vector<double> mean(size, 0.0);
    vector<double> MW(size, 0.0);

    for (auto p : msgs)
    {
        const int from = p.first;
        const GaussianMessage &msg = p.second;

        if (from == to)
            continue;

        // W+...+W
        transform(result.precision(), result.precision() + size2, msg.precision(),
                  result.precision(), std::plus<double>());

        matrix_vector_mult(size, size, msg.precision(), msg.mean(), MW.data());
        // Wm
        transform(mean.begin(), mean.end(), MW.begin(),
                  mean.begin(), std::plus<double>());
    }

    // TODO: check if the normal inverse will work
    vector<double> pinv(size * size);
    matrix_pseudo_inverse(result.precision(), size, size, pinv.data());
    matrix_vector_mult(size, size, pinv.data(), mean.data(), result.mean());

    return result;
}







