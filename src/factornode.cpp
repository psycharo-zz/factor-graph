#include "factornode.h"

#include "network.h"

using namespace std;



FactorNode::FactorNode(Network *nwk)
{
    m_id = nwk->addNode(this);
}


void FactorNode::addIncoming(FactorNode *node)
{
    m_nodes.insert(make_pair(node->id(), node));
    m_incoming.insert(node->id());
}


void FactorNode::addOutgoing(FactorNode *node)
{
    m_nodes.insert(make_pair(node->id(), node));
    m_outgoing.insert(node->id());
}


void FactorNode::addConnection(FactorNode *node, const string &tag)
{
    m_nodes.insert(make_pair(node->id(), node));
    m_connections.insert(make_pair(node->id(), tag));
}




//! a single update for messages
void FactorNode::propagate(int from, const GaussianMessage &msg)
{
    for (map<int, FactorNode*>::const_iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
    {
        FactorNode *n =  it->second;
        if (n->id() == from)
            continue;

        // creating a temporary box of current messages
        MessageBox currMessages = m_messages;
        // FIXME
        pair<MessageBox::iterator, bool> res = currMessages.insert(make_pair(from, msg));
        if (!res.second)
            res.first->second = msg;

        // when we have all the messages
        // or when we have all messages except for the one where we want to send
        if (currMessages.size() == m_nodes.size() ||
            (currMessages.size()+1 == m_nodes.size() && !currMessages.count(n->id())))
            n->propagate(id(), function(n->id(), currMessages));
    }

    // adding in message after all iterations are finished
    setMessage(from, msg);
}


void FactorNode::receive(int from, const GaussianMessage &msg)
{
    setMessage(from, msg);
}

void FactorNode::send(int to)
{
    if (m_nodes.count(to) == 0)
        throw std::runtime_error("FactorNode(" + id_to_string(m_id) + ")::send: unknown recipient");

    // have all the necessary messages
    if (m_messages.size() >= (m_incoming.size() + m_outgoing.size()) || // m_messages.size() > m_nodes.size() && m_messages.count(UNDEFINED_ID)??
        (m_messages.size()+1 == (m_incoming.size() + m_outgoing.size()) && m_messages.count(to) == 0))
        m_nodes.at(to)->receive(id(), function(to, m_messages));
}





