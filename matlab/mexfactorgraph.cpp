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
        size_t maxNumIters = mxArrayTo<int>(prhs[POINTER_IDX+5]);

        double *speech;
        size_t rows_s, cols_s;
        mxArrayToDoubleArray(prhs[POINTER_IDX+1], speech, rows_s, cols_s);
        size_t numSpeechComps = mxArrayTo<int>(prhs[POINTER_IDX+2]);

        double *noise;
        size_t rows_n, cols_n;
        mxArrayToDoubleArray(prhs[POINTER_IDX+3], noise, rows_n, cols_n);
        size_t numNoiseComps = mxArrayTo<int>(prhs[POINTER_IDX+4]);

        nwk->train(speech, cols_s, numSpeechComps, noise, cols_n, numNoiseComps, maxNumIters);

        plhs[0] = toMxStruct(nwk->speechDistr());
        plhs[1] = toMxStruct(nwk->noiseDistr());
    }
    else if (functionName == "process")
    {
        pair<double,double> result = nwk->process(mxArrayTo<double>(prhs[POINTER_IDX+1]));
        plhs[0] = toMxArray(result.first);
        plhs[1] = toMxArray(result.second);
    }
    else if (functionName == "setDistrs")
    {
        cout << "not supported" << endl;
//        nwk->setDistributions(mxStructTo<MoG>(prhs[POINTER_IDX+1]),
//                              mxStructTo<MoG>(prhs[POINTER_IDX+2]));
    }
    else if (functionName == "distrs")
    {
        if (nwk->trained())
        {
            plhs[0] = toMxStruct(nwk->speechDistr());
            plhs[1] = toMxStruct(nwk->noiseDistr());
        }
    }
    else if (functionName == "delete")
        delete nwk;
}





inline void processNetworkArray(const std::string &functionName,
                                int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    if (functionName == "create")
    {
        plhs[0] = ptrToMxArray(new NetworkArray(mxArrayTo<int>(prhs[POINTER_IDX])));
        return;
    }

    NetworkArray *arr = mxArrayToPtr<NetworkArray>(prhs[POINTER_IDX]);
    if (functionName == "train")
    {
        double *speech;
        size_t numFramesS, numBinsS;
        mxArrayToDoubleArray(prhs[POINTER_IDX+1], speech, numFramesS, numBinsS);
        size_t numSpeechComps = mxArrayTo<int>(prhs[POINTER_IDX+2]);

        double *noise;
        size_t numFramesN, numBinsN;
        mxArrayToDoubleArray(prhs[POINTER_IDX+3], noise, numFramesN, numBinsN);
        size_t numNoiseComps = mxArrayTo<int>(prhs[POINTER_IDX+4]);

        size_t maxIters = mxArrayTo<int>(prhs[POINTER_IDX+5]);

        arr->train(speech, numFramesS, numSpeechComps,
                   noise, numFramesN, numNoiseComps,
                   maxIters);
    }
    else if (functionName == "speechDistr")
    {
        plhs[0] = toMxStruct(arr->speechDistr(mxArrayTo<int>(prhs[POINTER_IDX+1])));
    }
    else if (functionName == "noiseDistr")
    {
        plhs[0] = toMxStruct(arr->noiseDistr(mxArrayTo<int>(prhs[POINTER_IDX+1])));
    }
    else if (functionName == "process")
    {
        double *frame;
        size_t rows, cols;
        mxArrayToDoubleArray(prhs[POINTER_IDX+1], frame, rows, cols);
        auto result = arr->process(frame);
        plhs[0] = toMxArray(result.first, rows, cols);
        plhs[1] = toMxArray(result.second, rows, cols);
    }
    else if (functionName == "delete")
        delete arr;
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
        else if (typeName == "NetworkArray")
            processNetworkArray(functionName, nlhs, plhs, nrhs, prhs);
        else if (typeName == "GMM")
            processGMM(nlhs, plhs, nrhs, prhs);
        else
            mexErrMsgTxt("Unsupported operation\n");
    }
    catch (...)
    {
        mexErrMsgTxt("Unknown error occured\n");
    }


}
