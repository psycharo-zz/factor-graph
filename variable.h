#ifndef VARIABLE_H
#define VARIABLE_H

#include <vector>
#include <map>
#include <stdexcept>
using namespace std;


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

    //! check whether any parents are non-constant
    virtual bool hasParents() const = 0;

    //! compute the constitute to the lower bound on the log-evidence
    virtual double logEvidenceLowerBound() const = 0;

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


template <typename ParentType>
class HasParent
{
public:
    virtual void receiveFromParent(const Moments<ParentType> &ms, ParentType *parent) = 0;
    virtual Parameters<ParentType> messageToParent(ParentType *parent) const = 0;

};


/**
 * interface that specifies the form of the distribution
 */
template <typename MomentType>
class HasForm
{
public:
    //! get the message to all the children
    virtual Moments<MomentType> messageToChildren() const { return moments(); }

    //! obtain a message from a child
    virtual void receiveFromChild(const Parameters<MomentType> &msg, Variable *child)
    {
        pair<ChildIter, bool> res = m_childMsgs.insert(make_pair(child->id(), msg));
        if (!res.second)
            res.first->second = msg;
    }

    //! get the moments TODO: the same as message to children
    virtual Moments<MomentType> moments() const = 0;

    //! get the parameters
    virtual Parameters<MomentType> parametersFromParents() const = 0;

    //! updating the posterior w.r.t to the current messages
    void updatePosterior()
    {
        // for all the children,
        m_params = parametersFromParents();
        for (ChildIter it = m_childMsgs.begin(); it != m_childMsgs.end(); ++it)
            m_params = m_params + it->second;
    }

protected:
    //! current parameters of the approximate posterior
    Parameters<MomentType> m_params;

    // messages received from children
    std::map<size_t, Parameters<MomentType> > m_childMsgs;
    typedef typename map<size_t, Parameters<MomentType> >::iterator ChildIter;

};





};

#endif // VARIABLE_H
