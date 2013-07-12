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


Parameters<Gaussian> vmp::trainUnivariateGaussian(const vec &points, size_t maxNumIters, size_t &iters, vec &lbEvidence)
{
    auto PRIOR_MEAN = ConstGaussian(0.0);
    auto PRIOR_PREC = ConstGamma(1e-3);

    auto mean = Gaussian(&PRIOR_MEAN, &PRIOR_PREC);
    auto prec = Gamma(1e-3, 1e-3);
    auto x = GaussianArray<Gaussian, Gamma>(points.size(), &mean, &prec);
    x.observe(points);

    auto sequence = make_tuple(make_pair(&x, &mean), make_pair(&x, &prec));

    double lbPrev = LB_INIT;
    iters = maxNumIters;
    vector<double> lbs;
    for (size_t i = 0; i < maxNumIters; ++i)
    {
        for_each(sequence, SendToParent());

        double lbCurr = mean.logEvidence() + prec.logEvidence() + x.logEvidence();

        if (abs(lbCurr - lbPrev) < EPSILON)
        {
            iters = i;
            lbPrev = lbCurr;
            break;
        }
        lbPrev = lbCurr;
        lbs.push_back(lbCurr);
    }

    lbEvidence = lbs;

    return Parameters<Gaussian>(mean.moments().mean * prec.moments().precision,
                                prec.moments().precision);
}



vmp::MixtureNetwork *vmp::trainUnivariateMixture(const double *points, size_t numPoints, size_t numMixtures, size_t maxNumIters)
{
    MixtureNetwork *nwk = new MixtureNetwork;

    auto dirichlet = Dirichlet(numMixtures, DIRICHLET_PRIOR);
    auto selector = DiscreteArray(numPoints, &dirichlet);
    selector.initialize(randomv(numPoints, numMixtures));

    auto meanPrior = ConstGaussian(GAUSS_PRIOR_MEAN);
    auto precPrior = ConstGamma(GAUSS_PRIOR_PREC);
    // distributions over parameters
    auto mean = GaussianArray<Gaussian, Gamma>(numMixtures, &meanPrior, &precPrior);
    auto prec = GammaArray(numMixtures, Parameters<Gamma>(GAMMA_PRIOR_SHAPE, GAMMA_PRIOR_RATE));
    auto data = MoGArray<Gaussian, Gaussian, Gamma>(numPoints, &mean, &prec, &selector);

    // messages
    data.observe(points);

    auto sequence = make_tuple(make_pair(&data, &mean),
                               make_pair(&data, &prec),
                               make_pair(&data, &selector),
                               make_pair(&selector, &dirichlet));
    auto dirMsg = dirichlet.addChild(&selector);

    selector.messageToParent(dirMsg);
    dirichlet.updatePosterior();

    auto network = make_tuple(&data, &mean, &prec, &selector, &dirichlet);

    // running inference
    vector<double> lbs;
    lbs.push_back(LB_INIT);
    for (size_t i = 0; i < maxNumIters; i++)
    {
        for_each(sequence, SendToParent());
        AddEvidence lbCurr;
        for_each(network, lbCurr);

        if (lbCurr.value - lbs[i] <= EPSILON)
        {
            nwk->iters = i;
            lbs.push_back(lbCurr.value);
            break;
        }
        nwk->iters = i;
        lbs.push_back(lbCurr.value);
    }
    nwk->evidence = vec(lbs.data()+1, lbs.size()-1);

    nwk->meanPrior = new ConstGaussian(GAUSS_PRIOR_MEAN);
    nwk->precPrior = new ConstGamma(GAUSS_PRIOR_PREC);
    nwk->means = new GaussianArray<Gaussian, Gamma>(mean.parameters(), nwk->meanPrior, nwk->precPrior);
    nwk->precs = new GammaArray(prec.parameters());
    nwk->weightsPrior = new Dirichlet(dirichlet.parameters());
    nwk->weights = new Discrete(nwk->weightsPrior);

    return nwk;
}



double computeLB(const double v0, const mat &W0, const vec &m0, const double beta0, const double u0,
                 const vec &Du, const cube &Wscale, const vec &Wdegrees, const mat &Mmean, const vec &Mbeta,
                 const mat &Emean, const cube &Esigma,
                 const vec &ElogPrecDet, const cube &Eprec,
                 const mat &resp, const mat &logResp,
                 const vec &logPi, const vec &counts)
{

    // number of mixtures
    size_t M = Du.size();
    size_t D = W0.n_rows;

    // data
    double lbData = 0.0;

    // EpX = 0.5*dot(nk,ElogLambda-d./kappa-v.*trSW-v.*xbarmWxbarm-d*log(2*pi));
    for (size_t m = 0; m < M; ++m)
    {
        const vec &dMean = Emean.col(m) - Mmean.col(m);
        lbData += 0.5 * counts(m) * (ElogPrecDet(m)
                                     - D / Mbeta(m) - D * LN_2PI
                                     - trace(Esigma.slice(m) * Eprec.slice(m))
                                     - as_scalar(dMean.t() * Eprec.slice(m) * dMean));
    }


    // hidden variables
    double lp = 0;
    double lq = 0;

    // Dirichlet: responsibilities prior
    double lbDirichlet = (sum(u0 * logPi) + gammaln(M * u0) - M * (gammaln(u0)))
                        -(dot(Du, logPi) + gammaln(sum(Du)) - sum(gammalnv(Du)));
    // Discrete: responsibilities
    double lbResp = dot(counts, logPi) - dot(resp, logResp);


    //    temp = temp + D*(beta_0/beta(k)-log(beta_0/beta(k))-v(k)-1) + ...
    //                  beta_0*v(k)*(mean(:,k)-mean_0)'*W(:,:,k)*(mean(:,k)-mean_0) + ...
    //                  v(k)*trace(invW_0*W(:,:,k)) + ...
    //                  (v(k)-v_0)*log(lambda_eff(k));


    // Gaussian: mean prior
    lp = 0;
    lq = 0;
    for (size_t m = 0; m < M; ++m)
    {
        const vec &dMean = Mmean.col(m) - m0;
        const mat &prec = Eprec.slice(m);
        const double beta = Mbeta(m);
        // Epmu = 0.5 * sum(D*log(beta0/(2*pi))- D * beta0 ./ beta + ElogLambda - beta0 * (v.*mm0Wmm0));
        lp += D * (log(beta0) - beta0 / Mbeta(m) - LN_2PI) + ElogPrecDet(m) - beta0 * as_scalar(dMean.t() * prec * dMean);
        // Eqmu = 0.5 * sum(ElogLambda+D*log(beta/(2*pi)))-0.5*D*M;
        lq += D * (log(beta) - LN_2PI - 1) + ElogPrecDet(m);
    }
    double lbMean = 0.5 * lp - 0.5 * lq;

    // Wishart: prec prior
    lp = 0;
    lq = 0;
    for (size_t m = 0; m < M; ++m)
    {
        const double v = Wdegrees(m);
        // EpLambda = k*logB0+0.5*(v0-d-1)*sum(ElogLambda)-0.5*dot(v,trM0W);
        lp += 0.5 * (v0 - D - 1) * ElogPrecDet(m) - 0.5 * trace(W0 * Eprec.slice(m));
//        EqLambda = 0.5 * sum( (v-D-1) .* ElogLambda - v*D)+sum(logB);
        lq += 0.5 * (v - D - 1) * ElogPrecDet(m) - 0.5 * v * D;
    }
    // logB0 = v0*sum(log(diag(U0)))-0.5*v0*d*log(2)-logmvgamma(0.5*v0,d);
    // return 0.5*v0*logdet(W) - 0.5*v*D*M_LN2 - gammaln2(0.5*v,D);
    lp += M * (v0 * logdet(W0) - 0.5 * v0 * D * M_LN2 - gammaln2(0.5 * v0, D));
    for (size_t m = 0; m < M; ++m)
        lq += (0.5 * Wdegrees(m) * logdet(Wscale.slice(m)) - gammaln2(0.5 * Wdegrees(m), D));
    double lbPrec = lp - lq;

    return lbDirichlet + lbResp + lbMean + lbPrec + lbData;

}




void vmp::trainMVMixtureVB(const mat &POINTS, size_t numMixtures, size_t maxNumIters,
                           const vec &initAssignments, const mat &initMeans,
                           mat &_means, cube &_sigmas, vec &_weights,
                           double &lbEvidence, size_t &numIters)
{
    const size_t numPoints = POINTS.n_rows;

    // the number of dimensions
    const size_t D = POINTS.n_cols;

    // u0
    const double DIRICHLET_PRIOR = 1;
    const vec u0 = DIRICHLET_PRIOR * ones(numMixtures,1);
    // a0
    const double W_DEGREES_PRIOR = D;
    const double v0 = W_DEGREES_PRIOR;
    // B0
    const mat W_SCALE_PRIOR = 1e-2 * eye(D, D); // TODO: it says to use covariance = 0.01 I, that's weird
    const mat &W0 = W_SCALE_PRIOR;
    // m0
    const vec M_MEAN_PRIOR = mean(POINTS).t();
    const vec &m0 = M_MEAN_PRIOR;
    // b0
    double M_BETA_PRIOR = 1;
    const double beta0 = M_BETA_PRIOR;

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
    vec Eweights(numMixtures);
    Eweights.fill(1.0 / numMixtures);
    // counts Ns
    vec counts(numMixtures);
    // means mu_s
    mat Emeans(D, numMixtures);
    mat Emeans2(D, numMixtures);
    Emeans.zeros();
    Emeans2.zeros();

    // sigmas S_s
    cube Esigmas(D, D, numMixtures);
    for (size_t i = 0; i < numMixtures; ++i)
        Esigmas.slice(i).eye();

    // precisions
    cube Eprecs = zeros(D, D, numMixtures);

    double lbPrev = -DBL_MAX;

    for (size_t iter = 0; iter < maxNumIters; ++iter)
    {
        // M step
        for (size_t m = 0; m < numMixtures; ++m)
        {
            // Ns = N * pi_s
            counts(m) = sum(resp.row(m));
            // pi_s = 1/N * \sum prob_s,n
            Eweights(m) = counts(m) / numPoints;

            // mu_s = 1/Ns * \sum prob_s,n * y_p
            Emeans.col(m).zeros();
            for (size_t i = 0; i < numPoints; ++i)
            {
                const vec &y = POINTS.row(i).t();
                Emeans.col(m) += resp(m, i) * y;
            }
            Emeans.col(m) /= counts(m);

            // S_s = 1/Ns \sum \prob (y_p - mu_s)(y_p - mu_s)^T
            Esigmas.slice(m).zeros();
            for (size_t i = 0; i < numPoints; ++i)
            {
                const vec &dMean = (POINTS.row(i).t() - Emeans.col(m));
                Esigmas.slice(m) += resp(m, i) * dMean * dMean.t();
            }
            Esigmas.slice(m) /= counts(m);
        }

        // updating hyperparameters
        for (size_t m = 0; m < numMixtures; ++m)
        {
            // dirichlet
            dirU(m) = DIRICHLET_PRIOR + counts(m);
            // beta
            Mbeta(m) = M_BETA_PRIOR + counts(m);
            // mean
            Mmean.col(m) = (M_BETA_PRIOR * M_MEAN_PRIOR + counts(m) * Emeans.col(m)) / Mbeta(m);

            // wishart
            vec deltaM = (Emeans.col(m) - M_MEAN_PRIOR);

            Wscale.slice(m) = sympd(W_SCALE_PRIOR +
                      counts(m) * Esigmas.slice(m) +
                      counts(m) * M_BETA_PRIOR * deltaM * deltaM.t() / (counts(m) + M_BETA_PRIOR));
            Wdegrees(m) = W_DEGREES_PRIOR + counts(m);
        }


        // E step - updating indicators
        vec logPi = digammav(dirU) - digamma(sum(dirU));
        Eprecs.zeros();
        vec logDetPrec = zeros(numMixtures);
        vec logDetScale = zeros(numMixtures);
        for (size_t m = 0; m < numMixtures; ++m)
        {
            // G
            Eprecs.slice(m) = Wdegrees(m) * inv(Wscale.slice(m));
            // log|W|
            logDetScale(m) = logdet(Wscale.slice(m));

            // logG~
            logDetPrec(m) = digamma_d(0.5 * Wdegrees(m), D) - logDetScale(m) + D * M_LN2;

            const vec &ms = Mmean.col(m);
            for (size_t i = 0; i < numPoints; ++i)
            {
                const vec &dMean = POINTS.row(i).t() - ms;
                // logPi_s + 0.5 * logG~ - 0.5(y -ms)T * G * (y - ms) - 0.5 * dims / b_s
                logResp(m, i) = -0.5 * as_scalar(dMean.t() * Eprecs.slice(m) * dMean)
                                -0.5 * D / Mbeta(m)
                                +logPi(m) + 0.5 * logDetPrec(m);
            }
        }

        // normalizing
        for (size_t i = 0; i < numPoints; ++i)
        {
            double _max = max(logResp.col(i));
            vec _p = exp(logResp.col(i) - _max);
            resp.col(i) = _p / sum(_p);
            logResp.col(i) -= _max + log(sum(_p));
        }

        double lbCurr = computeLB(W_DEGREES_PRIOR, W_SCALE_PRIOR, M_MEAN_PRIOR, M_BETA_PRIOR, DIRICHLET_PRIOR,
                                  dirU, Wscale, Wdegrees, Mmean, Mbeta, Emeans, Esigmas, logDetPrec, Eprecs, resp, logResp, logPi, counts);

        if (fabs(lbCurr - lbPrev) < EPSILON && iter > MIN_NUM_ITERS)
        {
            lbEvidence = lbCurr;
            numIters = iter;
            break;
        }

        lbPrev = lbCurr;
    }
    lbEvidence = lbPrev;

    cout << lbEvidence << "\t" << numIters << endl;

    _weights = Eweights;
    _means = Emeans;
    _sigmas = Esigmas;
}



void vmp::trainMVMixture(const mat &POINTS, size_t numMixtures, size_t maxNumIters,mat &_means, mat &_sigmas, vec &_weights)
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
    size_t numMixtures = 5;
    size_t maxIters = 700;
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





void vmp::testDirichlet()
{
    const size_t numPoints = 100;
    const size_t numDims = 5;

    const size_t maxNumIters = 10;

    auto dirichlet = Dirichlet(numDims, DIRICHLET_PRIOR);

    auto points = randomv(numPoints, 5);
    for (size_t i = 0; i < numPoints; ++i)
        points[i] = i % 3;

    auto discrete = DiscreteArray(numPoints, &dirichlet);
    discrete.observe(points);

    auto msg = dirichlet.addChild(&discrete);

    for (size_t i = 0; i < maxNumIters; ++i)
    {
        discrete.messageToParent(msg);
        dirichlet.updatePosterior();

        cout << discrete.logEvidence() + dirichlet.logEvidence() << endl;

        cout << exp(dirichlet.moments().logProb) << endl;
    }






}









