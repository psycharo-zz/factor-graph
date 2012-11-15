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
    typedef vector<pair<FactorNode*, FactorNode*> > Schedule;

//    /**
//     * @brief addEdge create a directed edge between two nodes
//     * @param start
//     * @param end
//     */
//    inline void addEdge(FactorNode *start, FactorNode *end)
//    {
//        start->addOutgoing(end);
//        end->addIncoming(start);

//        m_nodes.insert(make_pair(start->id(), start));
//        m_nodes.insert(make_pair(end->id(), end));
//    }

    /**
     * @brief addEdge add
     * @param A
     * @param B
     * @param tagForA
     * @param tagForB
     */
    inline void addEdge(FactorNode *A, FactorNode *B, const string &tagForA = "", const string &tagForB = "")
    {
        tagForA.empty() ? A->addOutgoing(B) : A->addConnection(B, tagForA);
        tagForB.empty() ? B->addIncoming(A) : B->addConnection(A, tagForB);

        m_nodes.insert(make_pair(A->id(), A));
        m_nodes.insert(make_pair(B->id(), B));
    }


    /**
     * @brief setSchedule specify the schedule
     * @param schedule
     */
    inline void setSchedule(const Schedule &schedule)
    {
        m_schedule = schedule;
        assert(m_schedule.size() >= m_nodes.size()-1);
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
    map<int, FactorNode*> m_nodes;
    Schedule m_schedule;
};

#endif // NETWORK_H
