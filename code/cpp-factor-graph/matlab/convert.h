#ifndef CONVERT_H
#define CONVERT_H

/**
 * Utility functions for converting matlab types to C++
 */

#include <mex.h>
#include <string>
#include <stdint.h>
#include <cstring>

static const char *MEX_MSG_FIELDS[] = {"from", "to", "type", "mean", "var"};
static const int NUM_MEX_MSG_FIELDS = 5;


/**
 * convert a pointer to matlab representation
 */
inline mxArray *ptrToArray(void *ptr)
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
    return *((FactorNode **)mxGetData(arr));
}


inline Message::Type messageType(const mxArray *msg)
{
    return mxGetField(msg, 0, "type") ? (Message::Type) mxGetPr(mxGetField(msg, 0, "type"))[0] : Message::UNKNOWN;
}



inline GaussianMessage createGaussianMessage(const mxArray *msg)
{
    // TODO: make it safer
    int from = mxGetField(msg, 0, "from") ? mxGetPr(mxGetField(msg, 0, "from"))[0] : Message::UNDEFINED_ID;
    int to = mxGetField(msg, 0, "to") ? mxGetPr(mxGetField(msg, 0, "to"))[0] : Message::UNDEFINED_ID;

    // assuming the type is gaussian
    mxArray *meanArr = mxGetField(msg, 0, "mean");
    mxArray *varArr = mxGetField(msg, 0, "var");

    return GaussianMessage(from, to, mxGetPr(meanArr), mxGetPr(varArr), mxGetN(meanArr));
}


/**
 * convert message to the matlab structure
 */
inline mxArray *messageToStruct(const GaussianMessage &msg)
{
    mxArray *result = mxCreateStructMatrix(1, 1, 5, MEX_MSG_FIELDS);

    mxSetField(result, 0, "from", mxCreateDoubleScalar(msg.from()));
    mxSetField(result, 0, "to", mxCreateDoubleScalar(msg.to()));
    mxSetField(result, 0, "type", mxCreateDoubleScalar(msg.type()));
    mxSetField(result, 0, "mean", arrayToArray(msg.mean(), 1, msg.size()));
    mxSetField(result, 0, "var", arrayToArray(msg.variance(), msg.size(), msg.size()));

    return result;
}





#endif // CONVERT_H
