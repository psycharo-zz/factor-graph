#ifndef DYNAMICNETWORK_H
#define DYNAMICNETWORK_H

#include "network.h"
#include "evidencenode.h"
#include <iostream>

using namespace std;

/**
 * @brief network with repeated structure
 * The dynamic network consists of interconnected time slices, each
 * represented by a smaller static network
 */
class DynamicNetwork : public Network
{
public:
    virtual ~DynamicNetwork() {}

    /**
     * @brief add a temporal connection between two time slices
     * @param src - source node at the step (n-1)
     * @param dst - destination node at the step (n)
     */
    inline void addTemporalEdge(EvidenceNode *src, EvidenceNode *dst)
    {
        m_temporal.insert(make_pair(src->id(), dst->id()));
    }

    /**
     * @brief get the adjacency list for the temporal connections
     */
    inline const AdjList &adjListTemporal() const
    {
        return m_temporal;
    }


    /**
     * TODO: put it into cpp (also for Network)
     * @brief make several message-passing runs
     * @param the observation data for several steps, each pair in map is (factorId, Message)
     *
     */
    void step(const vector<map<int, GaussianMessage> > &data)
    {
        for (size_t i = 0; i < data.size(); ++i)
        {
            // observing evidence
            for (map<int, GaussianMessage>::const_iterator it = data[i].begin(); it != data[i].end(); ++it)
            {
                EvidenceNode *node = static_cast<EvidenceNode*>(m_nodes.at(it->first));
                node->receive(it->second);
            }

            // internal propagation
            for (Schedule::iterator it = m_schedule.begin(); it != m_schedule.end(); ++it)
                it->first->send(it->second->id());

            // temporal propagation
            for (AdjListIt it = m_temporal.begin(); it != m_temporal.end(); ++it)
            {
                EvidenceNode *src = static_cast<EvidenceNode*>(m_nodes.at(it->first));
                EvidenceNode *dst = static_cast<EvidenceNode*>(m_nodes.at(it->second));
                dst->receive(src->evidence());
            }
        }
    }


protected:
    AdjList m_temporal;
};

#endif // DYNAMICNETWORK_H
