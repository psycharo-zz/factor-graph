#include <examples.h>


#include <variable.h>
#include <gaussian.h>
#include <gaussianarray.h>
#include <gamma.h>
#include <gammaarray.h>
#include <discrete.h>
#include <mixture.h>
#include <network.h>
#include <mvgaussian.h>

#include <ostream>
#include <ctime>
#include <limits>

using namespace vmp;
#include <cfloat>



vmp::MixtureNetwork *vmp::trainMixture(const double *points, size_t numPoints, size_t numMixtures, size_t maxNumIters)
{
    /*
    points: 200
    iters:  466
    mixtures:4
    evidence:-552.123
    means = [4.22319 -3.46179 -6.42874 0.00133329 ]
    precs = [0.520569 0.909226 0.0220358 1.01377 ]
    weights = [0.509738	0.278057	0.010094	0.202111	]
    1.01 seconds.


    points: 999
    iters:  611
    mixtures:4
    evidence:-2546.75
    means = [-3.98751 0.638693 3.99423 0 ]
    precs = [0.265769 0.197848 1.0533 1 ]
    weights = [0.255284	0.319829	0.424326	0.000560864	]
    7.06 seconds.


    points: 999
    iters:  455
    mixtures:4
    evidence:-2557.95
    means = [0.442826 -3.69693 3.92595 0 ]
    precs = [0.421325 0.246173 0.92228 1 ]
    weights = [0.209986	0.303197	0.486256	0.000560864	]
    0.3 seconds.
     *
     */


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
    auto data = MoGArray<>(numPoints, &mean, &prec, &selector);

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
        nwk->iters = i;
    }

    nwk->meanPrior = new ConstGaussian(GAUSS_PRIOR_MEAN);
    nwk->precPrior = new ConstGamma(GAUSS_PRIOR_PREC);
    nwk->means = new GaussianArray<Gaussian, Gamma>(mean.parameters(), nwk->meanPrior, nwk->precPrior);
    nwk->precs = new GammaArray(prec.parameters());
    nwk->weightsPrior = new Dirichlet(dirichlet.parameters());
    nwk->weights = new Discrete(nwk->weightsPrior);

    return nwk;
}



void vmp::trainMVMixture(const mat &POINTS, size_t numMixtures, size_t maxNumIters,
                          vector<vec> &means, vector<mat> &sigmas, vector<double> &weights)
{
    size_t numPoints = POINTS.n_rows;

    auto meanPrecPrior = ConstWishart(1e-2 * eye(2,2));
    auto meanMeanPrior = ConstMVGaussian({1, 1});
    auto mean = MVGaussianArray<MVGaussian, Wishart>(numMixtures, &meanMeanPrior, &meanPrecPrior);
    auto prec = WishartArray(numMixtures, Wishart::TParameters(3.0, 10 * eye(2,2)));

    auto dirichlet = Dirichlet(numMixtures, 1.0);
    auto selector = DiscreteArray(numPoints, &dirichlet);
    selector.initialize(vmp::randomv(numPoints, numMixtures));

    auto data = MoGArray<MVGaussian, MVGaussian, Wishart>(numPoints, &mean, &prec, &selector);
    data.observe(POINTS);
    auto sequence = make_tuple(make_pair(&data, &mean),
                               make_pair(&data, &prec),
                               make_pair(&data, &selector),
                               make_pair(&selector, &dirichlet));


    for (size_t iter = 0; iter < maxNumIters; ++iter)
        for_each(sequence, SendToParent());

    weights = expv(dirichlet.moments().logProb);
    for (size_t m = 0; m < numMixtures; ++m)
    {
        means.push_back(mean.moments(m).mean);
        sigmas.push_back(inv(prec.moments(m).prec));
    }
}







void vmp::testLogPDF()
{
    mat POINTS =
       {1.5101,   -1.2452,
        2.7398,    0.8343,
        -1.1429,   -1.6988,
        1.8179,   -0.6585,
        1.3024,   -0.6157,
       -0.2406,   -1.6230,
        0.5887,   -1.2262,
        1.3250,   -0.3347,
        4.3948,    1.0014,
        3.6273,    0.6632};

    vec RESULT = { 0.1670,0.0009,0.0288,0.3303, 0.3533,0.1998, 0.4335, 0.1492, 0.0003, 0.0038};

    POINTS.reshape(2,10);
    POINTS = POINTS.t();

    vec MU = {1, -1};
    auto meanMsg = Moments<MVGaussian>::fromValue(MU);
    mat SIGMA = {0.9, 0.4, 0.4, 0.3};
    SIGMA.reshape(2,2);
    auto precMsg = Moments<Wishart>::fromValue(inv(SIGMA));
    for (size_t i = 0; i < POINTS.n_rows; ++i)
    {
        auto moment = MVGaussian::TMoments::fromValue(POINTS.row(i).t());
        double value = exp(MVGaussian::logPDF(moment, meanMsg, precMsg));
        assert(abs(value - RESULT[i]) < 1e-3);
        cout << value << " " << RESULT[i] << endl;
    }
}


void vmp::testMVMoG()
{
    clock_t startTime = clock();

    auto MU1 = {1.0, 2.0};
    auto MU2 = {-3.0, -5.0};
    auto MU3 = {-11.0, -2.0};
    auto SIGMA1 = {2.0, 0.5};
    auto SIGMA2 = {1.0, 1.0};
    auto SIGMA3 = {1.0, 1.0};

    vector<vec> MU = { MU1, MU2, MU3 };
    vector<vec> SIGMA = {SIGMA1, SIGMA2, SIGMA3};

    vec WEIGHTS = {0.25, 0.5, 0.25};
    size_t numPoints = 1001;
    size_t numMixtures = 32;
    size_t maxIters = 30;

    auto POINTS = gmmrand(numPoints, MU, SIGMA, WEIGHTS);

    vector<vec> means;
    vector<mat> sigmas;
    vector<double> weights;
    trainMVMixture(POINTS, numMixtures, maxIters, means, sigmas, weights);

    cout << weights << endl;
    for (size_t m = 0; m < numMixtures; ++m)
    {
        cout << means[m].t() << " "
             << sigmas[m] << endl;
    }

    cout << double(clock() - startTime) / (double) CLOCKS_PER_SEC << " seconds." << endl;
}

void vmp::testSpeechGMM(const vector<double> &bin)
{
    const size_t numPoints = 999;
    const size_t maxNumIters = 456;
    const size_t numMixtures = 4;

    clock_t startTime;
    double runTime;

    startTime = clock();

    auto mixture = vmp::trainMixture(bin.data(), numPoints, numMixtures, maxNumIters);

    cout << "points: " << numPoints << endl
         << "iters:  " << mixture->iters << endl
         << "mixtures:" << numMixtures << endl
         << "evidence:" << mixture->evidence << endl
         << mixture->parameters() << endl;

    cout << double(clock() - startTime) / (double) CLOCKS_PER_SEC << " seconds." << endl;
}


