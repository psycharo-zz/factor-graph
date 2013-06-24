#ifndef GAMMAARRAY_H
#define GAMMAARRAY_H

#include <gamma.h>



namespace vmp
{


/**
 * this gamma can't have any parents: TODO: introduce a more-or-less decent prior (even a fake one would go)
 */
class GammaArray : public VariableArray<Gamma>
{
public:
    GammaArray(size_t _size, const TParameters &_prior):
        VariableArray<Gamma>(_size, _prior),
        m_priors(_size, _prior)
    {}

    GammaArray(const TParamsVector &_priors):
        VariableArray<Gamma>(_priors),
        m_priors(_priors)
    {}

//    void messageToParent();

    //! override VariableArray
    inline TParameters parametersFromParents(size_t idx) const { return m_priors[idx]; }

    //! override VariableArray
    void updateMoments()
    {
        for (size_t i = 0; i < size(); ++i)
            Gamma::updateMoments(m_moments[i], m_parameters[i]);
    }

    //! override VariableArray
    inline double logNormalization() const
    {
        double result = 0.0;
        for (size_t i = 0; i < size(); ++i)
            result += Gamma::logNormalization(m_parameters[i]);
        return result;
    }

    //! override Variable
    inline double logNormalizationParents() const
    {
        double result = 0.0;
        for (size_t i = 0; i < size(); ++i)
            result += Gamma::logNormalizationParents(m_priors[i].shape, m_priors[i].rate);
        return result;
    }
private:
    TParamsVector m_priors;

};


}

#endif // GAMMAARRAY_H
