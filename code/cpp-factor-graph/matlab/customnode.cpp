#include "customnode.h"


#include <mex.h>

#include "convert.h"


GaussianMessage CustomNode::function(int to, const MessageBox &msgs)
{
    // this function simply calls the matlab implementation
    mxArray *mexMsgs = mxCreateStructMatrix(1, msgs.size(), NUM_MEX_MSG_FIELDS, MEX_MSG_FIELDS);

    // converting the message
    MessageBox::const_iterator it = msgs.begin();
    for (int i = 0; i < msgs.size(); i++)
    {
        const GaussianMessage &msg = it->second;

        mxSetField(mexMsgs, i, "from", mxCreateDoubleScalar(msg.from()));
        mxSetField(mexMsgs, i, "to", mxCreateDoubleScalar(msg.to()));
        mxSetField(mexMsgs, i, "type", mxCreateDoubleScalar(msg.type()));
        mxSetField(mexMsgs, i, "mean", arrayToArray(msg.mean(), 1, msg.size()));
        mxSetField(mexMsgs, i, "var", arrayToArray(msg.variance(), msg.size(), msg.size()));

        ++it;
    }

    mxArray *plhs;
    mxArray *prhs[2];

    prhs[0] = mxCreateDoubleScalar(20);
    prhs[1] = mexMsgs;
    mexCallMATLAB(1, &plhs, 2, prhs, m_functionName.c_str());

    mxDestroyArray(prhs[0]);
    mxDestroyArray(prhs[1]);

    return createGaussianMessage(plhs);
}


