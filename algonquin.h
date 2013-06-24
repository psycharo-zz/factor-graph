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

#include <memory>



namespace vmp
{

static const double DIRICHLET_PRIOR = 1;
static const double MEAN_PRIOR = 0;
static const double PREC_PRIOR = 1e-3;
static const double SHAPE_PRIOR = 1e-3;
static const double RATE_PRIOR = 1e-3;


//! compute the approximation at the given point
inline double sumlog(double s, double n)
{
    return s + log(1 + exp(n - s));
}

//! compute the jacobian \returns a pair(dg/ds, dg/dn)
inline pair<double,double> sumlog_jacobian(double s, double n)
{
    double tmp = exp(n - s) / (1 + exp(n - s));
    return make_pair(1 - tmp, tmp);
}

static const size_t DEFAULT_NUM_ITERATIONS = 4;


// TODO: implement

/**
 * @brief The AlgonquinVariable class
 * scalar deterministic (?) node implementing algonquin inference
 */
class AlgonquinVariable : public BaseVariable
{
public:
    AlgonquinVariable(MoG *_speechParent, MoG *_noiseParent):
        m_speechParent(_speechParent),
        m_noiseParent(_noiseParent),
        m_numIters(DEFAULT_NUM_ITERATIONS),
        m_speechMeans(numParameters(), 0.0), m_noiseMeans(numParameters(), 0.0),
        m_speechVars(numParameters(), 1.0), m_noiseVars(numParameters(), 1.0),
        m_weightParams(numParameters(), 1.0 / numParameters()),
        m_postPrec(numParameters(), 1.0)
    {
        // intialising variational parameters
        for (size_t s = 0; s < numSpeech(); ++s)
            for (size_t n = 0; n < numNoise(); ++n)
            {
                size_t i = index(s,n);
                m_speechMeans[i] = priorMeanSpeech(s);
                m_speechVars[i] = 1.0 / priorPrecSpeech(s);
                m_noiseMeans[i] = priorMeanNoise(n);
                m_noiseVars[i] = 1.0 / priorPrecNoise(n);
                m_weightParams[i] = priorWeightSpeech(s) * priorWeightNoise(n);
            }
    }

    inline void setNumIterations(size_t numIters) { m_numIters = numIters; }

    virtual ~AlgonquinVariable() {}

    inline double priorMeanSpeech(size_t s) const { return m_speechParent->meanMoments(s).mean; }
    inline double priorMeanNoise(size_t n) const { return m_noiseParent->meanMoments(n).mean; }
    inline double priorPrecSpeech(size_t s) const { return m_speechParent->precMoments(s).precision; }
    inline double priorPrecNoise(size_t n) const { return m_noiseParent->precMoments(n).precision; }

    // prior weights
    inline double priorWeightSpeech(size_t s) const { return m_speechParent->weight(s); }
    inline double priorWeightNoise(size_t n) const { return m_noiseParent->weight(n); }


    inline size_t numSpeech() const { return m_speechParent->dims(); }
    inline size_t numNoise() const { return m_noiseParent->dims(); }


    //! observe
    void observe(double value)
    {
        m_value = value;
    }

    //! override TODO: Variable/BaseVariable
    virtual void updatePosterior();

    //! override
    virtual void updateMoments();

    // messages
    // TODO: this looks weird: direct parents are MoG's, while here we are directly updating priors
    // the reason is that MoGs are in fact not real distributions ( since updatePosterior() is not implemented)

    void messageToParent(VariableArray<Gaussian>::TParameters *vparams)
    {
        throw NotImplementedException;
    }

    void messageToParent(VariableArray<Gamma>::TParameters *vparams)
    {
        throw NotImplementedException;
    }

    void messageToParent(Discrete::TParameters *params)
    {
        throw NotImplementedException;
    }

    // TODO: fixme
    pair<double, double> m_fakeMoments;


private:
    inline size_t index(size_t s, size_t n) const { return numSpeech() * n + s; }

    inline double meanSpeech(size_t index) const {  return m_speechMeans[index]; }
    inline double meanNoise(size_t index) const { return m_noiseMeans[index]; }
    inline size_t numParameters() const { return numSpeech() * numNoise(); }


    MoG *m_speechParent;
    MoG *m_noiseParent;

    size_t m_numIters;

    // variational parameters
    vector<double> m_speechMeans;
    vector<double> m_noiseMeans;

    vector<double> m_speechVars;
    vector<double> m_noiseVars;

    // \rho
    vector<double> m_weightParams;

    // constant posterior precision
    vector<double> m_postPrec;

    // the observed value. TODO: make this an array instead?
    double m_value;


};





class Network : public PersistentObject
{
public:
    static const size_t MAX_NUM_ITERS = 100;

    // hyperparameters
    Network(size_t numIters = MAX_NUM_ITERS):
        m_speech(NULL),
        m_noise(NULL),
        m_speechNetwork(NULL),
        m_noiseNetwork(NULL),
        m_algonquin(NULL),
        m_numIters(MAX_NUM_ITERS)
    {}

    virtual ~Network()
    {
        delete m_speech;
        delete m_noise;
        delete m_algonquin;
    }

    // TODO: use tuple/real network instead
    struct MixtureNetwork
    {
        Dirichlet *weightsPrior;
        Discrete *weights;
        ConstGaussian *meanPrior;
        ConstGamma *precPrior;
        VariableArray<Gaussian> *means;
        VariableArray<Gamma> *precs;

        MixtureNetwork():
            weightsPrior(NULL),
            weights(NULL),
            meanPrior(NULL),
            precPrior(NULL),
            means(NULL),
            precs(NULL)
        {}

        virtual ~MixtureNetwork()
        {
            delete weightsPrior;
            delete weights;
            delete meanPrior;
            delete precPrior;
            delete means;
            delete precs;
        }
    };

    static MixtureNetwork *trainMixture(const double *points, size_t numPoints, size_t numMixtures, size_t maxNumIters)
    {


        auto dirichlet = Dirichlet(numMixtures, DIRICHLET_PRIOR);
        auto selector = DiscreteArray(numPoints, &dirichlet);
        selector.initialize(randomv(numPoints, numMixtures));

        auto meanPrior = ConstGaussian(MEAN_PRIOR);
        auto precPrior = ConstGamma(PREC_PRIOR);
        // distributions over parameters
        auto mean = GaussianArray<Gaussian, Gamma>(numMixtures, &meanPrior, &precPrior);
        auto prec = GammaArray(numMixtures, Parameters<Gamma>(GAMMA_PRIOR_RATE, GAMMA_PRIOR_SHAPE));
        auto data = MoGArray(numPoints, &mean, &prec, &selector);

        // messages
        data.observe(points);

        auto sequence = make_tuple(make_pair(&data, &mean),
                                   make_pair(&data, &prec),
                                   make_pair(&data, &selector),
                                   make_pair(&selector, &dirichlet));

        // running inference
        for (size_t i = 0; i < maxNumIters; i++)
            for_each(sequence, SendToParent());

        // initialising the resulting network
        MixtureNetwork *nwk = new MixtureNetwork;

        nwk->meanPrior = new ConstGaussian(MEAN_PRIOR);
        nwk->precPrior = new ConstGamma(PREC_PRIOR);
        nwk->means = new GaussianArray<Gaussian, Gamma>(mean.parameters(), nwk->meanPrior, nwk->precPrior);
        nwk->precs = new GammaArray(prec.parameters());
        nwk->weightsPrior = new Dirichlet(dirichlet.parameters());
        nwk->weights = new Discrete(nwk->weightsPrior);

        return nwk;
    }

    void trainSpeech(const double *frames, size_t numFrames, size_t numComps, size_t numIters)
    {
        m_speechNetwork = trainMixture(frames, numFrames, numComps, numIters);
        m_speech = new MoG(m_speechNetwork->means,
                           m_speechNetwork->precs,
                           m_speechNetwork->weights);
    }

    void trainNoise(const double *frames, size_t numFrames, size_t numComps, size_t numIters)
    {
        m_noiseNetwork = trainMixture(frames, numFrames, numComps, numIters);
        m_noise = new MoG(m_noiseNetwork->means,
                          m_noiseNetwork->precs,
                          m_noiseNetwork->weights);
    }


    pair<double, double> process(double frame)
    {
        if (m_algonquin == NULL)
            m_algonquin = new AlgonquinVariable(m_speech, m_noise);
        m_algonquin->observe(frame);
        m_algonquin->updatePosterior();
        return m_algonquin->m_fakeMoments;
    }


    //! check whether the network is already trained
    bool trained() const { return m_speech != NULL && m_noise != NULL; }

    const MoG *speechDistr() const { return m_speech; }
    const MoG *noiseDistr() const { return m_noise; }


private:
    MoG *m_speech;
    MoG *m_noise;
    MixtureNetwork *m_speechNetwork;
    MixtureNetwork *m_noiseNetwork;

    AlgonquinVariable *m_algonquin;

    size_t m_numIters;

};




} // namespace vmp
#endif // ALGONQUIN_H
