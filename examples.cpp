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


    double lbPrev = std::numeric_limits<double>::lowest();

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


    double lbPrev = numeric_limits<double>::lowest();

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




void trainMixtureOfUnivariateGaussians(const size_t maxNumIters)
{
    const size_t DIMS = 20;

    // TODO: random dirichlet initialisation
    Dirichlet *dir = new Dirichlet(vector<double>(DIMS, 1));
    vector<Gaussian*> mean(DIMS, NULL);
    vector<Gamma*> prec(DIMS, NULL);

    for (size_t m = 0; m < DIMS; ++m)
    {
        // TODO: put the first updatePosterior() call into constructor?
        mean[m] = new Gaussian(0, 1e-3);
        mean[m]->updatePosterior();

        prec[m] = new Gamma(1e-3, 1e-3);
        prec[m]->updatePosterior();
    }


    // TODO: this should be done inside of the mixture??
    vector<Discrete*> discr(MIX_NUM_POINTS, NULL);
    vector<MoG*> data(MIX_NUM_POINTS, NULL);
    for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
    {
        discr[p] = new Discrete(dir);
        data[p] = new MoG(mean, prec, discr[p]);

        // TODO: using the message passing instead of observing directly
        vector<double> logProb(DIMS, -1);
        logProb[discr[p]->sample()] = 0;

        discr[p]->receiveFromParent(dir->messageToChildren(), dir);
        discr[p]->receiveFromChild(Parameters<Discrete>(logProb), data[p]);
        discr[p]->updatePosterior();

        data[p]->observe(MIX_DATA[p]);
        data[p]->receiveFromParent(discr[p]->messageToChildren(), discr[p]);
    }

    double lbPrev = std::numeric_limits<double>::lowest();

    // doing the inference
    for (size_t iter = 0; iter < maxNumIters; ++iter)
    {
        // updating the mean components
        for (size_t m = 0; m < DIMS; ++m)
        {
            for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
            {
                data[p]->receiveFromParent(m, prec[m]->messageToChildren(), prec[m]);
                mean[m]->receiveFromChild(data[p]->messageToParent(m, mean[m]), data[p]);
            }
            // TODO: UNIFY computation of the moments() and messageToParents()!
            mean[m]->updatePosterior();
        }
        // updating the precision components
        for (size_t m = 0; m < DIMS; ++m)
        {
            // receive all the messages from data
            for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
            {
                data[p]->receiveFromParent(m, mean[m]->messageToChildren(), mean[m]);
                prec[m]->receiveFromChild(data[p]->messageToParent(m, prec[m]), data[p]);
            }
            prec[m]->updatePosterior();
        }
        // propagating the precision info
        for (size_t m = 0; m < DIMS; ++m)
        {
            Moments<Gamma> msg = prec[m]->messageToChildren();
            for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
                data[p]->receiveFromParent(m, msg, prec[m]);
        }

        // updating the discrete distributions
        for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
        {
            discr[p]->receiveFromParent(dir->messageToChildren(), dir);
            discr[p]->receiveFromChild(data[p]->messageToParent(discr[p]), data[p]);
            discr[p]->updatePosterior();
        }

        // updating the dirichlet
        for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
            dir->receiveFromChild(discr[p]->messageToParent(dir), discr[p]);
        dir->updatePosterior();

        for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
            // updating the data
            data[p]->receiveFromParent(discr[p]->messageToChildren(), discr[p]);

        // computing the lower bound
        double lbMean = 0;
        double lbPrec = 0;
        for (size_t m = 0; m < DIMS; ++m)
        {
            lbMean += mean[m]->logEvidenceLowerBound();
            lbPrec += prec[m]->logEvidenceLowerBound();
        }

        double lbDiscr = 0;
        for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
            lbDiscr += discr[p]->logEvidenceLowerBound();

        double lbData = 0;
        for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
            lbData += data[p]->logEvidenceLowerBound();

        double lbCurr = 0;
        lbCurr += lbData + lbDiscr + lbMean + lbPrec;

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


    for (size_t m = 0; m < DIMS; ++m)
        cout << mean[m]->moments().mean << " ";
    cout << endl;

    for (size_t m = 0; m < DIMS; ++m)
        cout << prec[m]->moments().precision << " ";
    cout << endl;

    for (size_t m = 0; m < DIMS; ++m)
        cout << exp(dir->moments().logProb[m]) << " ";
    cout << endl;

    delete dir;
    deleteAll(mean);
    deleteAll(prec);
    deleteAll(data);
    deleteAll(discr);
}





