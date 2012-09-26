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

    Message function(int to, const MessageBox &msgs)
    {
        return msgs.begin()->second;
    }


public:
    EvidenceNode(int id):
        FactorNode(id, "Evidence"),
        m_dest(NULL)
    {}

    void receive(int from, const Message &msg)
    {
//        cout <<  from << " " << m_type << "(" << id() << ") " << msg.mean << endl;
        addInMessage(from, msg);
    }


    void receive(const Message &msg)
    {
//        cout << "x " << "Evidence(" << id() << ") " << msg.mean << endl;

        m_dest->receive(id(), msg);
    }

    void setDest(FactorNode *node)
    {
        m_dest = node;
    }


    Message evidence()
    {
        return inMessage(m_dest->id());
    }
};






#endif // EVIDENCENODE_H
