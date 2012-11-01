#include "factornode.h"


using namespace std;

int FactorNode::s_idCounter = 0;


FactorNode::FactorNode():
    m_id(s_idCounter++)
{}


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



//! a single update for messages
void FactorNode::propagate(int from, const GaussianMessage &msg)
{
    for (map<int, FactorNode*>::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
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
    addMessage(from, msg);
}


void FactorNode::receive(int from, const GaussianMessage &msg)
{
    addMessage(from, msg);
}

void FactorNode::send(int to)
{
    assert(m_nodes.count(to) != 0);
    // have all the necessary messages
    if (m_messages.size() == m_nodes.size() ||
        (m_messages.size()+1 == m_nodes.size() && m_messages.count(to) == 0))
        m_nodes.at(to)->receive(id(), function(to, m_messages));
}





