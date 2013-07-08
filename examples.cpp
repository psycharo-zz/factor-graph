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


//#include <mlpack/methods/kmeans/kmeans.hpp>

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




double logNormWishart(const mat &W, double v)
{
    const size_t D = W.n_rows;
    return 0.5*v*logdet(W) - gammaln2(0.5*v,D) - 0.5*v*D*M_LN2;
}


double logNormDirichlet(const vec &U)
{
     return gammaln(sum(U)) - sum(gammalnv(U));
}


void vmp::trainMVMixtureVB(const mat &POINTS, size_t numMixtures, size_t maxNumIters,
                           const vec &initAssignments, const mat &initMeans,
                           mat &_means, cube &_sigmas, vec &_weights,
                           double &lowerBound, size_t &numIters)
{
    const size_t numPoints = POINTS.n_rows;

    // the number of dimensions
    const size_t D = POINTS.n_cols;

    // u0
    const double DIRICHLET_PRIOR = 1;
    // a0
    const double W_DEGREES_PRIOR = D;
    // B0
    const mat W_SCALE_PRIOR = 1e-2 * eye(D, D); // TODO: it says to use covariance = 0.01 I, that's weird
    // m0
    vec M_MEAN_PRIOR = mean(POINTS).t();
    // b0
    double M_BETA_PRIOR = 1;

    // hyperparameters
    // u
    vec dirU = DIRICHLET_PRIOR * ones(numMixtures,1);

    // # of degrees of freedom (a_s | v_i)
    vec Wdegrees = W_DEGREES_PRIOR * ones(numMixtures,1);
    // scale matrix (B_s | W_i | V_i)
    cube Wscale(D, D, numMixtures);
    for (size_t i = 0; i < numMixtures; ++i)
        Wscale.slice(i) = W_SCALE_PRIOR;

    // m_s
    mat Mmean(D, numMixtures);
    Mmean = initMeans;
//    for (size_t i = 0; i < numMixtures; ++i)
//        Mmean.col(i) = M_MEAN_PRIOR;

    // b_s
    vec Mbeta(numMixtures);
    Mbeta.fill(M_BETA_PRIOR);

    // indicators gamma
    mat resp(numMixtures, numPoints);
    resp.fill(0);

    for (size_t i = 0; i < numPoints; ++i)
        resp(initAssignments(i), i) = 1.0;

    mat logResp(numMixtures, numPoints);

    // weights pi
    vec weights(numMixtures);
    weights.fill(1.0 / numMixtures);
    // counts Ns
    vec counts(numMixtures);
    // means mu_s
    mat means(D, numMixtures);
    mat means2(D, numMixtures);
    means.zeros();
    means2.zeros();

    // sigmas S_s
    cube sigmas(D, D, numMixtures);
    for (size_t i = 0; i < numMixtures; ++i)
        sigmas.slice(i).eye();

    // precisions
    cube precs = zeros(D, D, numMixtures);


    double lbPrev = -DBL_MAX;

    for (size_t iter = 0; iter < maxNumIters; ++iter)
    {
        // M step
        for (size_t m = 0; m < numMixtures; ++m)
        {
            // Ns = N * pi_s
            counts(m) = sum(resp.row(m));
            // pi_s = 1/N * \sum prob_s,n
            weights(m) = counts(m) / numPoints;

            // mu_s = 1/Ns * \sum prob_s,n * y_p
            means.col(m).zeros();
            for (size_t i = 0; i < numPoints; ++i)
            {
                vec y = POINTS.row(i).t();
                means.col(m) += resp(m, i) * y;
            }
            means.col(m) /= counts(m);

            // S_s = 1/Ns \sum \prob (y_p - mu_s)(y_p - mu_s)^T
            sigmas.slice(m).zeros();
            for (size_t i = 0; i < numPoints; ++i)
            {
                vec y = POINTS.row(i).t();
                sigmas.slice(m) += resp(m, i) * (y - means.col(m)) * (y - means.col(m)).t();
            }
            sigmas.slice(m) /= counts(m);

            // updating hyperparameters
            // dirichlet
            dirU[m] = DIRICHLET_PRIOR + counts(m);
            // beta
            Mbeta[m] = M_BETA_PRIOR + counts(m);
            // mean
            Mmean.col(m) = (M_BETA_PRIOR * M_MEAN_PRIOR + counts(m) * means.col(m)) / Mbeta[m];

            // wishart
            vec deltaM = (means.col(m) - M_MEAN_PRIOR);

            Wscale.slice(m) = W_SCALE_PRIOR +
                      counts(m) * sigmas.slice(m) +
                      counts(m) * M_BETA_PRIOR * deltaM * deltaM.t() / (counts(m) + M_BETA_PRIOR);
            Wdegrees[m] = W_DEGREES_PRIOR + counts(m);
        }


        // E step - updating indicators
        vec logPi = digammav(dirU) - digamma(sum(dirU));


        precs.zeros();
        vec logDetPrec = zeros(numMixtures);
        for (size_t m = 0; m < numMixtures; ++m)
        {
            // G
            precs.slice(m) = Wdegrees(m) * inv(Wscale.slice(m));
            // logG~
            logDetPrec(m) = digamma_d(0.5 * Wdegrees[m], D) - logdet(Wscale.slice(m)) + D * log(2);

            const vec &ms = Mmean.col(m);
            for (size_t i = 0; i < numPoints; ++i)
            {
                vec y = POINTS.row(i).t();
                // logPi_s + 0.5 * logG~ - 0.5(y -ms)T * G * (y - ms) - 0.5 * dims / b_s
                logResp(m, i) = as_scalar(-0.5 * (y - ms).t() * precs.slice(m) * (y - ms))
                                    -0.5 * D / Mbeta[m]
                                    + logPi(m) + 0.5 * logDetPrec(m);
            }
        }

        // normalizing
        for (size_t i = 0; i < numPoints; ++i)
        {
            double _max = max(logResp.col(i));
            vec _p = exp(logResp.col(i) - _max);
            resp.col(i) = _p / sum(_p);
        }

        // computing the lower bound
        double lbCurr = 0;

        // responsibilities
        double lbRes = 0;
        for (size_t m = 0; m < numMixtures; ++m)
            for (size_t p = 0; p < numPoints; ++p)
                lbRes += resp(m, p) * log(resp(m, p));

        // dirichlet
        double lbDir = 0;
        lbDir += dot((dirU - DIRICHLET_PRIOR), logPi);
        lbDir += logNormDirichlet(DIRICHLET_PRIOR * ones(numMixtures,1)) - logNormDirichlet(dirU);

        /// prior precs (wishart)
        double lbWis = 0;
        for (size_t m = 0; m < numMixtures; ++m)
        {
            double v = Wdegrees(m);
            double logDet = logDetPrec(m);
            const mat &W = Wscale.slice(m);
            const mat &prec = precs.slice(m);

            lbWis -= (0.5 * (v - D - 1) * logDet - 0.5 * trace(W * prec)); // Q(prec|v,W)

            double v0 = W_DEGREES_PRIOR;
            const mat &W0 = W_SCALE_PRIOR;
            lbWis += (0.5 * (v0 - D - 1) * logDet - 0.5 * trace(W0 * prec)); // P(prec)
        }
        // normalization
        for (size_t m = 0; m < numMixtures; ++m)
            lbWis -= logNormWishart(Wscale.slice(m), Wdegrees(m));
        lbWis += D * logNormWishart(W_SCALE_PRIOR, W_DEGREES_PRIOR);

        // prior means (gaussians)

        double lbMeans = 0;
        for (size_t m = 0; m < numMixtures; ++m)
        {
            lbMeans += 0;

            lbMeans -= 0.5 * (logDetPrec(m) + D * (1 + LN_2PI));
        }


/*
for k=1:K
    temp = temp + D*(beta_0/beta(k)-log(beta_0/beta(k))-v(k)-1) + ...
                  beta_0*v(k)*(mean(:,k)-mean_0)'*W(:,:,k)*(mean(:,k)-mean_0) + ...
                  v(k)*trace(invW_0*W(:,:,k)) + ...
                  (v(k)-v_0)*log(lambda_eff(k));
end
+ 1/2 * temp;

temp = 0;
for k=1:K
    temp = temp + N_eff(k)*(log(lambda_eff(k)) + 2*log(pi_eff(k)) - D/beta(k) - D*log(2*pi) - ...
                            v(k)*trace(S(:,:,k)*W(:,:,k)) - ...
                            v(k)*(x_avg(:,k)-mean(:,k))'*W(:,:,k)*(x_avg(:,k)-mean(:,k)) )
- 1/2 * temp
 */

        // observations
        double lbData = 0;
        for (size_t m = 0; m < numMixtures; ++m)
        {
            double logDet = logDetPrec(m);
            const mat &prec = precs.slice(m);

            for (size_t p = 0; p < numPoints; ++p)
            {
                vec y = POINTS.row(p).t();
                const vec &mu = means.col(m);
                const mat &mu2 = mu * mu.t();
                double norm = logDetPrec(m) - D * LN_2PI;
                double pdf =  -trace(prec * (y * y.t() - y * mu.t() - mu * y.t() + mu2));
                lbData += resp(m, p) * 0.5 * (norm + pdf);
            }
        }

        cout << lbRes + lbDir + lbWis + lbData << endl;





    }

    _weights = weights;
    _means = means;
    _sigmas = sigmas;
}



void vmp::trainMVMixture(const mat &POINTS, size_t numMixtures, size_t maxNumIters,
                         mat &_means, mat &_sigmas, vec &_weights)
{
    const size_t numPoints = POINTS.n_rows;
    const size_t dims = POINTS.n_cols;

    auto meanPrecPrior = ConstWishart(diagmat(1e-2 * eye(dims,dims)));
    auto meanMeanPrior = ConstMVGaussian(zeros(dims,1));
    auto mean = MVGaussianArray<MVGaussian, Wishart>(numMixtures, &meanMeanPrior, &meanPrecPrior);
    auto prec = WishartArray(numMixtures, Wishart::TParameters(dims+1, 10 * (dims+1) * eye(dims,dims)));

    auto dirichlet = Dirichlet(numMixtures, 1.0);
    auto selector = DiscreteArray(numPoints, &dirichlet);
    selector.initialize(vmp::randomv(numPoints, numMixtures));

    auto data = MoGArray<MVGaussian, MVGaussian, Wishart>(numPoints, &mean, &prec, &selector);
    data.observe(POINTS);
    auto sequence = make_tuple(make_pair(&data, &mean),
                               make_pair(&data, &prec),
                               make_pair(&data, &selector),
                               make_pair(&selector, &dirichlet));

    auto msgMean = mean.addChild(&data);
    auto msgPrec = prec.addChild(&data);
    auto msgDiscr = selector.addChild(&data);
    auto msgDir = dirichlet.addChild(&selector);

    for (size_t iter = 0; iter < maxNumIters; ++iter)
    {
        data.messageToParent(msgMean);
        mean.updatePosterior();

        data.messageToParent(msgPrec);
        prec.updatePosterior();

        data.messageToParent(msgDiscr);
        selector.updatePosterior();

        selector.messageToParent(msgDir);
        dirichlet.updatePosterior();
    }

    _weights = exp(dirichlet.moments().logProb);
    _means.resize(dims, numMixtures);
    _sigmas.resize(dims, numMixtures);
    for (size_t m = 0; m < numMixtures; ++m)
    {
        _means.col(m) = mean.moments(m).mean;
        _sigmas.col(m) = diagvec(inv(prec.moments(m).prec));
    }
}



MVMixtureNetwork *vmp::trainMVMixture(const mat &POINTS, size_t numMixtures, size_t maxNumIters)
{
    MVMixtureNetwork *nwk = new MVMixtureNetwork;

    const size_t numPoints = POINTS.n_rows;
    const size_t dims = POINTS.n_cols;

    auto meanPrecPrior = ConstWishart(diagmat(1e-2 * eye(dims,dims)));
    auto meanMeanPrior = ConstMVGaussian(zeros(dims,1));
    auto mean = MVGaussianArray<MVGaussian, Wishart>(numMixtures, &meanMeanPrior, &meanPrecPrior);
    auto prec = WishartArray(numMixtures, Wishart::TParameters(dims+1, 10 * (dims + 1) * eye(dims,dims)));

    auto dirichlet = Dirichlet(numMixtures, 1.0);
    auto selector = DiscreteArray(numPoints, &dirichlet);
    selector.initialize(vmp::randomv(numPoints, numMixtures));

    auto data = MoGArray<MVGaussian, MVGaussian, Wishart>(numPoints, &mean, &prec, &selector);
    data.observe(POINTS);
    auto sequence = make_tuple(make_pair(&data, &mean),
                               make_pair(&data, &prec),
                               make_pair(&data, &selector),
                               make_pair(&selector, &dirichlet));
    // inference
    for (size_t iter = 0; iter < maxNumIters; ++iter)
        for_each(sequence, SendToParent());

    nwk->precPrior = new ConstWishart(diagmat(1e-2 * eye(dims,dims)));
    nwk->meanPrior = new ConstMVGaussian(zeros(dims,1));

    // TODO: initialise from array
    nwk->means = new MVGaussianArray<MVGaussian, Wishart>(mean.parameters(), &meanMeanPrior, &meanPrecPrior);
    nwk->precs = new WishartArray(prec.parameters());

    nwk->weightsPrior = new Dirichlet(dirichlet.parameters());
    nwk->weights = new Discrete(&dirichlet);

    return nwk;
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

    vector<vec> MU = { {0, -2}, {0, 0}, {0,2} };
    vector<vec> SIGMA = { {2, 0.2}, {2, 0.2}, {2, 0.2} };
    vec WEIGHTS = ones(3,1) / 3;

    size_t dims = MU[0].n_rows;

    size_t numPoints = 400;
    size_t numMixtures = 3;
    size_t maxIters = 500;
    auto POINTS = gmmrand(numPoints, MU, SIGMA, WEIGHTS);

    mat means;
    cube sigmas;
    vec weights;

    size_t iter;
    double lb;

    vec assignments(numPoints, 1);
    for (size_t i = 0; i < numPoints; ++i)
        assignments[i] = i % numMixtures;

    mat initMeans(dims, numMixtures);

    trainMVMixtureVB(POINTS, numMixtures, maxIters, assignments, initMeans, means, sigmas, weights, lb, iter);

    cout << means << endl;
    cout << sigmas << endl;
    cout << weights.t() << endl;
}


void vmp::testLogSum()
{
    vec a = {-0.5911,1.9281,-1.5054,2.9966,-2.7622,-0.2766,-0.3826, 0.2686, 0.4302,-0.8913};
    vec b = {0.4159, -1.7199, -0.8560, 1.5157, 0.9344, -1.0388, 1.5259, -0.1821, 0.0425, 0.4759};

    vec EXP_LOGSUM = {0.7273,1.9539,-0.4357,3.2015,0.9589,0.1064,1.6642,0.7616,0.9482,0.7029};
    assert(fabs(sum(EXP_LOGSUM - vmp::logSumV(a, b))) < 1e-3);


    pair<vec,vec> EXP_JACOB = {{0.2676,0.9746,0.3431,0.8147,0.0242,0.6818,0.1291,0.6108,0.5957,0.2031},
                               {0.7324,0.0254,0.6569,0.1853,0.9758,0.3182,0.8709,0.3892,0.4043,0.7969}};
    auto result = vmp::logSumVJacobian(a, b);

    assert(fabs(sum(EXP_JACOB.first - result.first)) < 1e-3);
    assert(fabs(sum(EXP_JACOB.second - result.second)) < 1e-3);
}












