#include <mex.h>

#include <string>
#include <iostream>
using namespace std;

#include "../algonquin.h"
#include "../algonquinnetwork.h"
#include "convert.h"




const size_t FUNCTION_IDX = 0;
const size_t TYPE_IDX = 1;
const size_t POINTER_IDX = 2;



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
//        plhs[0] = toMxStruct(nwk->speechDistr());
//        plhs[1] = toMxStruct(nwk->noiseDistr());
    }
    else if (functionName == "process")
    {
        pair<vec,vec> result = nwk->process(mxArrayTo<vec>(prhs[POINTER_IDX+1]));
        plhs[0] = toMxArray<vec>(result.first);
        plhs[1] = toMxArray<vec>(result.second);
    }
    else if (functionName == "delete")
        delete nwk;
}




void processGMM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *data = NULL;
    size_t rows;
    size_t cols;
    mxArrayToDoubleArray(prhs[POINTER_IDX], data, rows, cols);

    const size_t numPoints = cols;
    const size_t maxNumIters = 150;
    const size_t numMixtures = 6;

    auto mixture = trainMixture(data, numPoints, numMixtures, maxNumIters);

    double means[numMixtures], precs[numMixtures];
    for (size_t m = 0; m < mixture->means->size(); ++m)
    {
        means[m] = mixture->means->moments(m).mean;
        precs[m] = mixture->precs->moments(m).precision;
    }
    plhs[0] = toMxArray(means, 1, numMixtures);
    plhs[1] = toMxArray(precs, 1, numMixtures);
    plhs[2] = toMxArray(mixture->weights->moments().probs.data(), 1, numMixtures);
}

void processMVGMM(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    mat POINTS = mxArrayTo<mat>(prhs[POINTER_IDX]);
    size_t numMixtures = mxArrayTo<int>(prhs[POINTER_IDX+1]);
    size_t numIters = mxArrayTo<int>(prhs[POINTER_IDX+2]);

    vector<vec> means;
    vector<mat> sigmas;
    vector<double> weights;

    trainMVMixture(POINTS, numMixtures, numIters,
                   means, sigmas, weights);
//    for (size_t i = 0; i < 2; ++i)
//        cout << means[i] << endl;
//    cout << weights << endl;

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

        // here the only possible typeName is Network
        if (typeName == "Network")
            processNetwork(functionName, nlhs, plhs, nrhs, prhs);
        else if (typeName == "GMM")
            processGMM(nlhs, plhs, nrhs, prhs);
        else if (typeName == "MVGMM")
            processMVGMM(nlhs, plhs, nrhs, prhs);
        else
            mexErrMsgTxt("Unsupported operation\n");
    }
    catch (...)
    {
        mexErrMsgTxt("Unknown error occured\n");
    }


}
