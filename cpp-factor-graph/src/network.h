#ifndef NETWORK_H
#define NETWORK_H

#include "factornode.h"

#include <utility>
#include <map>
#include <exception>

/**
 * @brief The Network class
 */

class Network
{
public:

    virtual ~Network() {};
    /**
     * @brief Schedule type represents an update schedule
     * TODO: maybe use nodes themselves (to avoid hacks etc)
     */
    typedef std::vector<std::pair<FactorNode*, FactorNode*> > Schedule;

    /**
     * @brief addEdge add
     * @param A
     * @param B
     * @param tagForA
     * @param tagForB
     */
    inline void addEdge(FactorNode *A, FactorNode *B, const std::string &tagForA = "", const std::string &tagForB = "")
    {
        tagForA.empty() ? A->addOutgoing(B) : A->addConnection(B, tagForA);
        tagForB.empty() ? B->addIncoming(A) : B->addConnection(A, tagForB);

        m_nodes.insert(std::make_pair(A->id(), A));
        m_nodes.insert(std::make_pair(B->id(), B));
    }


    /**
     * @brief setSchedule specify the schedule
     * @param schedule
     */
    inline void setSchedule(const Schedule &schedule)
    {
        m_schedule = schedule;
        if (m_schedule.size() < m_nodes.size()-1)
            throw std::runtime_error("Network::setSchedule()");
        // TODO: check the schedule?
    }


    /**
     * @brief step make a single step for the given sce
     */
    inline void step()
    {
        // TODO: make dummy messages?
        for (Schedule::iterator it = m_schedule.begin(); it != m_schedule.end(); ++it)
            it->first->send(it->second->id());
    }


private:
    std::map<int, FactorNode*> m_nodes;
    Schedule m_schedule;
};

#endif // NETWORK_H
