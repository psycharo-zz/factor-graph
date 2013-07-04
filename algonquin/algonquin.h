#ifndef ALGONQUIN_H
#define ALGONQUIN_H


#include <variable.h>
#include <discrete.h>
#include <dirichlet.h>
#include <gammaarray.h>
#include <gaussianarray.h>
#include <mixture.h>
#include <network.h>

#include <mathutil.h>


#include <examples.h>
#include <matlab/persistentobject.h>

#include <thread>



namespace vmp
{


class Algonquin;


/**
 * contain variational parameters of the approx posterior
 */
template<>
class Parameters<Algonquin>
{
public:
    Parameters(size_t _numSpeech, size_t _numNoise):
        dims(_numSpeech * _numNoise),
        meansSpeech(dims, 0.0), meansNoise(dims, 0.0),
        varsSpeech(dims, 1.0), varsNoise(dims, 1.0),
        weights(dims, 1.0 / dims),
        precision(dims, 1.0)
    {}

    //! the number of variational parameters
    const size_t dims;

    //
    vector<double> meansSpeech;
    vector<double> meansNoise;
    vector<double> varsSpeech;
    vector<double> varsNoise;
    // component weights
    vector<double> weights;
    // constant posterior precision
    vector<double> precision;
};

/**
 * @brief The AlgonquinVariable class
 * scalar deterministic (?) node implementing algonquin inference
 */
class Algonquin : public BaseVariable,
                  public HasParent<VariableArray<Gaussian> >,
                  public HasParent<VariableArray<Gamma> >,
                  public HasParent<Discrete>
{
public:
    const size_t DEFAULT_NUM_ITERATIONS = 20;

    Algonquin(UnivariateMixture *_speechParent, UnivariateMixture *_noiseParent):
        m_speechParent(_speechParent),
        m_noiseParent(_noiseParent),
        m_numIters(DEFAULT_NUM_ITERATIONS),
        m_parameters(numSpeech(), numNoise())
    {
        // intialising variational parameters
        for (size_t s = 0; s < numSpeech(); ++s)
            for (size_t n = 0; n < numNoise(); ++n)
            {
                size_t i = index(s,n);
                m_parameters.meansSpeech[i] = priorMeanSpeech(s);
                m_parameters.varsSpeech[i] = 1.0 / priorPrecSpeech(s);
                m_parameters.meansNoise[i] = priorMeanNoise(n);
                m_parameters.varsNoise[i] = 1.0 / priorPrecNoise(n);
                m_parameters.weights[i] = priorWeightSpeech(s) * priorWeightNoise(n);
            }
    }

    virtual ~Algonquin() {}

    inline size_t numSpeech() const { return m_speechParent->numComps(); }
    inline size_t numNoise() const { return m_noiseParent->numComps(); }
    inline size_t numParameters() const { return numSpeech() * numNoise(); }

    inline void observe(double value) { m_value = value; }

    inline size_t index(size_t s, size_t n) const { return numSpeech() * n + s; }


    inline void setNumIterations(size_t numIters) { m_numIters = numIters; }

    inline double priorMeanSpeech(size_t s) const { return m_speechParent->meanMoment(s).mean; }
    inline double priorMeanNoise(size_t n) const { return m_noiseParent->meanMoment(n).mean; }
    inline double priorPrecSpeech(size_t s) const { return m_speechParent->precMoment(s).precision; }
    inline double priorPrecNoise(size_t n) const { return m_noiseParent->precMoment(n).precision; }

    // prior weights
    inline double priorWeightSpeech(size_t s) const { return m_speechParent->weight(s); }
    inline double priorWeightNoise(size_t n) const { return m_noiseParent->weight(n); }


    //! override TODO: Variable/BaseVariable
    virtual void updatePosterior();
    //! override
    virtual void updateMoments();
    // messages
    // TODO: this looks weird: direct parents are MoG's, while here we are directly updating priors
    // the reason is that MoGs are in fact not real distributions ( since updatePosterior() is not implemented)
    void messageToParent(VariableArray<Gaussian>::TParameters *vparams) const { throw NotImplementedException; }
    //! override HasParent<GammaArray>
    void messageToParent(VariableArray<Gamma>::TParameters *vparams) const { throw NotImplementedException; }
    //! override HasParent<Discrete>
    void messageToParent(Discrete::TParameters *params) const { throw NotImplementedException; }
    // TODO: fixme
    pair<double, double> m_fakeMoments;
private:
    UnivariateMixture *m_speechParent;
    UnivariateMixture *m_noiseParent;
    size_t m_numIters;
    // variational parameters
    Parameters<Algonquin> m_parameters;
    // constant posterior precision
    vector<double> precision;
    // the observed value. TODO: make this an array instead?
    double m_value;
};



/**
 * this is an experimental node supporting online inference
 * TODO: meaning that Discrete should also be online-like?
 */
class AlgonquinArray : public BaseVariable,
                       public HasParent<VariableArray<Gaussian> >,
                       public HasParent<VariableArray<Gamma> >,
                       public HasParent<DiscreteArray>
{
public:
    typedef Parameters<Algonquin> TParameters;


    AlgonquinArray(UnivariateMixture *_speechParent,
                   UnivariateMixture *_noiseParent,
                   DiscreteArray *_selector,
                   size_t _maxSize):
        m_speechParent(_speechParent),
        m_noiseParent(_noiseParent)
    {}

    //! the maximum possible size of the
    size_t maxSize() const { throw NotImplementedException; }

    size_t size() const { throw NotImplementedException; }

    void observe(double value)
    {
        m_moments.mean = value;
        m_moments.mean2 = value;
    }

    inline size_t numSpeech() const { return m_speechParent->numComps(); }
    inline size_t numNoise() const { return m_noiseParent->numComps(); }
    inline size_t numParameters() const { return numSpeech() * numNoise(); }


    // this only updates the last (maxSize-1) parameter
    void updatePosterior()
    {
        throw NotImplementedException;
    }
    void updateMoments() { throw NotImplementedException; }


    void messageToParent(VariableArray<Gaussian>::TParameters *v) const
    {
        auto &params = v->params;
        Gaussian::TParameters tmp;
        for (size_t m = 0; m < numNoise(); ++m)
        {
            auto &precMsg = m_noiseParent->precMoment(m);
            for (size_t i = 0; i < size(); ++i)
            {
                Gaussian::messageToParent(&tmp, m_moments, precMsg);
                tmp *= m_selector->weight(i, m);
                params[m] += tmp;
            }
        }
    }

    void messageToParent(VariableArray<Gamma>::TParameters *v) const
    {
        auto &params = v->params;
        Gamma::TParameters tmp;
        for (size_t m = 0; m < numNoise(); ++m)
        {
            auto &meanMsg = m_noiseParent->meanMoment(m);
            for (size_t i = 0; i < size(); ++i)
            {
                Gaussian::messageToParent(&tmp, m_moments, meanMsg);
                tmp *= m_selector->weight(i, m);
                params[m] += tmp;
            }
        }
    }

    void messageToParent(DiscreteArray::TParameters *v) const
    {
        throw NotImplementedException;
    }

private:
    // TODO: MoGs are used for convenience only
    UnivariateMixture *m_speechParent;
    UnivariateMixture *m_noiseParent;
    DiscreteArray *m_selector;


    //! posterior parameters for all
    vector<TParameters> m_variational;

    //! averaged parameters used to construct messages to parents
    vector<Gaussian::TParameters> m_parameters;

    //! observed values
    Gaussian::TMoments m_moments;
};





} // namespace vmp
#endif // ALGONQUIN_H
