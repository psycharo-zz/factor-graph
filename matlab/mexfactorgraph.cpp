#include <mex.h>

#include <string>
#include <iostream>
using namespace std;

#include "../algonquin.h"
#include "convert.h"


using namespace algonquin;



const size_t FUNCTION_IDX = 0;
const size_t TYPE_IDX = 1;
const size_t POINTER_IDX = 2;



void processNetwork(const std::string &functionName,
                    int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (functionName == "create")
    {
        plhs[0] = pointerToMxArray(new Network());
        return;
    }

    Network *nwk = static_cast<Network*>(mxArrayToObject(prhs[POINTER_IDX]));
    if (functionName == "train")
    {
        double *speech;
        size_t rows_s, cols_s;
        mxArrayToDoubleArray(prhs[POINTER_IDX+1], speech, rows_s, cols_s);

        double *noise;
        size_t rows_n, cols_n;
        mxArrayToDoubleArray(prhs[POINTER_IDX+2], noise, rows_n, cols_n);

        // training mixtures
        nwk->train(speech, cols_s, noise, cols_n);

        plhs[0] = toStruct(nwk->speechPrior());
        plhs[1] = toStruct(nwk->noisePrior());
    }
    else if (functionName == "process")
    {
        double frame = mxArrayToDouble(prhs[POINTER_IDX+1]);
        cout << frame << endl;
    }
    else if (functionName == "setPriors")
    {
        nwk->setPriors(structToMoG(prhs[POINTER_IDX+1]),
                       structToMoG(prhs[POINTER_IDX+2]));
    }
    else if (functionName == "delete")
        delete nwk;
}


void processNetworkArray(const std::string &functionName,
                         int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (functionName == "create")
    {
        plhs[0] = pointerToMxArray(new NetworkArray(mxArrayToInt(prhs[TYPE_IDX+1])));
        return;
    }

    NetworkArray *nwk = static_cast<NetworkArray*>(mxArrayToObject(prhs[POINTER_IDX]));

    if (functionName == "train")
    {
        double *speech, *noise;
        size_t numDataSpeech, numDataNoise;
        size_t numFreqs;

        mxArrayToDoubleArray(prhs[POINTER_IDX+1], speech, numDataSpeech, numFreqs);
        mxArrayToDoubleArray(prhs[POINTER_IDX+2], noise, numDataNoise, numFreqs);

        if (numFreqs != nwk->numFreqs())
        {
            mexErrMsgTxt("the number of frequencies is not correct");
            return;
        }

        nwk->train(speech, numDataSpeech,
                   noise, numDataNoise);
    }
    else if (functionName == "process")
    {

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
    const size_t maxNumIters = 500;
    const size_t numMixtures = 8;

    Parameters<MoG> params;
    double evidence;

    params =  trainGMM(data,
                       numPoints,
                       maxNumIters,
                       numMixtures,
                       1, GaussianParameters(0, 1e-3), GammaParameters(1e-3, 1e-3),
                       evidence);

    cout << params << endl;

    double means[numMixtures], precs[numMixtures];
    for (size_t m = 0; m < params.numComponents(); ++m)
    {
        means[m] = params.components[m].mean();
        precs[m] = params.components[m].precision;
    }
    plhs[0] = arrayToMxArray(means, 1, numMixtures);
    plhs[1] = arrayToMxArray(precs, 1, numMixtures);
    plhs[2] = arrayToMxArray(params.weights.data(), 1, numMixtures);
}




// the entry point into the function
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs < 1)
    {
        mexErrMsgTxt("Not enough arguments");
        return;
    }

    string functionName(mxArrayToString(prhs[FUNCTION_IDX]));
    string typeName(mxArrayToString(prhs[TYPE_IDX]));

    // here the only possible typeName is Network
    if (typeName == "Network")
        processNetwork(functionName, nlhs, plhs, nrhs, prhs);
    else if (typeName == "NetworkArray")
        processNetworkArray(functionName, nlhs, plhs, nrhs, prhs);
    else if (typeName == "GMM")
        processGMM(nlhs, plhs, nrhs, prhs);
    else if (typeName == "Gaussian")
    {
//        cout << structToGaussian(prhs[POINTER_IDX]) << endl;
        plhs[0] = toStruct(GaussianParameters(0, 1e-3));
    }
    else if (typeName == "Gamma")
    {
//        cout << structToGamma(prhs[POINTER_IDX]) << endl;
        plhs[0] = toStruct(GammaParameters(1e-1, 1e-3));
    }
    else if (typeName == "MoG")
    {
        cout << structToMoG(prhs[POINTER_IDX]) << endl;
    }
    else
        mexErrMsgTxt("Unsupported operation\n");

}
