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
 * TBase - the type
 */
template <typename TBase>
class IsMixture : public ContinuousVariable<TBase>,
                  public HasParent<Discrete>
{
public:
    IsMixture(Discrete *_discrete):
        m_components(_discrete->dims(), NULL),
        m_discretePar(_discrete),
        m_weightMsg(_discrete->dims())
    {
        assert(m_components.size() == _discrete->dims());
    }

    //! get the number of mixtures
    inline size_t numComponents() const { return m_components.size(); }

    //! get the weight of the specified mixture component
    inline double componentWeight(size_t idx) const { return m_weightMsg.probs[idx]; }

    //! get mixture component by its index
    inline TBase *component(size_t idx) const { return m_components[idx]; }


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
    double logNormalization() const
    {
        throw std::runtime_error("Mixture::logNormalization(): not implemented");
    }

    //! override Variable
    double logNormalizationParents() const
    {
        throw std::runtime_error("Mixture::logNormalization(): not implemented");
    }


    //! override ContinuousVariable
    virtual double logProbabilityDensity(double /*value*/) const
    {
        throw std::runtime_error("IsMixture:;logProbabilityDensity(double): not implemented");
    }

    //! override HasParent<Discrete>
    void receiveFromParent(const Moments<Discrete> &msg, Discrete *parent)
    {
        assert(m_discretePar == parent);
        m_weightMsg = msg;
    }

    //! override HasParent<Discrete>
    Parameters<Discrete> messageToParent(Discrete *parent) const
    {
        assert(m_discretePar == parent);

        DiscreteParameters params(numComponents());
        double value = this->isObserved() ? this->m_value :
                                            throw std::runtime_error("Mixture::messageToParent(): non-observed not supported yet");
        // TODO: when non-observed it actually might depend on mean AND mean2, not only on the value itself
        for (size_t m = 0; m < numComponents(); ++m)
           params.logProb[m] = m_components[m]->logProbabilityDensity(value);

        return params;
    }

    //! override HasForm<TBase>
    virtual Moments<TBase> moments() const
    {
        throw std::runtime_error("IsMixture::moments(): not implemented");
    }

    //! override HasForm<TBase>
    virtual Parameters<TBase> parametersFromParents() const
    {
        throw std::runtime_error("IsMixture::parametersFromParents(): not implemented");
    }


// DEBUG
//private:
    //! components
    vector<TBase*> m_components;
    //! the discrete distribution
    Discrete *m_discretePar;
    //! message from the discrete parent
    DiscreteMoments m_weightMsg;


};

template <typename TBase, typename TParent>
class HasMixtureParent : public virtual IsMixture<TBase>
{
public:
    HasMixtureParent<TBase, TParent>(Discrete *_discr):
        IsMixture<TBase>(_discr)
    {}

    void receiveFromParent(size_t idx, const Moments<TParent> &ms, TParent *parent)
    {
        this->component(idx)->receiveFromParent(ms, parent);
    }

    Parameters<TParent> messageToParent(size_t idx, TParent *parent) const
    {
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
        for (size_t m = 0; m < numComponents(); ++m)
            m_components[m] = new Gaussian(_meanPars[m], _precPars[m]);


    }

    using IsMixture<Gaussian>::m_components;

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
