#ifndef VARIABLE_H
#define VARIABLE_H

#include <vector>
#include <stdexcept>
using namespace std;


// TODO: create a separate implementation file?

namespace vmp
{


class Variable
{
public:
    //! TODO: should it take a Network as a parameter?
    Variable():
        m_observed(false)
    {}

    virtual ~Variable() {};

    virtual void observe(double value)
    {

    }

    //! updating the posterior w.r.t to current messages
    virtual void update()
    {
        // get a message from parents

        // and from children

    }

    //! the part of the lower bound corresponding to this variable
    virtual double logEvidence() { return 0.0; }


    bool isObserved() const { return m_observed; }

protected:
    //! flag whether the variable is observed or not
    bool m_observed;

private:
    vector<Variable*> m_parents;
    vector<Variable*> m_children;

};



class ConstantVariable
{
public:

};








/**
 * @brief The VariableArray class defines an array of random variables
 */
class VariableArray : public Variable
{
public:
    VariableArray() {}


    void observe(const vector<double> &vals)
    {
        m_observed = true;
        m_values = vals;
    }

private:
    vector<double> m_values;


};


typedef pair<double, double> Message;


// scalar message
struct ScalarMessage
{

};



};

#endif // VARIABLE_H
