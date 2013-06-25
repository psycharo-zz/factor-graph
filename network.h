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


}



#endif // NETWORK_H
