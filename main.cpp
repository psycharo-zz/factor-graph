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
const size_t NUM_POINTS = sizeof(DATA) / sizeof(double);



const double MIX_DATA[] = {
// mixtures,
2.898981,6.346756,5.322507,5.437963,5.802548,6.052655,4.558769,5.964154,2.894862,4.679303,5.442567,6.392286,6.392888,7.791463,8.379164,
6.629269,4.831047,6.754143,7.771573,7.595588,6.144927,3.941465,4.177482,5.482249,6.736269,5.166452,3.781340,7.868162,5.359981,7.414742,
6.618159,4.154087,6.077984,6.952991,5.030229,5.018088,6.453009,4.532262,5.035943,7.874976,5.513752,6.642983,5.308715,3.816932,5.323380,
5.205870,6.284371,5.288584,7.428910,3.092650,6.550115,5.247324,7.364147,6.096512,8.307791,5.422271,5.550971,6.672110,7.489852,4.291722,
2.252269,5.730714,6.352011,8.154111,5.631319,6.129845,6.948193,5.125343,4.565992,5.674843,3.487446,6.714949,4.534375,3.800037,6.082789,
6.676557,5.668322,6.750809,7.066570,6.964305,5.705956,6.375258,5.218972,8.135029,4.981805,4.370035,5.114452,5.018166,4.191858,4.194128,
8.445652,6.094941,4.693597,7.509118,4.644552,8.169739,5.587574,4.912959,7.867187,5.211286,6.012649,7.028360,6.410661,5.096248,3.021252,
4.893937,6.965197,7.522283,6.347378,7.311136,6.456528,8.053796,8.120093,5.175068,6.028512,6.850333,7.814896,6.208811,5.271917,5.957709
};

//const size_t MIX_NUM_POINTS = sizeof(MIX_DATA) / sizeof(double);
const size_t MIX_NUM_POINTS = 120;





void trainUnivariateGaussian()
{
    Gaussian mu(0, 1e-2);
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


template <typename T>
void deleteAll(vector<T*> &vec)
{
    for (size_t i = 0; i < vec.size(); ++i)
        delete vec[i];
}






void trainMixtureOfUnivariateGaussians()
{
    const size_t DIMS = 10;
    Dirichlet dir(vector<double>(DIMS, 20));

    vector<Gaussian*> mean(DIMS, NULL);
    vector<Gamma*> prec(DIMS, NULL);

    for (size_t m = 0; m < DIMS; ++m)
    {
        // TODO: put the first updatePosterior() call into constructor?
        mean[m] = new Gaussian(0, 1);
        mean[m]->updatePosterior();

        prec[m] = new Gamma(1e-3, 1e-3);
        prec[m]->updatePosterior();
    }


    // TODO: this should be done inside of the mixture??

    vector<Discrete*> discr(MIX_NUM_POINTS, NULL);
    vector<MoG*> data(MIX_NUM_POINTS, NULL);
    for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
    {

        discr[p] = new Discrete(&dir);
        discr[p]->observe(discr[p]->sample());
        data[p] = new MoG(mean, prec, discr[p]);
        data[p]->observe(DATA[p]);
        data[p]->receiveFromParent(discr[p]->messageToChildren(), discr[p]);

        // needs a message from a parent TODO: arrays are indeed necessary
        for (size_t m = 0; m < DIMS; ++m)
            data[p]->receiveFromParent(m, prec[m]->messageToChildren(), prec[m]);
    }


    // doing the inference
    const size_t numIters = 10;
    for (size_t i = 0; i < numIters; i++)
    {
        // updating the mean components
        for (size_t m = 0; m < DIMS; ++m)
        {
            for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
                mean[m]->receiveFromChild(data[p]->messageToParent(m, mean[m]), data[p]);

            mean[m]->updatePosterior();
            cout << m << ":" << mean[m]->moments().mean << endl;
        }


        // sending messages from means to points
        for (size_t m = 0; m < DIMS; ++m)
        {
            Moments<Gaussian> msg = mean[m]->messageToChildren();
            for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
                data[p]->receiveFromParent(m, msg, mean[m]);
        }



        // updating the precsion components
        for (size_t m = 0; m < DIMS; ++m)
        {
            // receive all the messages from data
            for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
                prec[m]->receiveFromChild(data[p]->messageToParent(m, prec[m]), data[p]);
            prec[m]->updatePosterior();
        }

        // sending messages from precs to points
        for (size_t m = 0; m < DIMS; ++m)
        {
            Moments<Gamma> msg = prec[m]->messageToChildren();
            for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
                data[p]->receiveFromParent(m, msg, prec[m]);
        }

        // updating the discrete distributions
        for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
        {
            Parameters<Discrete> msg = data[p]->messageToParent(discr[p]);
            discr[p]->receiveFromChild(msg, data[p]);
            discr[p]->updatePosterior();
        }


        // updating the dirichlet
        for (size_t p = 0; p < MIX_NUM_POINTS; ++p)
            dir.receiveFromChild(discr[p]->messageToParent(&dir), discr[p]);
        dir.updatePosterior();

        cout << expv(dir.moments().logProb) << endl;



    }






    deleteAll(mean);
    deleteAll(prec);
    deleteAll(data);
    deleteAll(discr);
}



int main()
{

    trainMixtureOfUnivariateGaussians();


    return 0;
}

