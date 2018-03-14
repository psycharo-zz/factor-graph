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

    if (msgX.type() == GaussianMessage::GAUSSIAN_VARIANCE)
    {
        const Matrix &meanX = msgX.mean();
        const Matrix &varX = msgX.variance();

        const Matrix &A = m_matrix;
        GaussianMessage result(A.rows());
        mult(A, meanX, result.mean());
        // A * varX * A.T()
        mult(A * varX, A.T(), result.variance());

        return result;
    }
    else if (msgX.type() == GaussianMessage::GAUSSIAN_PRECISION)
    {
        const Matrix &meanX = msgX.mean();
        Matrix varX = inv(msgX.precision());

        const Matrix &A = m_matrix;
        GaussianMessage result(A.rows(), GaussianMessage::GAUSSIAN_PRECISION);
        mult(A, meanX, result.mean());
        // A * varX * A.T()
        mult(A * varX, A.T(), result.precision());

        result.precision().inv();

        return result;
    }
}



GaussianMessage MultiplicationNode::backwardFunction(int to, const MessageBox &msgs)
{
    const GaussianMessage &msgY = msgs.at(*m_outgoing.begin());

    if (msgY.type() == GaussianMessage::GAUSSIAN_PRECISION)
    {
        const Matrix &meanY = msgY.mean();
        const Matrix &precY = msgY.precision();

        const Matrix &A = m_matrix;

        GaussianMessage result(A.cols(), GaussianMessage::GAUSSIAN_PRECISION);
        result.precision() = A.T() * precY * A;
        result.mean() = pinv(result.precision()) * A.T() * precY * meanY;
        return result;
    }
    else // if (msgY.type() == GaussianMessage::GAUSSIAN_VARIANCE)
    {
        const Matrix &meanY = msgY.mean();
        const Matrix &varY = msgY.variance();

        Matrix A = m_matrix;
        A.pinv();

        GaussianMessage result(A.rows(), GaussianMessage::GAUSSIAN_VARIANCE);

        result.mean() = A * meanY;
        result.variance() = A * varY * A.T();

        return result;
    }
}

