#include "estimatemultiplicationnode.h"

#include "matrixutil.h"

#include <algorithm>
#include <functional>


using namespace std;


const char *EstimateMultiplicationNode::ESTIMATED_TAG = "estimated";


bool EstimateMultiplicationNode::isSupported(Message::Type type)
{
    return type == GaussianMessage::GAUSSIAN_VARIANCE ||
           type == GaussianMessage::GAUSSIAN_PRECISION;
}


void EstimateMultiplicationNode::receive(int from, const GaussianMessage &msg)
{
    if (!(isForward(from) || isBackward(from) || isConnection(from, ESTIMATED_TAG)))
        throw std::runtime_error("EstimateMultiplicationNode::receive: unknown connection");

    if (isConnection(from, ESTIMATED_TAG))
    {
        setMessage(from, msg);
        // filling matrix like this:
        // [ a  ]
        // [ I 0]
        m_matrix = Matrix(msg.size(), msg.size());
        for (size_t i = 1; i < msg.size2() - size(); i += (size()+1))
            m_matrix.data()[i] = 1;
        for (size_t i = 0; i < size(); i++)
            m_matrix.data()[size() * i] = msg.mean()[i];
    }
    else
        FactorNode::receive(from, msg);
}




GaussianMessage EstimateMultiplicationNode::function(int to, const MessageBox &msgs)
{
    // TODO: handle msgs.empty() separately
    if (!m_nodes.count(to) || msgs.empty())
        throw std::runtime_error("EstimateMultilpicationNode::function(): unknown recipient");
    // the dimensionality of the message
    size_t msg_size = msgs.begin()->second.size();
    if (msg_size != size())
        throw std::runtime_error("EstimateMultiplicationNode::function(): inconsistent dimensionalities");

    // forward
    if (isForward(to))
        return forwardFunction(to, msgs);
    else if (isBackward(to))
        return backwardFunction(to, msgs);
    else // if (isEstimated(to))
    {
        GaussianMessage result(size(), GaussianMessage::GAUSSIAN_PRECISION);
        const GaussianMessage &msgX = msgs.at(*m_incoming.begin());
        const GaussianMessage &msgY = msgs.at(*m_outgoing.begin());

        const Matrix &meanX = msgX.mean();

        result.mean() = meanX;

        // TODO: change to matrices
        // m_a = m_y[1] * m_x / ||m_x||^2
        double scalar = msgY.mean()(0,0) / vector_dot(msgX.mean().data(), msgX.mean().data(), msgX.size());

        matrix_scalar_mult(1, result.size(), result.mean().data(), scalar);

        double sd2 = (msgY.type() == GaussianMessage::GAUSSIAN_VARIANCE) ? msgY.variance()(0, 0) : 1.0 / msgY.precision()(0, 0);

        // W_a = m_x * m_x^T / sd_y2
        mult(meanX, meanX.T(), result.precision(), 1.0 / sd2);

        return result;
    }
}






