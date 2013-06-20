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
    Moments<Gamma>() {}

    Moments<Gamma>(double _precision, double _logPrecision):
        precision(_precision),
        logPrecision(_logPrecision)
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
class Gamma: public ContinuousVariable<Gamma>
{
public:
    Gamma(double shape, double rate):
        m_shapeMsg(shape),
        m_rateMsg(rate)
    {
        updatePosterior();
    }

    virtual ~Gamma() {}

    //! override Variable
    inline bool hasParents() const { return false; }


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
        throw std::runtime_error("Gamma::logProbabilityDensity(double): not implemented");
    }

    //! override Variable. [a/b, (ln(Gamma(a)))' - log(b)]
    inline Moments<Gamma> updatedMoments() const
    {
        return Moments<Gamma>(parameters().shape / parameters().rate,
                              digamma(parameters().shape) - log(parameters().rate));
    }

    //! override HasForm<Gamma>. simply a constant for gamma distribution
    Parameters<Gamma> parametersFromParents() const
    {
        return Parameters<Gamma>(m_shapeMsg, m_rateMsg);
    }

private:
    // current messages received from both parents
    // TODO: introduce a fake const distribution
    const double m_shapeMsg;
    const double m_rateMsg;
};

/**
 * "fake" gamma distribution that does not update its parameters
 * allows doing message passing with constant
 */
class ConstGamma : public Gamma
{
};


}



#endif // GAMMA_H
