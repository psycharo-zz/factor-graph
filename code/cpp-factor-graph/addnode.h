#ifndef ADDNODE_H
#define ADDNODE_H

#include "factornode.h"


class AddNode : public FactorNode
{
protected:
    GaussianMessage function(int to, const MessageBox &msgs);

public:
    void addOutgoing(FactorNode *node);
};



#endif // ADDNODE_H
