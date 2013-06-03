#ifndef VARIABLE_H
#define VARIABLE_H

#include <vector>
#include <stdexcept>
using namespace std;


// TODO: create a separate implementation file?

namespace vmp
{




class Parameters
{
// TODO: introduce summation operator?
public:
    virtual ~Parameters() {}
};


class Moments
{
public:
    virtual ~Moments() {}
};





class Variable
{
public:
    //! TODO: should it take a Network as a parameter?
    Variable():
        m_observed(false),
        m_id(++s_idCounter)
    {}

    virtual ~Variable() {};

    //! observe a scalar value TODO: make a template/vector version?
    virtual void observe(double _value)
    {
        m_observed = true;
        m_value = _value;
    }

    //! check whether the variable is observed
    inline bool isObserved() const { return m_observed; }

    //! get a unique id
    inline size_t id() const { return m_id; }

// TODO: template-based polymorphism? or via interfaces like CanHaveGaussianParent
    virtual Moments *messageToChildren() const = 0;
    virtual Parameters *messageToParent(Variable *parent) const = 0;

    //! updating the posterior w.r.t to the current messages
    virtual void updatePosterior() = 0;

    //! compute the constitute to the lower bound on the log-evidence
    virtual double logEvidenceLowerBound() const = 0;

    //! whether it has any parents (needed to determine whether the messages should be sent)
    virtual bool hasParents() const = 0;

    //! obtain a message from a parent
    virtual void receiveFromParent(Moments *ms, Variable *parent) = 0;

    //! obtain a message from a child
    virtual void receiveFromChild(Parameters *ps, Variable *child) = 0;

protected:
    //! unique identifier
    size_t m_id;

    //! flag whether the variable is observed or not
    bool m_observed;

    //! stored value in case it is observed
    double m_value;

private:
    static size_t s_idCounter;
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





};

#endif // VARIABLE_H
