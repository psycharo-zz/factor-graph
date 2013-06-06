#ifndef MIXTURE_H
#define MIXTURE_H


#include <vector>
using namespace std;


#include <variable.h>
#include <gaussian.h>
#include <discrete.h>


namespace vmp
{


class Mixture;


template<>
class Parameters<Mixture>
{
public:
    Parameters &operator+=(const Parameters &other)
    {
        throw std::runtime_error("Parameters<Mixture>::operator+=(): not implemented");
    }

    vector<double> weights;
    vector<GaussianParameters> mixtures;

};


template<>
class Moments<Mixture>
{
public:

};






/**
 * variable distributed as a mixture of (univariate gaussian) distributions
 */
class Mixture : public ContinuousVariable,
                public HasForm<Mixture>,
                public HasParent<Discrete>,
                public HasParent<Gaussian>
{
public:
    Mixture(const vector<Gaussian*> &_elements, Discrete *_weight):
        m_elements(_elements),
        m_weight(_weight)
    {}


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
    void receiveFromParent(const Moments<Discrete> &ms, Discrete *parent) = 0;

    //! override HasParent<Discrete>
    Parameters<Discrete> messageToParent(Discrete *parent) const = 0;

    //! override HasParent<Gaussian>
    void receiveFromParent(const Moments<Gaussian> &ms, Gaussian *parent) = 0;

    //! override HasParent<Gaussian>
    Parameters<Gaussian> messageToParent(Gaussian *parent) const = 0;




private:
    //! the mixture distributions themselves
    // TODO: make a map instead of vector?
    vector<Gaussian*> m_elements;
    //! the weight prior
    Discrete *m_weight;


    //! message from the discrete parent


    //! all the messages from the
};





}


#endif // MIXTURE_H
