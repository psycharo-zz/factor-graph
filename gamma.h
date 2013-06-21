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

    double shape;
    double rate;

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

    Moments(const Parameters<Gamma> &params):
        precision(params.shape / params.rate),
        logPrecision(digamma(params.shape) - log(params.rate))
    {}

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
        Variable(GammaParameters(_shape, _rate)),
        m_shapeMsg(_shape),
        m_rateMsg(_rate)
    {
        updatePosterior();
    }

    virtual ~Gamma() {}

    //! override Variable
    double logNormalization() const
    {
        return parameters().shape * log(parameters().rate) - gammaln(parameters().shape);
    }

    //! override Variable
    double logNormalizationParents() const
    {
//        Parameters<Gamma> params = parametersFromParents();
        return m_shapeMsg * log(m_rateMsg) - gammaln(m_shapeMsg);
    }

    //! override ContinuousVariable
    double logProbabilityDensity(const Moments<Gamma> &/*ms*/) const
    {
        throw NotImplementedException;
    }

    //! override Variable
    inline void updateMoments()
    {
        Gamma::updateMoments(m_moments, parameters());
    }

    //! override Variable. simply a constant for gamma distribution
    inline Parameters<Gamma> parametersFromParents() const
    {
        return Parameters<Gamma>(m_shapeMsg, m_rateMsg);
    }


    // static
    inline static void updateMoments(TMoments &moments, const TParameters &params)
    {
        moments.precision = params.shape / params.rate;
        moments.logPrecision = digamma(params.shape) - log(params.rate);
    }


    //! override Variable
    inline static double logNormalization(const TParameters &params)
    {
        return params.shape * log(params.rate) - gammaln(params.shape);
    }

    //! override Variable
    inline static double logNormalizationParents(double shape, double rate)
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



/**
 * the array version, can be used e.g. for mixtures
 */
class GammaArray : public VariableArray<Gamma>
{
public:
    GammaArray(size_t _size, const GammaParameters &prior):
        VariableArray<Gamma>(_size, prior, GammaMoments(prior)),
        m_prior(prior)
    {}

//    GammaArray(size_t _size, const TParamsVector &priors):
//    {}

    // our gamma can't have any parents
//    void messageToParent();


    //! override VariableArray
    TParameters parametersFromParents(size_t /*idx*/) const { return m_prior; }


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
        return Gamma::logNormalizationParents(m_prior.shape, m_prior.rate) * size();
    }



protected:
    // constant priors, no parents (TODO?)
    TParameters m_prior;
};




} // namespace vmp



#endif // GAMMA_H
