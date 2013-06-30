#ifndef WISHARTVARIABLE_H
#define WISHARTVARIABLE_H

#include <variable.h>
#include <mathutil.h>

#include <armadillo>

using namespace arma;


namespace vmp
{



class Wishart;

template<>
class Parameters<Wishart>
{
public:
    Parameters()
    {}

    Parameters(size_t size):
        degrees(size),
        scale(size, size)
    {}

    Parameters(double _degrees, const mat &_scale):
        degrees(_degrees),
        scale(trimatu(_scale))
    {
        assert(scale.is_square() && _degrees >= scale.n_rows);
    }

    inline size_t dims() const { return scale.n_rows; }

    inline void clear()
    {
        degrees = 0;
        scale.fill(0);
    }


    Parameters &operator+=(const Parameters &other)
    {
        degrees += other.degrees;
        scale += other.scale;
        return *this;
    }

    Parameters &operator*=(double value)
    {
        degrees *= value;
        scale *= value;
        return *this;
    }

    double degrees;
    mat scale;
};


template<>
class Moments<Wishart>
{
public:
    Moments(const Parameters<Wishart> &ps)
    {
        Moments::fromParameters(*this, ps);
    }

    Moments(const mat &_prec, double _logDet):
        prec(trimatu(_prec)),
        logDet(_logDet)
    {}

    inline size_t dims() const { return prec.n_rows; }

    inline static Moments fromValue(const mat &_prec)
    {
        return Moments(_prec, log(det(_prec)));
    }

    inline static void fromParameters(Moments &moments, const Parameters<Wishart> &params)
    {
        const size_t K = moments.dims();
        const mat phi0 = trimatu(-0.5 * params.scale);
        const double phi1 = 0.5 * params.degrees;

        moments.prec = -phi1 * inv(phi0);
        moments.logDet = -log(det(-phi0)) + digamma_d(phi1, K);
    }


    mat prec;
    double logDet;
};



inline double operator*(const Parameters<Wishart> &params, const Moments<Wishart> &moments)
{
    throw NotImplementedException;
    return 0;
}



class Wishart : public Variable<Wishart>
{
public:
    // TODO: implement fake parents for wishart distribution?
    Wishart(double _degrees, const mat &_scale):
        Variable(TParameters(_degrees, _scale)),
        m_prior(_degrees, _scale)
    {}

    //! override Variable
    inline virtual TParameters parametersFromParents() const { return m_prior; }

    //! compute the log-normalization based from parents
    virtual double logNormParents() const { throw NotImplementedException; }

    //! compute the log-probability function
    virtual double logPDF(const TMoments &moments) const { throw NotImplementedException; }

    static inline double logNorm(const TParameters &params) { throw NotImplementedException; }

    static inline double logNormParents(double degrees, const mat &scale)
    {
        throw NotImplementedException;
    }

protected:
    // TODO: 'too complex
    Wishart(const TMoments &_moments):
        Variable(TParameters(_moments.dims()), _moments),
        m_prior(_moments.dims())
    {}

    TParameters m_prior;
};


class ConstWishart : public Wishart
{
public:
    ConstWishart(const mat &prec):
        Wishart(TMoments::fromValue(prec))
    {}
    //! override Variable
    // TODO: make a mixin that does this?
    void updatePosterior() { throw NotImplementedException; }
};


class WishartArray : public VariableArray<Wishart>
{
public:
    WishartArray(size_t _size, const TBaseParameters &_prior):
        VariableArray<Wishart>(_size, _prior),
        m_prior(_size, _prior)
    {}

    WishartArray(const TParameters &_prior):
        VariableArray<Wishart>(_prior),
        m_prior(_prior)
    {}

    //! override VariableArray
    inline TBaseParameters parametersFromParents(size_t idx) const { return m_prior[idx]; }

    //! override Variable
    inline double logNormParents() const
    {
        double result = 0.0;
        for (size_t i = 0; i < size(); ++i)
            result += TBase::logNormParents(m_prior[i].degrees, m_prior[i].scale);
        return result;
    }
protected:
    TParameters m_prior;
};




} // namespace vmp

#endif // WISHARTVARIABLE_H
