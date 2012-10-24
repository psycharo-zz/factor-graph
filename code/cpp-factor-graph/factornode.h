#ifndef FACTORNODE_H
#define FACTORNODE_H


#include <vector>
#include <memory>
#include <map>
#include <string>
#include <cassert>
#include <algorithm>


using namespace std;

#include "message.h"



/**
 * @brief The FactorNode class basic abstract node
 */
class FactorNode
{
public:
    FactorNode();

    virtual ~FactorNode() {}

    //! id accessor
    inline int id() const { return m_id; }

    //! a single update for messages
    virtual void receive(const GaussianMessage &msg);

    //! add ingoing connection to the node
    virtual void addIncoming(FactorNode *node);

    //! add outgoing connection to the node
    virtual void addOutgoing(FactorNode *node);


protected:
    //! the action that this node is actually doing
    virtual GaussianMessage function(int to, const MessageBox &msgs) = 0;

    //! check if there is a message for the given id
    inline bool hasInMessage(int node_id) const
    {
        return m_inMessages.count(node_id) != 0;
    }

    //! get the message for the specified id
    inline GaussianMessage inMessage(int node_id) const
    {
        return m_inMessages.at(node_id);
    }


    inline MessageBox &inMessages()
    {
        return m_inMessages;
    }

    //! add an incoming message
    inline void addInMessage(const GaussianMessage &msg)
    {
        pair<MessageBox::iterator, bool> res = m_inMessages.insert(make_pair(msg.from(), msg));
        if (!res.second)
            res.first->second = msg;
    }

    //! TODO: at most three of these??
    vector<FactorNode*> m_nodes;

    //! incoming connections
    vector<FactorNode*> m_incomingNodes;

    //! outgoing connections
    vector<FactorNode*> m_outgoingNodes;

private:
    //! the unique (TODO) id
    int m_id;


    //! all incoming messages
    MessageBox m_inMessages;


    //! the overall
    static int s_idCounter;

};



#endif // FACTORNODE_H
