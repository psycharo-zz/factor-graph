#ifndef NETWORK_H
#define NETWORK_H
#include <mex.h>

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

    Network():
        m_currId(0)
    {}

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
     * @brief the list of nodes
     */
    typedef std::map<int, FactorNode*> NodeList;

    /**
     * @brief iterator
     */
    typedef AdjList::const_iterator AdjListIt;


    /**
     * @brief addNode adding node to the network and assigning it an id
     * @param node
     */
    inline int addNode(FactorNode *node)
    {
        m_nodes.insert(std::make_pair(++m_currId, node));
        return m_currId;
    }



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
        // TODO: decent error reporting on message receiving
        for (Schedule::iterator it = m_schedule.begin(); it != m_schedule.end(); ++it)
            it->first->send(it->second->id());
    }

    /**
     * @brief get the list of nodes
     */
    inline const NodeList &nodes() const
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

protected:
    //! the counter
    int m_currId;

    //! the list of all nodes in the network
    std::map<int, FactorNode*> m_nodes;

    //! adjacency list
    AdjList m_adjacency;

    //! the schedule
    Schedule m_schedule;
};

#endif // NETWORK_H
