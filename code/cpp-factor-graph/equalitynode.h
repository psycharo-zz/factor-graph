#ifndef EQUALITYNODE_H
#define EQUALITYNODE_H


#include "factornode.h"
#include "util.h"



class EqualityNode : public FactorNode
{
protected:
    /**
     * @brief function
     * @param to
     * @param msgs
     * @return gaussian message (m_j, V_j)
     */
    GaussianMessage function(int to, const MessageBox &msgs);

public:
    EqualityNode(int id):
        FactorNode(id)
    {}

    // setup connections
    // TODO: actually arbitrary number of connections works for this node type
    void setConnections(FactorNode *a, FactorNode *b, FactorNode *c);
};



#endif // EQUALITYNODE_H
