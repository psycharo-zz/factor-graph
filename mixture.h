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


class MoG : public Variable<Gaussian>
{
public:

};

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

    vector<GaussianParameters> components;
    vector<double> weights;
};




/**
 * TODO: maybe make generic mixtures out of this
 */
class MoGArray : public VariableArray<Gaussian>,
                 public HasParent<VariableArray<Gaussian> >,
                 public HasParent<VariableArray<Gamma> > // TODO: not sure if <Gaussian> is better than <MoG>
{
public:

    MoGArray(size_t _size,
             VariableArray<Gaussian> *_meanComps,
             VariableArray<Gamma> *_precComps,
             DiscreteArray *_selector)
        : VariableArray<Gaussian>(_size),
          m_meanComps(_meanComps),
          m_precComps(_precComps),
          m_selector(_selector)
    {
        assert(_meanComps->size() == _selector->dims() &&
               _precComps->size() == _selector->dims());
    }

    VariableArray<Gaussian> *mean() const { return m_meanComps; }
    VariableArray<Gamma> *precision() const { return m_precComps; }

    virtual ~MoGArray() {}

    inline size_t dims() const { return m_selector->dims(); }

    void observe(const vector<double> &values)
    {
        for (size_t i = 0; i < size(); ++i)
        {
            m_moments[i].mean = values[i];
            m_moments[i].mean2 = sqr(values[i]);
        }
        m_observed = true;
    }


    // message to discrete variables
    void messageToParent(VariableArray<Discrete>::TParamsVector *v) const
    {
        assert(v->params.size() == size());
        vector<Discrete::TParameters> &params = v->params;
        // TODO: checkme, implement logProbabilityDensity
        for (size_t i = 0; i < size(); ++i)
        {
            params[i].logProb = logProbabilityDensity(m_moments[i]);
            params[i].logProb -= lognorm(params[i].logProb);
        }
    }

    // message to mean mixtures
    void messageToParent(VariableArray<Gaussian>::TParamsVector *v) const
    {
        vector<Gaussian::TParameters> &params = v->params;

        Gaussian::TParameters tmp;
        for (size_t m = 0; m < dims(); ++m)
        {
            const Gamma::TMoments &precMsg = m_precComps->moments(m);
            params[m] = Gaussian::TParameters();
            // TODO: a common function to use everywhere
            for (size_t i = 0; i < size(); ++i)
            {
                Gaussian::messageToParent(&tmp, m_moments[i], precMsg);
                tmp *= m_selector->moments(i).probs[m];
                params[m] += tmp;
            }
        }
    }

    // message to variance mixtures
    void messageToParent(VariableArray<Gamma>::TParamsVector *v) const
    {
        vector<Gamma::TParameters> &params = v->params;

        Gamma::TParameters tmp;
        for (size_t m = 0; m < dims(); ++m)
        {
            const Gaussian::TMoments &meanMsg = m_meanComps->moments(m);
            // TODO: a common function to use everywhere
            params[m] = Gamma::TParameters();
            for (size_t i = 0; i < size(); ++i)
            {
                Gaussian::messageToParent(&tmp, m_moments[i], meanMsg);
                tmp *= m_selector->moments(i).probs[m];
                params[m] += tmp;
            }
        }
    }

    //! override VariableArray
    Parameters<Gaussian> parametersFromParents(size_t idx) const
    {
        Parameters<Gaussian> result;
        for (size_t m = 0; m < dims(); ++m)
            result += Gaussian::parametersFromParents(m_meanComps->moments(m), m_precComps->moments(m)) * m_selector->moments(idx).probs[m];
        return result;
    }

    //! override VariableArray
    void updatePosterior()
    {
        throw NotImplementedException;
    }

    //! override VariableArray
    void updateMoments()
    {
        throw NotImplementedException;
    }

    //! override VariableArray
    inline virtual double logNormalization() const
    {
        throw NotImplementedException;
    }

    //! override Variable
    inline virtual double logNormalizationParents() const
    {
        double result = 0;
        for (size_t m = 0; m < dims(); ++m)
        {
            double logNorm = Gaussian::logNormalizationParents(m_meanComps->moments(m), m_precComps->moments(m));
            // TODO: precompute weights?
            for (size_t i = 0; i < size(); ++i)
                result += m_selector->moments(i).probs[m] * logNorm;
        }
        return result;
    }

    // TODO: link instead of copying
    vector<double> logProbabilityDensity(const TMoments &value) const
    {
        vector<double> result(dims());
        for (size_t m = 0; m < dims(); ++m)
            result[m] = Gaussian::logProbabilityDensity(value, m_meanComps->moments(m), m_precComps->moments(m));
        return result;
    }





private:
    //! components
    VariableArray<Gaussian> *m_meanComps;
    VariableArray<Gamma> *m_precComps;

    //! selector variable
    DiscreteArray *m_selector;
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
