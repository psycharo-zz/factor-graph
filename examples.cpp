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


vmp::MixtureNetwork *vmp::trainMixture(const double *points, size_t numPoints, size_t numMixtures, size_t maxNumIters)
{
    // initialising the resulting network
    MixtureNetwork *nwk = new MixtureNetwork;

    auto dirichlet = Dirichlet(numMixtures, DIRICHLET_PRIOR);
    auto selector = DiscreteArray(numPoints, &dirichlet);
    selector.initialize(randomv(numPoints, numMixtures));

    auto meanPrior = ConstGaussian(GAUSS_PRIOR_MEAN);
    auto precPrior = ConstGamma(GAUSS_PRIOR_PREC);
    // distributions over parameters
    auto mean = GaussianArray<Gaussian, Gamma>(numMixtures, &meanPrior, &precPrior);
    auto prec = GammaArray(numMixtures, Parameters<Gamma>(GAMMA_PRIOR_SHAPE, GAMMA_PRIOR_RATE));
    auto data = MoGArray(numPoints, &mean, &prec, &selector);

    // messages
    data.observe(points);

    auto sequence = make_tuple(make_pair(&data, &mean),
                               make_pair(&data, &prec),
                               make_pair(&data, &selector),
                               make_pair(&selector, &dirichlet));

    auto network = make_tuple(&data, &mean, &prec, &selector, &dirichlet);

    // running inference
    for (size_t i = 0; i < maxNumIters; i++)
    {
        for_each(sequence, SendToParent());

        AddEvidence lbCurr;
        for_each(network, lbCurr);

        if (lbCurr.value - nwk->evidence <= EPSILON)
        {
            nwk->iters = i;
            nwk->evidence = lbCurr.value;
            break;
        }
        nwk->evidence = lbCurr.value;
    }

    nwk->meanPrior = new ConstGaussian(GAUSS_PRIOR_MEAN);
    nwk->precPrior = new ConstGamma(GAUSS_PRIOR_PREC);
    nwk->means = new GaussianArray<Gaussian, Gamma>(mean.parameters(), nwk->meanPrior, nwk->precPrior);
    nwk->precs = new GammaArray(prec.parameters());
    nwk->weightsPrior = new Dirichlet(dirichlet.parameters());
    nwk->weights = new Discrete(nwk->weightsPrior);

    return nwk;
}

