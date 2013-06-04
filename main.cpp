#include <iostream>
#include <memory>
using namespace std;


#include <variable.h>
#include <gaussian.h>
#include <gamma.h>
#include <network.h>



using namespace vmp;


void trainUnivariateGaussian()
{
    Gaussian *mu = new Gaussian(0, 1e-2);
    mu->updatePosterior();
    Gamma *gamma = new Gamma(1e-3, 1e-3);
    gamma->updatePosterior();

    vector<Gaussian*> x;
    // TODO: try for various parameters
    const double data[] = {10.748077,9.807581,10.888610,9.235151,8.597731,8.577624,10.488194,
                           9.822625,9.803947,11.419310,10.291584,10.197811,11.587699,9.195534,
                           10.696624,10.835088,9.756285,10.215670,8.834156,8.852047};
    vector<double> v(data, data + sizeof(data) / sizeof(double));
    for (int i = 0; i < v.size(); ++i)
    {
        x.push_back(new Gaussian(mu, gamma));
        x[i]->observe(data[i]);
    }

    // doing the updates
    size_t numIterations = 10;
    for (size_t i = 0; i < numIterations; ++i)
    {
        for (size_t t = 0; t < x.size(); ++t)
        {
            // send messages from gamma to all x
            x[t]->receiveFromParent(gamma->messageToChildren(), gamma);
            // send messages from all x to mu
            mu->receiveFromChild(x[t]->messageToParent(mu), x[t]);
        }
        mu->updatePosterior();

        for (size_t t = 0; t < x.size(); ++t)
        {
            // send messages from mu to all x
            x[t]->receiveFromParent(mu->messageToChildren(), mu);
            // send messages from x to all gamma
            gamma->receiveFromChild(x[t]->messageToParent(gamma), x[t]);
        }
        gamma->updatePosterior();
    }
    cout << "mean,prec:" << mu->moments().mean << " " << gamma->moments().precision << endl;
}





int main()
{
    trainUnivariateGaussian();
    return 0;
}

