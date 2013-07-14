#include <mex.h>

#include <string>
#include <iostream>
using namespace std;

#include <algonquin/algonquin.h>
#include <algonquin/algonquinnetwork.h>
#include <matlab/convert.h>




const size_t FUNCTION_IDX = 0;
const size_t TYPE_IDX = 1;
const size_t POINTER_IDX = 2;
const size_t PARAM_IDX = 2;



void processNetwork(const std::string &functionName,
                    int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    if (functionName == "create")
    {
        plhs[0] = ptrToMxArray(new AlgonquinNetwork());
        return;
    }

    AlgonquinNetwork *nwk = mxArrayToPtr<AlgonquinNetwork>(prhs[POINTER_IDX]);
    if (functionName == "train")
    {
        mat speech = mxArrayTo<mat>(prhs[POINTER_IDX+1]);
        size_t numSpeechComps = mxArrayTo<int>(prhs[POINTER_IDX+2]);

        mat noise = mxArrayTo<mat>(prhs[POINTER_IDX+3]);
        size_t numNoiseComps = mxArrayTo<int>(prhs[POINTER_IDX+4]);

        size_t maxNumIters = mxArrayTo<int>(prhs[POINTER_IDX+5]);

        nwk->train(speech, numSpeechComps, noise, numNoiseComps, maxNumIters);
        plhs[0] = toMxStruct(nwk->speechDistr());
        plhs[1] = toMxStruct(nwk->noiseDistr());
    }
    else if (functionName == "process")
    {
        vec frame = mxArrayTo<vec>(prhs[POINTER_IDX+1]);
        auto result = nwk->process(frame);
        plhs[0] = toMxArray(*result.first);
        plhs[1] = toMxArray(*result.second);
    }
    else if (functionName == "delete")
        delete nwk;
}



void processMVGMM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    mat POINTS = mxArrayTo<mat>(prhs[POINTER_IDX]);
    size_t numMixtures = mxArrayTo<int>(prhs[POINTER_IDX+1]);
    size_t maxNumIters = mxArrayTo<int>(prhs[POINTER_IDX+2]);
    vec assignments = mxArrayTo<vec>(prhs[POINTER_IDX+3]);
    mat centers = mxArrayTo<mat>(prhs[POINTER_IDX+4]);

    mat means;
    cube sigmas;
    vec weights;
    double evidence;
    size_t iters;
    mat resps;
    trainMVMixtureVB(POINTS, numMixtures, maxNumIters, assignments, centers,
                     means, sigmas, weights, evidence, iters, resps);
    plhs[0] = toMxArray(means);
    plhs[1] = toMxArray(sigmas);
    plhs[2] = toMxArray(weights);
    plhs[3] = toMxArray(evidence);
    plhs[4] = toMxArray(iters);
    plhs[5] = toMxArray(resps);
}



void processAlgonquin(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    mat frames = mxArrayTo<mat>(prhs[POINTER_IDX]);
    mat sMeans = mxArrayTo<mat>(prhs[POINTER_IDX+1]);
    mat sVars = mxArrayTo<mat>(prhs[POINTER_IDX+2]);
    vec sWeights = mxArrayTo<vec>(prhs[POINTER_IDX+3]);

    mat nMeans = mxArrayTo<mat>(prhs[POINTER_IDX+4]);
    mat nVars = mxArrayTo<mat>(prhs[POINTER_IDX+5]);
    vec nWeights = mxArrayTo<vec>(prhs[POINTER_IDX+6]);

    size_t numIters = mxArrayTo<int>(prhs[POINTER_IDX+7]);

    pair<mat,mat> result = vmp::runAlgonquin(sMeans, sVars, sWeights,
                                             nMeans, nVars, nWeights,
                                             frames, numIters);
    plhs[0] = toMxArray(result.first);
    plhs[1] = toMxArray(result.second);
}




void processExample(const string &name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (name == "univariate")
    {
        if (nrhs-1 != POINTER_IDX+1)
            throw std::runtime_error("processExample(): univariate");

        vec data = mxArrayTo<vec>(prhs[POINTER_IDX]);
        size_t maxNumIters = mxArrayTo<int>(prhs[POINTER_IDX+1]);

        size_t iters;
        vec lbEvidence;

        Parameters<Gaussian> res = trainUnivariateGaussian(data, maxNumIters, iters, lbEvidence);

        plhs[0] = toMxArray(res.mean());
        plhs[1] = toMxArray(res.precision);
        plhs[2] = toMxArray(iters);
        plhs[3] = toMxArray(lbEvidence);
    }
    else if (name == "univariateMixture")
    {
        vec data = mxArrayTo<vec>(prhs[PARAM_IDX]);
        const size_t numMixtures = mxArrayTo<int>(prhs[PARAM_IDX+1]);
        const size_t maxNumIters = mxArrayTo<int>(prhs[PARAM_IDX+2]);

        auto mixture = trainUnivariateMixture(data.memptr(), data.n_elem, numMixtures, maxNumIters);

        vec means(numMixtures);
        vec precs(numMixtures);
        for (size_t m = 0; m < mixture->means->size(); ++m)
        {
            means[m] = mixture->means->moments(m).mean;
            precs[m] = mixture->precs->moments(m).precision;
        }
        plhs[0] = toMxArray(means);
        plhs[1] = toMxArray(precs);
        plhs[2] = toMxArray(mixture->weights->moments().probs);
        plhs[3] = toMxArray(mixture->iters);
        plhs[4] = toMxArray(mixture->evidence);

        delete mixture;
    }
    else if (name == "multivariateMixture")
    {
        const mat data = mxArrayTo<mat>(prhs[PARAM_IDX]);
        const size_t numMixtures = mxArrayTo<int>(prhs[PARAM_IDX+1]);
        const size_t maxNumIters = mxArrayTo<int>(prhs[PARAM_IDX+2]);
        const vec initAssigns = mxArrayTo<vec>(prhs[PARAM_IDX+3]);
        const mat initMeans = mxArrayTo<mat>(prhs[PARAM_IDX+4]);

        const size_t dims = data.n_cols;
        const size_t numPoints = data.n_rows;

        if (dims > 100)
            throw std::runtime_error("too many dimensions");

    //        const mat M_MEAN_PRIOR = mxArrayTo<mat>(prhs[PARAM_IDX+3]);
//        const mat M_PREC_PRIOR = mxArrayTo<mat>(prhs[PARAM_IDX+4]);
//        const double W_DEGREES_PRIOR = mxArrayTo<double>(prhs[PARAM_IDX+5]);
//        const mat W_SCALE_PRIOR = mxArrayTo<mat>(prhs[PARAM_IDX+5]);

        cout << numPoints << "x" << dims << endl;

        mat means;
        cube sigmas;
        vec weights;
        mat resps;
        size_t iters;
        double evidence;

        const double initDegrees = dims+1;
        const mat initScale = 1e-2 * eye(dims,dims);

        trainMultivariateMixture(data, numMixtures, maxNumIters,
                                 initAssigns, initMeans,
                                 initDegrees, initScale,
                                 means, sigmas, weights,
                                 iters, evidence);



        plhs[0] = toMxArray(means);
        plhs[1] = toMxArray(sigmas);
        plhs[2] = toMxArray(weights);
        plhs[3] = toMxArray(evidence);
        plhs[4] = toMxArray(iters);
        plhs[5] = toMxArray(resps);



    }
    else
        throw std::runtime_error("processExample(): unknown");
}




// the entry point into the function
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs < 1)
    {
        mexErrMsgTxt("Not enough arguments");
        return;
    }

    try
    {
        string functionName(mxArrayToString(prhs[FUNCTION_IDX]));
        string typeName(mxArrayToString(prhs[TYPE_IDX]));

        if (functionName == "examples")
            processExample(typeName, nlhs, plhs, nrhs, prhs);
        else
        {
            // here the only possible typeName is Network
            if (typeName == "Network")
                processNetwork(functionName, nlhs, plhs, nrhs, prhs);
            else if (typeName == "MVGMM")
                processMVGMM(nlhs, plhs, nrhs, prhs);
            else if (typeName == "algonquin")
                processAlgonquin(nlhs, plhs, nrhs, prhs);
            else
                mexErrMsgTxt("Unsupported operation\n");
        }
    }
    catch (const std::runtime_error &err)
    {
        mexErrMsgTxt(err.what());
    }
    catch (...)
    {
        mexErrMsgTxt("Unknown error occured\n");
    }


}
