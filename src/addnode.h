#ifndef ADDNODE_H
#define ADDNODE_H

#include "factornode.h"


class AddNode : public FactorNode
{
public:
    AddNode(Network *nwk):
        FactorNode(nwk)
    {}

    //! @overload
    bool isSupported(Message::Type type);

    //! @overload
    void addOutgoing(FactorNode *node);

protected:
    GaussianMessage function(int to, const MessageBox &msgs);

    GaussianMessage forwardFunction(int to, const MessageBox &msgs);
    GaussianMessage backwardFunction(int to, const MessageBox &msgs);

};


#endif // ADDNODE_H
