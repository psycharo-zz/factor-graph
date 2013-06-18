#include <examples.h>



#include <variable.h>
#include <gaussian.h>
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
    for (size_t p = 0; p < 100; ++p)
    {
        lambda.push_back(new Discrete(dir));
        // initialising
        lambda[p]->observe(p % 3);
    }


    double lbPrev = -DBL_MAX;

    for (size_t iter = 0; iter < maxNumIters; ++iter)
    {
        // running the inference
        for (size_t p = 0; p < lambda.size(); ++p)
            dir->receiveFromChild(lambda[p]->messageToParent(dir), lambda[p]);
        dir->updatePosterior();

        for (size_t p = 0; p < lambda.size(); ++p)
            lambda[p]->receiveFromParent(dir->messageToChildren(), dir);

        double lbData = 0.0;
        // summing up the lower bound
        for (size_t p = 0; p < lambda.size(); ++p)
            lbData += lambda[p]->logEvidenceLowerBound();

        double lbCurr = dir->logEvidenceLowerBound() +  lbData;

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




void trainUnivariateGaussian(const size_t maxNumIters)
{
    Gaussian mu(0, 1e-3);
    mu.updatePosterior();
    Gamma gamma(1e-3, 1e-3);
    gamma.updatePosterior();

    vector<Gaussian> x;
    // TODO: try for various parameters

    for (size_t i = 0; i < NUM_POINTS; ++i)
    {
        x.push_back(Gaussian(&mu, &gamma));
        x[i].observe(DATA[i]);
    }


    cout << NUM_POINTS << endl;


    double lbPrev = -DBL_MAX;

    // doing the updates
    for (size_t iter = 0; iter < maxNumIters; ++iter)
    {
        for (size_t p = 0; p < x.size(); ++p)
        {
            // send messages from gamma to all x
            x[p].receiveFromParent(gamma.messageToChildren(), &gamma);
            // send messages from all x to mu
            mu.receiveFromChild(x[p].messageToParent(&mu), &x[p]);
        }
        mu.updatePosterior();

        for (size_t p = 0; p < x.size(); ++p)
        {
            // send messages from mu to all x
            x[p].receiveFromParent(mu.messageToChildren(), &mu);
            // send messages from x to all gamma
            gamma.receiveFromChild(x[p].messageToParent(&gamma), &x[p]);
        }
        gamma.updatePosterior();

        double lbData = 0;
        for (size_t p = 0; p < x.size(); ++p)
            lbData += x[p].logEvidenceLowerBound();

        double lbCurr = lbData + mu.logEvidenceLowerBound() + gamma.logEvidenceLowerBound();

        if (fabs(lbCurr - lbPrev) < EPSILON)
        {
            cout << "stopped at:" << iter << " iterations"
                 << endl
                 << "lb=" << lbCurr
                 << endl;
            break;
        }
        lbPrev = lbCurr;
    }

    cout << "mean,var:" << mu.moments().mean << " " << 1./ gamma.moments().precision << endl;
    cout << "stdev:" << sqrt(1./gamma.moments().precision) << endl;

}


Parameters<MoG> trainGMM(const double *trainingData,
                         size_t numPoints,
                         size_t maxNumIters,
                         const size_t numMixtures,
                         double dirichletPrior,
                         const GaussianParameters &priorMean,
                         const GammaParameters &priorGamma,
                         double &evidence)
{
    Parameters<MoG> result;

    Dirichlet *dir = new Dirichlet(vector<double>(numMixtures, dirichletPrior));
    vector<Gaussian*> mean(numMixtures, NULL);
    vector<Gamma*> prec(numMixtures, NULL);

    for (size_t m = 0; m < numMixtures; ++m)
    {
        // TODO: put the first updatePosterior() call into constructor?
        mean[m] = new Gaussian(priorMean.mean(), priorMean.precision);
        mean[m]->updatePosterior();

        prec[m] = new Gamma(priorGamma.shape, priorGamma.rate);
        prec[m]->updatePosterior();
    }


    // TODO: this should be done inside of the mixture??
    vector<Discrete*> discr(numPoints, NULL);
    vector<MoG*> data(numPoints, NULL);

    for (size_t p = 0; p < numPoints; ++p)
    {
        discr[p] = new Discrete(dir);
        data[p] = new MoG(mean, prec, discr[p]);

        // TODO: using the message passing instead of observing directly
        vector<double> logProb(numMixtures, -1);
        logProb[discr[p]->sample()] = 0;

        discr[p]->receiveFromParent(dir->messageToChildren(), dir);
        discr[p]->receiveFromChild(Parameters<Discrete>(logProb), data[p]);
        discr[p]->updatePosterior();

        data[p]->observe(trainingData[p]);
        data[p]->receiveFromParent(discr[p]->messageToChildren(), discr[p]);
    }

    double lbPrev = -DBL_MAX;

    // doing the inference
    for (size_t iter = 0; iter < maxNumIters; ++iter)
    {
        // updating the mean components
        for (size_t m = 0; m < numMixtures; ++m)
        {
            for (size_t p = 0; p < numPoints; ++p)
            {
                data[p]->receiveFromParent(m, prec[m]->messageToChildren(), prec[m]);
                mean[m]->receiveFromChild(data[p]->messageToParent(m, mean[m]), data[p]);
            }
            // TODO: UNIFY computation of the moments() and messageToParents()!
            mean[m]->updatePosterior();
        }
        // updating the precision components
        for (size_t m = 0; m < numMixtures; ++m)
        {
            // receive all the messages from data
            for (size_t p = 0; p < numPoints; ++p)
            {
                data[p]->receiveFromParent(m, mean[m]->messageToChildren(), mean[m]);
                prec[m]->receiveFromChild(data[p]->messageToParent(m, prec[m]), data[p]);
            }
            prec[m]->updatePosterior();
        }
        // propagating the precision info
        for (size_t m = 0; m < numMixtures; ++m)
        {
            Moments<Gamma> msg = prec[m]->messageToChildren();
            for (size_t p = 0; p < numPoints; ++p)
                data[p]->receiveFromParent(m, msg, prec[m]);
        }

        // updating the discrete distributions
        for (size_t p = 0; p < numPoints; ++p)
        {
            discr[p]->receiveFromParent(dir->messageToChildren(), dir);
            discr[p]->receiveFromChild(data[p]->messageToParent(discr[p]), data[p]);
            discr[p]->updatePosterior();
        }

        // updating the dirichlet
        for (size_t p = 0; p < numPoints; ++p)
            dir->receiveFromChild(discr[p]->messageToParent(dir), discr[p]);
        dir->updatePosterior();

        for (size_t p = 0; p < numPoints; ++p)
            // updating the data
            data[p]->receiveFromParent(discr[p]->messageToChildren(), discr[p]);

        // computing the lower bound
        double lbMean = 0;
        double lbPrec = 0;
        for (size_t m = 0; m < numMixtures; ++m)
        {
            lbMean += mean[m]->logEvidenceLowerBound();
            lbPrec += prec[m]->logEvidenceLowerBound();
        }

        double lbDiscr = 0;
        for (size_t p = 0; p < numPoints; ++p)
            lbDiscr += discr[p]->logEvidenceLowerBound();

        double lbData = 0;
        for (size_t p = 0; p < numPoints; ++p)
            lbData += data[p]->logEvidenceLowerBound();

        double lbCurr = 0;
        lbCurr += lbData + lbDiscr + lbMean + lbPrec;

        // TODO: save the numbr of iterations?
        if (fabs(lbCurr - lbPrev) < EPSILON)
            break;
        lbPrev = lbCurr;
    }


    result.components.resize(numMixtures);
    for (size_t m = 0; m < numMixtures; ++m)
    {
        result.components[m].meanPrecision = mean[m]->moments().mean * prec[m]->moments().precision;
        result.components[m].precision = prec[m]->moments().precision;
    }

    result.weights.resize(numMixtures);
    for (size_t m = 0; m < numMixtures; ++m)
        result.weights[m] = exp(dir->moments().logProb[m]);
    evidence = lbPrev;

    delete dir;
    deleteAll(mean);
    deleteAll(prec);
    deleteAll(data);
    deleteAll(discr);

    return result;
}





