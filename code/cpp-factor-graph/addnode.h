#ifndef ADDNODE_H
#define ADDNODE_H


#include "factornode.h"


class AddNode : public FactorNode
{
private:
    FactorNode *m_summand1;
    FactorNode *m_summand2;
    FactorNode *m_result;



protected:
    Message function(int to, const MessageBox &msgs)
    {
        if (to == m_result->id())
        {
            Message msg1 = msgs.at(m_summand1->id());
            Message msg2 = msgs.at(m_summand2->id());
            return Message(msg1.mean + msg2.mean, msg1.var + msg2.var);
        }
        else
        {
            Message resMsg = msgs.at(m_result->id());
            Message sumMsg = (to == m_summand1->id()) ? msgs.at(m_summand2->id()) : msgs.at(m_summand1->id());
            return Message(resMsg.mean - sumMsg.mean, resMsg.var + sumMsg.var);
        }
    }

public:

    AddNode(int id):
        FactorNode(id, "Add"),
        m_summand1(NULL),
        m_summand2(NULL),
        m_result(NULL)
    {}

    /**
     * @brief AddNode
     * @param id
     * @param param1
     * @param param2
     * @param result
     */
    AddNode(int id, FactorNode *summand1, FactorNode *summand2, FactorNode *result):
        FactorNode(id, "Add"),
        m_summand1(summand1),
        m_summand2(summand2),
        m_result(result)
    {}


    void setConnections(FactorNode *summand1, FactorNode *summand2, FactorNode *result)
    {
        m_summand1 = summand1;
        m_summand2 = summand2;
        m_result = result;
        m_nodes.clear();
        m_nodes.push_back(m_summand1);
        m_nodes.push_back(m_summand2);
        m_nodes.push_back(m_result);

    }
};



#endif // ADDNODE_H
