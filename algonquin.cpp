#include <algonquin.h>

using namespace algonquin;




pair<double, double> AlgonquinNode::run()
{
    for (size_t iter = 0; iter < NUM_ITERATIONS; ++iter)
        for (size_t s = 0; s < numSpeech(); ++s)
            for (size_t n = 0; n < numNoise(); ++n)
            {
                size_t i = index(s, n);
                double meanS = m_speechMeans[i];
                double meanN = m_noiseMeans[i];

                pair<double,double> jacob = sumLogJacobian(meanS, meanN);

                // phiS(:,c_s,c_n) = 1 ./ (prec_s + (dg_s .^ 2) .* inv_psi);
                m_speechVars[i] = 1.0  / (priorPrecSpeech(s) + sqr(jacob.first) * m_postPrec[i]);
                m_noiseVars[i] = 1.0 / (priorPrecNoise(n) + sqr(jacob.second) * m_postPrec[i]);

                // tmpS = (prec_s .* (muS(:,c_s) - meanS) + dg_s .* inv_psi .* (y - gFunc(meanS, meanN)));
                double tmpS = priorPrecSpeech(s) * (priorMeanSpeech(s) - meanS)
                        + jacob.first * m_postPrec[i] * (m_value - sumLog(meanS, meanN));

                // etaS(:,c_s,c_n) = meanS + phiS(:,c_s,c_n) .* tmpS
                m_speechMeans[i] += m_speechVars[i] * tmpS;

                double tmpN = priorPrecNoise(n) * (priorMeanNoise(n) - meanN)
                        + jacob.second * m_postPrec[i] * (m_value - sumLog(meanS, meanN));

                m_noiseMeans[i] += m_noiseVars[i] * tmpN;
            }

    // updating weight parameters
    for (size_t s = 0; s < numSpeech(); ++s)
        for (size_t n = 0; n < numNoise(); ++n)
        {
            size_t i = index(s,n);

            double meanS = meanSpeech(i);
            double meanN = meanNoise(i);

            pair<double,double> jacob = sumLogJacobian(meanS, meanN);

            double means = sqr(m_value - sumLog(meanS, meanN)) * m_postPrec[i] +
                    sqr(meanS - priorMeanSpeech(s)) * priorPrecSpeech(s) +
                    sqr(meanN - priorMeanNoise(n)) * priorPrecNoise(n);

            double tmp_s = sqr(jacob.first) * m_postPrec[i];
            double tmp_n = sqr(jacob.second) * m_postPrec[i];

            double traces = priorPrecSpeech(s) * m_speechVars[i] +
                            priorPrecNoise(n) * m_noiseVars[i] +
                    tmp_s * m_speechVars[i] +
                    tmp_n * m_noiseVars[i];

            double determs = log(1.0 / priorPrecSpeech(s)) + log(1.0 / priorPrecNoise(n))
                    - log(m_speechVars[i] * m_noiseVars[i]);

            m_weightParams[i] = priorWeightSpeech(s) * priorWeightNoise(n) * exp(-0.5 * (means + traces + determs));
        }

    normalize(m_weightParams);

    pair<double, double> result(0, 0);
    for (size_t i = 0; i < numParameters(); ++i)
    {
        result.first += m_speechMeans[i] * m_weightParams[i];
        result.second += m_noiseMeans[i] * m_weightParams[i];
    }

    return result;
}

