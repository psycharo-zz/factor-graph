#ifndef GAUSSIANARRAY_H
#define GAUSSIANARRAY_H

#include <variable.h>
#include <gamma.h>
#include <gaussian.h>


namespace vmp
{


/**
 * an array version
 */
template <typename TMeanParent, typename TPrecParent>
class GaussianArray : public VariableArray<Gaussian>,
                      public HasParent<TMeanParent>,
                      public HasParent<TPrecParent>
{
public:
    // TODO: derive dimensions for arrays?
    GaussianArray(size_t _size, TMeanParent *mean, TPrecParent *precision):
        VariableArray<Gaussian>(_size),
        m_meanParent(mean),
        m_precParent(precision)
    {}

    //! initialise with given parameters
    GaussianArray(const TParameters &_params, TMeanParent *mean, TPrecParent *precision):
        VariableArray<Gaussian>(_params),
        m_meanParent(mean),
        m_precParent(precision)
    {}

    //! TODO: generic?
    inline void observe(const vector<double> &values)
    {
        for (size_t i = 0; i < size(); ++i)
            m_moments[i] = TBaseMoments::fromValue(values[i]);
        m_observed = true;
    }

    //! override VariableArray. implementations below
    virtual double logNormParents() const;

    //! override VariableArray. see implemtations below
    virtual TBaseParameters parametersFromParents(size_t idx) const;

    //! override VariableArray
    vector<double> logProbabilityDensity(const vector<TBaseMoments> &_moments) const { throw NotImplementedException; }

    // TODO: shouldn't this have some default implementation?
    //! override HasParent<TMeanParent>
    void messageToParent(Parameters<TMeanParent> *params) const;
    //! override HasParent<TPrecParent>
    void messageToParent(Parameters<TPrecParent> *params) const;

protected:
    const TMeanParent *m_meanParent;
    const TPrecParent *m_precParent;
};



// log probabilities
template<>
inline vector<double> GaussianArray<Gaussian, Gamma>::logProbabilityDensity(const vector<TBaseMoments> &_moments) const
{
    throw NotImplementedException;
}


// below follow implementations for various types of parents


// getting parameters from parents
template <>
inline Parameters<Gaussian> GaussianArray<Gaussian, Gamma>::parametersFromParents(size_t idx) const
{
    return Gaussian::parametersFromParents(m_meanParent->moments(), m_precParent->moments());
}

template <>
inline Parameters<Gaussian> GaussianArray<VariableArray<Gaussian>, VariableArray<Gamma> >::parametersFromParents(size_t idx) const
{
    return Gaussian::parametersFromParents(m_meanParent->moments(idx), m_precParent->moments(idx));
}


// computing log evidence
template <>
inline double GaussianArray<Gaussian, Gamma>::logNormParents() const
{
    double result = 0.0;
    for (size_t i = 0; i < size(); ++i)
        result += Gaussian::logNormParents(m_meanParent->moments(), m_precParent->moments());
    return result;
}


template <>
inline double GaussianArray<VariableArray<Gaussian>, VariableArray<Gamma> >::logNormParents() const
{
    double result = 0.0;
    for (size_t i = 0; i < size(); ++i)
        result += Gaussian::logNormParents(m_meanParent->moments(i), m_precParent->moments(i));
    return result;
}





// messages
template<>
inline void GaussianArray<Gaussian, Gamma>::messageToParent(Parameters<Gaussian> *params) const
{
    const Moments<Gamma> &msg = m_precParent->moments();
    params->meanPrecision = 0;
    params->precision = msg.precision * size();
    for (size_t i = 0; i < size(); ++i)
        params->meanPrecision += moments(i).mean * msg.precision;
}

// from a vector to a _single gamma_ parent having _single gaussian_ parent
template<>
inline void GaussianArray<Gaussian, Gamma>::messageToParent(Parameters<Gamma> *params) const
{
    const Moments<Gaussian> &msg = m_meanParent->moments();
    params->shape = 0.5 * size();
    params->rate = 0;
    for (size_t i = 0; i < size(); ++i)
        params->rate += (sqr(moments(i).mean) - 2 * moments(i).mean2 * msg.mean + msg.mean2);
    params->rate *= 0.5;
}


template<>
inline void GaussianArray<VariableArray<Gaussian>, Gamma>::messageToParent(VariableArray<Gaussian>::TParameters *v) const
{
    const Moments<Gamma> &msg = m_precParent->moments();
    assert(v->params.size() == size());
    for (size_t i = 0; i < size(); ++i)
    {
        v->params[i].precision = msg.precision;
        v->params[i].meanPrecision = moments(i).mean * msg.precision;
    }
}

// from a vector to _vector gamma_ having _single gaussian_ parent
template<>
inline void GaussianArray<Gaussian, VariableArray<Gamma> >::messageToParent(VariableArray<Gamma>::TParameters *v) const
{
    const Moments<Gaussian> &msg = m_meanParent->moments();
    assert(v->params.size() == size());
    for (size_t i = 0; i < size(); ++i)
    {
        v->params[i].shape = 0.5;
        v->params[i].rate = 0.5 * (sqr(moments(i).mean) - 2 * moments(i).mean2 * msg.mean + msg.mean2);
    }
}


// from a vector to _vector gaussian_, both parents are vectors
template<>
inline void GaussianArray<VariableArray<Gaussian>, VariableArray<Gamma> >::messageToParent(VariableArray<Gaussian>::TParameters *v) const
{
    const vector<GammaMoments> &msgs = m_precParent->moments();
    assert(v->params.size() == msgs.size());
    for (size_t i = 0; i < size(); ++i)
    {
        v->params[i].precision = msgs[i].precision;
        v->params[i].meanPrecision = moments(i).mean * msgs[i].precision;
    }
}

// from a vector to _vector gamma_, both parents are vectors
template<>
inline void GaussianArray<VariableArray<Gaussian>, VariableArray<Gamma> >::messageToParent(VariableArray<Gamma>::TParameters *v) const
{
    const vector<Gaussian::TMoments> &msgs = m_meanParent->moments();
    assert(v->params.size() == msgs.size());
    for (size_t i = 0; i < size(); ++i)
    {
        v->params[i].shape = 0.5;
        v->params[i].rate = 0.5 * (sqr(moments(i).mean) - 2 * moments(i).mean2 * msgs[i].mean + msgs[i].mean2);
    }
}



} // namespace vmp


#endif // GAUSSIANARRAY_H
