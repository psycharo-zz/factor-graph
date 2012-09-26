#ifndef EQUALITYNODE_H
#define EQUALITYNODE_H


#include "factornode.h"


class EqualityNode : public FactorNode
{
protected:

    Message function(int to, const MessageBox &msgs)
    {
        double dot = 0;
        double cov = 0;

        for (MessageBox::const_iterator it = msgs.begin(); it != msgs.end(); ++it)
        {
            if (it->first == to)
                continue;
            cov += 1 / it->second.var;
            dot += it->second.mean / it->second.var;
        }
        double var = 1 / cov;
        double mean = dot * var;

        return Message(mean, var);
    }

public:
    EqualityNode(int id):
        FactorNode(id, "Equality")
    {}

    // setup connections, TODO: actually arbitrary number of connections works for this node type
    void setConnections(FactorNode *a, FactorNode *b, FactorNode *c)
    {
        m_nodes.clear();
        m_nodes.push_back(a);
        m_nodes.push_back(b);
        m_nodes.push_back(c);
    }


};



#endif // EQUALITYNODE_H
