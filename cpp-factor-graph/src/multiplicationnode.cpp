#include "multiplicationnode.h"



bool MultiplicationNode::isSupported(Message::Type type)
{
    return type == GaussianMessage::GAUSSIAN_VARIANCE ||
           type == GaussianMessage::GAUSSIAN_PRECISION;
}



//! the action that this node is actually doing
GaussianMessage MultiplicationNode::function(int to, const MessageBox &msgs)
{
    // the dimensionality of the message
    return isForward(to) ? forwardFunction(to, msgs) : backwardFunction(to, msgs);
}


GaussianMessage MultiplicationNode::forwardFunction(int to, const MessageBox &msgs)
{
    // TODO: implement for precision

    const GaussianMessage &msgX = msgs.at(*m_incoming.begin());

    Matrix meanX(msgX.mean(), msgX.size(), 1);
    Matrix varX(msgX.variance(), msgX.size(), msgX.size());

    Matrix &A = m_matrix;

    Matrix meanY = A * meanX;
    Matrix varY = A * varX * A.T();

    return GaussianMessage(meanY.data(), varY.data(), meanY.size());
}



GaussianMessage MultiplicationNode::backwardFunction(int to, const MessageBox &msgs)
{
    const GaussianMessage &msgY = msgs.at(*m_outgoing.begin());
    if (msgY.type() == GaussianMessage::GAUSSIAN_PRECISION)
    {
        Matrix meanY(msgY.mean(), msgY.size(), 1);
        Matrix precY(msgY.precision(), msgY.size(), msgY.size());

        Matrix &A = m_matrix;

        Matrix precX = A.T() * precY * A;
        Matrix meanX = precX.pinv() * A.T() * precY * meanY;

        return GaussianMessage(meanX.data(), precX.data(), meanX.size(), GaussianMessage::GAUSSIAN_PRECISION);
    }
    else if (msgY.type() == GaussianMessage::GAUSSIAN_VARIANCE)
    {
        Matrix meanY(msgY.mean(), msgY.size(), 1);
        Matrix varY(msgY.variance(), msgY.size(), msgY.size());

        Matrix A = m_matrix;
        A.inv();

        Matrix meanX = A * meanY;
        Matrix varX = A * varY * A.T();
        return GaussianMessage(meanX.data(), varX.data(), meanX.size(), GaussianMessage::GAUSSIAN_VARIANCE);

    }
}

