#ifndef CONVERT_H
#define CONVERT_H

/**
 * Utility functions for converting matlab types to C++
 */

#include <mex.h>
#include <string>
#include <stdint.h>
#include <cstring>

#include <factorgraph.h>

// TODO: make a template-based type caster
static const char *MEX_FROM = "from";
static const char *MEX_TYPE = "type";
static const char *MEX_CONN = "connection";
static const char *MEX_MEAN = "mean";
static const char *MEX_VAR = "var";
static const char *MEX_PRECISION = "precision";
static const char *MSG_FIELDS_GAUSSIAN_VAR[] = {MEX_FROM, MEX_TYPE, MEX_MEAN, MEX_VAR};
static const char *MSG_FIELDS_GAUSSIAN_PRECISION[] = {MEX_FROM, MEX_TYPE, MEX_MEAN, MEX_PRECISION};
static const int NUM_MEX_MSG_FIELDS = sizeof(MSG_FIELDS_GAUSSIAN_VAR) / sizeof(char *);


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
inline mxArray *arrayToMatrix(const double *src, size_t M, size_t N)
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
    std::string typeName = mxArrayToString(mxGetField(msg, 0, "type"));
    if (typeName == Message::typeName(Message::GAUSSIAN_VARIANCE))
        return Message::GAUSSIAN_VARIANCE;
    else if (typeName == Message::typeName(Message::GAUSSIAN_PRECISION))
        return Message::GAUSSIAN_PRECISION;
    return Message::UNKNOWN;
}



inline GaussianMessage createGaussianMessage(const mxArray *msg)
{
    Message::Type type = messageType(msg);

    if (type == GaussianMessage::GAUSSIAN_VARIANCE)
    {
        mxArray *meanArr = mxGetField(msg, 0, MEX_MEAN);
        mxArray *varArr = mxGetField(msg, 0, MEX_VAR);
        if (varArr == NULL || meanArr == NULL)
        {
            mexErrMsgTxt("createGaussianMessage: wrong message format");
            return GaussianMessage(0);
        }

        return GaussianMessage(mxGetPr(meanArr), mxGetPr(varArr), mxGetN(meanArr), type);
    }
    else if (type == GaussianMessage::GAUSSIAN_PRECISION)
    {
        mxArray *meanArr = mxGetField(msg, 0, MEX_MEAN);
        mxArray *precisionArr = mxGetField(msg, 0, MEX_PRECISION);

        if (precisionArr == NULL || meanArr == NULL)
        {
            mexErrMsgTxt("createGaussianMessage: wrong message format");
            return GaussianMessage(0);
        }

        return GaussianMessage(mxGetPr(meanArr), mxGetPr(precisionArr), mxGetN(meanArr), type);
    }
    else
        return GaussianMessage(0);
}


/**
 * convert message to the matlab structure
 */
inline mxArray *messageToStruct(const GaussianMessage &msg, int from = Message::UNDEFINED_ID)
{
    mxArray *result = NULL;

    if (msg.type() == GaussianMessage::GAUSSIAN_VARIANCE)
    {
        result = mxCreateStructMatrix(1, 1, NUM_MEX_MSG_FIELDS, MSG_FIELDS_GAUSSIAN_VAR);
        mxSetField(result, 0, MEX_FROM, mxCreateDoubleScalar(from));
        mxSetField(result, 0, MEX_TYPE, mxCreateString(Message::typeName(msg.type()).c_str()));
        mxSetField(result, 0, MEX_MEAN, arrayToMatrix(msg.mean(), 1, msg.size()));
        mxSetField(result, 0, MEX_VAR, arrayToMatrix(msg.variance(), msg.size(), msg.size()));
    }
    else // if (msg.type() == GaussianMessage::)
    {
        result = mxCreateStructMatrix(1, 1, NUM_MEX_MSG_FIELDS, MSG_FIELDS_GAUSSIAN_PRECISION);
        mxSetField(result, 0, MEX_FROM, mxCreateDoubleScalar(from));
        mxSetField(result, 0, MEX_TYPE, mxCreateString(Message::typeName(msg.type()).c_str()));
        mxSetField(result, 0, MEX_MEAN, arrayToMatrix(msg.mean(), 1, msg.size()));
        mxSetField(result, 0, MEX_PRECISION, arrayToMatrix(msg.precision(), msg.size(), msg.size()));
    }

    return result;
}


/**
 * @brief convert the map to a cell array of structs
 */
inline mxArray *messagesToCellArray(const MessageBox &msgs)
{
    mxArray *mexMsgs = mxCreateCellMatrix(1, msgs.size());
    MessageBox::const_iterator it = msgs.begin();
    for (int i = 0; i < msgs.size(); ++i)
    {
        mxSetCell(mexMsgs, i, messageToStruct(it->second, it->first));
        ++it;
    }
    return mexMsgs;

}


/**
 * @brief convert the map to an array of unified structs (mean, var)
 */
inline mxArray *messagesToStructArray(const MessageBox &msgs)
{
    mxArray *result = mxCreateStructMatrix(1, msgs.size(), NUM_MEX_MSG_FIELDS, MSG_FIELDS_GAUSSIAN_VAR);
    MessageBox::const_iterator it = msgs.begin();
    for (int i = 0; i < msgs.size(); ++i)
    {
        int from = it->first;
        const GaussianMessage &msg = it->second;
        mxSetField(result, i, MEX_FROM, mxCreateDoubleScalar(from));
        mxSetField(result, i, MEX_TYPE, mxCreateString(Message::typeName(msg.type()).c_str()));
        mxSetField(result, i, MEX_MEAN, arrayToMatrix(msg.mean(), 1, msg.size()));
        if (msg.type() == GaussianMessage::GAUSSIAN_VARIANCE)
            mxSetField(result, i, MEX_VAR, arrayToMatrix(msg.variance(), msg.size(), msg.size()));
        else // if (msg.type() == GaussianMessage::GAUSSIAN_PRECISION)
            mxSetField(result, i, MEX_VAR, arrayToMatrix(msg.precision(), msg.size(), msg.size()));
        ++it;
    }

    return result;
}






#endif // CONVERT_H
