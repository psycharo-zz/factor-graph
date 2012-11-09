#ifndef ADDMULTNODE_H
#define ADDMULTNODE_H

#include "factornode.h"

/**
 * @brief The AddMultNode class implements composite block (Table 4) [Loeliger2007]
 * TODO:
 */
class AddMultNode : public FactorNode
{
protected:
    //! @overload
    GaussianMessage function(int to, const MessageBox &msgs);


private:
};

#endif // ADDMULTNODE_H
