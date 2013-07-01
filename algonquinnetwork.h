#ifndef ALGONQUINNETWORK_H
#define ALGONQUINNETWORK_H

#include <persistentobject.h>
#include <mvalgonquin.h>
#include <examples.h>


namespace vmp
{


class AlgonquinNetwork : public PersistentObject
{
public:
    static const size_t MAX_NUM_ITERS = 5;

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

    void train(const mat &framesS, size_t numCompsS,
               const mat &framesN, size_t numCompsN,
               size_t numIters)
    {
        m_speechNetwork = trainMVMixture(framesS, numCompsS, numIters);
        m_speech = new MultivariateMixture(m_speechNetwork->means, m_speechNetwork->precs, m_speechNetwork->weights);

        m_noiseNetwork = trainMVMixture(framesN, numCompsN, numIters);
        m_noise = new MultivariateMixture(m_noiseNetwork->means, m_noiseNetwork->precs, m_noiseNetwork->weights);

        m_algonquin = new MVAlgonquin(m_speech, m_noise);

    }

    pair<vec*,vec*> process(const vec &frame)
    {
        m_algonquin->observe(frame);
        m_algonquin->updatePosterior();
        return make_pair(&m_algonquin->m_speech, &m_algonquin->m_noise);
    }

    //! check whether the network is already trained
    bool trained() const { return m_speech != NULL && m_noise != NULL; }

    const MultivariateMixture *speechDistr() const { return m_speech; }
    const MultivariateMixture *noiseDistr() const { return m_noise; }

private:
    MultivariateMixture *m_speech;
    MultivariateMixture *m_noise;
    MVMixtureNetwork *m_speechNetwork;
    MVMixtureNetwork *m_noiseNetwork;

    MVAlgonquin *m_algonquin;

    size_t m_numIters;
};







} // namespace vmp


#endif // ALGONQUINNETWORK_H
