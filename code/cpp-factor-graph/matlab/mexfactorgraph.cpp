#include "mex.h"



#include "../factorgraph.h"

#include "customnode.h"
#include "convert.h"


using namespace std;




void createNode(const string &type_name, mxArray *plhs[], const mxArray *prhs[])
{
    FactorNode *result = NULL;
    if (type_name == "evidencenode")
        result = new EvidenceNode;
    else if (type_name == "addnode")
        result = new AddNode;
    else if (type_name == "equalitynode")
        result = new EqualityNode;
    else if (type_name == "customnode")
        result = new CustomNode;

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
    CustomNode *custNode = static_cast<CustomNode*>(node);
    if (function_name == "setFunction")
        custNode->setFunction(mxArrayToString(prhs[3]));
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
        else if (type_name == "customnode")
            processCustomNode(node, function_name, nlhs, plhs, nrhs, prhs);
        else mexErrMsgTxt("Unknown node type or function name");

    }
    else
        mexErrMsgTxt("Not enough arguments");


}

