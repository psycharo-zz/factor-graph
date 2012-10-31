#ifndef NETWORK_H
#define NETWORK_H

#include "factornode.h"

/**
 * @brief The Network class
 */

class Network
{
public:
    /**
     * @brief addEdge create a directed edge between two nodes
     * @param start
     * @param end
     */
    inline void addEdge(FactorNode *start, FactorNode *end)
    {
        start->addOutgoing(end);
        end->addIncoming(start);
    }

private:
    map<int, FactorNode*> m_nodes;
};

#endif // NETWORK_H
