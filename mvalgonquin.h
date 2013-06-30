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
    // TODO: the number of components

    vector<vec> meansSpeech;
    vector<vec> meansNoise;
    vector<vec> varsSpeech;
    vector<vec> varsNoise;
    // component weights
    mat weights;
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
    const size_t DEFAULT_NUM_ITERATIONS = 20;


    MVAlgonquin(MultivariateMixture *_speechParent, MultivariateMixture *_noiseParent):
        m_speechParent(_speechParent),
        m_noiseParent(_noiseParent),
        m_numIters(DEFAULT_NUM_ITERATIONS)
      //        m_parameters(numSpeech(), numNoise())
    {}

    virtual ~MVAlgonquin() {}

    inline size_t numSpeech() const { return m_speechParent->dims(); }
    inline size_t numNoise() const { return m_noiseParent->dims(); }
    inline size_t numParameters() const { return numSpeech() * numNoise(); }

    inline void observe(const vec &value) { m_value = value; }

    inline size_t index(size_t s, size_t n) const { return numSpeech() * n + s; }


    inline void setNumIterations(size_t numIters) { m_numIters = numIters; }

    inline const vec &priorMeanSpeech(size_t s) const { return m_speechParent->meanMsg(s).mean; }
    inline const vec &priorMeanNoise(size_t n) const { return m_noiseParent->meanMsg(n).mean; }
    inline const vec &priorPrecSpeech(size_t s) const { return m_speechParent->precMsg(s).prec; }
    inline const vec &priorPrecNoise(size_t n) const { return m_noiseParent->precMsg(n).prec; }

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
    Parameters<MVAlgonquin> m_parameters;
    // constant posterior precision
    // the observed value. TODO: make this an array instead?
    vec m_value;
};



} // namespace vmp

#endif // MVALGONQUIN_H
