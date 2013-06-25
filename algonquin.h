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


//! compute the approximation at the given point
//inline double logSum(double s, double n)
//{
////    return s + log(1 + exp(n - s));
//}

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





class AlgonquinNetwork : public PersistentObject
{
public:
    static const size_t MAX_NUM_ITERS = 100;

    // hyperparameters
    AlgonquinNetwork(size_t numIters = MAX_NUM_ITERS):
        m_speech(NULL),
        m_noise(NULL),
        m_speechNetwork(NULL),
        m_noiseNetwork(NULL),
        m_algonquin(NULL),
        m_numIters(MAX_NUM_ITERS)
    {}

    virtual ~AlgonquinNetwork()
    {
        delete m_speech;
        delete m_noise;
        delete m_algonquin;
    }

    void train(const double *framesS, size_t numFramesS, size_t numCompsS,
               const double *framesN, size_t numFramesN, size_t numCompsN,
                     size_t numIters)
    {
        m_speechNetwork = trainMixture(framesS, numFramesS, numCompsS, numIters);
        m_speech = new MoG(m_speechNetwork->means,
                           m_speechNetwork->precs,
                           m_speechNetwork->weights);

        m_noiseNetwork = trainMixture(framesN, numFramesN, numCompsN, numIters);
        m_noise = new MoG(m_noiseNetwork->means,
                          m_noiseNetwork->precs,
                          m_noiseNetwork->weights);

        m_algonquin = new AlgonquinVariable(m_speech, m_noise);
    }

    const pair<double, double> &process(double frame)
    {
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



class NetworkArray : public PersistentObject
{
public:
    NetworkArray(size_t numBins):
        m_networks(numBins),
        m_speech(numBins, 0.0),
        m_noise(numBins, 0.0)
    {}

    size_t numBins() const { return m_networks.size(); }

    //! train an array of networks
    void train(double *speech, size_t numSpeechFrames, size_t numSpeechComps,
               double *noise, size_t numNoiseFrames, size_t numNoiseComps,
               size_t numIters)
    {
        for (size_t bin = 0; bin < numBins(); ++bin)
        {
            m_networks[bin].train(&speech[numSpeechFrames * bin], numSpeechFrames, numSpeechComps,
                                  &noise[numNoiseFrames * bin], numNoiseFrames, numNoiseComps,
                                  numIters);
        }
    }

    pair<double*, double*> process(double *frame)
    {
        for (size_t bin = 0; bin < numBins(); ++bin)
        {
            auto result = m_networks[bin].process(frame[bin]);
            m_speech[bin] = result.first;
            m_noise[bin] = result.second;
        }
        return make_pair(m_speech.data(), m_noise.data());
    }

    const MoG *speechDistr(size_t m) const { return m_networks[m].speechDistr(); }
    const MoG *noiseDistr(size_t m) const { return m_networks[m].noiseDistr(); }

private:
    vector<AlgonquinNetwork> m_networks;

    vector<double> m_speech;
    vector<double> m_noise;


};




} // namespace vmp
#endif // ALGONQUIN_H
