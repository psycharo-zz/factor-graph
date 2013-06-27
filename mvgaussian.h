#ifndef MULTIVARIATEGAUSSIAN_H
#define MULTIVARIATEGAUSSIAN_H

#include <variable.h>
#include <wishart.h>


namespace vmp
{


class MVGaussian;


template <>
class Parameters<MVGaussian>
{
public:
    // TODO: FIXME
    Parameters(size_t _dims = 0):
        precMean(_dims),
        prec(_dims, _dims)
    {}

    Parameters(const vec &_precMean, const mat &_prec):
        precMean(_precMean),
        prec(_prec)
    {}

    inline size_t dims() const { return precMean.n_rows; }

    inline void clear()
    {
        precMean.fill(0);
        prec.fill(0);
    }

    inline Parameters &operator+=(const Parameters &other)
    {
        precMean += other.precMean;
        prec += other.prec;
        return *this;
    }

    Parameters &operator*=(double value)
    {
        precMean *= value;
        prec *= value;
        return *this;
    }


    vec precMean;
    mat prec;
};


inline Parameters<MVGaussian> operator*(const Parameters<MVGaussian> &params, double val)
{
    return Parameters<MVGaussian>(params.precMean * val, params.prec * val);
}



template <>
class Moments<MVGaussian>
{
public:
    Moments()
    {}

    Moments(const Parameters<MVGaussian> &ps)
    {
        fromParameters(*this, ps);
    }

    Moments(const vec &_mean, const mat &_mean2):
        mean(_mean),
        mean2(_mean2)
    {}

    inline size_t dims() const { return mean.n_rows; }

    static void fromParameters(Moments &moments, const Parameters<MVGaussian> &params)
    {
        const mat inv_phi1 = inv(-0.5 * params.prec);
        moments.mean = -0.5 * inv_phi1 * params.precMean;
        moments.mean2 = moments.mean * moments.mean.t() - 0.5 * inv_phi1;
    }

    inline static Moments fromValue(const vec &value)
    {
        return Moments(value, value * value.t());
    }

    vec mean;
    mat mean2;
};


inline double operator*(const Parameters<MVGaussian> &ps, const Moments<MVGaussian> &ms)
{
    throw NotImplementedException;
    return 0;
}


class MVGaussian : public Variable<MVGaussian>,
                   public HasParent<MVGaussian>,
                   public HasParent<Wishart>
{
public:
    typedef vec TValueType;


    MVGaussian(MVGaussian *_meanParent, Wishart *_precParent):
        Variable(parametersFromParents(_meanParent->moments(), _precParent->moments())),
        m_meanParent(_meanParent),
        m_precParent(_precParent)
    {}

    //! get the number of dimensions
    size_t dims() const { return m_moments.dims(); }

    //! TODO: generalize this
    void observe(const vec &value)
    {
        m_observed = true;
        m_moments = TMoments::fromValue(value);
    }

    // TODO: CACHE this
    //! override Variable
    TParameters parametersFromParents() const { return parametersFromParents(m_meanParent->moments(), m_precParent->moments()); }


    //! override Variable
    virtual double logNormParents() const { throw NotImplementedException; }
    //! override Variable
    virtual double logPDF(const TMoments &moments) const { throw NotImplementedException; }


    //! override HasParent<MultivariateGaussian>
    void messageToParent(Parameters<MVGaussian> *params) const { messageToParent(params, moments(), m_precParent->moments()); }

    //! override HasParent<Wishart>
    void messageToParent(Parameters<Wishart> *params) const { messageToParent(params, moments(), m_meanParent->moments()); }



    inline static void messageToParent(Parameters<MVGaussian> *params, const TMoments &moments, const Moments<Wishart> &precMsg)
    {
        params->precMean = precMsg.prec * moments.mean;
        params->prec = precMsg.prec;
    }

    inline static void messageToParent(Parameters<Wishart> *params, const TMoments &moments, const TMoments &meanMsg)
    {
        auto &x = moments.mean;
        auto &x2 = moments.mean2;
        auto &mu = meanMsg.mean;
        auto mu2 = meanMsg.mean2;
        params->scale = (x2 - x * mu.t() - mu * x.t() + mu2);
        params->degrees = 1;
    }

    inline static TParameters parametersFromParents(const Moments<MVGaussian> &meanMsg, const Moments<Wishart> &precMsg)
    {
        return TParameters(precMsg.prec * meanMsg.mean, precMsg.prec);
    }

    inline static double logNorm(const TParameters &ps) { throw NotImplementedException; }

    inline static double logNormParents(const Moments<MVGaussian> &meanMsg, const Moments<Wishart> &precMsg)
    {
        throw NotImplementedException;
    }

    inline static double logPDF(const TMoments &value, const Moments<MVGaussian> &meanMsg, const Moments<Wishart> &precMsg)
    {
        auto &x = value.mean;
        auto &mu = meanMsg.mean;
        auto &prec = precMsg.prec;
        auto &logDet = precMsg.logDet;
        double D = value.dims();

        return as_scalar(- 0.5 * x.t() * prec * x + x.t() * prec * mu - 0.5 * mu.t() * prec * mu)
                         + 0.5 * logDet - 0.5 * D * LN_2PI;
    }


protected:
    MVGaussian(const TMoments &_moments):
        Variable(_moments),
        m_meanParent(NULL),
        m_precParent(NULL)
    {}

    MVGaussian *m_meanParent;
    Wishart *m_precParent;
};



class ConstMVGaussian : public MVGaussian
{
public:
    ConstMVGaussian(const vec &value):
        MVGaussian(TMoments::fromValue(value))
    {}
};



template <typename TMeanParent = MVGaussian,
          typename TPrecParent = Wishart>
class MVGaussianArray : public VariableArray<MVGaussian>,
                        public HasParent<TMeanParent>,
                        public HasParent<TPrecParent>
{
public:

    // TODO: derive dimensions for arrays?
    MVGaussianArray(size_t _size, TMeanParent *mean, TPrecParent *prec):
        VariableArray<MVGaussian>(_size, TBaseParameters(0)), // TODO: FIXME
        m_meanParent(mean),
        m_precParent(prec)
    {
        // TODO: this should be done in VariableArray constructor
        updatePosterior();
    }

    //! initialise with given parameters
    MVGaussianArray(const TParameters &_params, TMeanParent *mean, TPrecParent *prec):
        VariableArray<MVGaussian>(_params),
        m_meanParent(mean),
        m_precParent(prec)
    {}

    //! TODO: generic?
    inline void observe(const vector<vec> &values)
    {
        for (size_t i = 0; i < size(); ++i)
            m_moments[i] = TBaseMoments::fromValue(values[i]);
        m_observed = true;
    }
    inline void observe(const mat &values)
    {
        for (size_t i = 0; i < size(); ++i)
            m_moments[i] = TBaseMoments::fromValue(values.row(i).t());
        m_observed = true;
    }


    //! override VariableArray. implementations below
    virtual double logNormParents() const;

    //! override VariableArray. see implemtations below
    virtual TBaseParameters parametersFromParents(size_t idx) const;

    //! override VariableArray
    vector<double> logPDF(const vector<TBaseMoments> &_moments) const { throw NotImplementedException; }

    // TODO: shouldn't this have some default implementation?
    //! override HasParent<TMeanParent>
    void messageToParent(Parameters<TMeanParent> *params) const;
    //! override HasParent<TPrecParent>
    void messageToParent(Parameters<TPrecParent> *params) const;


    static double logNormParents(const Moments<TMeanParent> &, const Moments<TPrecParent> &);


protected:
    const TMeanParent *m_meanParent;
    const TPrecParent *m_precParent;
};


// implementations

// getting parameters from parents
template <>
inline Parameters<MVGaussian> MVGaussianArray<MVGaussian, Wishart>::parametersFromParents(size_t /*idx*/) const
{
    return MVGaussian::parametersFromParents(m_meanParent->moments(), m_precParent->moments());
}

template <>
inline Parameters<MVGaussian> MVGaussianArray<VariableArray<MVGaussian>, VariableArray<Wishart> >::parametersFromParents(size_t idx) const
{
    return MVGaussian::parametersFromParents(m_meanParent->moments(idx), m_precParent->moments(idx));
}



// message implementations

// messages
template<>
inline void MVGaussianArray<MVGaussian, Wishart>::messageToParent(Parameters<MVGaussian> *params) const
{
    const Moments<Wishart> &precMsg = m_precParent->moments();
    params->precMean = zeros(precMsg.dims(), 1);
    params->prec = precMsg.prec * size();
    for (size_t i = 0; i < size(); ++i)
        params->precMean += precMsg.prec * moments(i).mean;
}



// from a vector to a _single gamma_ parent having _single gaussian_ parent
template<>
inline void MVGaussianArray<MVGaussian, Wishart>::messageToParent(Parameters<Wishart> *params) const
{
    const Moments<MVGaussian> &meanMsg = m_meanParent->moments();
    auto &mu = meanMsg.mean;
    auto &mu2 = meanMsg.mean2;
    params->scale = zeros(meanMsg.dims(), meanMsg.dims());
    params->degrees = size();
    for (size_t i = 0; i < size(); ++i)
    {
        auto &x = moments(i).mean;
        auto &x2 = moments(i).mean2;
        params->scale += (x2 - x * mu.t() - mu * x.t() + mu2);
    }
}



// lb-related implementations
template<>
inline double MVGaussianArray<MVGaussian, Wishart>::logNormParents() const
{
    throw NotImplementedException;
}

template<>
inline double MVGaussianArray<VariableArray<MVGaussian>, VariableArray<Wishart> >::logNormParents() const
{
    throw NotImplementedException;
}





} // namespace vmp

#endif // MULTIVARIATEGAUSSIAN_H
