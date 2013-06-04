#ifndef NETWORK_H
#define NETWORK_H

#include <variable.h>

#include <vector>
#include <map>
#include <memory>

using namespace std;

namespace vmp
{
class Network
{
public:
    Network()
    {}

    virtual ~Network() {}

    /**
     * @brief runInference
     * @param maxIterations
     * @return
     */
    double runInference(int maxIterations)
    {
        std::runtime_error("Network::runInference(int): not implemented");
    }



private:
    // TODO: smart pointers
    // TODO: separate thing for array?
    map<size_t, auto_ptr<Variable> > m_variables;

    //! indexed children (id -> children[])
    multimap<size_t, size_t> m_children;
    //! indexed parents (id -> parents[])
    multimap<size_t, size_t> m_parents;
};

}



#endif // NETWORK_H
