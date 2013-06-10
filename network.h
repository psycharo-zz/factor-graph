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
     * @brief runInference
     * @param maxIterations
     * @return
     */
    double runInference(int )
    {
        std::runtime_error("Network::runInference(int): not implemented");
        return 0;
    }


private:
    // TODO: smart pointers
    // TODO: separate thing for array?
//    map<size_t, auto_ptr<BaseVariable> > m_variables;

    //! indexed children (id -> children[])
    multimap<size_t, size_t> m_children;
    //! indexed parents (id -> parents[])
    multimap<size_t, size_t> m_parents;
};

}



#endif // NETWORK_H
