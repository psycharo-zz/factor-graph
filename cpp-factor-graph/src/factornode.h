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

    //! propagate after receiving a message (i.e. receive in non-loopy mode)
    virtual void propagate(int from, const GaussianMessage &msg);

    //! receive update
    virtual void receive(int from, const GaussianMessage &msg);

    //! send update to a node (loopy mode)
    virtual void send(int to);

    //! add ingoing connection to the node
    virtual void addIncoming(FactorNode *node);

    //! add outgoing connection to the node
    virtual void addOutgoing(FactorNode *node);

    //! add a custom connection with the given tag
    virtual void addConnection(FactorNode *node, const string &tag);

    //! check if the message type is supported
    virtual bool isSupported(Message::Type type) = 0;


protected:
    //! the action that this node is actually doing
    virtual GaussianMessage function(int to, const MessageBox &msgs) = 0;

    //! check if the message is forward or not
    inline bool isForward(int node_id) const
    {
        return m_outgoing.count(node_id) != 0;
    }

    inline bool isBackward(int node_id) const
    {
        return m_incoming.count(node_id) != 0;
    }

    /**
     * @brief isConnection check whether the node is a connection with the specified tag
     */
    inline bool isConnection(int node_id, const string &tag)
    {
        map<int, string>::const_iterator it = m_connections.find(node_id);
        return it == m_connections.end() ? false : it->second == tag;
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
    inline void addMessage(int from, const GaussianMessage &msg)
    {
        assert(isSupported(msg.type()));
        pair<MessageBox::iterator, bool> res = m_messages.insert(make_pair(from, msg));
        if (!res.second)
            res.first->second = msg;
    }

    //! the list of all nodes
    map<int, FactorNode*> m_nodes;

    //! incoming connections
    set<int> m_incoming;

    //! outgoing connections
    set<int> m_outgoing;

    //! custom connections
    map<int, string> m_connections;


private:
    //! the unique (TODO) id
    int m_id;

    //! all incoming messages
    MessageBox m_messages;

    //! the overall
    static int s_idCounter;

};



#endif // FACTORNODE_H
