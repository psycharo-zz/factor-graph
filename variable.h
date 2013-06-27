#ifndef VARIABLE_H
#define VARIABLE_H

#include <vector>
#include <map>
using namespace std;



#include <util.h>

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
 * simply assigns ids
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
    typedef Parameters<TDistribution> TParameters;
    typedef Moments<TDistribution> TMoments;
    typedef TDistribution TVariableType;

    Variable():
        m_observed(false)
    {}

    Variable(const TParameters &_params, const TMoments &_moments):
        m_observed(false),
        m_params(_params),
        m_moments(_moments)
    {}

    Variable(const TParameters &_params):
        m_observed(false),
        m_params(_params),
        m_moments(_params)
    {}

    virtual ~Variable() {}

    //! check whether the variable is observed
    inline virtual bool isObserved() const { return m_observed; }

    // TODO: add other messages
    TParameters *addChild(BaseVariable *var)
    {
        // TODO: is such initialisation bad?
        pair<MessageIt, bool> it = m_messages.insert(make_pair(var->id(), parameters()));
        return &it.first->second;
    }

    //! update the moments
    virtual void updateMoments() = 0;

    //! get the updated moments
    inline virtual const TMoments &moments() const { return m_moments; }

    //! prior parameters, received from the parents
    virtual TParameters parametersFromParents() const = 0;

    //! posterior parameters of the distribution
    inline virtual const TParameters &parameters() const { return m_params; }


    //! compute the constitute to the lower bound on the log-evidence
    virtual double logEvidence() const
    {
        return isObserved() ? logEvidenceObserved() :
                              logEvidenceHidden();
    }

    //! compute LB in case the variable is observed
    double logEvidenceObserved() const
    {
        // TODO: m_params SHOULD NOT BE USED for the observed variables!
        return parametersFromParents() * moments() + logNormalizationParents();
    }

    //! compute LB in case the variable is hidden
    double logEvidenceHidden() const
    {
        // TODO: change to updatedMoments() in case it fails
        return parametersFromParents() * moments() - parameters() * moments()
               + logNormalizationParents()
               - logNormalization();
    }

    //! compute the log-normalization (the g() function) based on the current distribution
    virtual double logNormalization() const = 0;

    //! compute the log-normalization based from parents
    virtual double logNormalizationParents() const = 0;

    //! compute the log-probability function
    virtual double logProbabilityDensity(const TMoments &moments) const = 0;

    // TODO: the log-??something?? to compute the f() function
    // virtual double computeF() const = 0;

    //! updating the posterior w.r.t to the current messages
    virtual void updatePosterior()
    {
        assert(!isObserved());
        m_params = parametersFromParents();
        for (MessageIt it = m_messages.begin(); it != m_messages.end(); ++it)
            m_params += it->second;
        updateMoments();
    }



protected:
    //! flag whether the variable is observed or not
    bool m_observed;

    //! current parameters of the approximate posterior
    Parameters<TDistribution> m_params;

    //! the latest parameters retrieved from parents

    //! current values of the moments
    Moments<TDistribution> m_moments;

    //! storage for children's messages
    map<size_t, TParameters> m_messages;
    typedef typename map<size_t, TParameters>::iterator MessageIt;
};



/**
 * mixin that specifies the parent-child relationship
 * TODO: use variadic templates
 */
template <typename TParent>
class HasParent
{
public:
    // TODO: here there is no need to have the second parameter,
    // since it is assumed that there is only one parent of this type
    // mb introduce yet another class like HasMultipleParents
//    virtual void receiveFromParent(const Moments<TParent> &ms, TParent *parent) = 0;
    virtual void messageToParent(Parameters<TParent> *params) const = 0;
};



template <typename TDistribution>
class VariableArray;



// TODO: is this needed?
template <typename TDistribution>
class Parameters<VariableArray<TDistribution> >
{
public:
    typedef Parameters<TDistribution> TParameters;
    typedef VariableArray<TDistribution> TArray;

    Parameters(size_t _size, const TParameters &_params = TParameters()):
        params(_size, _params)
    {}

    Parameters(size_t _size, const vector<TParameters> &_params):
        params(_params)
    {}

    inline TParameters &operator[](size_t idx) { return params[idx]; }
    inline const TParameters &operator[](size_t idx) const { return params[idx]; }

    inline size_t size() const { return params.size(); }

    vector<TParameters> params;
};



template <typename TDistribution>
class VariableArray : public BaseVariable
{
public:
    typedef VariableArray<TDistribution> Type;
    typedef Parameters<VariableArray<TDistribution> > TParameters;

    // Base meaning base distribution
    typedef TDistribution TBase;
    typedef Parameters<TDistribution> TBaseParameters;
    typedef Moments<TDistribution> TBaseMoments;

    //! create a variable array with _uninitialized_ moments parameters. TODO: this should not be necessart
    VariableArray(size_t _size):
        m_parameters(_size),
        m_moments(_size),
        m_observed(false)
    {}

    //! create a variable array with identical initial values of parameters
    VariableArray(size_t _size, const TBaseParameters &_params):
        m_parameters(_size, _params),
        m_moments(_size, TBaseMoments(_params)),
        m_observed(false)
    {}

    //! create a variable array with different parameter values for each elements
    VariableArray(const TParameters &_params):
        m_parameters(_params),
        m_observed(false)
    {
        // TODO: funcitonal-style?
        for (size_t i = 0; i < _params.size(); ++i)
            m_moments.push_back(TBaseMoments(_params[i]));
    }



    virtual ~VariableArray() {}

    //! register as a child, returns a place to store message for this child
    TParameters *addChild(BaseVariable *var)
    {
        // TODO: is such initialisation bad?
        pair<MessageIt, bool> it = m_messages.insert(make_pair(var->id(), m_parameters));
        return &it.first->second;
    }

    //!
    inline bool isObserved() const { return m_observed; }

    //! get the number of
    inline size_t size() const { return m_moments.size(); }

    //! message 1-1
    inline const TBaseMoments &moments(size_t idx) const { return m_moments[idx]; }

    //! all the moments
    inline const vector<TBaseMoments> &moments() const { return m_moments; }

    //! update parameters for each separate distribution
    virtual void updatePosterior()
    {
        assert(!isObserved());
        // initialisation
        for (size_t i = 0; i < size(); ++i)
            m_parameters[i] = parametersFromParents(i);

        // going through all the messages
        for (MessageIt it = m_messages.begin(); it != m_messages.end(); ++it)
        {
            const TParameters &params = it->second;
            for (size_t i = 0; i < size(); ++i)
                m_parameters[i] += params[i];
        }
        // updating the moments
        updateMoments();
    }

    //! update moments
    virtual void updateMoments() = 0;

    //! get the parameters for a specific variable
    virtual TBaseParameters parametersFromParents(size_t idx) const = 0;

    //! normalization
    virtual double logNormalization() const = 0;
    virtual double logNormalizationParents() const = 0;

    //! get parameters for a specific point
    inline virtual const TBaseParameters &parameters(size_t idx) const
    {
        assert(!isObserved());
        return m_parameters.params[idx];
    }

    //! get all the parameters vector
    inline virtual const TParameters &parameters() const
    {
        assert(!isObserved());
        return m_parameters;
    }

    //! evidence lower bound
    virtual double logEvidence() const
    {
        return isObserved() ? logEvidenceObserved() : logEvidenceHidden();
    }

    double logEvidenceObserved() const
    {
        double result = 0.0;
        for (size_t i = 0; i < size(); ++i)
            result += parametersFromParents(i) * m_moments[i];
        return result + logNormalizationParents();
    }

    double logEvidenceHidden() const
    {
        // TODO: change to updatedMoments() in case it fails
        double result = 0.0;
        for (size_t i = 0; i < size(); ++i)
            result += parametersFromParents(i) * moments(i) - parameters(i) * moments(i);
        return result + logNormalizationParents() - logNormalization();
    }

    //! probability densities vector
    inline virtual vector<double> logProbabilityDensity(const TBaseMoments &_moments) const { throw NotImplementedException; }


protected:
    //! parameters TODO: should it actually have this? -only if hidden, and non-deterministic
    TParameters m_parameters;

    //! moments/observations
    vector<TBaseMoments> m_moments;

    //! observed/hidden flag
    bool m_observed;

    //! messages received from children, indexed by node ids
    map<size_t, TParameters> m_messages;
    typedef typename map<size_t, TParameters>::iterator MessageIt;


};


// TODO: continuous/discrete variables?


} // namespace vmp

#endif // VARIABLE_H
