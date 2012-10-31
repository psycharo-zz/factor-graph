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
void FactorNode::receive(const GaussianMessage &msg)
{
    for (map<int, FactorNode*>::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
    {
        FactorNode *n =  it->second;
        if (n->id() == msg.from())
            continue;

        // creating a temporary box of current messages
        MessageBox currMessages = m_messages;
        // FIXME
        pair<MessageBox::iterator, bool> res = currMessages.insert(make_pair(msg.from(), msg));
        if (!res.second)
            res.first->second = msg;

        // when we have all the messages
        if (currMessages.size() == m_nodes.size())
            n->receive(function(n->id(), currMessages));
        // or when we have all messages except for the one where we want to send
        else if (currMessages.size() == m_nodes.size()-1 && !currMessages.count(n->id()))
            n->receive(function(n->id(), currMessages));
    }

    // adding in message after all iterations are finished
    addMessage(msg);
}
