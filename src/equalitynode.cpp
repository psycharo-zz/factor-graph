#include "equalitynode.h"

using namespace std;



bool EqualityNode::isSupported(Message::Type type)
{
    return type == GaussianMessage::GAUSSIAN_VARIANCE ||
           type == GaussianMessage::GAUSSIAN_PRECISION;

}


GaussianMessage EqualityNode::function(int to, const MessageBox &msgs)
{
    GaussianMessage::Type msgType = msgs.begin()->second.type();
    return (msgType == GaussianMessage::GAUSSIAN_VARIANCE) ? functionVariance(to, msgs)
                                                           : functionPrecision(to, msgs);
}


GaussianMessage EqualityNode::functionVariance(int to, const MessageBox &msgs)
{
    if (msgs.empty())
        throw std::runtime_error("EqualityNode::functionVariance(): no messages");

    // TODO: assert on various sizes of messages
    size_t size = msgs.begin()->second.size();

    GaussianMessage result(size);

    Matrix &mean = result.mean();
    Matrix &variance = result.variance();

    Matrix sum(size, 1);
    for (MessageBox::const_iterator it = msgs.begin(); it != msgs.end(); ++it)
    {
        const int from = it->first;
        const GaussianMessage &msg = it->second;

        // skipping the message itself
        if (from == to)
            continue;

        const Matrix &msgMean = msg.mean();
        const Matrix &msgVar = msg.variance();

        // W_i = V^-1 = tmp^-1
        Matrix msgPrec = inv(msgVar);
        // W_j += W_i
        variance += msgPrec;
        // tmp_sum += W_i m_i
        sum += msgPrec * msgMean;
    }

    // V_j = W_j^-1
    variance.inv();

    // TODO: mult(variance, sum, out)
    mean = variance * sum;

    return result;
}


GaussianMessage EqualityNode::functionPrecision(int to, const MessageBox &msgs)
{
    size_t size = msgs.begin()->second.size();

    GaussianMessage result(size, GaussianMessage::GAUSSIAN_PRECISION);

    Matrix &mean = result.mean();
    Matrix &prec = result.precision();

    for (MessageBox::const_iterator it = msgs.begin(); it != msgs.end(); ++it)
    {
        const int from = it->first;
        const GaussianMessage &msg = it->second;

        if (from == to)
            continue;

        const Matrix &msgMean = msg.mean();
        const Matrix &msgPrec = msg.precision();

        prec += msgPrec;
        mean += msgPrec * msgMean;
    }


    // TODO: mult(in1, in2, out)
    mean = pinv(prec) * mean;

    return result;
}







