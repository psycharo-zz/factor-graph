#ifndef NETWORK_H
#define NETWORK_H

#include <variable.h>

#include <vector>
#include <map>
#include <memory>
#include <stdexcept>

using namespace std;

namespace vmp
{

/**
 * @brief The AddEvidence
 * functor to sum up node evidence
 */
class AddEvidence
{
public:
    AddEvidence():
        value(0)
    {}

    template <typename T>
    inline void operator()(const T &node) { value += node->logEvidence(); }

    double value;
};

/**
 * @brief The SendToParent
 * functor to send message from child to parent and update its posterior
 * TODO: this requires
 */
class SendToParent
{
public:
    template <typename TChild, typename TParent>
    inline void operator()(const pair<TChild, TParent> &p) const
    {
        TChild child = p.first;
        TParent parent = p.second;
        auto params = parent->addChild(child);
        child->messageToParent(params);
        parent->updatePosterior();
    }
};





// TODO: implement generic functions
//template <class ...Args>
//void runInference(size_t numIterations, Args...);

//class Network
//{
//public:
//    Network()
//    {}

//    virtual ~Network() {}

//    /**
//     * @brief running inference
//     */
//    double runInference(const vector<BaseVariable*> vars, size_t numIters)
//    {

//        for (size_t iters = 0; iters < vars.size(); iters += 2)
//        {

//        }


//        std::runtime_error("Network::runInference(int): not implemented");
//        return 0;
//    }


//private:
//    // TODO: smart pointers
//    map<size_t, BaseVariable*> m_variables;

//    //! indexed children (id -> children[])
//    multimap<size_t, size_t> m_children;
//    //! indexed parents (id -> parents[])
//    multimap<size_t, size_t> m_parents;
//};

}



#endif // NETWORK_H
