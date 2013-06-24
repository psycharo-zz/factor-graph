#include <examples.h>




#include <variable.h>

#include <gaussian.h>
#include <gaussianarray.h>
#include <gamma.h>
#include <gammaarray.h>
#include <discrete.h>


#include <mixture.h>
#include <network.h>

#include <ostream>
#include <ctime>
#include <limits>


using namespace vmp;
#include <cfloat>


Parameters<MoG> trainGMM(const double *trainingData, size_t numPoints, size_t maxNumIters, const size_t numMixtures,
                         double dirichletPrior, const GaussianParameters &priorMean, const GammaParameters &priorGamma,
                         double &evidence, size_t &iters)
{
    Parameters<MoG> result;
    result.components.resize(numMixtures);
    result.weights.resize(numMixtures);

    auto dirichlet = Dirichlet(numMixtures, dirichletPrior);
    auto selector = DiscreteArray(numPoints, &dirichlet);
    selector.initialize(randomv(numPoints, numMixtures));

    auto meanPrior = ConstGaussian(priorMean.mean());
    auto precPrior = ConstGamma(priorMean.precision);
    auto mean = GaussianArray<Gaussian, Gamma>(numMixtures, &meanPrior, &precPrior);
    auto prec = GammaArray(numMixtures, priorGamma);

    auto data = MoGArray(numPoints, &mean, &prec, &selector);
    data.observe(trainingData);

    auto sequence = make_tuple(make_pair(&data, &mean),
                               make_pair(&data, &prec),
                               make_pair(&data, &selector),
                               make_pair(&selector, &dirichlet));

    auto network = make_tuple(&data, &mean, &prec, &selector, &dirichlet);

    iters = maxNumIters;
    evidence = LB_INIT;
    for (size_t i = 0; i < maxNumIters; i++)
    {
        for_each(sequence, SendToParent());

        // computing evidence
        AddEvidence lbCurr;
        for_each(network, lbCurr);

        if (lbCurr.value - evidence <= EPSILON)
        {
            iters = i;
            evidence = lbCurr.value;
            break;
        }
        evidence = lbCurr.value;
    }

    for (size_t m = 0; m < numMixtures; ++m)
    {
        result.components[m] = Parameters<Gaussian>(mean.moments(m).mean * prec.moments(m).precision,
                                                    prec.moments(m).precision);
    }
    result.weights = expv(dirichlet.moments().logProb);

    return result;
}

