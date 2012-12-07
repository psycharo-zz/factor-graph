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

    virtual ~Network() {}
    /**
     * @brief the (ordered) list of message updates, where each pair is (receiver, sender)
     */
    typedef std::vector<std::pair<FactorNode*, FactorNode*> > Schedule;


    /**
     * @brief the adjacency list (id --> (id,...,id))
     */
    typedef std::multimap<int, int> AdjList;

    /**
     * @brief iterator
     */
    typedef AdjList::const_iterator AdjListIt;



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

        // TODO: make several types of connections?
        m_adjacency.insert(std::make_pair(A->id(), B->id()));
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

    /**
     * @brief get the list of nodes
     */
    inline const std::map<int, FactorNode*> &nodes() const
    {
        return m_nodes;
    }

    /**
     * @brief get the adjacency map (id --> id)
     */
    inline const AdjList &adjList() const
    {
        return m_adjacency;
    }

private:
    //! the list of all nodes in the network
    std::map<int, FactorNode*> m_nodes;

    //! adjacency list
    AdjList m_adjacency;

    //! the schedule
    Schedule m_schedule;
};

#endif // NETWORK_H
