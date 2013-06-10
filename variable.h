#ifndef VARIABLE_H
#define VARIABLE_H

#include <vector>
#include <map>

using namespace std;


#include <util.h>



// TODO: create a separate implementation file?

namespace vmp
{

template <typename T>
class Parameters
{
// TODO: introduce summation operator?
public:
    virtual ~Parameters() {}
};


template <typename T>
class Moments
{
public:
    virtual ~Moments() {}
};



/**
 * @brief the base class for all the variables
 */
class BaseVariable
{
public:
    BaseVariable():
        m_id(++s_idCounter)
    {}

    virtual ~BaseVariable() {};

    //! get a unique id
    inline size_t id() const { return m_id; }

private:
    //! unique identifier
    size_t m_id;

    static size_t s_idCounter;
};


/**
 * Base class for statically specifed distribution forms
 */
template <typename TDistribution>
class Variable : public BaseVariable
{
public:
    Variable():
        m_observed(false)
    {}

    Variable(const Parameters<TDistribution> &params):
        m_observed(false),
        m_params(params)
    {}

    virtual ~Variable() {};

    //! check whether the variable is observed
    inline virtual bool isObserved() const { return m_observed; }


    //! check whether any parents are non-constant
    virtual bool hasParents() const = 0;

    //! get the moments TODO: the same as message to children
    virtual Moments<TDistribution> moments() const = 0;

    //! prior parameters, received from the parents
    virtual Parameters<TDistribution> parametersFromParents() const = 0;

    //! posterior parameters of the distribution
    inline virtual const Parameters<TDistribution> &parameters() const { return m_params; }


    //! compute the constitute to the lower bound on the log-evidence
    virtual double logEvidenceLowerBound() const
    {
        return isObserved() ? logEvidenceLowerBoundObserved() :
                              logEvidenceLowerBoundHidden();
    }

    //! compute LB in case the variable is observed
    double logEvidenceLowerBoundObserved() const
    {
        return parameters() * moments()
             + logNormalizationParents();
    }

    //! compute LB in case the variable is hidden
    double logEvidenceLowerBoundHidden() const
    {
        return parametersFromParents() * moments() - parameters() * moments()
               + logNormalizationParents()
               - logNormalization();
    }

    //! compute the log-normalization (the g() function) based on the current distribution
    virtual double logNormalization() const = 0;

    //! compute the log-normalization based from parents
    virtual double logNormalizationParents() const = 0;

    // TODO: the log-??something?? to compute the f() function
    // virtual double computeF() const = 0;


    //! get the message to all the children. TODO: the same as `moments()`
    Moments<TDistribution> messageToChildren() const { return moments(); }

    //! obtain a message from a child
    // TODO: make a check that there is indeed such a child
    virtual void receiveFromChild(const Parameters<TDistribution> &msg, BaseVariable *child)
    {
        // TODO: check if this is correct
        m_observed = false;
        map_insert(m_childMsgs, make_pair(child->id(), msg));
    }

    //! updating the posterior w.r.t to the current messages
    virtual void updatePosterior()
    {
        assert(!isObserved());
        // for all the children,
        m_params = parametersFromParents();
        for (ChildIter it = m_childMsgs.begin(); it != m_childMsgs.end(); ++it)
            m_params += it->second;
    }

protected:
    //! flag whether the variable is observed or not
    bool m_observed;

    //! current parameters of the approximate posterior
    Parameters<TDistribution> m_params;

    // messages received from children
    std::map<size_t, Parameters<TDistribution> > m_childMsgs;
    typedef typename map<size_t, Parameters<TDistribution> >::iterator ChildIter;
};



/**
 * variable taking only continuous values
 */
template <typename TDistribution>
class ContinuousVariable : public Variable<TDistribution>
{
public:
    //! observe a scalar value TODO: make a template/vector version?
    virtual void observe(double _value)
    {
        this->m_observed = true;
        m_value = _value;
    }

    // add a method to derive random samples
    // virtual double sample() = 0;

    //! compute the pdf value fpor the given value
    virtual double logProbabilityDensity(double value) const = 0;

    //! get the value in case it is observed
    // TODO: is it necessary? maybe just use moments()
    inline double value() const { return m_value; }

protected:
    //! stored value in case it is observed
    double m_value;
};



/**
 * mixin that specifies the parent-child relationship
 */
template <typename TParent>
class HasParent
{
public:
    // TODO: here there is no need to have the second parameter,
    // since it is assumed that there is only one parent of this type
    // mb introduce yet another class like HasMultipleParents
    virtual void receiveFromParent(const Moments<TParent> &ms, TParent *parent) = 0;
    virtual Parameters<TParent> messageToParent(TParent *parent) const = 0;
};









};

#endif // VARIABLE_H
