#include <examples.h>



#include <variable.h>
#include <gaussian.h>
#include <gaussianarray.h>
#include <gamma.h>
#include <mixture.h>
#include <network.h>
#include <discrete.h>
#include <ostream>
#include <ctime>
#include <limits>


using namespace vmp;
#include <cfloat>


void trainDirichlet(const size_t maxNumIters)
{
    const size_t DIMS = 5;
    Dirichlet *dir = new Dirichlet(vector<double>(DIMS, 1));
    vector<Discrete*> lambda;
    vector<Dirichlet::TParameters*> params;

    for (size_t p = 0; p < 100; ++p)
    {
        lambda.push_back(new Discrete(dir));
        params.push_back(dir->addChild(lambda[p]));
        // initialising
        lambda[p]->observe(p % 3);
    }


    double lbPrev = -DBL_MAX;

    for (size_t iter = 0; iter < maxNumIters; ++iter)
    {
        // running the inference
        for (size_t p = 0; p < lambda.size(); ++p)
            lambda[p]->messageToParent(params[p]);
        dir->updatePosterior();

        double lbData = 0.0;
        // summing up the lower bound
        for (size_t p = 0; p < lambda.size(); ++p)
            lbData += lambda[p]->logEvidence();

        double lbCurr = dir->logEvidence() +  lbData;

        if (lbCurr - lbPrev < EPSILON)
        {
            cout << "finished at: " << iter << " iterations "
                 << "with lb=" << lbCurr << endl;
            break;
        }
        lbPrev = lbCurr;
    }

    cout << expv(dir->moments().logProb) << endl;

    // sanity check
    cout << sumv(expv(dir->moments().logProb)) << endl;

    deleteAll(lambda);
    delete dir;
}



Parameters<MoG> trainGMM(const double *trainingData,
                              size_t numPoints,
                              size_t maxNumIters,
                                const size_t numMixtures,
                         double dirichletPrior,
                         const GaussianParameters &priorMean,
                         const GammaParameters &priorGamma,
                         double &evidence,
                         size_t &iters)
{
    Parameters<MoG> result;
    result.components.resize(numMixtures);
    result.weights.resize(numMixtures);

    Dirichlet dirichlet(vector<double>(numMixtures, dirichletPrior));

    DiscreteArray *selector = new DiscreteArray(numPoints, &dirichlet);
    vector<size_t> init(selector->size());
    for (size_t i = 0; i < selector->size(); ++i)
        init[i] = i % selector->dims();
    selector->initialize(init);

    // message
    Parameters<Dirichlet> *paramsDirichlet = dirichlet.addChild(selector);

    // TODO: pointers, so everything looks like
    // VectorArray<Gaussian> = GaussianArray<Gaussian, Gamma>;
    ConstGaussian *meanPrior = new ConstGaussian(priorMean.mean());
    ConstGamma *precPrior = new ConstGamma(priorMean.precision);

    VariableArray<Gaussian> *mean = new GaussianArray<Gaussian, Gamma>(numMixtures, meanPrior, precPrior);
    VariableArray<Gamma> *prec = new GammaArray(numMixtures, priorGamma);

    MoGArray data(numPoints, mean, prec, selector);
    data.observe(vector<double>(trainingData, trainingData + numPoints));

    VariableArray<Gaussian>::TParamsVector *paramsMean = mean->addChild(&data);
    VariableArray<Gamma>::TParamsVector *paramsPrec = prec->addChild(&data);
    VariableArray<Discrete>::TParamsVector *paramsSelector = selector->addChild(&data);

    double lbPrev = -DBL_MAX;
    for (size_t iter = 0; iter < maxNumIters; ++iter)
    {
        data.messageToParent(paramsMean);
        mean->updatePosterior();

        data.messageToParent(paramsPrec);
        prec->updatePosterior();

        data.messageToParent(paramsSelector);
        selector->updatePosterior();

        selector->messageToParent(paramsDirichlet);
        dirichlet.updatePosterior();

        double lbCurr = mean->logEvidence()
                        + prec->logEvidence()
                        + dirichlet.logEvidence()
                        + selector->logEvidence()
                        + data.logEvidence();
        assert(lbCurr > lbPrev);

        if (lbCurr - lbPrev < EPSILON)
        {
            iters = iter;
            evidence = lbCurr;
            break;
        }
        lbPrev = lbCurr;
    }


    for (size_t m = 0; m < numMixtures; ++m)
    {
        result.components[m] = GaussianParameters(mean->moments(m).mean * prec->moments(m).precision,
                                                  prec->moments(m).precision);
    }
    result.weights = expv(dirichlet.moments().logProb);



    delete meanPrior;
    delete precPrior;

    delete selector;
    delete mean;
    delete prec;

    return result;

}
