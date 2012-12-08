#include "addnode.h"

#include "matrix.h"


using namespace std;

bool AddNode::isSupported(Message::Type type)
{
    return type == GaussianMessage::GAUSSIAN_VARIANCE;
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

GaussianMessage AddNode::backwardFunction(int to, const MessageBox &msgs)
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









