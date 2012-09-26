#include "mex.h"


#include <iostream>
#include <cmath>
#include <string>
#include <stdint.h>
#include <cstring>

// DEBUG
#include <typeinfo>

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
 * convert an interger to matlab representation
 */
mxArray *intToArray(int value)
{
    mxArray *out = mxCreateNumericMatrix(1, 1, mxINT64_CLASS, mxREAL);
    *((int*)mxGetData(out)) = value;
    return out;

}

/**
 * convert a message to matlab representation
 */
mxArray *messageToArray(const Message &msg)
{
    mxArray *out = mxCreateDoubleMatrix(1, 2, mxREAL);
    memcpy(mxGetPr(out), &msg, sizeof(msg));
    return out;
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





/**
 * the entry point into the function
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    /* Check for proper number of arguments */

    string function_name(mxArrayToString(prhs[0]));
    string type_name(mxArrayToString(prhs[1]));
    
    if (function_name == "create") 
    {
        int id = arrayToInt(prhs[2]);
       
        FactorNode *result = NULL;
        if (type_name == "evidencenode")
            result = new EvidenceNode(id);
        else if (type_name == "addnode")
            result = new AddNode(id);
        else if (type_name == "equalitynode")
            result = new EqualityNode(id);
       
        plhs[0] = ptrToArray(result);
    } 
    else
    {    
        FactorNode *node = arrayToNode(prhs[2]);
        
        
        if (function_name == "receive" && type_name == "evidencenode" && nrhs == 4) 
        {
            EvidenceNode *evidence = static_cast<EvidenceNode*>(node);
            double *msg = mxGetPr(prhs[3]);
            evidence->receive(Message(msg[0], msg[1]));
        }
        else if (function_name == "setDest" && type_name == "evidencenode")
        {
            EvidenceNode *evidence = static_cast<EvidenceNode*>(node);
            evidence->setDest(arrayToNode(prhs[3]));
        }
        else if (function_name == "evidence" && type_name == "evidencenode")
        {
            EvidenceNode *evidence = static_cast<EvidenceNode*>(node);
            Message msg = evidence->evidence();
            plhs[0] = messageToArray(msg);            
        }
        
        else if (function_name == "setConnections" && type_name == "addnode")
        {
            AddNode *addNode = static_cast<AddNode*>(node);
            addNode->setConnections(arrayToNode(prhs[3]), arrayToNode(prhs[4]), arrayToNode(prhs[5]));
        }        
        
        else if (function_name == "setConnections" && type_name == "equalitynode")
        {
            EqualityNode *eqNode = static_cast<EqualityNode*>(node);
            eqNode->setConnections(arrayToNode(prhs[3]), arrayToNode(prhs[4]), arrayToNode(prhs[5]));
        }        
        
        else if (function_name == "delete")
            delete node;
        else if (function_name == "get_id")
            plhs[0] = intToArray(node->id());
        else if (function_name == "receive")
        {
            int from = arrayToInt(prhs[3]);
            double *msg = mxGetPr(prhs[3]);
            node->receive(from, Message(msg[0], msg[1]));
        }
       
    }
}

