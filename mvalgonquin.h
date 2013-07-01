#ifndef MVALGONQUIN_H
#define MVALGONQUIN_H


#include <mathutil.h>
#include <mixture.h>


namespace vmp
{


class MVAlgonquin;

template<>
class Parameters<MVAlgonquin>
{
public:
    Parameters(size_t num):
        meansSpeech(num), meansNoise(num),
        varsSpeech(num), varsNoise(num),
        weights(num), precision(num)
    {}
    // TODO: the number of components
    vector<vec> meansSpeech;
    vector<vec> meansNoise;
    vector<vec> varsSpeech;
    vector<vec> varsNoise;
    // component weights
    vec weights;
    // constant posterior precision
    vec precision;
};




/**
 * @brief The AlgonquinVariable class
 * scalar deterministic (?) node implementing algonquin inference
 */
class MVAlgonquin : public BaseVariable
//                    public HasParent<VariableArray<MVGaussian> >,
//                    public HasParent<VariableArray<Wishart> >,
//                    public HasParent<Discrete>
{
public:
    const size_t DEFAULT_NUM_ITERATIONS = 4;


    MVAlgonquin(MultivariateMixture *_speechParent, MultivariateMixture *_noiseParent):
        m_speechParent(_speechParent),
        m_noiseParent(_noiseParent),
        m_numIters(DEFAULT_NUM_ITERATIONS),
        m_params(numSpeech() * numNoise())
    {

        for (size_t s = 0; s < numSpeech(); ++s)
        {
            cout << diagvec(m_speechParent->precMsg(s).prec).t() << endl;
            for (size_t n = 0; n < numNoise(); ++n)
            {
                size_t i = index(s,n);
                m_params.meansSpeech[i] = m_speechParent->meanMsg(s).mean;
                m_params.meansNoise[i] = m_noiseParent->meanMsg(n).mean;
                m_params.varsSpeech[i] = 1.0 / diagvec(m_speechParent->precMsg(s).prec);
                m_params.varsNoise[i] = 1.0 / diagvec(m_noiseParent->precMsg(n).prec);
                m_params.weights[i] = m_speechParent->weight(s) * m_noiseParent->weight(n);
            }
        }
        m_params.precision.fill(1e-3);
    }

    virtual ~MVAlgonquin() {}

    inline size_t numSpeech() const { return m_speechParent->dims(); }
    inline size_t numNoise() const { return m_noiseParent->dims(); }
    inline size_t numParameters() const { return numSpeech() * numNoise(); }

    inline void observe(const vec &value)
    {
        m_value = value;
        m_speech = m_value;
        m_noise = m_value;
    }

    inline size_t index(size_t s, size_t n) const { return numSpeech() * n + s; }

    inline void setNumIterations(size_t numIters) { m_numIters = numIters; }

    inline const vec &priorMeanSpeech(size_t s) const { return m_speechParent->meanMsg(s).mean; }
    inline const vec &priorMeanNoise(size_t n) const { return m_noiseParent->meanMsg(n).mean; }
    inline vec priorPrecSpeech(size_t s) const { return diagvec(m_speechParent->precMsg(s).prec); }
    inline vec priorPrecNoise(size_t n) const { return diagvec(m_noiseParent->precMsg(n).prec); }

    // prior weights
    inline double priorWeightSpeech(size_t s) const { return m_speechParent->weight(s); }
    inline double priorWeightNoise(size_t n) const { return m_noiseParent->weight(n); }


    //! override TODO: Variable/BaseVariable
    virtual void updatePosterior();
    //! override
    virtual void updateMoments();


    // TODO: moments?
    vec m_speech;
    vec m_noise;


private:
    MultivariateMixture *m_speechParent;
    MultivariateMixture *m_noiseParent;
    size_t m_numIters;
    // variational parameters
    Parameters<MVAlgonquin> m_params;
    // constant posterior precision
    // the observed value. TODO: make this an array instead?
    vec m_value;
};



} // namespace vmp

#endif // MVALGONQUIN_H
