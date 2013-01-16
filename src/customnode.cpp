#include "customnode.h"

#include <mex.h>

#include "convert.h"

using namespace std;


bool CustomNode::isSupported(Message::Type type)
{
    return GaussianMessage::GAUSSIAN_VARIANCE;
}



GaussianMessage CustomNode::function(int to, const MessageBox &msgs)
{
    // number of outputs
    const int NRLS = 1;

    mxArray *plhs;

    const char *FIELDS[] = {MEX_FROM, MEX_TYPE, MEX_CONN, MEX_MEAN, MEX_VAR};
    const size_t NUM_FIELDS = 5;

    mxArray *mexMsgs = mxCreateStructMatrix(1, msgs.size(), NUM_FIELDS, FIELDS);

    // number of inputs
    const int NRHS = 3;
    mxArray *prhs[NRHS];
    prhs[0] = mxCreateDoubleScalar(id());
    prhs[1] = mxCreateDoubleScalar(to);
    prhs[2] = mexMsgs;

    MessageBox::const_iterator it = msgs.begin();
    for (int i = 0; i < msgs.size(); ++i)
    {
        int from = it->first;
        const GaussianMessage &msg = it->second;

        string connType;
        if (m_incoming.count(from))
            connType = "INCOMING";
        else if (m_outgoing.count(from))
            connType = "OUTGOING";
        else
            connType = m_connections.at(from);


        mxSetField(mexMsgs, i, MEX_FROM, mxCreateDoubleScalar(from));
        mxSetField(mexMsgs, i, MEX_TYPE, mxCreateString(Message::typeName(msg.type()).c_str()));
        mxSetField(mexMsgs, i, MEX_CONN, mxCreateString(connType.c_str()));
        mxSetField(mexMsgs, i, MEX_MEAN, arrayToMatrix(msg.mean().data(), 1, msg.size()));
        if (msg.type() == GaussianMessage::GAUSSIAN_VARIANCE)
            mxSetField(mexMsgs, i, MEX_VAR, arrayToMatrix(msg.variance().data(), msg.size(), msg.size()));
        else // if (msg.type() == GaussianMessage::GAUSSIAN_PRECISION)
            mxSetField(mexMsgs, i, MEX_VAR, arrayToMatrix(msg.precision().data(), msg.size(), msg.size()));

        ++it;
    }

    mexCallMATLAB(NRLS, &plhs, NRHS, prhs, m_functionName.c_str());
    for (int i = 0; i < NRHS; i++)
        mxDestroyArray(prhs[i]);

    return structToMessage(plhs);
}





