#include "equmultnode.h"

#include <algorithm>
#include <matrix.h>

using namespace std;



bool EquMultNode::isSupported(Message::Type type)
{
    return type == GaussianMessage::GAUSSIAN_VARIANCE;
}

GaussianMessage EquMultNode::function(int to, const MessageBox &msgs)
{
    return isForward(to) ? functionForward(to, msgs)
                         : functionBackward(to, msgs);
}


GaussianMessage EquMultNode::functionForward(int to, const MessageBox &/*msgs*/)
{
    // TODO:
    throw std::runtime_error("EquMultNode::functionForward: not implemented yet");
}



GaussianMessage EquMultNode::functionBackward(int to, const MessageBox &msgs)
{
    // y stands for message from multiplication subnode
    const GaussianMessage &msgY = msgs.at(*m_outgoing.begin());

    // x stands for the message from one of equality endpoints
    set<int>::const_iterator it = m_incoming.begin();
    const GaussianMessage &msgX = (*it != to) ? msgs.at(*it) : msgs.at(*++it);

    const Matrix &meanX = msgX.mean();
    const Matrix &varX = msgX.variance();

    const Matrix &meanY = msgY.mean();
    const Matrix &varY = msgY.variance();

    const Matrix &A = m_matrix;

    Matrix G = varY + A * varX * A.T();
    G.inv();

    GaussianMessage result(meanX.size());
    result.mean() = meanX + varX * A.T() * G * (meanY - A * meanX);
    result.variance() = varX - varX * A.T() * G * A * varX;
    return result;
}
