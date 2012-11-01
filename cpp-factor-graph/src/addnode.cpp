#include "addnode.h"


GaussianMessage AddNode::function(int to, const MessageBox &msgs)
{
    assert(!msgs.empty());
    assert(isForward(to) || isBackward(to));
    assert(m_outgoing.size() == 1);

    size_t size = msgs.begin()->second.size();
    size_t size2 = size * size;

    GaussianMessage result(size);

    double *median = result.mean();
    double *variance = result.variance();

    if (isForward(to))
    {
        for (set<int>::iterator it = m_incoming.begin(); it != m_incoming.end(); ++it)
        {
            const GaussianMessage &msg = msgs.at(*it);
            transform(msg.mean(), msg.mean() + size, median, median, std::plus<double>());
            transform(msg.variance(), msg.variance() + size2, variance, variance, std::plus<double>());
        }
    }
    else
    {
        const GaussianMessage &outMsg = msgs.at(*m_outgoing.begin());
        transform(outMsg.mean(), outMsg.mean() + size, median, median, std::plus<double>());
        transform(outMsg.variance(), outMsg.variance() + size2, variance, variance, std::plus<double>());

        for (set<int>::iterator it = m_incoming.begin(); it != m_incoming.end(); ++it)
        {
            if (*it == to)
                continue;
            const GaussianMessage &msg = msgs.at(*it);
            transform(median, median + size, msg.mean(), median, std::minus<double>());
            transform(msg.variance(), msg.variance() + size2, variance, variance, std::plus<double>());
        }
    }

    return result;
}


void AddNode::addOutgoing(FactorNode *node)
{
    assert(m_outgoing.size() == 0);
    FactorNode::addOutgoing(node);
}










