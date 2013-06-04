#ifndef GAMMA_H
#define GAMMA_H

#include <variable.h>
#include <mathutil.h>


#include <map>
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

    double rate;
    double shape;

    Parameters operator+(const Parameters &other)
    {
        return Parameters(shape + other.shape, rate + other.rate);
    }
};




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
    Moments() {}

    Moments(double _precision, double _logPrecision):
        precision(_precision),
        logPrecision(_logPrecision)
    {}

    double precision;
    double logPrecision;
};




/**
 * @brief Gamma - represents variable distributed according to the
 * univariate gamma distribution.
 * TODO: add references to all the distributions (e.g. paper/wikipedia article)
 */
class Gamma: public Variable,
             public HasForm<Gamma>
{
public:
    Gamma(double shape, double rate):
        m_shapeMsg(shape),
        m_rateMsg(rate)
    {}


    //! compute the constitute to the lower bound on the log-evidence
    double logEvidenceLowerBound() const
    {
        throw std::runtime_error("Gamma::logEvidenceLowerBound(): not implemented");
    }


    //! [a/b, (ln(Gamma(a)))' - log(b)]
    inline Moments<Gamma> moments() const
    {
        return Moments<Gamma>(m_params.shape / m_params.rate,
                              digamma(m_params.shape) - log(m_params.rate));
    }

    //! simply a constant for gamma distribution
    Parameters<Gamma> parametersFromParents() const
    {
        return Parameters<Gamma>(m_shapeMsg, m_rateMsg);
    }

    //! no distributions allowed for gamma
    inline bool hasParents() const { return false; }

private:
    // current messages received from both parents
    double m_shapeMsg;
    double m_rateMsg;
};


}



#endif // GAMMA_H
