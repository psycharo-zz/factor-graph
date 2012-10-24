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
    GaussianMessage function(int to, const MessageBox &msgs);

public:
    AddNode();

    void addIncoming(FactorNode *node);

    void addOutgoing(FactorNode *node);

    void setConnections(FactorNode *summand1, FactorNode *summand2, FactorNode *result);
};



#endif // ADDNODE_H
