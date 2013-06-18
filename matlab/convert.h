#ifndef CONVERT_H
#define CONVERT_H

/**
 * Utility functions for converting matlab types to C++
 */

#include <mex.h>
#include <string>
#include <stdint.h>
#include <cstring>
#include <vector>

#include "persistentobject.h"



/**
 * convert a pointer to matlab representation
 */
inline mxArray *pointerToMxArray(void *ptr)
{
    mxArray *result = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
    *((uint64_t *)mxGetData(result)) = reinterpret_cast<uint64_t>(ptr);
    return result;
}


/**
 * convert C array to mxArray
 * @param M - number of rows
 * @param N - number of cols
 */
inline mxArray *arrayToMxArray(const double *src, size_t M, size_t N)
{
    mxArray *result = mxCreateDoubleMatrix(M, N, mxREAL);
    memcpy(mxGetPr(result), src, sizeof(double) * M * N);
    return result;
}





//! convert mex array to c array. note that this does not allocate any memory
void mxArrayToDoubleArray(const mxArray *input, double* &data, size_t &rows, size_t &cols)
{
    data = static_cast<double*>(mxGetData(input));
    rows = mxGetM(input);
    cols = mxGetN(input);
}


//! conver mex array to vector<double>
std::vector<double> mxArrayToDoubleVector(const mxArray *input)
{
    double *data = static_cast<double*>(mxGetData(input));
    size_t rows = mxGetM(input);
    size_t cols = mxGetN(input);
    return std::vector<double>(data, data + rows * cols);
}


//! convert mex array to a double
double mxArrayToDouble(const mxArray *input)
{
    return *static_cast<double*>(mxGetData(input));
}


/**
 * get int from array. note that matlab ALWAYS passes ints as doubles
 */
inline int mxArrayToInt(const mxArray *arr)
{
    return (int)(*mxGetPr(arr));
}

inline PersistentObject *mxArrayToObject(const mxArray *arr)
{
    // TODO: this is ugly
    return *(static_cast<PersistentObject**>(mxGetData(arr)));
}





static const char *MEX_TYPE = "type";

static const char *MEX_MEANPREC = "meanPrecision";
static const char *MEX_PREC = "precision";

static const char *MEX_SHAPE = "shape";
static const char *MEX_RATE = "rate";

static const char *MEX_LOGPROB = "logProb";

static const char *MEX_WEIGHT = "weight";

static const char *FIELDS_GAUSSIAN[] = {MEX_TYPE, MEX_MEANPREC, MEX_PREC};
static const char *FIELDS_GAMMA[] = {MEX_TYPE, MEX_SHAPE, MEX_RATE};
static const char *FIELDS_DISCRETE[] = {MEX_TYPE, MEX_LOGPROB};
static const char *FIELDS_GMM[] = {MEX_TYPE, MEX_MEANPREC, MEX_PREC, MEX_WEIGHT};

using namespace vmp;

template<typename TDistr>
mxArray *toStruct(const Parameters<TDistr> &params);


inline Parameters<Gaussian> structToGaussian(const mxArray *msg)
{
    mxArray *meanPrecArr = mxGetField(msg, 0, MEX_MEANPREC);
    mxArray *precArr = mxGetField(msg, 0, MEX_PREC);
    return Parameters<Gaussian>(*mxGetPr(meanPrecArr), *mxGetPr(precArr));
}

template<>
mxArray *toStruct(const Parameters<Gaussian> &params)
{
    mxArray *result = mxCreateStructMatrix(1, 1, 3, FIELDS_GAUSSIAN);
    mxSetField(result, 0, MEX_TYPE, mxCreateString("Gaussian"));
    mxSetField(result, 0, MEX_MEANPREC, mxCreateDoubleScalar(params.meanPrecision));
    mxSetField(result, 0, MEX_PREC, mxCreateDoubleScalar(params.precision));
    return result;
}

inline Parameters<Gamma> structToGamma(const mxArray *msg)
{
    mxArray *shapeArr = mxGetField(msg, 0, MEX_SHAPE);
    mxArray *rateArr = mxGetField(msg, 0, MEX_RATE);
    return Parameters<Gamma>(*mxGetPr(shapeArr), *mxGetPr(rateArr));
}

template<>
mxArray *toStruct(const Parameters<Gamma> &params)
{
    mxArray *result = mxCreateStructMatrix(1, 1, 3, FIELDS_GAMMA);
    mxSetField(result, 0, MEX_TYPE, mxCreateString("Gamma"));
    mxSetField(result, 0, MEX_SHAPE, mxCreateDoubleScalar(params.shape));
    mxSetField(result, 0, MEX_RATE, mxCreateDoubleScalar(params.rate));
    return result;
}


inline Parameters<Discrete> structToDiscrete(const mxArray *msg)
{
    mxArray *logProb = mxGetField(msg, 0, MEX_LOGPROB);
    return Parameters<Discrete>(mxArrayToDoubleVector(logProb));
}

template<>
mxArray *toStruct(const Parameters<Discrete> &params)
{
    mxArray *result = mxCreateStructMatrix(1, 1, 2, FIELDS_DISCRETE);
    mxSetField(result, 0, MEX_TYPE, mxCreateString("Discrete"));
    mxSetField(result, 0, MEX_LOGPROB, arrayToMxArray(params.logProb.data(), 1, params.logProb.size()));
    return result;
}



inline Parameters<MoG> structToMoG(const mxArray *msg)
{
    Parameters<MoG> result;
    const size_t count = mxGetN(msg);

    result.components.resize(count);
    result.weights.resize(count);

    for (size_t m = 0; m < count; ++m)
    {
        double weight = mxArrayToDouble(mxGetField(msg, m, MEX_WEIGHT));
        double meanPrecision = mxArrayToDouble(mxGetField(msg, m, MEX_MEANPREC));
        double precision = mxArrayToDouble(mxGetField(msg, m, MEX_PREC));

        result.components[m] = Parameters<Gaussian>(meanPrecision,precision);
        result.weights[m] = weight;
    }
    return result;
}


mxArray *toStruct(const MoG *mog)
{
    mxArray *result = mxCreateStructMatrix(1, mog->numComponents(), 4, FIELDS_GMM);
    for (size_t i = 0; i < mog->numComponents(); ++i)
    {
        mxSetField(result, i, MEX_TYPE, mxCreateString("GMM"));
        mxSetField(result, i, MEX_MEANPREC, mxCreateDoubleScalar(mog->parameters(i).meanPrecision));
        mxSetField(result, i, MEX_PREC, mxCreateDoubleScalar(mog->parameters(i).precision));
        mxSetField(result, i, MEX_WEIGHT, mxCreateDoubleScalar(mog->weight(i)));
    }
    return result;
}







#endif // CONVERT_H
