#include "customnode.h"


#include <mex.h>

#include "convert.h"


GaussianMessage CustomNode::function(int to, const MessageBox &msgs)
{
    mxArray *mexMsgs = mxCreateStructMatrix(1, msgs.size(), NUM_MEX_MSG_FIELDS, MEX_MSG_FIELDS);

    // converting the messages
    MessageBox::const_iterator it = msgs.begin();
    for (int i = 0; i < msgs.size(); ++i)
    {
        const GaussianMessage &msg = it->second;
        mxSetField(mexMsgs, i, MEX_FROM, mxCreateDoubleScalar(msg.from()));
        mxSetField(mexMsgs, i, MEX_TO, mxCreateDoubleScalar(msg.to()));
        mxSetField(mexMsgs, i, MEX_TYPE, mxCreateDoubleScalar(msg.type()));
        mxSetField(mexMsgs, i, MEX_MEAN, arrayToArray(msg.mean(), 1, msg.size()));
        mxSetField(mexMsgs, i, MEX_VAR, arrayToArray(msg.variance(), msg.size(), msg.size()));
        ++it;
    }

    // number of outputs
    const int NRLS = 1;
    mxArray *plhs;

    // number of inputs
    const int NRHS = 5;
    mxArray *prhs[NRHS];
    prhs[0] = mxCreateDoubleScalar(id());
    prhs[1] = mxCreateDoubleScalar(to);
    prhs[2] = mexMsgs;

    // TODO: change double to int?
    vector<double> tmp_data;

    tmp_data.assign(m_incoming.begin(), m_incoming.end());
    prhs[3] = arrayToArray(tmp_data.data(), 1, tmp_data.size());

    tmp_data.assign(m_outgoing.begin(), m_outgoing.end());
    prhs[4] = arrayToArray(tmp_data.data(), 1, tmp_data.size());

    mexCallMATLAB(NRLS, &plhs, NRHS, prhs, m_functionName.c_str());

    for (int i = 0; i < NRHS; i++)
        mxDestroyArray(prhs[i]);

    return createGaussianMessage(plhs);;
}


