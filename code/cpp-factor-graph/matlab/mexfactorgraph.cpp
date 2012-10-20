#include "mex.h"


#include <string>
#include <stdint.h>
#include <cstring>

#include "../factorgraph.h"

using namespace std;


/**
 * convert a pointer to matlab representation
 */
mxArray *ptrToArray(void *ptr)
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
mxArray *arrayToArray(const double *src, size_t M, size_t N)
{
    mxArray *result = mxCreateDoubleMatrix(M, N, mxREAL);
    memcpy(mxGetPr(result), src, sizeof(double) * M * N);
    return result;
}



/**
 * get int from array. note that matlab ALWAYS passes ints as doubles
 */
int arrayToInt(const mxArray *arr)
{
    return (int)(*mxGetPr(arr));
}


FactorNode *arrayToNode(const mxArray *arr)
{
    // TODO: this is ugly
    return *((FactorNode **)mxGetData(arr));
}


Message::Type messageType(const mxArray *msg)
{
    return mxGetField(msg, 0, "type") ? (Message::Type) mxGetPr(mxGetField(msg, 0, "type"))[0] : Message::UNKNOWN;
}



GaussianMessage createGaussianMessage(const mxArray *msg)
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
mxArray *messageToStruct(const GaussianMessage &msg)
{
    static const char *FIELDS[] = {"from", "to", "type", "mean", "var"};

    mxArray *result = mxCreateStructMatrix(1, 1, 5, FIELDS);

    mxSetField(result, 0, "from", mxCreateDoubleScalar(msg.from()));
    mxSetField(result, 0, "to", mxCreateDoubleScalar(msg.to()));
    mxSetField(result, 0, "type", mxCreateDoubleScalar(msg.type()));
    mxSetField(result, 0, "mean", arrayToArray(msg.mean(), 1, msg.size()));
    mxSetField(result, 0, "var", arrayToArray(msg.variance(), msg.size(), msg.size()));

    return result;
}


void createNode(const string &type_name, mxArray *plhs[], const mxArray *prhs[])
{
    int id = arrayToInt(prhs[2]);

    FactorNode *result = NULL;
    if (type_name == "evidencenode")
        result = new EvidenceNode(id);
    else if (type_name == "addnode")
        result = new AddNode(id);
    else if (type_name == "equalitynode")
        result = new EqualityNode(id);

//    else if (type_name == "multiplicationnode")
//        result = new MultiplicationNode(id);

    // saving the pointer
    plhs[0] = ptrToArray(result);
}


void processEvidenceNode(FactorNode *node, const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    EvidenceNode *evdNode = static_cast<EvidenceNode*>(node);

    if (function_name == "setInitial")
    {
        // constructing gaussian message
        const int MESSAGE_IDX = 3;
        GaussianMessage msg = createGaussianMessage(prhs[MESSAGE_IDX]);
        evdNode->setInitital(msg);
    }
    else if (function_name == "setDest")
    {
        const int DEST_NODE_IDX = 3;
        evdNode->setDest(arrayToNode(prhs[DEST_NODE_IDX]));
    }
    else if (function_name == "evidence")
    {
        GaussianMessage msg = evdNode->evidence();
        plhs[0] = messageToStruct(msg);
    }
}


void processAddNode(FactorNode *node, const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    AddNode *addNode = static_cast<AddNode*>(node);

    if (function_name == "setConnections")
        addNode->setConnections(arrayToNode(prhs[3]), arrayToNode(prhs[4]), arrayToNode(prhs[5]));
}


void processEqualityNode(FactorNode *node, const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    EqualityNode *eqNode = static_cast<EqualityNode*>(node);
    // TODO: check for # of arguments
    if (function_name == "setConnections")
        eqNode->setConnections(arrayToNode(prhs[3]), arrayToNode(prhs[4]), arrayToNode(prhs[5]));
}


// TODO:
void processCustomNode(FactorNode *node, const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
}






/**
 * the entry point into the function
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs < 2)
    {
        mexErrMsgTxt("Not enough arguments");
        return;
    }

    /* Check for proper number of arguments */
    static const size_t FUNCTION_IDX = 0;
    static const size_t TYPE_IDX = 1;
    static const size_t NODE_IDX = 2;

    string function_name(mxArrayToString(prhs[FUNCTION_IDX]));
    string type_name(mxArrayToString(prhs[TYPE_IDX]));


    if (function_name == "create")
        createNode(type_name, plhs, prhs);
    else if (nrhs >= 3)
    {
        FactorNode *node = arrayToNode(prhs[NODE_IDX]);

        if (function_name == "delete")
            delete node;
        else if (function_name == "get_id")
            plhs[0] = mxCreateDoubleScalar(node->id());
        else if (function_name == "receive")
        {
            // TODO: multiple message types
            int MESSAGE_IDX = 3;
            node->receive(createGaussianMessage(prhs[MESSAGE_IDX]));
        }
        else if (type_name == "evidencenode")
            processEvidenceNode(node, function_name, nlhs, plhs, nrhs, prhs);
        else if (type_name == "addnode")
            processAddNode(node, function_name, nlhs, plhs, nrhs, prhs);
        else if (type_name == "equalitynode")
            processEqualityNode(node, function_name, nlhs, plhs, nrhs, prhs);
        else mexErrMsgTxt("Unknown node type or function name");

    }
    else
        mexErrMsgTxt("Not enough arguments");


}

