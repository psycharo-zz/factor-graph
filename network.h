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
class Network
{
public:
    Network()
    {}

    virtual ~Network() {}

    /**
     * @brief running inference
     */
    double runInference(const vector<BaseVariable*> vars, size_t numIters)
    {

        for (size_t iters = 0; iters < vars.size(); iters += 2)
        {

        }


        std::runtime_error("Network::runInference(int): not implemented");
        return 0;
    }


private:
    // TODO: smart pointers
    map<size_t, BaseVariable*> m_variables;

    //! indexed children (id -> children[])
    multimap<size_t, size_t> m_children;
    //! indexed parents (id -> parents[])
    multimap<size_t, size_t> m_parents;
};

}



#endif // NETWORK_H
