#ifndef ALGONQUIN_H
#define ALGONQUIN_H

#include <mixture.h>

#include <examples.h>
#include <matlab/persistentobject.h>



namespace algonquin
{

using namespace vmp;


const size_t MAX_NUM_ITERATIONS = 400;


/**
 * @brief a compound noise implementing algonquin algorithm for a single
 * frequency band
 * note that it has a compound distribution of its own
 */
class AlgonquinNode : public HasParent<Gaussian>
{
public:

    AlgonquinNode(const MoG *_speechPrior,
                  const MoG *_noisePrior):
        m_speechParent(_speechPrior),
        m_noiseParent(_noisePrior),
        m_speechMeans(numParameters(), 0.0), m_noiseMeans(numParameters(), 0.0),
        m_speechVars(numParameters(), 1.0), m_noiseVars(numParameters(), 1.0),
        m_weightParams(numParameters(), 1.0 / numParameters()),
        m_postPrec(numParameters(), 1.0)
    {
        // initialising parameters using priors
        // TODO: shouldn't this be done using message passing?
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


    // run a single iteration of the inference
    virtual void updatePosterior();

    void observe(double frame)
    {
        m_value = frame;
    }

    //! override HasParent<Gaussian>
    virtual void receiveFromParent(const Moments<Gaussian> &ms, Gaussian *parent)
    {
        throw NotImplementedException;
    }

    //! override HasParent<Gaussian>
    virtual Parameters<Gaussian> messageToParent(Gaussian *parent) const
    {
        throw NotImplementedException;
    }

    //! compute the approximation at the given point
    static inline double sumLog(double s, double n)
    {
        return s + log(1 + exp(n - s));
    }

    //! compute the jacobian \returns a pair(dg/ds, dg/dn)
    static inline pair<double,double> sumLogJacobian(double s, double n)
    {
        double tmp = exp(n - s) / (1 + exp(n - s));
        return make_pair(1 - tmp, tmp);
   }


private:
    //! get the unique index given s and n
    inline size_t index(size_t s, size_t n) const { return numSpeech() * n + s; }

    //! the number of speech/noise components
    inline size_t numSpeech() const { return m_speechParent->numComponents(); }
    inline size_t numNoise() const { return m_noiseParent->numComponents(); }
    //! get the total number of variational parameters
    inline size_t numParameters() const { return numSpeech() * numNoise(); }

    // current variational parameters
    inline double meanSpeech(size_t index) const {  return m_speechMeans[index]; }
    inline double meanNoise(size_t index) const { return m_noiseMeans[index]; }

    // prior accessors
    // TODO: add to MoG convenient functions?
    inline double priorMeanSpeech(size_t s) const { return m_speechParent->moments(s).mean; }
    inline double priorMeanNoise(size_t n) const { return m_noiseParent->moments(n).mean; }
    inline double priorPrecSpeech(size_t s) const { return m_speechParent->parameters(s).precision; }
    inline double priorPrecNoise(size_t n) const { return m_noiseParent->parameters(n).precision;; }

    // prior weights
    inline double priorWeightSpeech(size_t s) const { return m_speechParent->weight(s); }
    inline double priorWeightNoise(size_t n) const { return m_noiseParent->weight(n); }

    // parents
    const MoG *m_speechParent;
    const MoG *m_noiseParent;

    // TODO: add a prior over the indices

    // variational parameters
// TODO: exchange this with this:
//    vector<GaussianParameters> m_noiseParams;
//    vector<GaussianParameters> m_speechParams;
    vector<double> m_speechMeans;
    vector<double> m_noiseMeans;

    vector<double> m_speechVars;
    vector<double> m_noiseVars;

    vector<double> m_weightParams;

    // constant posterior precision
    vector<double> m_postPrec;

    // the observed value. TODO: make this an array instead?
    double m_value;

};



/**
 * utility class to store full algonquin network structure
 * TODO: make a generic version of algonquin::Network
 */
class Network : public PersistentObject
{
public:

    static const size_t NUM_SPEECH = 4;
    static const size_t NUM_NOISE = 1;

    Network():
        m_weightSpeech(NULL),
        m_weightNoise(NULL),
        m_speechPrior(NULL),
        m_noisePrior(NULL),
        m_node(NULL)
    {}

    virtual ~Network()
    {
        delete m_weightSpeech;
        delete m_weightNoise;

        delete m_speechPrior;
        delete m_noisePrior;
        delete m_node;
    }



    // TODO: make a constructor instead?
    static MoG *mixtureFromParameters(const Parameters<MoG> &params)
    {
        vector<Gaussian*> comps(params.numComponents(), NULL);
        Discrete *discr = new Discrete(logv(params.weights));
        for (size_t m = 0; m < params.numComponents(); ++m)
            comps[m] = new Gaussian(params.components[m].mean(),
                                    params.components[m].precision);
        return new MoG(comps, discr);
    }

    void train(const double *speechFrames, size_t numSpeechFrames,
               const double *noiseFrames, size_t numNoiseFrames)
    {
        double evidence;
        Parameters<MoG> params = trainGMM(speechFrames, numSpeechFrames,
                                          MAX_NUM_ITERATIONS, NUM_SPEECH,
                                          1, GaussianParameters(0, 1e-3), GammaParameters(1e-3, 1e-3),
                                          evidence);
        m_speechPrior = mixtureFromParameters(params);

        params = trainGMM(noiseFrames, numNoiseFrames,
                          MAX_NUM_ITERATIONS, NUM_NOISE,
                          1, GaussianParameters(0, 1e-3), GammaParameters(1e-3, 1e-3),
                          evidence);

        m_noisePrior = mixtureFromParameters(params);

        m_node = new AlgonquinNode(m_speechPrior, m_noisePrior);
    }


    void setPriors(const Parameters<MoG> &paramsSpeech, const Parameters<MoG> &paramsNoise)

    {
        cout << "HERE" << endl;
        cout << paramsSpeech << endl;
        cout << paramsNoise << endl;
    }

    inline const MoG *speechPrior() const { return m_speechPrior; }
    inline const MoG *noisePrior() const { return m_noisePrior; }


    void process(double frame)
    {
        m_node->observe(frame);
        m_node->updatePosterior();
    }

private:
    Discrete *m_weightSpeech;
    Discrete *m_weightNoise;

    MoG *m_speechPrior;
    MoG *m_noisePrior;
    AlgonquinNode *m_node;
};



/**
 * an abstraction representing an array of networks, each for a specific frequency
 */
class NetworkArray : public PersistentObject
{
public:
    NetworkArray(size_t _numFreqs):
        m_networks(_numFreqs)
    {}

    inline size_t numFreqs() const { return m_networks.size(); }

    //
    void train(double *speech, size_t numSpeech,
               double *noise, size_t numNoise)
    {
        for (size_t f = 0; f < numFreqs(); ++f)
        {
            double *s = speech + f * numSpeech;
            double *n = noise + f * numNoise;
            m_networks[f].train(s, numSpeech,
                                n, numNoise);
        }
    }

    void process(double *frame)
    {
        for (size_t f = 0; f < numFreqs(); ++f)
            m_networks[f].process(frame[f]);
    }

    inline const Network &network(size_t f) { return m_networks[f]; }


private:
    vector<Network> m_networks;
};



}


#endif // ALGONQUIN_H
