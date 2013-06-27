#ifndef ALGONQUINNETWORK_H
#define ALGONQUINNETWORK_H

#include <algonquin.h>

#include <future>

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


#endif // ALGONQUINNETWORK_H
