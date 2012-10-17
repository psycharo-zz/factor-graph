#include "factornode.h"


#include <iostream>
using namespace std;

//! a single update for messages
void FactorNode::receive(const GaussianMessage &msg)
{
    for (vector<FactorNode*>::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
    {
        FactorNode *n =  *it;
        if (n->id() == msg.from())
            continue;

        // creating a temporary box of current messages
        MessageBox currMessages = m_inMessages;
        // FIXME
        pair<MessageBox::iterator, bool> res = currMessages.insert(make_pair(msg.from(), msg));
        if (!res.second)
            res.first->second = msg;

        if (currMessages.size() == m_nodes.size())
            n->receive(function(n->id(), currMessages));
        else if (currMessages.size() == m_nodes.size()-1 && !currMessages.count(n->id()))
            n->receive(function(n->id(), currMessages));
    }

    // adding in message after all iterations are finished
    addInMessage(msg);
}
