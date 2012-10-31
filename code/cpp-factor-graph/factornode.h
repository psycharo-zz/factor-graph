#ifndef FACTORNODE_H
#define FACTORNODE_H


#include <vector>
#include <memory>
#include <map>
#include <set>
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

    //! check if the message is forward or not
    inline bool isForward(int to_id) const
    {
        return m_outgoing.count(to_id) != 0;
    }

    inline bool isBackward(int to_id) const
    {
        return m_incoming.count(to_id) != 0;
    }


    //! check if there is a message for the given id
    inline bool hasMessage(int node_id) const
    {
        return m_messages.count(node_id) != 0;
    }

    //! get the message for the specified id
    inline GaussianMessage message(int node_id) const
    {
        return m_messages.at(node_id);
    }


    inline MessageBox &messages()
    {
        return m_messages;
    }

    //! add an incoming message
    inline void addMessage(const GaussianMessage &msg)
    {
        pair<MessageBox::iterator, bool> res = m_messages.insert(make_pair(msg.from(), msg));
        if (!res.second)
            res.first->second = msg;
    }

    //! TODO: at most three of these??
    map<int, FactorNode*> m_nodes;

    //! incoming connections
    set<int> m_incoming;

    //! outgoing connections
    set<int> m_outgoing;


private:
    //! the unique (TODO) id
    int m_id;


    //! all incoming messages
    MessageBox m_messages;

    //! the overall
    static int s_idCounter;

};



#endif // FACTORNODE_H
