#ifndef GAMMA_H
#define GAMMA_H

#include <factor.h>
#include <variable.h>


namespace vmp
{



/**
 * @brief The GammaParameters - stores Gamma distribution (natural) parameters
 * Typically is used to propagate updates from children to parents
 */
class GammaParameters
{
public:
    GammaParameters(double _rate, double _shape):
        rate(_rate),
        shape(_shape)
    {}

    double rate;
    double shape;
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
class GammaMoments
{
public:
    GammaMoments(double _precision, double _logPrecision):
        precision(_precision),
        logPrecision(_logPrecision)
    {}

    double precision;
    double logPrecision;
};





class Gamma: public Variable
{
public:
    Gamma(double shape, double rate):
        m_expShape(shape),
        m_expRate(rate)
    {}


    //! get a vector of natural parameters
    Message natural() const
    {
        return Message(0, 0);
    }


private:
    double m_expShape;
    double m_expRate;
};


}



#endif // GAMMA_H
