#ifndef GAMMA_H
#define GAMMA_H

#include <variable.h>
#include <mathutil.h>


#include <map>
#include <stdexcept>
using namespace std;

namespace vmp
{

class Gamma;

/**
 * @brief The GammaParameters - stores Gamma distribution (natural) parameters
 * Typically is used to propagate updates from children to parents
 */
template <>
class Parameters<Gamma>
{
public:
    Parameters()
    {}

    Parameters(double _shape, double _rate):
        shape(_shape),
        rate(_rate)
    {}


    inline void clear()
    {
        shape = 0;
        rate = 0;
    }

    Parameters &operator+=(const Parameters &other)
    {
        shape += other.shape;
        rate += other.rate;
        return *this;
    }

    Parameters &operator*=(double value)
    {
        shape *= value;
        rate *= value;
        return *this;
    }

    double shape;
    double rate;
};

inline ostream &operator<<(ostream &out, const Parameters<Gamma> &params)
{
    out << "Gamma(" << params.shape << "," << params.rate << ")";
    return out;
}

inline Parameters<Gamma> operator*(const Parameters<Gamma> &params, double val)
{
    return Parameters<Gamma>(params.shape * val, params.rate * val);
}


inline Parameters<Gamma> operator+(const Parameters<Gamma> &a,
                                   const Parameters<Gamma> &b)
{
    return Parameters<Gamma>(a.shape + b.shape, b.rate + b.rate);
}

inline Parameters<Gamma> operator-(const Parameters<Gamma> &a,
                                   const Parameters<Gamma> &b)
{
    return Parameters<Gamma>(a.shape - b.shape, b.rate - b.rate);
}
typedef Parameters<Gamma> GammaParameters;






/**
 * @brief The GammaMoments - stores expectation of the natural statistics vector
 * for the Gamma distribution.
 * Typically is used to propagate updates from parents to children.
 *
 * Note that the expectations are nothing that numbers, so the "precision" here is
 * more-or-less artificial, simply because in the framework it only is used as a prior
 * for the gaussian precision.
 */
template<>
class Moments<Gamma>
{
public:
    Moments(double _precision, double _logPrecision):
        precision(_precision),
        logPrecision(_logPrecision)
    {}

    Moments(const Parameters<Gamma> &params)
    {
        fromParameters(*this, params);
    }

    static void fromParameters(Moments &ms, Parameters<Gamma> const &ps)
    {
        ms.precision = ps.shape / ps.rate;
        ms.logPrecision = digamma(ps.shape) - log(ps.rate);
    }

    double precision;
    double logPrecision;
};

typedef Moments<Gamma> GammaMoments;


// dot product
inline double operator*(const Parameters<Gamma> &params,
                        const Moments<Gamma> &moments)
{
    return -params.rate * moments.precision
           +(params.shape - 1) * moments.logPrecision;
}





/**
 * @brief Gamma - represents variable distributed according to the
 * univariate gamma distribution.
 * TODO: add references to all the distributions (e.g. paper/wikipedia article)
 */
class Gamma: public Variable<Gamma>
{
public:
    Gamma(double _shape, double _rate):
        Variable(TParameters(_shape, _rate)),
        m_shapeMsg(_shape),
        m_rateMsg(_rate)
    {
        updatePosterior();
    }

    virtual ~Gamma() {}

    //! override Variable
    double logNorm() const
    {
        return Gamma::logNorm(parameters());
    }

    //! override Variable
    double logNormParents() const
    {
//        Parameters<Gamma> params = parametersFromParents();
        return Gamma::logNormParents(m_shapeMsg, m_rateMsg);
    }

    //! override ContinuousVariable
    double logPDF(const TMoments &/*ms*/) const { throw NotImplementedException; }

    //! override Variable. simply a constant for gamma distribution
    inline TParameters parametersFromParents() const
    {
        return TParameters(m_shapeMsg, m_rateMsg);
    }

    //! override Variable
    inline static double logNorm(const TParameters &params)
    {
        return params.shape * log(params.rate) - gammaln(params.shape);
    }

    //! override Variable
    inline static double logNormParents(double shape, double rate)
    {
        return shape * log(rate) - gammaln(shape);
    }


protected:
    Gamma(const TMoments &moments):
        Variable(TParameters(), moments),
        m_shapeMsg(0),
        m_rateMsg(0) // TODO: magic numbers?
    {}


    // current messages received from both parents
    // TODO: introduce a fake const distribution
    const double m_shapeMsg;
    const double m_rateMsg;


};



class ConstGamma : public Gamma
{
public:
    ConstGamma(double _prec):
        Gamma(Moments<Gamma>(_prec, log(_prec)))
    {}
};







} // namespace vmp



#endif // GAMMA_H
