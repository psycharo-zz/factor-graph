#ifndef CUSTOMNODE_H
#define CUSTOMNODE_H

#include "../factornode.h"

/**
 * @brief The CustomNode class
 * allows using with matlab functions
 */

class CustomNode : public FactorNode
{
public:
    /**
     * set the name of the function
     * @param _functionName the global name of the function to call
     */
    inline void setFunction(const char *name)
    {
        m_functionName = name;
    }

protected:
    GaussianMessage function(int to, const MessageBox &msgs);

private:
    std::string m_functionName;
};

#endif // CUSTOMNODE_H
