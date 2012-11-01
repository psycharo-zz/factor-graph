#ifndef CONVERT_H
#define CONVERT_H

/**
 * Utility functions for converting matlab types to C++
 */

#include <mex.h>
#include <string>
#include <stdint.h>
#include <cstring>

static const char *MEX_FROM = "from";
static const char *MEX_TYPE = "type";
static const char *MEX_MEAN = "mean";
static const char *MEX_VAR = "var";
static const char *MEX_MSG_FIELDS[] = {MEX_FROM, MEX_TYPE, MEX_MEAN, MEX_VAR};
static const int NUM_MEX_MSG_FIELDS = sizeof(MEX_MSG_FIELDS) / sizeof(char *);


/**
 * convert a pointer to matlab representation
 */
inline mxArray *pointerToArray(void *ptr)
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
inline mxArray *arrayToArray(const double *src, size_t M, size_t N)
{
    mxArray *result = mxCreateDoubleMatrix(M, N, mxREAL);
    memcpy(mxGetPr(result), src, sizeof(double) * M * N);
    return result;
}



/**
 * get int from array. note that matlab ALWAYS passes ints as doubles
 */
inline int arrayToInt(const mxArray *arr)
{
    return (int)(*mxGetPr(arr));
}


inline FactorNode *arrayToNode(const mxArray *arr)
{
    // TODO: this is ugly
    return *(static_cast<FactorNode**>(mxGetData(arr)));
}




inline Message::Type messageType(const mxArray *msg)
{
    return mxGetField(msg, 0, "type") ? (Message::Type) mxGetPr(mxGetField(msg, 0, "type"))[0] : Message::UNKNOWN;
}



inline GaussianMessage createGaussianMessage(const mxArray *msg)
{
    // assuming the type is gaussian
    mxArray *meanArr = mxGetField(msg, 0, MEX_MEAN);
    mxArray *varArr = mxGetField(msg, 0, MEX_VAR);

    return GaussianMessage(mxGetPr(meanArr), mxGetPr(varArr), mxGetN(meanArr));
}


/**
 * convert message to the matlab structure
 */
inline mxArray *messageToStruct(const GaussianMessage &msg)
{
    mxArray *result = mxCreateStructMatrix(1, 1, NUM_MEX_MSG_FIELDS, MEX_MSG_FIELDS);

    mxSetField(result, 0, MEX_TYPE, mxCreateDoubleScalar(msg.type()));
    mxSetField(result, 0, MEX_MEAN, arrayToArray(msg.mean(), 1, msg.size()));
    mxSetField(result, 0, MEX_VAR, arrayToArray(msg.variance(), msg.size(), msg.size()));

    return result;
}





#endif // CONVERT_H
