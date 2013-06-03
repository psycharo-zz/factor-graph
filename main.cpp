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
    Network nwk;

    Gaussian *mu = nwk.createVariable<Gaussian>(0, 1e-2);
    mu->updatePosterior();
    Gamma *gamma = nwk.createVariable<Gamma>(1e-3, 1e-3);
    gamma->updatePosterior();

    vector<Gaussian*> x;
    // TODO: try for various parameters
    const double data[] = {10.748077,9.807581,10.888610,9.235151,8.597731,8.577624,10.488194,
                           9.822625,9.803947,11.419310,10.291584,10.197811,11.587699,9.195534,
                           10.696624,10.835088,9.756285,10.215670,8.834156,8.852047};
    vector<double> v(data, data + sizeof(data) / sizeof(double));
    for (int i = 0; i < v.size(); ++i)
    {
        x.push_back(nwk.createVariable<Gaussian>(mu, gamma));
        x[i]->observe(data[i]);
    }

    // doing the updates
    size_t numIterations = 10;
    for (size_t i = 0; i < numIterations; ++i)
    {
        auto_ptr<Moments> msg_gamma_x(gamma->messageToChildren());
        for (size_t t = 0; t < x.size(); ++t)
        {
            // send messages from gamma to all x
            x[t]->receiveFromParent(msg_gamma_x.get(), gamma);
            auto_ptr<Parameters> msg_x_mu(x[t]->messageToParent(mu));
            // send messages from all x to mu
            mu->receiveFromChild(msg_x_mu.get(), x[t]);
        }

        mu->updatePosterior();

        auto_ptr<Moments> msg_mu_x(mu->messageToChildren());
        for (size_t t = 0; t < x.size(); ++t)
        {
            // send messages from mu to all x
            x[t]->receiveFromParent(msg_mu_x.get(), mu);
            auto_ptr<Parameters> msg_x_gamma(x[t]->messageToParent(gamma));
            // send messages from x to all gamma
            gamma->receiveFromChild(msg_x_gamma.get(), x[t]);
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

