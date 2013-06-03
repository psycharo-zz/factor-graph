#ifndef GAMMA_H
#define GAMMA_H

#include <variable.h>
#include <mathutil.h>


#include <map>
using namespace std;

namespace vmp
{

/**
 * @brief The GammaParameters - stores Gamma distribution (natural) parameters
 * Typically is used to propagate updates from children to parents
 */
class GammaParameters : public Parameters
{
public:
    GammaParameters()
    {}

    GammaParameters(double _shape, double _rate):
        shape(_shape),
        rate(_rate)
    {}

    double rate;
    double shape;
};




/**
 * @brief The GammaMoments - stores expectation of the natural statistics vector
 * for the Gamma distribution.
 * Typically is used to propagate updates from parents to children.
 *
 * Note that the expectations are nothing that numbers, so the "precision" here is
 * more-or-less artificial, simply because in the framework it only is used as a prior
 * for the gaussian precision.
 */
class GammaMoments : public Moments
{
public:
    GammaMoments()
    {}

    GammaMoments(double _precision, double _logPrecision):
        precision(_precision),
        logPrecision(_logPrecision)
    {}

    double precision;
    double logPrecision;
};




/**
 * @brief Gamma - represents variable distributed according to the
 * univariate gamma distribution.
 * TODO: add references to all the distributions (e.g. paper/wikipedia article)
 */
class Gamma: public Variable
{
public:
    Gamma(double shape, double rate):
        m_shapeMsg(shape),
        m_rateMsg(rate)
    {}


    // in our implementation gamma variable can't have non-fixed prior distributions (only consts)
    bool hasParents() const
    {
        return false;
    }

    //!
    Moments *messageToChildren() const
    {
        return new GammaMoments(moments());
    }


    //! there is no supported priors for the gamma distribution so far
    Parameters *messageToParent(Variable *parent) const
    {
        throw std::runtime_error("Gamma::messageToParent(Variable*): not supported");
    }

    //! updating the posterior w.r.t to the current messages
    // TODO: make a template-based function or a virtual one
    void updatePosterior()
    {
        m_params = GammaParameters(m_shapeMsg, m_rateMsg);
        for (map<size_t, GammaParameters>::iterator it = m_childMsgs.begin();
                                                       it != m_childMsgs.end(); ++it)
        {
            const GammaParameters &msg = it->second;
            m_params.shape += msg.shape;
            m_params.rate += msg.rate;
        }
    }

    //! compute the constitute to the lower bound on the log-evidence
    double logEvidenceLowerBound() const
    {
        throw std::runtime_error("Gamma::logEvidenceLowerBound(): not implemented");
    }

    //! unsupported for gamma
    void receiveFromParent(Moments *moments, Variable *parent)
    {
        throw std::runtime_error("Gamma::receiveFromParent(Moments*,Variable*): not supported");
    }

    //! obtain a message from one of the children
    void receiveFromChild(Parameters *params, Variable *child)
    {
        GammaParameters msg = *static_cast<GammaParameters*>(params);
        pair<map<size_t, GammaParameters>::iterator, bool> res = m_childMsgs.insert(std::make_pair(child->id(), msg));
        if (!res.second)
            res.first->second = msg;
    }

    GammaMoments moments() const
    {
        double precision = m_params.shape / m_params.rate;
        double logPrecision = digamma(m_params.shape) - log(m_params.rate);
        return GammaMoments(precision, logPrecision);
    }

private:
    //! current parameters of the approximate posterior
    GammaParameters m_params;

    // current messages received from both parents
    double m_shapeMsg;
    double m_rateMsg;

    // messages received for each children
    map<size_t, GammaParameters> m_childMsgs;

};


}



#endif // GAMMA_H
