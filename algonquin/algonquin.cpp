#include <algonquin/algonquin.h>

#include <algorithm>

using namespace vmp;


void Algonquin::updatePosterior()
{
    vector<double> &meansSpeech = m_parameters.meansSpeech;
    vector<double> &meansNoise = m_parameters.meansNoise;
    vector<double> &varsSpeech = m_parameters.varsSpeech;
    vector<double> &varsNoise = m_parameters.varsSpeech;
    vector<double> &weights = m_parameters.weights;
    vector<double> &precision = m_parameters.precision;

    for (size_t iter = 0; iter < m_numIters; ++iter)
    {
        for (size_t s = 0; s < numSpeech(); ++s)
        {
            for (size_t n = 0; n < numNoise(); ++n)
            {
                size_t i = index(s, n);

                double logSumValue = logSum(meansSpeech[i], meansNoise[i]);
                pair<double,double> jacob = logSumJacobian(meansSpeech[i], meansNoise[i]);

                // phiS(:,c_s,c_n) = 1 ./ (prec_s + (dg_s .^ 2) .* inv_psi);
                varsSpeech[i] = 1.0  / (priorPrecSpeech(s) + sqr(jacob.first) * precision[i]);
                varsNoise[i] = 1.0 / (priorPrecNoise(n) + sqr(jacob.second) * precision[i]);

                // tmpS = (prec_s .* (muS(:,c_s) - meanS) + dg_s .* inv_psi .* (y - gFunc(meanS, meanN)));
                double tmpS = priorPrecSpeech(s) * (priorMeanSpeech(s) - meansSpeech[i])
                        + jacob.first * precision[i] * (m_value - logSumValue);

                // etaS(:,c_s,c_n) = meanS + phiS(:,c_s,c_n) .* tmpS
                meansSpeech[i] += varsSpeech[i] * tmpS;

                double tmpN = priorPrecNoise(n) * (priorMeanNoise(n) - meansNoise[i])
                        + jacob.second * precision[i] * (m_value - logSumValue);

                meansNoise[i] += varsNoise[i] * tmpN;
            }
        }
    }

    // updating weight parameters
    for (size_t s = 0; s < numSpeech(); ++s)
    {
        for (size_t n = 0; n < numNoise(); ++n)
        {
            size_t i = index(s,n);

            double meanS = meansSpeech[i];
            double meanN = meansNoise[i];

            double logSumValue = logSum(meanS, meanN);
            pair<double,double> jacob = logSumJacobian(meanS, meanN);

            double means = sqr(m_value - logSumValue) * precision[i] +
                    sqr(meanS - priorMeanSpeech(s)) * priorPrecSpeech(s) +
                    sqr(meanN - priorMeanNoise(n)) * priorPrecNoise(n);

            double tmp_s = sqr(jacob.first) * precision[i];
            double tmp_n = sqr(jacob.second) * precision[i];

            double traces = priorPrecSpeech(s) * varsSpeech[i] +
                            priorPrecNoise(n) * varsNoise[i] +
                    tmp_s * varsSpeech[i] +
                    tmp_n * varsNoise[i];

            double determs = log(1.0 / priorPrecSpeech(s)) + log(1.0 / priorPrecNoise(n))
                    - log(varsSpeech[i] * varsNoise[i]);

            weights[i] = priorWeightSpeech(s) * priorWeightNoise(n) * exp(-0.5 * (means + traces + determs));
        }
    }
    normalize(weights);

    // updating the moments (?)
    updateMoments();
}


void Algonquin::updateMoments()
{
    m_fakeMoments = make_pair(0, 0);
    for (size_t i = 0; i < numParameters(); ++i)
    {
        m_fakeMoments.first += m_parameters.meansSpeech[i] * m_parameters.weights[i];
        m_fakeMoments.second += m_parameters.meansNoise[i] * m_parameters.weights[i];
    }
}

