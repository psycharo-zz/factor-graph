#ifndef ALGONQUIN_H
#define ALGONQUIN_H

#include <mixture.h>

#include <examples.h>
#include <matlab/persistentobject.h>



namespace vmp {

/**
 * @brief a compound noise implementing algonquin algorithm for a single
 * frequency band
 * note that it has a compound distribution of its own
 */
class AlgonquinNode : public HasParent<Gaussian>
{
public:

    // TODO: do this up until convergence
    static const size_t NUM_ITERATIONS = 20;

    AlgonquinNode(const MoGArray *_speechPrior,
                  const MoGArray *_noisePrior):
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


    //! returns a pair <speech_estimation, noise_estimation>
    virtual pair<double,double> run();

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
    inline size_t numSpeech() const { return m_speechParent->dims(); }
    inline size_t numNoise() const { return m_noiseParent->dims(); }
    //! get the total number of variational parameters
    inline size_t numParameters() const { return numSpeech() * numNoise(); }

    // current variational parameters
    inline double meanSpeech(size_t index) const {  return m_speechMeans[index]; }
    inline double meanNoise(size_t index) const { return m_noiseMeans[index]; }

    // prior accessors
    // TODO: add to MoG convenient functions? TODO: parameters() ?
    inline double priorMeanSpeech(size_t s) const { throw NotImplementedException; }// { return m_speechParent->mean()->moments(s).mean; }
    inline double priorMeanNoise(size_t n) const { throw NotImplementedException; }//{ return m_noiseParent->mean()->moments(n).mean; }
    inline double priorPrecSpeech(size_t s) const { throw NotImplementedException; }//{ return m_speechParent->precision()->moments(s).precision; }
    inline double priorPrecNoise(size_t n) const { throw NotImplementedException; }// { return m_noiseParent->precision()->moments(n).precision; }

    // prior weights
    inline double priorWeightSpeech(size_t s) const { throw NotImplementedException; }//return m_speechParent->weight(s); }
    inline double priorWeightNoise(size_t n) const { throw NotImplementedException; }//return m_noiseParent->weight(n); }

    // parents
    const MoGArray *m_speechParent;
    const MoGArray *m_noiseParent;

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

    static const size_t NUM_ITERATIONS = 150;

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
    static MoG *mixtureFromParameters(const Parameters<MoG> &params, Discrete* &discrete)
    {
        VariableArray<Gaussian> *means = new GaussianArray<Gaussian, Gamma>(params.dims(), NULL, NULL);
//        VariableArray<Gamma> *precs = new GammaArray(params.dims(), );
        discrete = new Discrete(logv(params.weights));

        // priors

//        for (size_t m = 0; m < params.dims(); ++m)
//        {
//            comps[m] = new ConstGaussian(params.components[m].mean(), params.components[m].precision);
//        }
//        return new MoGArray(1, means, precs, _selector);
        return NULL;
    }

    void train(const double *speechFrames, size_t numSpeechFrames, size_t numSpeechComps,
               const double *noiseFrames, size_t numNoiseFrames, size_t numNoiseComps,
               size_t maxNumIters)
    {
        double evidence;
        size_t iterations;
        Parameters<MoG> params = trainGMM(speechFrames, numSpeechFrames,
                                          maxNumIters, numSpeechComps,
                                          1, GaussianParameters(0, 1e-3), GammaParameters(1e-3, 1e-3),
                                          evidence,
                                          iterations);
        m_speechPrior = mixtureFromParameters(params, m_weightSpeech);

//        params = trainGMM(noiseFrames, numNoiseFrames,
//                          maxNumIters, numNoiseComps,
//                          1, GaussianParameters(0, 1e-3), GammaParameters(1e-3, 1e-3),
//                          evidence,
//                          iterations);

        m_noisePrior = mixtureFromParameters(params, m_weightNoise);

//        m_node = new AlgonquinNode(m_speechPrior, m_noisePrior);
    }


    void setPriors(const Parameters<MoG> &paramsSpeech, const Parameters<MoG> &paramsNoise)
    {
        m_speechPrior = mixtureFromParameters(paramsSpeech, m_weightSpeech);
        m_noisePrior = mixtureFromParameters(paramsNoise, m_weightNoise);

//        m_node = new AlgonquinNode(m_speechPrior, m_noisePrior);
    }

    inline const MoG *speechPrior() const { return m_speechPrior; }
    inline const MoG *noisePrior() const { return m_noisePrior; }


    pair<double, double> process(double frame)
    {
        assert(m_node != NULL);
        m_node->observe(frame);
        return m_node->run();
    }

private:
    Discrete *m_weightSpeech;
    Discrete *m_weightNoise;

    MoG *m_speechPrior;
    MoG *m_noisePrior;
    AlgonquinNode *m_node;
};



};



#endif // ALGONQUIN_H
