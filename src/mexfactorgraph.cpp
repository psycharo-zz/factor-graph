#include <mex.h>

#include <string>
#include <stdexcept>
#include <iostream>

using namespace std;


// Check for proper number of arguments
static const size_t FUNCTION_IDX = 0;
static const size_t TYPE_IDX = 1;
static const size_t POINTER_IDX = 2;


#include <factorgraph.h>
#include "customnode.h"
#include "convert.h"



void createNode(const string &type_name, mxArray *plhs[], const mxArray *prhs[])
{
    FactorNode *result = NULL;

    Network *network = *(static_cast<Network**>(mxGetData(prhs[POINTER_IDX])));

    if (type_name == "EvidenceNode")
        result = new EvidenceNode(network);
    else if (type_name == "AddNode")
        result = new AddNode(network);
    else if (type_name == "EqualityNode")
        result = new EqualityNode(network);
    else if (type_name == "CustomNode")
        result = new CustomNode(network);
    else if (type_name == "MultiplicationNode")
        result = new MultiplicationNode(network);
    else if (type_name == "EstimateMultiplicationNode")
        result = new EstimateMultiplicationNode(network);
    else if (type_name == "EquMultNode")
        result = new EquMultNode(network);
    // saving the pointer
    plhs[0] = pointerToArray(result);
}


void processNetwork(const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (function_name == "create")
        plhs[0] = pointerToArray(new Network);
    else
    {
        Network *network = *(static_cast<Network**>(mxGetData(prhs[POINTER_IDX])));

        if (function_name == "addEdge" && nrhs == 5)
        {
            FactorNode *a = *(static_cast<FactorNode**>(mxGetData(prhs[POINTER_IDX+1])));
            FactorNode *b = *(static_cast<FactorNode**>(mxGetData(prhs[POINTER_IDX+2])));
            network->addEdge(a, b);
        }
        else if (function_name == "addEdge" && nrhs == 7)
        {
            FactorNode *a = *(static_cast<FactorNode**>(mxGetData(prhs[POINTER_IDX+1])));
            FactorNode *b = *(static_cast<FactorNode**>(mxGetData(prhs[POINTER_IDX+2])));
            string tagForA(mxArrayToString(prhs[POINTER_IDX+3]));
            string tagForB(mxArrayToString(prhs[POINTER_IDX+4]));
            network->addEdge(a, b, tagForA, tagForB);
        }
        else if (function_name == "setSchedule" && nrhs == 4)
        {
            uint64_t *pointers = (uint64_t *)mxGetData(prhs[POINTER_IDX+1]);
            size_t size = mxGetN(prhs[POINTER_IDX+1]);
            Network::Schedule schedule;
            for (size_t i = 0; i < size; i += 2)
                schedule.push_back(make_pair(reinterpret_cast<FactorNode*>(pointers[i]),
                                             reinterpret_cast<FactorNode*>(pointers[i+1])));
            network->setSchedule(schedule);
        }
        else if (function_name == "step")
            network->step();
        else if (function_name == "adjacencyMatrix")
            plhs[0] = networkAdjacencyMatrix(network->nodes(), network->adjList());
        else if (function_name == "nodes")
            plhs[0] = networkNodes(network->nodes());
        else if (function_name == "delete")
            delete network;
        else
            throw std::runtime_error("Network: unknown function or wrong number of parameters");
    }
}

void processDynamicNetwork(const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (function_name == "create")
        plhs[0] = pointerToArray(new DynamicNetwork);
    else if (function_name == "addTemporalEdge" && nrhs == 5)
    {
        DynamicNetwork *network = *(static_cast<DynamicNetwork**>(mxGetData(prhs[POINTER_IDX])));
        EvidenceNode *a = *(static_cast<EvidenceNode**>(mxGetData(prhs[POINTER_IDX+1])));
        EvidenceNode *b = *(static_cast<EvidenceNode**>(mxGetData(prhs[POINTER_IDX+2])));
        network->addTemporalEdge(a, b);
    }
    else if (function_name == "adjacencyMatrixTemporal")
    {
        DynamicNetwork *network = *(static_cast<DynamicNetwork**>(mxGetData(prhs[POINTER_IDX])));
        plhs[0] = networkAdjacencyMatrix(network->nodes(), network->adjListTemporal());
    }
    else if (function_name == "step" && nrhs == 5)
    {
        DynamicNetwork *network = *(static_cast<DynamicNetwork**>(mxGetData(prhs[POINTER_IDX])));
        network->step(createMessageBoxArray(prhs[POINTER_IDX+1], prhs[POINTER_IDX+2]));
    }
    else
        processNetwork(function_name, nlhs, plhs, nrhs, prhs);

}


void processEvidenceNode(FactorNode *node, const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    EvidenceNode *evdNode = static_cast<EvidenceNode*>(node);

    if (function_name == "propagate")
    {
        // constructing gaussian message
        const int MESSAGE_IDX = 3;
        GaussianMessage msg = structToMessage(prhs[MESSAGE_IDX]);
        evdNode->propagate(msg);
    }
    else if (function_name == "receive")
    {
        // constructing gaussian message
        const int MESSAGE_IDX = 3;
        GaussianMessage msg = structToMessage(prhs[MESSAGE_IDX]);
        evdNode->receive(msg);
    }
    else if (function_name == "evidence")
    {
        GaussianMessage msg = evdNode->evidence();
        plhs[0] = messageToStruct(msg);
    }
    else
        throw std::runtime_error("Network: unknown function or wrong number of parameters");
}


void processAddNode(FactorNode *node, const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
//    AddNode *addNode = static_cast<AddNode*>(node);
//    if (function_name == "setConnections")
//        addNode->setConnections(arrayToNode(prhs[3]), arrayToNode(prhs[4]), arrayToNode(prhs[5]));
}


void processEqualityNode(FactorNode *node, const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    EqualityNode *eqNode = static_cast<EqualityNode*>(node);
    // TODO: check for # of arguments
    if (function_name == "setType")
    {
        const char *typeName = mxArrayToString(prhs[POINTER_IDX+1]);
        if (typeName == NULL)
            throw std::runtime_error("processEqualityNode: string is expected");
        eqNode->setType(Message::typeByName(typeName));
    }
    else if (function_name == "type")
        plhs[0] = mxCreateString(Message::typeName(eqNode->type()).c_str());
    // TODO:
    //throw UnknownFunctionException;
}


void processMultiplicationNode(FactorNode *node, const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    MultiplicationNode *multNode = static_cast<MultiplicationNode*>(node);
    if (function_name == "setMatrix")
    {
        // matrix
        double *matrix = static_cast<double*>(mxGetData(prhs[POINTER_IDX+1]));
        size_t rows = mxGetM(prhs[POINTER_IDX+1]);
        size_t cols = mxGetN(prhs[POINTER_IDX+1]);
        multNode->setMatrix(matrix, rows, cols);
    }
    else if (function_name == "matrix")
    {
        const double *matrix = multNode->matrix();
        plhs[0] = arrayToMatrix(matrix, multNode->rows(), multNode->cols());
    }
}


void processEquMultNode(FactorNode *node, const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    EquMultNode *estNode = static_cast<EquMultNode*>(node);
    if (function_name == "setMatrix")
    {
        // matrix
        double *matrix = static_cast<double*>(mxGetData(prhs[POINTER_IDX+1]));
        size_t rows = mxGetM(prhs[POINTER_IDX+1]);
        size_t cols = mxGetN(prhs[POINTER_IDX+1]);
        estNode->setMatrix(matrix, rows, cols);
    }
}


void processEstimateMultiplicationNode(FactorNode *node, const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    EstimateMultiplicationNode *estNode = static_cast<EstimateMultiplicationNode*>(node);
    if (function_name == "setMatrix")
    {
        // matrix
        double *matrix = static_cast<double*>(mxGetData(prhs[POINTER_IDX+1]));
        size_t rows = mxGetM(prhs[POINTER_IDX+1]);
        size_t cols = mxGetN(prhs[POINTER_IDX+1]);
        if (rows != cols)
            throw std::runtime_error("processEstimateMultiplicationNode");
        estNode->setMatrix(matrix, rows, cols);
    }
}


// TODO:
void processCustomNode(FactorNode *node, const string &function_name, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    CustomNode *custNode = static_cast<CustomNode*>(node);
    if (function_name == "setFunction")
        custNode->setFunction(mxArrayToString(prhs[3]));
}





// the entry point into the function
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs < 2)
    {
        mexErrMsgTxt("Not enough arguments");
        return;
    }

    string function_name(mxArrayToString(prhs[FUNCTION_IDX]));
    string type_name(mxArrayToString(prhs[TYPE_IDX]));

    try {

        if (type_name == "Network")
            processNetwork(function_name, nlhs, plhs, nrhs, prhs);
        else if (type_name == "DynamicNetwork")
            processDynamicNetwork(function_name, nlhs, plhs, nrhs, prhs);
        else if (function_name == "create")
            createNode(type_name, plhs, prhs);
        else if (nrhs >= 3)
        {
            FactorNode *node = arrayToNode(prhs[POINTER_IDX]);

            if (function_name == "delete")
                delete node;
            else if (function_name == "id")
                plhs[0] = mxCreateDoubleScalar(node->id());
            else if (function_name == "messages")
                plhs[0] = messagesToCellArray(node->messages());
            else if (function_name == "message")
            {
                int from = arrayToInt(prhs[POINTER_IDX+1]);
                plhs[0] = messageToStruct(node->message(from), from);
            }
            else if (function_name == "setMessage")
            {
                const mxArray *msg = prhs[POINTER_IDX+1];
                node->setMessage(arrayToInt(mxGetField(msg, 0, MEX_FROM)),
                                 structToMessage(msg));
            }
            else if (type_name == "EvidenceNode")
                processEvidenceNode(node, function_name, nlhs, plhs, nrhs, prhs);
            else if (type_name == "AddNode")
                processAddNode(node, function_name, nlhs, plhs, nrhs, prhs);
            else if (type_name == "EqualityNode")
                processEqualityNode(node, function_name, nlhs, plhs, nrhs, prhs);
            else if (type_name == "CustomNode")
                processCustomNode(node, function_name, nlhs, plhs, nrhs, prhs);
            else if (type_name == "MultiplicationNode")
                processMultiplicationNode(node, function_name, nlhs, plhs, nrhs, prhs);
            else if (type_name == "EstimateMultiplicationNode")
                processEstimateMultiplicationNode(node, function_name, nlhs, plhs, nrhs, prhs);
            else if (type_name == "EquMultNode")
                processEquMultNode(node, function_name, nlhs, plhs, nrhs, prhs);
            else mexErrMsgTxt("Unknown node type or function name");

        }
        else
            mexErrMsgTxt("Not enough arguments");


    }
    catch (std::exception &e)
    {
        mexErrMsgTxt(e.what());
    }
    catch (...)
    {
        mexErrMsgTxt("Unknown error");
    }

}

