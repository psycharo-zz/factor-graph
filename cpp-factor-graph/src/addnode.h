#ifndef ADDNODE_H
#define ADDNODE_H

#include "factornode.h"


class AddNode : public FactorNode
{
public:
    //! @overload
    bool isSupported(Message::Type type);

    //! @overload
    void addOutgoing(FactorNode *node);

protected:
    GaussianMessage function(int to, const MessageBox &msgs);
};



#endif // ADDNODE_H
