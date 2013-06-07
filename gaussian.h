#ifndef GAUSSIAN_H
#define GAUSSIAN_H

#include <vector>
#include <map>

using namespace std;

#include <variable.h>
#include <mathutil.h>
#include <gamma.h>

namespace vmp
{


class Gaussian;

/**
 * represents parameters of a univariate Gaussian distribution.
 * Typically used to propagate updates from children
 */
template <>
class Parameters<Gaussian>
{
public:
    Parameters() {};
    // TODO: this constructor should not be necessary?
    Parameters(double mp, double p):
        meanPrecision(mp),
        precision(p)
    {}

    double meanPrecision;
    double precision;

    Parameters &operator+=(const Parameters &other)
    {
        meanPrecision += other.meanPrecision;
        precision += other.precision;
        return *this;
    }
};

typedef Parameters<Gaussian> GaussianParameters;



/**
 * stores expectation of the natural statistics vector
 * Typically used to propagate updates from parents to the children.
 */
template <>
class Moments<Gaussian>
{
public:
    // TODO: is this constructor necessary?
    Moments<Gaussian>()
    {}

    Moments<Gaussian>(double _mean, double _mean2):
        mean(_mean),
        mean2(_mean2)
    {}

    double mean;
    double mean2;
};

typedef Moments<Gaussian> GaussianMoments;


class Gaussian: public ContinuousVariable,
                public HasParent<Gaussian>,
                public HasParent<Gamma>,
                public HasForm<Gaussian>
{
public:
    Gaussian(double _mean, double _prec):
        m_meanPar(NULL),
        m_precPar(NULL),
        m_meanMsg(_mean, sqr(_mean)),
        m_precMsg(_prec, log(_prec))
    {}

    Gaussian(Gaussian *_meanParent, Gamma *_precParent):
        m_meanPar(_meanParent),
        m_precPar(_precParent)
    {}

    virtual ~Gaussian() {}

    //! check if there are any non-constant parents
    virtual bool hasParents() const
    {
        // TODO: maybe introduce a separate flag variable or even ask
        // whether a particular parent is present or not
        return (m_meanPar != NULL) && (m_precPar != NULL);
    }

    //! get message from mean parent
    virtual void receiveFromParent(const Moments<Gaussian> &msg, Gaussian */*parent*/)
    {
        m_meanMsg = msg;
    }

    //! message to mean parent
    virtual Parameters<Gaussian> messageToParent(Gaussian */*parent*/) const
    {
        double meanPrec = moments().mean * m_precMsg.precision;
        double prec = m_precMsg.precision;
        // TODO: check the sign here
        return Parameters<Gaussian>(meanPrec, prec);
    }

    //! get message from the gamma parent
    virtual void receiveFromParent(const Moments<Gamma> &msg, Gamma */*parent*/)
    {
        m_precMsg = msg;
    }

    //! message to the gamma parent
    Parameters<Gamma> messageToParent(Gamma */*parent*/) const
    {
        // for array it would look as follows
        // a'_m = a_m + 1/2 * \sum_i q_{im}
        // b'_, = b + 1/2 * \sum_i q_{im} (x_i^2 - 2x_i <mu_m> + <mu2_m>)
        double shape = 0.5;
        double rate = 0.5 * (moments().mean2 - 2 * moments().mean * m_meanMsg.mean + m_meanMsg.mean2);
        return Parameters<Gamma>(shape, rate);
    }

    //! <u> = [<mean>, <mean>^2 + 1./<precision>], expectations are based on the current
    inline Moments<Gaussian> moments() const
    {
        if (isObserved())
            return Moments<Gaussian>(m_value, sqr(m_value));
        else
        {
            double mean = m_params.meanPrecision / m_params.precision;
            double var = 1.0 / m_params.precision;
            return Moments<Gaussian>(mean, sqr(mean) + var);
        }
    }

    //! compute parameters from parents
    Parameters<Gaussian> parametersFromParents() const
    {
        return Parameters<Gaussian>(m_meanMsg.mean * m_precMsg.precision,
                                    m_precMsg.precision);
    }


    //! compute the constitute to the lower bound on the log-evidence
    virtual double logEvidenceLowerBound() const
    {
        throw std::runtime_error("Gamma::logEvidenceLowerBound(): not implemented");
    }

    //! compute the log-probability value of the provided value given the current natural parameters
    // TODO: put this into ContinuousVariable?
    virtual double logProbabilityDensity(double value) const
    {
        return 0.5 * (m_precMsg.logPrecision -
                      m_precMsg.precision * (sqr(value) - 2 * value * m_meanMsg.mean + m_meanMsg.mean2)
                      -log(2*M_PI));
    }


private:
    // parents
    Gaussian *m_meanPar;
    Gamma *m_precPar;

    // current messages received from both parents
    Moments<Gaussian> m_meanMsg;
    Moments<Gamma> m_precMsg;
};


}


#endif // GAUSSIAN_H
