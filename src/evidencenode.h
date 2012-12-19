#ifndef EVIDENCENODE_H
#define EVIDENCENODE_H

#include "factornode.h"


/**
 * @brief this nodes imitates half-edge in the network
 */
class EvidenceNode : public FactorNode
{
protected:
    /**
     * @overload
     * @brief function overriden
     * @param msgs
     * @return
     */
    GaussianMessage function(int to, const MessageBox &msgs)
    {
        return msgs.count(to) ? msgs.at(to) : msgs.at(Message::UNDEFINED_ID);
    }


public:
    //! @overload
    void addIncoming(FactorNode *node)
    {
        assert(m_incoming.size() == 0);
        FactorNode::addIncoming(node);
    }


    //! @overload
    void addOutgoing(FactorNode *node)
    {
        assert(m_outgoing.size() == 0);
        FactorNode::addOutgoing(node);
    }


    //! @overload
    bool isSupported(Message::Type /*type*/)
    {
        return true;
    }

    //! set the initial message
    void receive(const GaussianMessage &msg)
    {
        addMessage(Message::UNDEFINED_ID, msg);
    }


    //! start propagating with the initial message
    void propagate(const GaussianMessage &msg)
    {
        addMessage(Message::UNDEFINED_ID, msg);
        destination()->propagate(id(), msg);
    }

    /**
     * @overload
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
     */
    inline FactorNode *destination() const
    {
        if (m_nodes.size() != 1)
            throw std::runtime_error("EvidenceNode(" + id_to_string(id()) + ")::destination(): no destination specified");
        return m_nodes.begin()->second;
    }


    /**
     * @brief observe the evidence
     */
    inline const GaussianMessage &evidence() const
    {
        return message(destination()->id());
    }
};






#endif // EVIDENCENODE_H
