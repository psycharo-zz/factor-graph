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

using namespace vmp;


// pointers are handled a bit differently
template<typename T>
T *mxArrayToPtr(const mxArray *array)
{
    return *(static_cast<T**>(mxGetData(array)));
}

template<typename T>
inline mxArray *ptrToMxArray(T *ptr)
{
    mxArray *result = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
    *((uint64_t *)mxGetData(result)) = reinterpret_cast<uint64_t>(ptr);
    return result;
}



template <typename T>
mxArray *toMxArray(T value);


template<>
inline mxArray *toMxArray(double value)
{
    return mxCreateDoubleScalar(value);
}

template<>
inline mxArray *toMxArray(void *ptr)
{
    mxArray *result = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
    *((uint64_t *)mxGetData(result)) = reinterpret_cast<uint64_t>(ptr);
    return result;
}

template<>
inline mxArray *toMxArray(const char *str)
{
    return mxCreateString(str);
}

template<>
inline mxArray *toMxArray(const std::vector<double> &v)
{
    mxArray *result = mxCreateDoubleMatrix(1, v.size(), mxREAL);
    memcpy(mxGetPr(result), v.data(), sizeof(double) * v.size());
    return result;
}

//! convert C array to mxArray
inline mxArray *toMxArray(const double *src, size_t rows, size_t cols)
{
    mxArray *result = mxCreateDoubleMatrix(rows, cols, mxREAL);
    memcpy(mxGetPr(result), src, sizeof(double) * rows * cols);
    return result;
}





//! convert mex array to c++ types
template<typename T>
T mxArrayTo(const mxArray *array);


template<>
inline int mxArrayTo(const mxArray *arr)
{
    return (int)(*mxGetPr(arr));
}

template<>
inline std::vector<double> mxArrayTo(const mxArray *array)
{
    double *data = static_cast<double*>(mxGetData(array));
    size_t rows = mxGetM(array);
    size_t cols = mxGetN(array);
    return std::vector<double>(data, data + rows * cols);
}


template<>
inline double mxArrayTo(const mxArray *array)
{
    return *static_cast<double*>(mxGetData(array));
}


//template<>
//inline PersistentObject* mxArrayTo(const mxArray *array)
//{
//    return *(static_cast<PersistentObject**>(mxGetData(array)));
//}


//! convert mex array to c array. note that this does not allocate any memory
void mxArrayToDoubleArray(const mxArray *input, double* &data, size_t &rows, size_t &cols)
{
    data = mxGetPr(input);
    rows = mxGetM(input);
    cols = mxGetN(input);
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



template<typename TDistr>
Parameters<TDistr> mxStructTo(const mxArray *array);



template<>
inline Parameters<Gaussian> mxStructTo(const mxArray *msg)
{
    mxArray *meanPrecArr = mxGetField(msg, 0, MEX_MEANPREC);
    mxArray *precArr = mxGetField(msg, 0, MEX_PREC);
    return Parameters<Gaussian>(*mxGetPr(meanPrecArr), *mxGetPr(precArr));
}


template<>
inline Parameters<Gamma> mxStructTo(const mxArray *msg)
{
    mxArray *shapeArr = mxGetField(msg, 0, MEX_SHAPE);
    mxArray *rateArr = mxGetField(msg, 0, MEX_RATE);
    return Parameters<Gamma>(*mxGetPr(shapeArr), *mxGetPr(rateArr));
}

template<>
inline Parameters<Discrete> mxStructTo(const mxArray *msg)
{
    mxArray *logProb = mxGetField(msg, 0, MEX_LOGPROB);
    return Parameters<Discrete>(mxArrayTo<std::vector<double> >(logProb));
}


template<>
inline Parameters<MoG> mxStructTo(const mxArray *msg)
{
    Parameters<MoG> result;
    const size_t count = mxGetN(msg);

    result.components.resize(count);
    result.weights.resize(count);

    for (size_t m = 0; m < count; ++m)
    {
        double weight = mxArrayTo<double>(mxGetField(msg, m, MEX_WEIGHT));
        double meanPrecision = mxArrayTo<double>(mxGetField(msg, m, MEX_MEANPREC));
        double precision = mxArrayTo<double>(mxGetField(msg, m, MEX_PREC));

        result.components[m] = Parameters<Gaussian>(meanPrecision,precision);
        result.weights[m] = weight;
    }
    return result;
}





template<typename TDistr>
mxArray *toMxStruct(const Parameters<TDistr> &params);



template<>
mxArray *toMxStruct(const Parameters<Gaussian> &params)
{
    mxArray *result = mxCreateStructMatrix(1, 1, 3, FIELDS_GAUSSIAN);
    mxSetField(result, 0, MEX_TYPE, mxCreateString("Gaussian"));
    mxSetField(result, 0, MEX_MEANPREC, mxCreateDoubleScalar(params.meanPrecision));
    mxSetField(result, 0, MEX_PREC, mxCreateDoubleScalar(params.precision));
    return result;
}


template<>
mxArray *toMxStruct(const Parameters<Gamma> &params)
{
    mxArray *result = mxCreateStructMatrix(1, 1, 3, FIELDS_GAMMA);
    mxSetField(result, 0, MEX_TYPE, mxCreateString("Gamma"));
    mxSetField(result, 0, MEX_SHAPE, mxCreateDoubleScalar(params.shape));
    mxSetField(result, 0, MEX_RATE, mxCreateDoubleScalar(params.rate));
    return result;
}

template<>
mxArray *toMxStruct(const Parameters<Discrete> &params)
{
    mxArray *result = mxCreateStructMatrix(1, 1, 2, FIELDS_DISCRETE);
    mxSetField(result, 0, MEX_TYPE, mxCreateString("Discrete"));
    mxSetField(result, 0, MEX_LOGPROB, toMxArray(params.logProb.data(), 1, params.logProb.size()));
    return result;
}


// TODO: also use Parameters<MoG> instead
mxArray *toMxStruct(const MoG *mog)
{
    mxArray *result = mxCreateStructMatrix(1, mog->numComponents(), 4, FIELDS_GMM);
    for (size_t i = 0; i < mog->numComponents(); ++i)
    {
        mxSetField(result, i, MEX_TYPE, toMxArray("GMM"));
        mxSetField(result, i, MEX_MEANPREC, toMxArray(mog->parameters(i).meanPrecision));
        mxSetField(result, i, MEX_PREC, toMxArray(mog->parameters(i).precision));
        mxSetField(result, i, MEX_WEIGHT, toMxArray(mog->weight(i)));
    }
    return result;
}




#endif // CONVERT_H
