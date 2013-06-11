#ifndef MIXTURE_H
#define MIXTURE_H


#include <vector>
#include <cassert>
using namespace std;


#include <variable.h>
#include <gaussian.h>
#include <discrete.h>


namespace vmp
{

/**
 * base abstract class to implement mixture models
 * TDistribution - the type
 */
template <typename TDistribution>
class IsMixture : public ContinuousVariable<TDistribution>,
                  public HasParent<Discrete>
{
public:
    IsMixture(Discrete *_discrete):
        m_components(_discrete->dims(), NULL),
        m_discretePar(_discrete),
        m_weightMsg(_discrete->dims())
    {
        assert(_discrete->dims() > 0);
        assert(m_components.size() == _discrete->dims());
    }

    //! get the number of mixtures
    inline size_t numComponents() const { return m_components.size(); }

    //! get the weight of the specified mixture component
    inline double componentWeight(size_t idx) const { return m_weightMsg.probs[idx]; }

    //! get mixture component by its index
    inline TDistribution *component(size_t idx) const { return m_components[idx]; }

    //! override ContinuousVariable
    inline void observe(double _value)
    {
        for (size_t m = 0; m < numComponents(); ++m)
            m_components[m]->observe(_value);
        this->m_observed = true;
        this->m_value = _value;
    }

    //! override Variable. in mixture we always have parents
    inline bool hasParents() const { return true; }

    //! override Variable
    Moments<TDistribution> moments() const
    {
        // simply getting the value of one of the components
        // FIXME: only observed mixtures for now
        assert(this->isObserved());
        return Moments<TDistribution>(this->m_value, sqr(this->m_value));
    }

    //! override Variable
    Parameters<TDistribution> parametersFromParents() const
    {
        Parameters<TDistribution> params;
        for (size_t m = 0; m < numComponents(); ++m)
            params += component(m)->parametersFromParents() * componentWeight(m);
        return params;
    }

    //! override Variable
    // FIXME
    inline virtual double logNormalization() const
    {
        double result = 0;
        for (size_t m = 0; m < numComponents(); ++m)
            result += component(m)->logNormalization() * componentWeight(m);
        return result;
    }

    //! override Variable
    inline virtual double logNormalizationParents() const
    {
        double result = 0;
        for (size_t m = 0; m < numComponents(); ++m)
            result += component(m)->logNormalizationParents() * componentWeight(m);
        return result;
    }

    //! override ContinuousVariable
    // FIXME
    inline virtual double logProbabilityDensity(const Moments<TDistribution> &value) const
    {
        double result = 0.0;
        for (size_t m = 0; m < numComponents(); ++m)
            result += component(m)->logProbabilityDensity(value) * componentWeight(m);
        return result;
    }

    //! override HasParent<Discrete>
    inline virtual void receiveFromParent(const Moments<Discrete> &msg, Discrete *parent)
    {
        assert(m_discretePar == parent);
        m_weightMsg = msg;
    }

    //! override HasParent<Discrete>
    inline virtual Parameters<Discrete> messageToParent(Discrete *parent) const
    {
        assert(m_discretePar == parent);

        Parameters<Discrete> params(numComponents());
        Moments<TDistribution> value = moments();
        // TODO: when non-observed it actually might depend on mean AND mean2, not only on the value itself
        for (size_t m = 0; m < numComponents(); ++m)
            params.logProb[m] = component(m)->logProbabilityDensity(value);
        params.logProb -= lognorm(params.logProb);
        return params;
    }

protected:
    //! components
    vector<TDistribution*> m_components;
    //! the discrete distribution
    Discrete *m_discretePar;
    //! message from the discrete parent
    DiscreteMoments m_weightMsg;
};

template <typename TDistribution, typename TParent>
class HasMixtureParent : public virtual IsMixture<TDistribution>
{
public:
    HasMixtureParent<TDistribution, TParent>(Discrete *_discr):
        IsMixture<TDistribution>(_discr)
    {}

    void receiveFromParent(size_t idx, const Moments<TParent> &ms, TParent *parent)
    {
        assert(idx < this->numComponents());
        this->component(idx)->receiveFromParent(ms, parent);
    }

    Parameters<TParent> messageToParent(size_t idx, TParent *parent) const
    {
        assert(idx < this->numComponents());
        return this->component(idx)->messageToParent(parent) * this->componentWeight(idx);
    }

};


class MoG : public HasMixtureParent<Gaussian, Gaussian>,
            public HasMixtureParent<Gaussian, Gamma>
{
public:
    MoG(const vector<Gaussian*> _meanPars,
        const vector<Gamma*> _precPars,
        Discrete *_discr):
        IsMixture(_discr),
        HasMixtureParent<Gaussian, Gaussian>(_discr),
        HasMixtureParent<Gaussian, Gamma>(_discr)
    {
        assert(_meanPars.size() == _precPars.size() &&
               _meanPars.size() == _discr->dims());

        for (size_t m = 0; m < numComponents(); ++m)
            m_components[m] = new Gaussian(_meanPars[m], _precPars[m]);
    }

    using IsMixture<Gaussian>::receiveFromParent;
    using HasMixtureParent<Gaussian, Gaussian>::receiveFromParent;
    using HasMixtureParent<Gaussian, Gamma>::receiveFromParent;

    using IsMixture<Gaussian>::messageToParent;
    using HasMixtureParent<Gaussian, Gaussian>::messageToParent;
    using HasMixtureParent<Gaussian, Gamma>::messageToParent;


    virtual ~MoG()
    {
        for (size_t m = 0; m < numComponents(); ++m)
            delete m_components[m];
    }

};




}


#endif // MIXTURE_H
