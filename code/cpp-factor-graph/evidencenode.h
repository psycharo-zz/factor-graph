#ifndef EVIDENCENODE_H
#define EVIDENCENODE_H

#include "factornode.h"



/**
 * @brief The EvidenceNode class is used simply to
 */
class EvidenceNode : public FactorNode
{
private:
    FactorNode *m_dest;

protected:

    /**
     * @brief function overriden
     * @param msgs
     * @return
     */
    GaussianMessage function(int, const MessageBox &msgs)
    {
        return msgs.begin()->second;
    }


public:
    EvidenceNode():
        m_dest(NULL)
    {}


    void addIncoming(FactorNode *node)
    {
        // TODO: do nothing or
        m_dest = node;
    }


    void addOutgoing(FactorNode *node)
    {
        m_dest = node;
    }


    /**
     * @brief receive overriden
     * @param msg
     */
    void receive(const GaussianMessage &msg)
    {
        addMessage(msg);
    }


    void setInitial(const GaussianMessage &msg)
    {
        m_dest->receive(GaussianMessage(id(), m_dest->id(), msg.mean(), msg.variance(), msg.size()));
    }

    void setDest(FactorNode *node)
    {
        m_dest = node;
    }


    GaussianMessage evidence() const
    {
        return message(m_dest->id());
    }
};






#endif // EVIDENCENODE_H
