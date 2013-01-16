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

    // z by x and y
    GaussianMessage forwardFunction(int to, const MessageBox &msgs);

    GaussianMessage forwardFunctionVariance(int to, const MessageBox &msgs);
    GaussianMessage forwardFunctionPrecision(int to, const MessageBox &msgs);


    // x (or y) by z and y (or x)
    GaussianMessage backwardFunction(int to, const MessageBox &msgs);

    GaussianMessage backwardFunctionVariance(int to, const MessageBox &msgs);
    GaussianMessage backwardFunctionPrecision(int to, const MessageBox &msgs);

};


#endif // ADDNODE_H
