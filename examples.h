#ifndef EXAMPLES_H
#define EXAMPLES_H

#include <util.h>
#include <vector>
#include <memory>

#include <variable.h>
#include <gaussian.h>
#include <mixture.h>


namespace vmp {



//
const double DIRICHLET_PRIOR = 1;
const double GAMMA_PRIOR_SHAPE = 1e-3;
const double GAMMA_PRIOR_RATE = 1e-3;
const double GAUSS_PRIOR_MEAN = 0;
const double GAUSS_PRIOR_PREC = 1e-3;


// TODO: use tuple/real network instead
struct MixtureNetwork
{
    MixtureNetwork():
        weightsPrior(NULL),
        weights(NULL),
        meanPrior(NULL), precPrior(NULL),
        means(NULL), precs(NULL),
        evidence(LB_INIT),
        iters(0)
    {}

    virtual ~MixtureNetwork()
    {
        delete weightsPrior;
        delete weights;
        delete meanPrior;
        delete precPrior;
        delete means;
        delete precs;
    }

    Parameters<MoG> parameters() const
    {
        assert(means != NULL);
        Parameters<MoG> result;
        result.components.resize(means->size());
        result.weights.resize(means->size());

        for (size_t m = 0; m < means->size(); ++m)
            result.components[m] = Parameters<Gaussian>(means->moments(m).mean * precs->moments(m).precision,
                                                        precs->moments(m).precision);
        result.weights = weights->moments().probs;

        return result;
    }

    Dirichlet *weightsPrior;
    Discrete *weights;
    ConstGaussian *meanPrior;
    ConstGamma *precPrior;
    VariableArray<Gaussian> *means;
    VariableArray<Gamma> *precs;
    double evidence;
    size_t iters;

};

MixtureNetwork *trainMixture(const double *points, size_t numPoints, size_t numMixtures, size_t maxNumIters);



} // namespace vmp



#endif // EXAMPLES_H
