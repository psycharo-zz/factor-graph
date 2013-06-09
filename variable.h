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


class Variable
{
public:
    //! TODO: should it take a Network as a parameter?
    Variable():
        m_id(++s_idCounter),
        m_observed(false)
    {}

    virtual ~Variable() {};

    //! check whether the variable is observed
    inline virtual bool isObserved() const { return m_observed; }

    //! get a unique id
    inline size_t id() const { return m_id; }

    //! check whether any parents are non-constant
    virtual bool hasParents() const = 0;

    //! compute the constitute to the lower bound on the log-evidence
    virtual double logEvidenceLowerBound() const = 0;


protected:
    //! unique identifier
    size_t m_id;

    //! flag whether the variable is observed or not
    bool m_observed;

private:
    static size_t s_idCounter;
};


/**
 * variable taking only continuous values
 */
class ContinuousVariable : public Variable
{
public:
    //! observe a scalar value TODO: make a template/vector version?
    virtual void observe(double _value)
    {
        m_observed = true;
        m_value = _value;
    }

    // add a method to derive random samples
    // virtual double sample() = 0;

    //! compute the pdf value fpor the given value
    virtual double logProbabilityDensity(double value) const = 0;

// DEBUG
//protected:
    //! stored value in case it is observed
    double m_value;
};




template <typename ParentType>
class HasParent
{
public:
    // TODO: here there is no need to have the second parameter,
    // since it is assumed that there is only one parent of this type
    // mb introduce yet another class like HasMultipleParents
    virtual void receiveFromParent(const Moments<ParentType> &ms, ParentType *parent) = 0;
    virtual Parameters<ParentType> messageToParent(ParentType *parent) const = 0;
};



/**
 * interface that specifies the form of the distribution
 */
template <typename DistributionType>
class HasForm
{
public:
    HasForm()
    {}

    HasForm(const Parameters<DistributionType> &params):
        m_params(params)
    {}

    //! get the moments TODO: the same as message to children
    virtual Moments<DistributionType> moments() const = 0;

    //! get the parameters
    virtual Parameters<DistributionType> parametersFromParents() const = 0;

    //! get the message to all the children. TODO: the same as `moments()`
    Moments<DistributionType> messageToChildren() const { return moments(); }

    //! obtain a message from a child
    // TODO: make a check that there is indeed such a child
    virtual void receiveFromChild(const Parameters<DistributionType> &msg, Variable *child)
    {
        map_insert(m_childMsgs, make_pair(child->id(), msg));
    }

    //! updating the posterior w.r.t to the current messages
    virtual void updatePosterior()
    {
        // for all the children,
        m_params = parametersFromParents();
        for (ChildIter it = m_childMsgs.begin(); it != m_childMsgs.end(); ++it)
            m_params += it->second;
    }
// DEBUG
//protected:
    //! current parameters of the approximate posterior
    Parameters<DistributionType> m_params;

    // messages received from children
    std::map<size_t, Parameters<DistributionType> > m_childMsgs;
    typedef typename map<size_t, Parameters<DistributionType> >::iterator ChildIter;

};





};

#endif // VARIABLE_H
