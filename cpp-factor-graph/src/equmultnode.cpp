#include "equmultnode.h"


#include <algorithm>
#include "matrix.h"

bool EquMultNode::isSupported(Message::Type type)
{
    return type == GaussianMessage::GAUSSIAN_VARIANCE;
}

GaussianMessage EquMultNode::function(int to, const MessageBox &msgs)
{
    return isForward(to) ? functionForward(to, msgs)
                         : functionBackward(to, msgs);
}


GaussianMessage EquMultNode::functionForward(int to, const MessageBox &msgs)
{
    // TODO:
    throw Exception("EquMultNode::functionForward: not implemented yet");
}



GaussianMessage EquMultNode::functionBackward(int to, const MessageBox &msgs)
{
    // y stands for message from multiplication subnode
    const GaussianMessage &msgY = msgs.at(*m_outgoing.begin());

    // x stands for the message from one of equality endpoints
    set<int>::iterator it = m_incoming.begin();
    const GaussianMessage &msgX = (*it != to) ? msgs.at(*it) : msgs.at(*++it);

    Matrix meanX(msgX.mean(), msgX.size(), 1);
    Matrix varX(msgX.variance(), msgX.size(), msgX.size());

    Matrix meanY(msgY.mean(), msgY.size(), 1);
    Matrix varY(msgY.variance(), msgY.size(), msgY.size());

    Matrix &A = m_matrix;

    Matrix G = varY + A * varX * A.T();
    G.inv();
    Matrix mean = meanX + varX * A.T() * G * (meanY - A * meanX);
    Matrix var = varX - varX * A.T() * G * A * varX;

    return GaussianMessage(mean.data(), var.data(), mean.size());
}
