#include "addnode.h"

#include "matrix.h"


using namespace std;

bool AddNode::isSupported(Message::Type type)
{
    return type == GaussianMessage::GAUSSIAN_VARIANCE ||
           type == GaussianMessage::GAUSSIAN_PRECISION;
}


void AddNode::addOutgoing(FactorNode *node)
{
    if (m_outgoing.size() != 0)
        throw std::runtime_error("AddNode(" + id_to_string(id()) + ")::addOutgoing(): only one outgoing node is supported");

    FactorNode::addOutgoing(node);
}


GaussianMessage AddNode::function(int to, const MessageBox &msgs)
{
    if (msgs.empty() || m_outgoing.size() != 1)
        throw std::runtime_error("AddNode(" + id_to_string(id()) + ")::function(): no messages or network not configured");
    return isForward(to) ? forwardFunction(to, msgs) : backwardFunction(to, msgs);
}




GaussianMessage AddNode::forwardFunction(int to, const MessageBox &msgs)
{
    GaussianMessage::Type msgType = msgs.begin()->second.type();

    return (msgType == GaussianMessage::GAUSSIAN_VARIANCE) ? forwardFunctionVariance(to, msgs)
                                                           : forwardFunctionPrecision(to, msgs);
}


GaussianMessage AddNode::forwardFunctionVariance(int to, const MessageBox &msgs)
{
    size_t size = msgs.begin()->second.size();
    GaussianMessage result(size);

    Matrix &mean = result.mean();
    Matrix &variance = result.variance();
    for (set<int>::iterator it = m_incoming.begin(); it != m_incoming.end(); ++it)
    {
        const GaussianMessage &msg = msgs.at(*it);
        mean += msg.mean();
        variance += msg.variance();
    }

    return result;
}

GaussianMessage AddNode::forwardFunctionPrecision(int to, const MessageBox &msgs)
{
    size_t size = msgs.begin()->second.size();

    if (m_incoming.size() != 2)
        throw std::runtime_error("AddNode(" + id_to_string(id()) + ")::forwardFunction(): only two incoming edges are supported");

    GaussianMessage result(size, GaussianMessage::GAUSSIAN_PRECISION);

    Matrix &mean = result.mean();
    Matrix &precision = result.precision();

    // TODO: switch to named field instead?
    set<int>::iterator it = m_incoming.begin();
    const GaussianMessage &msgX = msgs.at(*it);
    const Matrix &precX = msgX.precision();

    const GaussianMessage &msgY = msgs.at(*++it);
    const Matrix &precY = msgY.precision();

    mean = msgX.mean() + msgY.mean();
    precision = precY * pinv(precY + precX) * precX;

    return result;
}


GaussianMessage AddNode::backwardFunction(int to, const MessageBox &msgs)
{
    GaussianMessage::Type msgType = msgs.begin()->second.type();

    return (msgType == GaussianMessage::GAUSSIAN_VARIANCE) ? backwardFunctionVariance(to, msgs)
                                                           : backwardFunctionPrecision(to, msgs);
}



GaussianMessage AddNode::backwardFunctionVariance(int to, const MessageBox &msgs)
{
    size_t size = msgs.begin()->second.size();

    GaussianMessage result(size);
    Matrix &mean = result.mean();
    Matrix &variance = result.variance();

    const GaussianMessage &outMsg = msgs.at(*m_outgoing.begin());
    mean += outMsg.mean();
    variance += outMsg.variance();

    for (set<int>::iterator it = m_incoming.begin(); it != m_incoming.end(); ++it)
    {
        if (*it == to)
            continue;
        const GaussianMessage &msg = msgs.at(*it);
        mean -= msg.mean();
        variance += msg.variance();
    }

    return result;
}


GaussianMessage AddNode::backwardFunctionPrecision(int to, const MessageBox &msgs)
{
    size_t size = msgs.begin()->second.size();

    GaussianMessage result(size, GaussianMessage::GAUSSIAN_PRECISION);

    Matrix &mean = result.mean();
    Matrix &precision = result.precision();

    const GaussianMessage &msgZ = msgs.at(*m_outgoing.begin());
    mean = msgZ.mean();
    const Matrix &precZ = msgZ.precision();


    set<int>::iterator it = m_incoming.begin();
    if (to == *it)
        ++it;
    const GaussianMessage &msgX = msgs.at(*it);
    const Matrix &precX = msgX.precision();

    mean = msgZ.mean() - msgX.mean();
    precision = precZ * pinv(precX + precZ) * precX;

    return result;
}












