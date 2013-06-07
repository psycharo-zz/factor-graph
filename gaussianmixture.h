#ifndef GAUSSIANMIXTURE_H
#define GAUSSIANMIXTURE_H



#include <vector>
#include <cassert>
using namespace std;


#include <variable.h>
#include <gaussian.h>
#include <discrete.h>


namespace vmp
{


class GaussianMixture;


template<>
class Parameters<GaussianMixture>
{
public:

    Parameters()
    {}

    Parameters(const vector<GaussianParameters> &_elements,
               const vector<double> &_weights)
    {
    }


    Parameters &operator+=(const Parameters &other)
    {
        throw std::runtime_error("Parameters<Mixture>::operator+=(): not implemented");
    }

    vector<double> weights;
    vector<GaussianParameters> mixtures;

};


template<>
class Moments<GaussianMixture>
{
public:


};


/**
 * variable distributed as a mixture of (univariate gaussian) distributions
 * it has N
 */
class GaussianMixture : public ContinuousVariable,
                public HasForm<Mixture>,
                public HasParent<Discrete>,
                public HasParent<Gaussian>,
                public HasParent<Gamma>
{
public:
    GaussianMixture(const vector<Gaussian*> &_meanElements,
                    const vector<Gamma*> &_precElements,
                    Discrete *_weight):
        m_meanPars(_meanElements),
        m_weight(_weight),
        m_weightMsg(_weight->dims())
    {
        assert(_meanElements.size() == _precElements.size() &&
               _meanElements.size() == _weight->dims());

    }


    //! get the number of mixtures
    inline size_t numMixtures() const { return m_elements.size(); }

    //! override Variable. in the current distribution
    inline bool hasParents() const { return true; }

    //! override Variable
    double logEvidenceLowerBound() const
    {
        throw std::runtime_error("Mixture::logEvidenceLowerBound(): not implemented");
    }

    //! override HasForm<Mixture>
    Moments<Mixture> moments() const
    {
        throw std::runtime_error("Mixture::moments(): not implemented");

        if (isObserved())
        {

        }
        else
        {

        }
    }

    //! override HasForm<Mixture>
    Parameters<Mixture> parametersFromParents() const
    {
        throw std::runtime_error("Mixture::parametersFromParents(): not implemented");
    }


    //! override HasForm<Mixture>
    void updatePosterior()
    {
        throw std::runtime_error("Mixture::updatePosterior(): not implemented");
    }

    //! override HasParent<Discrete>
    void receiveFromParent(const Moments<Discrete> &msg, Discrete *parent)
    {
        m_weightMsg = msg;
    }

    //! override HasParent<Discrete>
    Parameters<Discrete> messageToParent(Discrete *parent) const
    {
        DiscreteParameters params(numMixtures());
        double value = isObserved() ? m_value :
                                      throw std::runtime_error("Mixture::messageToParent(): non-observed not supported yet");
        // TODO: when non-observed it actually might depend on mean AND mean2, not only on the value itself
        for (size_t m = 0; m < numMixtures(); ++m)
           params.logProb[m] = m_elements[m].logProbabilityDensity(value);
        return params;
    }

    //! override HasParent<Gaussian>
    void receiveFromParent(const Moments<Gaussian> &msg, Gaussian *parent)
    {
        map_insert(m_meanMsgs, make_pair(parent->id(), msg));
    }

    //! override HasParent<Gaussian>
    Parameters<Gaussian> messageToParent(Gaussian *parent) const
    {
        Parameters<Gaussian> result;

        return result;
    }


    //! override HasParent<Gamm>
    void receiveFromParent(const Moments<Gamma> &msg, Gamma *parent)
    {
        map_insert(m_precMsgs, make_pair(parent->id(), msg));
    }

    //! override HasParent<Gaussian>
    Parameters<Gamma> messageToParent(Gamma *parent) const
    {
        Parameters<Gamma> result;

        return result;
    }



private:

    // TODO: make a map instead of vector?
    //! parents. TODO: is it necessary to store these separately?
    vector<Gaussian*> m_meanPars;
    vector<Gamma*> m_precPars;

    //! the mixture distributions themselves
    vector<Gaussian> m_elements;

    //! the weight prior
    Discrete *m_weight;

    //! message from the discrete parent
    DiscreteMoments m_weightMsg;
    //! all the messages from the mean variables
    map<size_t, GaussianMoments> m_meanMsgs;
    typedef map<size_t, GaussianMoments>::iterator MeanMsgIt;

    //! all the messages from the precision variables
    map<size_t, GammaMoments> m_precMsgs;
    typedef map<size_t, GammaMoments>::iterator GammaMsgIt;

};


}

#endif // GAUSSIANMIXTURE_H
