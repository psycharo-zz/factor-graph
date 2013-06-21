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
    Parameters():
        meanPrecision(0),
        precision(0)
    {}

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

    Parameters &operator*=(double value)
    {
        meanPrecision *= value;
        precision *= value;
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

    Moments<Gaussian>(const Parameters<Gaussian> &params):
        mean(params.mean()),
        mean2(sqr(params.mean()))
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
 * TODO: do smthing about overloading
 */
class Gaussian: public Variable<Gaussian>,
                public HasParent<Gaussian>,
                public HasParent<Gamma>

{
public:
    typedef Parameters<Gaussian> TParameters;
    typedef Moments<Gaussian> TMoments;

    Gaussian(double _mean, double _prec);

    Gaussian(Gaussian *_meanParent, Gamma *_precParent):
        Variable(TParameters(_meanParent->moments().mean, _precParent->moments().precision)),
        m_meanParent(_meanParent),
        m_precParent(_precParent),
        m_hasParents(true)
    {
        updatePosterior();
    }

    virtual ~Gaussian() {}

    //! observe a scalar value TODO: make a template/vector version?
    inline virtual void observe(double _value)
    {
        this->m_observed = true;
        this->m_moments.mean = _value;
        this->m_moments.mean2 = sqr(_value);
    }


    //! check if there are any non-constant parents
    inline virtual bool hasParents() const
    {
        // TODO: maybe introduce a separate flag variable or even ask
        // whether a particular parent is present or not
        return m_hasParents;
    }

    inline void messageToParent(Parameters<Gaussian> *params) const
    {
        messageToParent(params, moments(), precMsg());
//        params->meanPrecision = moments().mean * precMsg().precision;
//        params->precision = precMsg().precision;
    }

    inline void messageToParent(Parameters<Gamma> *params) const
    {
        messageToParent(params, moments(), meanMsg());
//        params->shape = 0.5;
//        params->rate = 0.5 * (moments().mean2 - 2 * moments().mean * meanMsg().mean + meanMsg().mean2);
    }

    //! override Variable
    inline virtual void updateMoments()
    {
        Gaussian::updateMoments(m_moments, parameters());
    }


    //! get messages from parents
    inline const Moments<Gaussian> &meanMsg() const
    {
        return m_meanParent->moments();
    }

    //! get message from parents
    inline const Moments<Gamma> &precMsg() const
    {
        return m_precParent->moments();
    }

    //! compute parameters from parents
    inline Parameters<Gaussian> parametersFromParents() const
    {
        return Parameters<Gaussian>(meanMsg().mean * precMsg().precision, precMsg().precision);
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
        return 0.5 * (precMsg().logPrecision - precMsg().precision * meanMsg().mean2 - LN_2PI);
    }

    //! compute the log-probability value of the provided value given the current natural parameters
    // TODO: put this into ContinuousVariable?
    virtual double logProbabilityDensity(const Moments<Gaussian> &ms) const
    {
        return Gaussian::logProbabilityDensity(ms, meanMsg(), precMsg());
    }

    // static versions
    inline static TParameters parametersFromParents(const Moments<Gaussian> &meanMsg, const Moments<Gamma> &precMsg)
    {
        return TParameters(meanMsg.mean * precMsg.precision,
                           precMsg.precision);
    }


    inline static void messageToParent(Parameters<Gamma> *params,
                                       const Moments<Gaussian> &value, const Moments<Gaussian> &meanMsg)
    {
        params->shape = 0.5;
        params->rate = 0.5 * (value.mean2 - 2 * value.mean * meanMsg.mean + meanMsg.mean2);
    }

    inline static void messageToParent(Parameters<Gaussian> *params,
                                       const Moments<Gaussian> &value, const Moments<Gamma> &precMsg)
    {
        params->meanPrecision = value.mean * precMsg.precision;
        params->precision = precMsg.precision;
    }


    inline static void updateMoments(TMoments &moments, const TParameters &params)
    {
        moments.mean = params.meanPrecision / params.precision;
        moments.mean2 = sqr(moments.mean) + 1.0 / params.precision;
    }

    // 1D gaussian
    inline static double logProbabilityDensity(const TMoments &value, const Moments<Gaussian> &meanMsg, const Moments<Gamma> &precMsg)
    {
        return 0.5 * (precMsg.logPrecision
                      -precMsg.precision * (value.mean2 - 2 * value.mean * meanMsg.mean + meanMsg.mean2)
                      -LN_2PI);
    }


    inline static double logNormalization(const TParameters &params)
    {
        return 0.5 * (log(params.precision)
                      - params.precision * sqr(params.meanPrecision / params.precision)
                      - LN_2PI);
    }

    inline static double logNormalizationParents(const Moments<Gaussian> &meanMsg, const Moments<Gamma> &precMsg)
    {
        return 0.5 * (precMsg.logPrecision
                      - precMsg.precision * meanMsg.mean2
                      - LN_2PI);
    }


protected:
    // parents
    Gaussian *m_meanParent;
    Gamma *m_precParent;

    bool m_hasParents;

    Gaussian():
        m_meanParent(NULL),
        m_precParent(NULL),
        m_hasParents(false)
    {}
};



/**
 * fake distribution with constants. TODO: add throws?
 */
class ConstGaussian : public Gaussian
{
public:
    ConstGaussian(double _mean)
    {
        m_moments = Moments<Gaussian>(_mean, sqr(_mean));
    }
};



}


#endif // GAUSSIAN_H
