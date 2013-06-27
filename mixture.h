#ifndef MIXTURE_H
#define MIXTURE_H


#include <vector>
#include <cassert>
using namespace std;


#include <variable.h>
#include <gaussian.h>
#include <discrete.h>
#include "discretearray.h"
#include "gaussianarray.h"


namespace vmp
{



class MoG;

template<>
class Parameters<MoG>
{
public:
    Parameters()
    {}

    Parameters(const vector<Gaussian::TParameters> &_comps,
               const vector<double> &_weights):
        components(_comps),
        weights(_weights)
    {
        assert(components.size() == weights.size());
    }

    inline size_t dims() const { return components.size(); }

    vector<Gaussian::TParameters> components;
    vector<double> weights;
};




// TODO: finish it
class MoG : public Variable<Gaussian>,
            public HasParent<VariableArray<Gaussian> >,
            public HasParent<VariableArray<Gamma> >,
            public HasParent<Discrete>
{
public:
    MoG(VariableArray<Gaussian> *_meanComps,
        VariableArray<Gamma> *_precComps,
        Discrete *_selector):
        Variable<Gaussian>(TParameters()), // FIXME
        m_meanComps(_meanComps),
        m_precComps(_precComps),
        m_selector(_selector)
    {
        assert(_meanComps->size() == _selector->dims() &&
               _precComps->size() == _selector->dims());
    }

    size_t dims() const { return m_selector->dims(); }

    // utility functions to access separate mixtures
    const Moments<Gaussian> &meanMoments(size_t m) const { return m_meanComps->moments(m); }
    const Moments<Gamma> &precMoments(size_t m) const { return m_precComps->moments(m); }
    double weight(size_t idx) const { return m_selector->moments().probs[idx]; }

    //! TODO:
    TParameters parameters(size_t m) const
    {
        return TParameters(m_meanComps->moments(m).mean * m_precComps->moments(m).precision,
                           m_precComps->moments(m).precision);
    }

    void observe(double value)
    {
        m_moments = TMoments::fromValue(value);
        m_observed = true;
    }

    //! override Variable
    void updatePosterior() { throw NotImplementedException; }

    //! override Variable
    void updateMoments() { throw NotImplementedException; }

    //! override Variable
    TParameters parametersFromParents() const { throw NotImplementedException; }

    //! override
    double logNormParents() const { throw NotImplementedException; }


    // message to discrete variables TODO: VariableArray<Discrete>
    void messageToParent(Discrete::TParameters *params) const
    {
        for (size_t m = 0; m < dims(); ++m)
            params->logProb[m] = logPDF(m_moments);
        params->logProb -= lognorm(params->logProb);
    }

    // message to mean mixtures
    void messageToParent(VariableArray<Gaussian>::TParameters *v) const
    {
        vector<Gaussian::TParameters> &params = v->params;

        Gaussian::TParameters tmp;
        for (size_t m = 0; m < dims(); ++m)
        {
            TVariableType::messageToParent(&params[m], m_moments, precMoments(m));
            params[m] *= weight(m);
        }
    }

    // message to variance mixtures
    void messageToParent(VariableArray<Gamma>::TParameters *v) const
    {
        vector<Gamma::TParameters> &params = v->params;
        for (size_t m = 0; m < dims(); ++m)
        {
            TVariableType::messageToParent(&params[m], m_moments, meanMoments(m));
            params[m] *= weight(m);
        }
    }

    double logPDF(const TMoments &moments) const
    {
        double result = 0.0;
        for (size_t m = 0; m < dims(); ++m)
            result += weight(m) * TVariableType::logPDF(moments, meanMoments(m), precMoments(m));
        return result;
    }


    //! override Variable
    static double logNorm(const TParameters &_params) { throw NotImplementedException; }


private:
    VariableArray<Gaussian> *m_meanComps;
    VariableArray<Gamma> *m_precComps;
    Discrete *m_selector;
};




/**
 * TODO: maybe make generic mixtures out of this
 */
template <typename TDistr = Gaussian,
          typename TMean = Gaussian,
          typename TPrec = Gamma>
class MoGArray : public VariableArray<TDistr>,
                 public HasParent<VariableArray<TMean> >,
                 public HasParent<VariableArray<TPrec> >,
                 public HasParent<VariableArray<Discrete> >
{
public:
    typedef VariableArray<TMean> TMeanArray;
    typedef VariableArray<TPrec> TPrecArray;
    typedef VariableArray<Discrete> TDiscrArray;
    typedef Moments<TDistr> TDistrMoments;
    typedef Parameters<TDistr> TDistrParameters;


    MoGArray(size_t _size,
             VariableArray<TMean> *_meanComps, VariableArray<TPrec> *_precComps,
             VariableArray<Discrete> *_selector)
        : VariableArray<TDistr>(_size),
          m_meanComps(_meanComps),
          m_precComps(_precComps),
          m_selector(_selector)
    {

        assert(_meanComps->size() == _precComps->size());

        // TODO: do something about DiscreteArray*
//        assert(_meanComps->size() == _selector->dims() &&
//               _precComps->size() == _selector->dims());
    }

    virtual ~MoGArray() {}

    //! component accessors
    VariableArray<TMean> *mean() const { return m_meanComps; }
    VariableArray<TPrec> *precision() const { return m_precComps; }

    inline const Moments<TMean> &meanMsg(size_t m) const { return m_meanComps->moments(m); }
    inline const Moments<TPrec> &precMsg(size_t m) const { return m_precComps->moments(m); }
    inline double weight(size_t i, size_t m) const { return m_selector->moments(i).probs[m]; }


    //! the number of components
    inline size_t dims() const { return m_meanComps->size(); }

    // TODO: make these functions mix-ins?
    void observe(const double *values)
    {
        for (size_t i = 0; i < this->size(); ++i)
            this->m_moments[i] = TDistrMoments::fromValue(values[i]);
        this->m_observed = true;
    }

    void observe(const vector<double> &values)
    {
        for (size_t i = 0; i < this->size(); ++i)
            this->m_moments[i] = TDistrMoments::fromValue(values[i]);
        this->m_observed = true;
    }


    void observe(const mat &values)
    {
        for (size_t i = 0; i < this->size(); ++i)
            this->m_moments[i] = TDistrMoments::fromValue(values.row(i).t());
        this->m_observed = true;
    }


    // message to discrete variables TODO: VariableArray<Discrete>
    void messageToParent(Parameters<TDiscrArray> *v) const
    {
        assert(v->size() == this->size());

        vector<Parameters<Discrete> > &params = v->params;
        // TODO: checkme, implement logProbabilityDensity
        for (size_t i = 0; i < this->size(); ++i)
        {
            params[i].logProb = logProbabilityDensity(this->m_moments[i]);
            params[i].logProb -= lognorm(params[i].logProb);
        }
    }

    // message to mean mixtures
    void messageToParent(Parameters<TMeanArray> *v) const
    {
        assert(v->size() == dims() && dims() > 0);
        auto &params = v->params;
        Parameters<TMean> tmp = params[0];
        for (size_t m = 0; m < dims(); ++m)
        {
            const Moments<TPrec> &precMsg = m_precComps->moments(m);
            params[m].clear();
            for (size_t i = 0; i < this->size(); ++i)
            {
                TDistr::messageToParent(&tmp, this->m_moments[i], precMsg);
                tmp *= this->weight(i, m);
                params[m] += tmp;
            }
        }
    }

    // message to variance mixtures
    void messageToParent(Parameters<TPrecArray> *v) const
    {
        assert(v->size() == dims() && dims() > 0);
        auto &params = v->params;
        Parameters<TPrec> tmp = params[0];
        for (size_t m = 0; m < dims(); ++m)
        {
            const Moments<TMean> &meanMsg = m_meanComps->moments(m);
            // TODO: a common function to use everywhere
            params[m].clear();
            for (size_t i = 0; i < this->size(); ++i)
            {
                TDistr::messageToParent(&tmp, this->m_moments[i], meanMsg);
                tmp *= this->m_selector->moments(i).probs[m];
                params[m] += tmp;
            }
        }
    }

    //! override VariableArray
    Parameters<TDistr> parametersFromParents(size_t i) const
    {
        Parameters<TDistr> result = TDistr::parametersFromParents(meanMsg(0), precMsg(0)) * weight(i, 0);
        for (size_t m = 1; m < dims(); ++m)
            result += TDistr::parametersFromParents(meanMsg(m), precMsg(m)) * weight(i, m);
        return result;
    }

    // TODO: either introduce richer parameters, or simply use an expectation \sum_i weight(i) * param(i)
    //! override VariableArray
    void updatePosterior() { throw NotImplementedException; }

    //! override VariableArray
    void updateMoments() { throw NotImplementedException; }

    //! override VariableArray
    inline virtual double logNorm() const { throw NotImplementedException; }

    //! override Variable
    inline virtual double logNormParents() const
    {
        double result = 0;
        for (size_t m = 0; m < dims(); ++m)
        {
            double logNorm = TDistr::logNormParents(m_meanComps->moments(m), m_precComps->moments(m));
            // TODO: precompute weights?
            for (size_t i = 0; i < this->size(); ++i)
                result += m_selector->moments(i).probs[m] * logNorm;
        }
        return result;
    }

    // TODO: link instead of copying
    vector<double> logProbabilityDensity(const Moments<TDistr> &value) const
    {
        vector<double> result(dims());
        for (size_t m = 0; m < dims(); ++m)
            result[m] = TDistr::logPDF(value, m_meanComps->moments(m), m_precComps->moments(m));
        return result;
    }

private:
    //! components
    VariableArray<TMean> *m_meanComps;
    VariableArray<TPrec> *m_precComps;
    VariableArray<Discrete> *m_selector;

};







inline ostream &operator<<(ostream &out, const Parameters<MoG> &params)
{
    out << "means = [";
    for (size_t m = 0; m < params.dims(); ++m)
        out << params.components[m].meanPrecision / params.components[m].precision << " ";
    out << "]" << endl;

    out << "precs = [";
    for (size_t m = 0; m < params.dims(); ++m)
        out << params.components[m].precision << " ";
    out << "]" << endl;

    out << "weights = [" << params.weights << "]" << endl;

    return out;
}




}


#endif // MIXTURE_H
