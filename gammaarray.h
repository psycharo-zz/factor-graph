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
    GammaArray(size_t _size, const TBaseParameters &_prior):
        VariableArray<Gamma>(_size, _prior),
        m_priors(_size, _prior)
    {}

    GammaArray(const TParameters &_priors):
        VariableArray<Gamma>(_priors),
        m_priors(_priors)
    {}

    //! override VariableArray
    inline TBaseParameters parametersFromParents(size_t idx) const { return m_priors[idx]; }

    //! override Variable
    inline double logNormParents() const
    {
        double result = 0.0;
        for (size_t i = 0; i < size(); ++i)
            result += TBase::logNormParents(m_priors[i].shape, m_priors[i].rate);
        return result;
    }
private:
    TParameters m_priors;
};


}

#endif // GAMMAARRAY_H
