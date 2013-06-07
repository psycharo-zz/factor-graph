#include <iostream>
#include <memory>
using namespace std;


#include <variable.h>
#include <gaussian.h>
#include <gamma.h>
#include <mixture.h>
#include <network.h>
#include <discrete.h>
#include <ostream>



using namespace vmp;

const double DATA[] = {10.748077,9.807581,10.888610,9.235151,8.597731,8.577624,10.488194,
                       9.822625,9.803947,11.419310,10.291584,10.197811,11.587699,9.195534,
                       10.696624,10.835088,9.756285,10.215670,8.834156,8.852047};
const size_t SIZE = sizeof(DATA) / sizeof(double);


void trainUnivariateGaussian()
{
    Gaussian mu(0, 1e-2);
    mu.updatePosterior();
    Gamma gamma(1e-3, 1e-3);
    gamma.updatePosterior();

    vector<Gaussian> x;
    // TODO: try for various parameters

    vector<double> v(DATA, DATA + SIZE);
    for (size_t i = 0; i < SIZE; ++i)
    {
        x.push_back(Gaussian(&mu, &gamma));
        x[i].observe(DATA[i]);
    }

    // doing the updates
    size_t numIterations = 3;
    for (size_t i = 0; i < numIterations; ++i)
    {
        for (size_t t = 0; t < x.size(); ++t)
        {
            // send messages from gamma to all x
            x[t].receiveFromParent(gamma.messageToChildren(), &gamma);
            // send messages from all x to mu
            mu.receiveFromChild(x[t].messageToParent(&mu), &x[t]);
        }
        mu.updatePosterior();

        for (size_t t = 0; t < x.size(); ++t)
        {
            // send messages from mu to all x
            x[t].receiveFromParent(mu.messageToChildren(), &mu);
            // send messages from x to all gamma
            gamma.receiveFromChild(x[t].messageToParent(&gamma), &x[t]);
        }
        gamma.updatePosterior();
    }
    cout << "mean,prec:" << mu.moments().mean << " " << gamma.moments().precision << endl;

}

template <typename T>
inline std::ostream& operator <<(std::ostream &os, const vector<T> &v)
{
    for (size_t i = 0; i < v.size(); i++)
        os << v[i] << "\t";
    return os;
}


void trainDirichlet()
{
    const size_t DIMS = 5;
    Dirichlet dir(vector<double>(DIMS, 20));
    vector<Discrete> lambda;
    for (size_t i = 0; i < 100; ++i)
    {
        lambda.push_back(Discrete(&dir));
        // initialising
        lambda[i].observe(i % 3);
    }

    // running the inference
    for (size_t t = 0; t < lambda.size(); ++t)
        dir.receiveFromChild(lambda[t].messageToParent(&dir), &lambda[t]);
    dir.updatePosterior();

    cout << expv(dir.moments().logProb) << endl;
    // sanity check
    cout << sumv(expv(dir.moments().logProb)) << endl;
}


void testGaussianPDF()
{
    Gaussian mu(10, 1);
    for (double x = 5; x < 16; ++x)
        cout << exp(mu.logProbabilityDensity(x)) << endl;
}

void trainMixtureOfUnivariateGaussians()
{
//    Mixture m;
}


int main()
{
    const size_t DIMS = 5;
    Dirichlet dir(vector<double>(DIMS, 20));

    vector<Gaussian*> means;
    vector<Gamma*> precs;

    for (size_t m = 0; m < DIMS; ++m)
    {
        means.push_back(new Gaussian(0, 1));
        precs.push_back(new Gamma(1e-3, 1e-3));
    }

    Discrete discr(&dir);
    discr.updatePosterior();


    MoG g(means, precs, &discr);

    g.receiveFromParent(discr.messageToChildren(), &discr);

    cout << g.componentWeight(1) << endl;


    return 0;
}

