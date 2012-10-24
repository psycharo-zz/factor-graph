#include "addnode.h"

AddNode::AddNode():
    m_summand1(NULL),
    m_summand2(NULL),
    m_result(NULL)
{}


GaussianMessage AddNode::function(int to, const MessageBox &msgs)
{
    assert(!msgs.empty());

    size_t size = msgs.begin()->second.size();
    size_t size2 = size * size;

    GaussianMessage result(id(), to, size);

    double *median = result.mean();
    double *variance = result.variance();

    if (to == m_result->id())
    {
        GaussianMessage msg1 = msgs.at(m_summand1->id());
        GaussianMessage msg2 = msgs.at(m_summand2->id());

        transform(msg1.mean(), msg1.mean() + size, msg2.mean(),
                  median, std::plus<double>());

        transform(msg1.variance(), msg1.variance() + size2, msg2.variance(),
                  variance, std::plus<double>());
    }
    else
    {
        GaussianMessage resMsg = msgs.at(m_result->id());
        GaussianMessage sumMsg = (to == m_summand1->id()) ? msgs.at(m_summand2->id()) : msgs.at(m_summand1->id());
        transform(resMsg.mean(), resMsg.mean() + size, sumMsg.mean(),
                  median, std::minus<double>());
        transform(resMsg.variance(), resMsg.variance() + size2, sumMsg.variance(),
                  variance, std::plus<double>());
    }

    return result;
}




void AddNode::addIncoming(FactorNode *node)
{
    if (m_summand1 == NULL)
    {
        m_summand1 = node;
        m_nodes.push_back(m_summand1);
    }
    else if (m_summand2 == NULL)
    {
        m_summand2 = node;
        m_nodes.push_back(m_summand2);
    }
    else
        // TODO: decent exceptions
        throw "AddNode: only two summands are supported";
}

void AddNode::addOutgoing(FactorNode *node)
{
    if (m_result == NULL)
    {
        m_result = node;
        m_nodes.push_back(m_result);
    }
    else
        throw "AddNode: only one result is possible";
}



void AddNode::setConnections(FactorNode *summand1, FactorNode *summand2, FactorNode *result)
{
    m_summand1 = summand1;
    m_summand2 = summand2;
    m_result = result;
    m_nodes.clear();
    m_nodes.push_back(m_summand1);
    m_nodes.push_back(m_summand2);
    m_nodes.push_back(m_result);
}









