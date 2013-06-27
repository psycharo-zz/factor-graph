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

    //! clear parameters so that they can be used when summing
    inline void clear()
    {
        meanPrecision = 0;
        precision = 0;
    }

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


inline ostream& operator<<(ostream &out, const Parameters<Gaussian> &params)
{
    out << "Gaussian(" << params.mean() << "," << params.precision << ")";
    return out;
}


inline Parameters<Gaussian> operator*(const Parameters<Gaussian> &params, double val)
{
    return Parameters<Gaussian>(params.meanPrecision * val, params.precision * val);
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
    Moments() {}

    Moments(const Parameters<Gaussian> &params)
    {
        fromParameters(*this, params);
    }

    Moments(double _mean, double _mean2):
        mean(_mean),
        mean2(_mean2)
    {}


    inline static void fromParameters(Moments &moments, const Parameters<Gaussian> &params)
    {
        moments.mean = params.meanPrecision / params.precision;
        moments.mean2 = sqr(moments.mean) + 1.0 / params.precision;
    }

    // TODO: make a version with two params?
    inline static Moments fromValue(double value) { return Moments(value, sqr(value)); }


    double mean;
    double mean2;
};


//! dot product
inline double operator*(const Parameters<Gaussian> &params, const Moments<Gaussian> &moments)
{
    return params.meanPrecision * moments.mean - 0.5 * params.precision * moments.mean2;
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
    typedef double TValue;


    Gaussian(Gaussian *_meanParent, Gamma *_precParent):
        Variable(parametersFromParents(_meanParent->moments(), _precParent->moments())),
        m_meanParent(_meanParent),
        m_precParent(_precParent),
        m_hasParents(true)
    {
        updatePosterior();
    }

    virtual ~Gaussian() {}

    //! observe a scalar value TODO: make a template/vector version?
    inline virtual void observe(double value)
    {
        this->m_observed = true;
        this->m_moments = TMoments::fromValue(value);
    }

    //! get messages from parents
    inline const Moments<Gaussian> &meanMsg() const { return m_meanParent->moments(); }
    //! get message from parents
    inline const Moments<Gamma> &precMsg() const { return m_precParent->moments(); }

    //! override HasParent<Gaussian>
    inline void messageToParent(Parameters<Gaussian> *params) const { messageToParent(params, moments(), precMsg()); }
    //! override HasParent<Gamma>
    inline void messageToParent(Parameters<Gamma> *params) const { messageToParent(params, moments(), meanMsg()); }

    //! override Variable
    inline Parameters<Gaussian> parametersFromParents() const { return parametersFromParents(meanMsg(), precMsg()); }

    //! override Variable
    inline double logNormParents() const { return logNormParents(meanMsg(), precMsg()); }

    //! compute the log-probability value of the provided value given the current natural parameters
    // TODO: put this into ContinuousVariable?
    virtual double logPDF(const Moments<Gaussian> &ms) const { return logPDF(ms, meanMsg(), precMsg()); }


    // static versions
    inline static TParameters parametersFromParents(const Moments<Gaussian> &meanMsg, const Moments<Gamma> &precMsg)
    {
        return TParameters(meanMsg.mean * precMsg.precision, precMsg.precision);
    }


    inline static void messageToParent(Parameters<Gamma> *params,
                                       const Moments<Gaussian> &value, const Moments<Gaussian> &meanMsg)
    {
        params->shape = 0.5;
        params->rate = 0.5 * (value.mean2 - 2 * value.mean * meanMsg.mean + meanMsg.mean2);
    }

    inline static void messageToParent(Parameters<Gaussian> *ps,
                                       const Moments<Gaussian> &value, const Moments<Gamma> &precMsg)
    {
        ps->meanPrecision = value.mean * precMsg.precision;
        ps->precision = precMsg.precision;
    }


    inline static void updateMoments(TMoments &ms, const TParameters &ps)
    {
        ms.mean = ps.meanPrecision / ps.precision;
        ms.mean2 = sqr(ms.mean) + 1.0 / ps.precision;
    }

    // 1D gaussian
    inline static double logPDF(const TMoments &value, const Moments<Gaussian> &meanMsg, const Moments<Gamma> &precMsg)
    {
        return 0.5 * (precMsg.logPrecision
                      -precMsg.precision * (value.mean2 - 2 * value.mean * meanMsg.mean + meanMsg.mean2)
                      -LN_2PI);
    }


    inline static double logNorm(const TParameters &ps)
    {
        return 0.5 * (log(ps.precision)
                      - ps.precision * sqr(ps.meanPrecision / ps.precision)
                      - LN_2PI);
    }

    inline static double logNormParents(const Moments<Gaussian> &meanMsg, const Moments<Gamma> &precMsg)
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

    Gaussian(const TMoments &_ms):
        Variable(_ms),
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
    ConstGaussian(double _mean):
        Gaussian(TMoments(_mean, sqr(_mean)))
    {}
};



}


#endif // GAUSSIAN_H
