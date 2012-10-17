#ifndef ADDNODE_H
#define ADDNODE_H



#include "factornode.h"
#include "util.h"



class AddNode : public FactorNode
{
private:
    FactorNode *m_summand1;
    FactorNode *m_summand2;
    FactorNode *m_result;

protected:
    GaussianMessage function(int to, const MessageBox &msgs);

public:
    AddNode(int id);

    void setConnections(FactorNode *summand1, FactorNode *summand2, FactorNode *result);
};



#endif // ADDNODE_H
