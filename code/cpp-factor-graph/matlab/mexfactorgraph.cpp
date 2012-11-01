#include "mex.h"



#include "../factorgraph.h"


#include "customnode.h"
#include "convert.h"


using namespace std;


/* Check for proper number of arguments */
static const size_t FUNCTION_IDX = 0;
static const size_t TYPE_IDX = 1;
static const size_t POINTER_IDX = 2;




void createNode(const string &type_name, mxArray *plhs[], const mxArray *prhs[])
{
    FactorNode *result = NULL;
    if (type_name == "EvidenceNode")
        result = new EvidenceNode;
    else if (type_name == "AddNode")
        result = new AddNode;
    else if (type_name == "EqualityNode")
        result = new EqualityNode;
    else if (type_name == "CustomNode")
        result = new CustomNode;
//    else if (type_name == "MultiplicationNode")
//        result = new MultiplicationNode;
    // saving the pointer
    plhs[0] = pointerToArray(result);
}



void processNetwork(const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (function_name == "create")
        plhs[0] = pointerToArray(new Network);
    else if (function_name == "addEdge")
    {
        Network *network = *(static_cast<Network**>(mxGetData(prhs[POINTER_IDX])));
        FactorNode *a = *(static_cast<FactorNode**>(mxGetData(prhs[POINTER_IDX+1])));
        FactorNode *b = *(static_cast<FactorNode**>(mxGetData(prhs[POINTER_IDX+2])));
        network->addEdge(a, b);
    }
    else if (function_name == "setSchedule")
    {
        Network *network = *(static_cast<Network**>(mxGetData(prhs[POINTER_IDX])));
        uint64_t *pointers = (uint64_t *)mxGetData(prhs[POINTER_IDX+1]);
        size_t size = mxGetN(prhs[POINTER_IDX+1]);

        Network::Schedule schedule;
        for (size_t i = 0; i < size; i += 2)
            schedule.push_back(make_pair(reinterpret_cast<FactorNode*>(pointers[i]),
                                         reinterpret_cast<FactorNode*>(pointers[i+1])));
        network->setSchedule(schedule);
    }
    else if (function_name == "step")
    {
        Network *network = *(static_cast<Network**>(mxGetData(prhs[POINTER_IDX])));
        network->step();
    }
}




void processEvidenceNode(FactorNode *node, const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    EvidenceNode *evdNode = static_cast<EvidenceNode*>(node);

    if (function_name == "propagate")
    {
        // constructing gaussian message
        const int MESSAGE_IDX = 3;
        GaussianMessage msg = createGaussianMessage(prhs[MESSAGE_IDX]);
        evdNode->propagate(msg);
    }
    else if (function_name == "receive")
    {
        // constructing gaussian message
        const int MESSAGE_IDX = 3;
        GaussianMessage msg = createGaussianMessage(prhs[MESSAGE_IDX]);
        evdNode->receive(msg);
    }
    else if (function_name == "evidence")
    {
        GaussianMessage msg = evdNode->evidence();
        plhs[0] = messageToStruct(msg);
    }

}


void processAddNode(FactorNode *node, const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
//    AddNode *addNode = static_cast<AddNode*>(node);
//    if (function_name == "setConnections")
//        addNode->setConnections(arrayToNode(prhs[3]), arrayToNode(prhs[4]), arrayToNode(prhs[5]));
}


void processEqualityNode(FactorNode *node, const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
//    EqualityNode *eqNode = static_cast<EqualityNode*>(node);
//    // TODO: check for # of arguments
//    if (function_name == "setConnections")
//        eqNode->setConnections(arrayToNode(prhs[3]), arrayToNode(prhs[4]), arrayToNode(prhs[5]));
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

    string function_name(mxArrayToString(prhs[FUNCTION_IDX]));
    string type_name(mxArrayToString(prhs[TYPE_IDX]));


    if (type_name == "Network")
        processNetwork(function_name, nlhs, plhs, nrhs, prhs);
    else if (function_name == "create")
        createNode(type_name, plhs, prhs);
    else if (nrhs >= 3)
    {
        FactorNode *node = arrayToNode(prhs[POINTER_IDX]);

        if (function_name == "delete")
            delete node;
        else if (function_name == "id")
            plhs[0] = mxCreateDoubleScalar(node->id());
//        else if (function_name == "propagate" && nrhs == 5)
//            node->propagate(arrayToInt(prhs[POINTER_IDX+1]), createGaussianMessage(prhs[POINTER_IDX+2]));
        else if (type_name == "EvidenceNode")
            processEvidenceNode(node, function_name, nlhs, plhs, nrhs, prhs);
        else if (type_name == "AddNode")
            processAddNode(node, function_name, nlhs, plhs, nrhs, prhs);
        else if (type_name == "EqualityNode")
            processEqualityNode(node, function_name, nlhs, plhs, nrhs, prhs);
        else if (type_name == "CustomNode")
            processCustomNode(node, function_name, nlhs, plhs, nrhs, prhs);
        else mexErrMsgTxt("Unknown node type or function name");

    }
    else
        mexErrMsgTxt("Not enough arguments");


}

