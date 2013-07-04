#include <algonquin/mvalgonquin.h>

#include <mathutil.h>

using namespace vmp;


void MVAlgonquin::updatePosterior()
{
    auto &precision = m_params.precision;
    auto &varsSpeech = m_params.varsSpeech;
    auto &varsNoise = m_params.varsNoise;
    auto &meansSpeech = m_params.meansSpeech;
    auto &meansNoise = m_params.meansNoise;
    auto &weights = m_params.weights;

    for (size_t iter = 0; iter < m_numIters; ++iter)
    {
        for (size_t s = 0; s < numSpeech(); ++s)
        {
            for (size_t n = 0; n < numNoise(); ++n)
            {
                size_t i = index(s, n);

                const vec &meanS = meansSpeech[i];
                const vec &meanN = meansNoise[i];

                vec logSumValue = logSumV(meanS, meanN);
                pair<vec,vec> jacob = logSumVJacobian(meanS, meanN);

                // fi = ( var^-1 + [dg_s, dg_n]^T * psi^-1 * [dg_s, dg_n])
                varsSpeech[i] = 1.0 / (priorPrecSpeech(s) + meanS % precision % meanS);
                varsNoise[i] = 1.0 / (priorPrecNoise(n) + meanN % precision % meanN);
                // nu^* = psi * (covar^-1 * (mu - nu) + g'^T * psi^-1 * (y - g))
                meansSpeech[i] = meanS + varsSpeech[i] % (priorPrecSpeech(s) % (priorMeanSpeech(s) - meanS) +
                                                          jacob.first % precision % (m_value - logSumValue));
                meansNoise[i] = meanN + varsNoise[i] % (priorPrecNoise(n) % (priorMeanNoise(n) - meanN) +
                                                        jacob.second % precision % (m_value - logSumValue));
            } // for n
        } // for s
    }


    for (size_t s = 0; s < numSpeech(); ++s)
    {
        for (size_t n = 0; n < numNoise(); ++n)
        {
            size_t i = index(s, n);

            const vec &meanS = meansSpeech[i];
            const vec &meanN = meansNoise[i];

            pair<vec,vec> jacob = logSumVJacobian(meanS, meanN);

            vec diffValue = (m_value - logSumV(meanS, meanN));
            vec diffS = (meanS - priorMeanSpeech(s));
            vec diffN = (meanN - priorMeanNoise(n));

            double means = sum(diffValue % diffValue % precision) +
                           sum(diffS % diffS % priorPrecSpeech(s)) +
                           sum(diffN % diffN % priorPrecNoise(n));


            vec tmpS = jacob.first % jacob.first % precision;
            vec tmpN = jacob.second % jacob.second % precision;

            double traces = sum(priorPrecSpeech(s) % varsSpeech[i]) +
                            sum(priorPrecNoise(n) % varsNoise[i]) +
                            sum(tmpS % varsSpeech[i] + tmpN % varsNoise[i]);

            double determs = -as_scalar(log(prod(priorPrecSpeech(s))) +
                                     log(prod(priorPrecNoise(n))) +
                                     log(prod(varsSpeech[i] % varsNoise[i])));

            weights[i] = priorWeightSpeech(s) * priorWeightNoise(n) * exp(-0.5 * (means + traces + determs));
        }
    }
    weights /= accu(weights);

    cout << weights.t() << endl;

    updateMoments();
}


void MVAlgonquin::updateMoments()
{
    m_speech.fill(0);
    m_noise.fill(0);
    for (size_t s = 0; s < numSpeech(); ++s)
        for (size_t n = 0; n < numNoise(); ++n)
        {
            size_t i = index(s,n);
            m_speech += m_params.weights[i] * m_params.meansSpeech[i];
            m_noise += m_params.weights[i] * m_params.meansNoise[i];
        }
}



Parameters<MVAlgonquin> vmp::initialize(const mat &sMeans, const mat &sVars, const vec &sWeights,
                                        const mat &nMeans,  const mat &nVars, const vec &nWeights,
                                        const double initPsi = 1e-3)
{
    size_t numSpeech = sWeights.size();
    size_t numNoise = nWeights.size();
    size_t numDims = sMeans.n_rows;

    Parameters<MVAlgonquin> params(numSpeech * numNoise, numDims);

    for (size_t s = 0; s < numSpeech; ++s)
    {
        for (size_t n = 0; n < numNoise; ++n)
        {
            size_t i = numSpeech * n + s;
            params.meansSpeech[i] = sMeans.col(s);
            params.meansNoise[i] = nMeans.col(n);
            params.varsSpeech[i] = sVars.col(s);
            params.varsNoise[i] = nVars.col(n);
            params.weights[i] = sWeights(s) * nWeights(n);
        }
    }
    params.precision.fill(initPsi);

    return params;
}


pair<mat,mat> vmp::runAlgonquin(const mat &sMeans, const mat &sVars, const vec &sWeights,
                                const mat &nMeans,  const mat &nVars, const vec &nWeights,
                                const mat &frames, const size_t numIters)
{
    mat resultS(frames);
    resultS.zeros();
    mat resultN(frames);
    resultN.zeros();

    size_t numFrames = frames.n_cols;
    size_t numSpeech = sWeights.size();
    size_t numNoise = nWeights.size();
    size_t numDims = sMeans.n_rows;

    mat priorPrecS = 1.0 / sVars;
    mat priorPrecN = 1.0 / nVars;

    auto index = [=](size_t s, size_t n) { return numSpeech * n + s; };

    auto processFrame = [&] (const vec &frame) -> pair<vec,vec>
    {
        Parameters<MVAlgonquin> vp = initialize(sMeans, sVars, sWeights, nMeans, nVars, nWeights, 1);

        // updating means/precisions
        for (size_t iter = 0; iter < numIters; ++iter)
        {
            for (size_t s = 0; s < numSpeech; ++s)
            {
                for (size_t n = 0; n < numNoise; ++n)
                {
                    size_t i = index(s, n);
                    vec &meanS = vp.meansSpeech[i];
                    vec &meanN = vp.meansNoise[i];

                    vec gDelta = frame - logSumV(meanS, meanN);
                    auto gJacob = logSumVJacobian(meanS, meanN);

//                    vec priorPrecS = 1.0 / sVars.col(s);
//                    vec priorPrecN = 1.0 / nVars.col(n);

                    // fi = ( var^-1 + [dg_s, dg_n]^T * psi^-1 * [dg_s, dg_n])
                    vp.varsSpeech[i] = 1.0 / (priorPrecS.col(s) + gJacob.first % vp.precision % gJacob.first);
                    vp.varsNoise[i] = 1.0 / (priorPrecN.col(n) + gJacob.second % vp.precision % gJacob.second);

                    // nu^* = psi * (covar^-1 * (mu - nu) + g'^T * psi^-1 * (y - g))
                    meanS += vp.varsSpeech[i] % (priorPrecS.col(s) % (sMeans.col(s) - meanS) + gJacob.first % vp.precision % gDelta);
                    meanN += vp.varsNoise[i] % (priorPrecN.col(n) % (nMeans.col(n) - meanN) + gJacob.second % vp.precision % gDelta);
                } // n
            } // s
        } // iter

        // updating weights
        for (size_t s = 0; s < numSpeech; ++s)
        {
            for (size_t n = 0; n < numNoise; ++n)
            {
                size_t i = index(s, n);

                vec &meanS = vp.meansSpeech[i];
                vec &meanN = vp.meansNoise[i];

                vec gDiff = frame - logSumV(meanS, meanN);
                pair<vec,vec> gJacob = logSumVJacobian(meanS, meanN);

                vec sDelta = (meanS - sMeans.col(s));
                vec nDelta = (meanN - nMeans.col(n));

                double means = dot(gDiff % gDiff, vp.precision) +
                               dot(sDelta % sDelta, priorPrecS.col(s)) +
                               dot(nDelta % nDelta, priorPrecN.col(n));

                double traces = dot(priorPrecS.col(s), vp.varsSpeech[i]) +
                                dot(priorPrecN.col(n), vp.varsNoise[i]) +
                                dot(gJacob.first % gJacob.first % vp.precision, vp.varsSpeech[i]) +
                                dot(gJacob.second % gJacob.second % vp.precision, vp.varsNoise[i]);

                double determs = sum(log(sVars.col(s))) +
                                 sum(log(nVars.col(n))) -
                                 sum(log(vp.varsSpeech[i] % vp.varsNoise[i]));

                vp.weights[i] = sWeights(s) * nWeights(n) * exp(-0.5 * (means + traces + determs));
            }
        }
        vp.weights /= accu(vp.weights);

        vec resultS = zeros(numDims, 1);
        vec resultN = zeros(numDims, 1);
        for (size_t s = 0; s < numSpeech; ++s)
            for (size_t n = 0; n < numNoise; ++n)
            {
                size_t i = index(s, n);
                resultS += vp.weights[i] * vp.meansSpeech[i];
                resultN += vp.weights[i] * vp.meansNoise[i];
            }
        return make_pair(resultS, resultN);
    };


    for (size_t f = 0; f < numFrames; ++f)
    {
        auto est = processFrame(frames.col(f));
        resultS.col(f) = est.first;
        resultN.col(f) = est.second;
    } // frames


    return make_pair(resultS, resultN);
}















