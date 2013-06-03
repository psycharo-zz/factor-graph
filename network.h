#ifndef NETWORK_H
#define NETWORK_H

#include <variable.h>

#include <vector>

using namespace std;

namespace vmp
{
class Network
{
public:
    Network()
    {}

    virtual ~Network()
    {
        for (size_t i = 0; i < m_variables.size(); ++i)
            delete m_variables[i];
        m_variables.clear();
    }

    /**
     * @brief runInference
     * @param maxIterations
     * @return
     */
    double runInference(int maxIterations)
    {
        double lb = 0.0;

        for (size_t i = 0; i < maxIterations; ++i)
        {

        }

        return lb;
    }

    template <typename T>
    T *createVariable(double param1, double param2)
    {
        T *var = new T(param1, param2);
        m_variables.push_back(var);
        return var;
    }

    template <typename T>
    T *createVariable(Variable *param1, Variable *param2)
    {
        T *var = new T(param1, param2);
        m_variables.push_back(var);
        return var;
    }


    // TODO: createArray?


private:
    // TODO: smart pointers
    // TODO: separate thing for array?
    vector<Variable*> m_variables;

};

}



#endif // NETWORK_H
