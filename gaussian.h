#ifndef GAUSSIAN_H
#define GAUSSIAN_H

#include <vector>
#include <map>
#include <cassert>
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
    Parameters() {}

    // TODO: this constructor should not be necessary?
    Parameters(double mp, double p):
        meanPrecision(mp),
        precision(p)
    {}

    double meanPrecision;
    double precision;

    inline double mean() const { return meanPrecision / precision; }
    inline double variance() const { return 1.0 / precision; }

    Parameters &operator+=(const Parameters &other)
    {
        meanPrecision += other.meanPrecision;
        precision += other.precision;
        return *this;
    }
};
typedef Parameters<Gaussian> GaussianParameters;


inline ostream& operator<<(ostream &out, const Parameters<Gaussian> &params)
{
    out << "Gaussian(" << params.mean() << "," << params.precision << ")";
    return out;
}


inline Parameters<Gaussian> operator*(const Parameters<Gaussian> &params, double val)
{
    return Parameters<Gaussian>(params.meanPrecision * val,
                                params.precision * val);
}

inline Parameters<Gaussian> operator+(const Parameters<Gaussian> &a,
                                      const Parameters<Gaussian> &b)
{
    return Parameters<Gaussian>(a.meanPrecision + b.meanPrecision, a.precision + b.precision);
}


inline Parameters<Gaussian> operator-(const Parameters<Gaussian> &a,
                                      const Parameters<Gaussian> &b)
{
    return Parameters<Gaussian>(a.meanPrecision - b.meanPrecision, a.precision - b.precision);
}



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


// dot product
inline double operator*(const Parameters<Gaussian> &params,
                        const Moments<Gaussian> &moments)
{
    return params.meanPrecision * moments.mean
           - 0.5 * params.precision * moments.mean2;
}



/**
 * gaussian distributed variable
 */
class Gaussian: public ContinuousVariable<Gaussian>,
                public HasParent<Gaussian>,
                public HasParent<Gamma>
{
public:
    Gaussian(double _mean, double _prec):
        m_meanPar(NULL),
        m_precPar(NULL),
        m_meanMsg(_mean, sqr(_mean)),
        m_precMsg(_prec, log(_prec))
    {
        // TODO:is this necessary?
        updatePosterior();
    }

    Gaussian(Gaussian *_meanParent, Gamma *_precParent):
        m_meanPar(_meanParent),
        m_precPar(_precParent)
    {
        updatePosterior();
    }

    virtual ~Gaussian() {}

    //! check if there are any non-constant parents
    virtual bool hasParents() const
    {
        // TODO: maybe introduce a separate flag variable or even ask
        // whether a particular parent is present or not
        return (m_meanPar != NULL) && (m_precPar != NULL);
    }

    //! get message from mean parent
    virtual void receiveFromParent(const Moments<Gaussian> &msg, Gaussian *parent)
    {
        assert(m_meanPar == parent);
        m_meanMsg = msg;
    }

    //! message to mean parent
    virtual Parameters<Gaussian> messageToParent(Gaussian *parent) const
    {
        assert(parent == m_meanPar);

        double meanPrec = moments().mean * m_precMsg.precision;
        double prec = m_precMsg.precision;
        // TODO: check the sign here
        return Parameters<Gaussian>(meanPrec, prec);
    }

    //! get message from the gamma parent
    virtual void receiveFromParent(const Moments<Gamma> &msg, Gamma *parent)
    {
        assert(parent == m_precPar);
        m_precMsg = msg;
    }

    //! message to the gamma parent
    Parameters<Gamma> messageToParent(Gamma *parent) const
    {
        assert(parent == m_precPar);

        // for array it would look as follows
        // a'_m = a_m + 1/2 * \sum_i q_{im}
        // b'_, = b + 1/2 * \sum_i q_{im} (x_i^2 - 2x_i <mu_m> + <mu2_m>)
        double shape = 0.5;
        double rate = 0.5 * (moments().mean2 - 2 * moments().mean * m_meanMsg.mean + m_meanMsg.mean2);
        return Parameters<Gamma>(shape, rate);
    }

    //! <u> = [<mean>, <mean>^2 + 1./<precision>], expectations are based on the current
    // TODO: this should only be updated when parameters are updated
    inline Moments<Gaussian> updatedMoments() const
    {
        // TODO: update this at the same time with the posterior?
        if (isObserved())
            return Moments<Gaussian>(m_value, sqr(m_value));
        else
        {
            double mean = parameters().meanPrecision / parameters().precision;
            double var = 1.0 / parameters().precision;
            return Moments<Gaussian>(mean, sqr(mean) + var);
        }
    }

    //! compute parameters from parents
    Parameters<Gaussian> parametersFromParents() const
    {
        return Parameters<Gaussian>(m_meanMsg.mean * m_precMsg.precision,
                                    m_precMsg.precision);
    }


    //! compute the log-probability value of the provided value given the current natural parameters
    // TODO: put this into ContinuousVariable?
    virtual double logProbabilityDensity(const Moments<Gaussian> &ms) const
    {
        double value = ms.mean;
        return 0.5 * (m_precMsg.logPrecision
                      -m_precMsg.precision * (sqr(value) - 2 * value * m_meanMsg.mean + m_meanMsg.mean2)
                      -LN_2PI);
    }


    //! override Variable
    inline double logNormalization() const
    {
        // TODO: shouldn't this be from parents
        const double mean2 = sqr(parameters().meanPrecision / parameters().precision);
        const double precision = parameters().precision;
        const double logPrecision = log(precision);
        return 0.5 * (logPrecision - precision * mean2 - LN_2PI);
    }

    //! override Variable
    inline double logNormalizationParents() const
    {
        // 0.5 (<log(\gamma)> - <\gamma> * <\mean^2> - ln(2*PI))
        // HOW TO COMPUTE THIS FROM parametersFromParents() ??
        return 0.5 * (m_precMsg.logPrecision - m_precMsg.precision * m_meanMsg.mean2 - log(2 * M_PI));
    }

    // parents
    Gaussian *m_meanPar;
    Gamma *m_precPar;

    // current messages received from both parents
    Moments<Gaussian> m_meanMsg;
    Moments<Gamma> m_precMsg;
};


}


#endif // GAUSSIAN_H
