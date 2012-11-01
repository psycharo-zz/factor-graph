#ifndef EVIDENCENODE_H
#define EVIDENCENODE_H

#include "factornode.h"



/**
 * @brief The EvidenceNode class is used simply to
 */
class EvidenceNode : public FactorNode
{
protected:
    /**
     * @brief function overriden
     * @param msgs
     * @return
     */
    GaussianMessage function(int, const MessageBox &msgs)
    {
        return msgs.at(Message::UNDEFINED_ID);
    }


public:

    void addIncoming(FactorNode *node)
    {
        assert(m_incoming.size() == 0);
        FactorNode::addIncoming(node);
    }


    void addOutgoing(FactorNode *node)
    {
        assert(m_outgoing.size() == 0);
        FactorNode::addOutgoing(node);
    }


    void receive(const GaussianMessage &msg)
    {
        addMessage(Message::UNDEFINED_ID, msg);
    }


    void propagate(const GaussianMessage &msg)
    {
        destination()->propagate(id(), GaussianMessage(msg.mean(), msg.variance(), msg.size()));
    }

    /**
     * @brief propagate overriden
     * @param msg either a dummy message or a message from the only connection - destination
     */
    void propagate(int from, const GaussianMessage &msg)
    {
        assert(from == destination()->id());
        FactorNode::propagate(from, msg);
    }

    /**
     * @brief get the destination of the node
     * @return
     */
    inline FactorNode *destination() const
    {
        assert(m_nodes.size() == 1);
        return m_nodes.begin()->second;
    }


    GaussianMessage evidence() const
    {
        return message(destination()->id());

    }
};






#endif // EVIDENCENODE_H
