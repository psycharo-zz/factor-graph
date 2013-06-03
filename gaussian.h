#ifndef GAUSSIAN_H
#define GAUSSIAN_H

#include <vector>
#include <map>

using namespace std;

#include <variable.h>
#include <mathutil.h>
#include <gamma.h>

namespace vmp
{

/**
 * @brief The GaussianParameters - represents parameters of a univariate
 * Gaussian distribution.
 * Typically used to propagate updates from children
 */
class GaussianParameters : public Parameters
{
public:
    // TODO: this constructor should not be necessary?
    GaussianParameters()
    {}

    GaussianParameters(double mp, double p):
        meanPrecision(mp),
        precision(p)
    {}

    double meanPrecision;
    double precision;
};



/**
 * @brief The GaussianMoments - stores expectation of the natural statistics vector
 * Typically used to propagate updates from parents to the children.
 */
class GaussianMoments : public Moments
{
public:
    // TODO: is this constructor necessary?
    GaussianMoments()
    {}

    GaussianMoments(double _mean, double _mean2):
        mean(_mean),
        mean2(_mean2)
    {}

    double mean;
    double mean2;
};



// TODO: this can be moved to the factor so that network manages all this
class Gaussian: public Variable
{
public:

    Gaussian(double _mean, double _prec):
        m_meanPar(NULL),
        m_precPar(NULL),
        m_meanMsg(_mean, sqr(_mean)),
        m_precMsg(_prec, log(_prec))
    {}

    Gaussian(Variable *_meanParent, Variable *_precParent):
        m_meanPar(_meanParent),
        m_precPar(_precParent)
    {
//        cout << id() << "Gaussian(" << _meanParent << "," << _precParent << ")" << endl;
    }


    virtual ~Gaussian() {}


    //! check if there are any non-constant parents
    bool hasParents() const
    {
        // TODO: maybe introduce a separate flag variable or even ask
        // whether a particular parent is present or not
        return (m_meanPar != NULL) && (m_precPar != NULL);
    }

    //! <u> = [<mean>, <mean>^2 + 1./<precision>], expectations are based on the current
    Moments *messageToChildren() const
    {
        return new GaussianMoments(moments());
    }


    Parameters *messageToParent(Variable *parent) const
    {
        GaussianMoments u = moments();

        // <phi> = []
        if (parent == m_meanPar)
        {
            double meanPrec = u.mean * m_precMsg.precision;
            double prec = m_precMsg.precision;
            // TODO: check the sign here
            return new GaussianParameters(meanPrec, prec);
        }
        else if (parent == m_precPar)
        {
            // for array it would look as follows
            // a'_m = a_m + 1/2 * \sum_i q_{im}
            // b'_, = b + 1/2 * \sum_i q_{im} (x_i^2 - 2x_i <mu_m> + <mu2_m>)
            // TODO: check the sign here
            double shape = 0.5;
            double rate = 0.5 * (u.mean2 - 2 * u.mean * m_meanMsg.mean + m_meanMsg.mean2);
            return new GammaParameters(shape, rate);
        }

        throw std::runtime_error("Gaussian::messageToParent(Variable*): unknown parent");
    }

    //! updating the posterior w.r.t to the current messages
    void updatePosterior()
    {
        // for all the children,
        m_params.meanPrecision = m_meanMsg.mean * m_precMsg.precision;
        m_params.precision = m_precMsg.precision;

        for (map<size_t, GaussianParameters>::iterator it = m_childMsgs.begin();
                                                       it != m_childMsgs.end(); ++it)
        {
            const GaussianParameters &msg = it->second;
            m_params.meanPrecision += msg.meanPrecision;
            m_params.precision += msg.precision;
        }
    }

    //! compute the constitute to the lower bound on the log-evidence
    double logEvidenceLowerBound() const
    {
        throw std::runtime_error("Gamma::logEvidenceLowerBound(): not implemented");
    }


    //! receive a message from parents
    void receiveFromParent(Moments *msg, Variable *parent)
    {
        if (parent == m_meanPar)
            m_meanMsg = *static_cast<GaussianMoments*>(msg);
        else if (parent == m_precPar)
            m_precMsg = *static_cast<GammaMoments*>(msg);
        else
            throw std::runtime_error("Gaussian::receiveFromParent(Moments*,Variable*): unknown parent");
    }

    //! receive a message from child
    void receiveFromChild(Parameters *params, Variable *child)
    {
        GaussianParameters msg = *static_cast<GaussianParameters*>(params);
        pair<map<size_t, GaussianParameters>::iterator, bool> res = m_childMsgs.insert(std::make_pair(child->id(), msg));
        if (!res.second)
            res.first->second = msg;
    }


    //! get the current posterior parameters
    inline const GaussianParameters &parameters() const { return m_params; }

    //! computing the moments w.r.t. current posterior
    GaussianMoments moments() const
    {
        if (isObserved())
            return GaussianMoments(m_value, sqr(m_value));
        else
        {
            double mean = m_params.meanPrecision / m_params.precision;
            double var = 1.0 / m_params.precision;
            return GaussianMoments(mean, sqr(mean) + var);
        }
    }


protected:

    //! computing the natural parameter vector from the parent nodes
    GaussianParameters paramsFromParents() const
    {
        return GaussianParameters(m_meanMsg.mean * m_precMsg.precision,
                                  m_precMsg.precision);
    }

    //! current parameters of the approximate posterior
    GaussianParameters m_params;

    // parents
    Variable *m_meanPar;
    Variable *m_precPar;

    // current messages received from both parents
    GaussianMoments m_meanMsg;
    GammaMoments m_precMsg;

    // message received for each children
    map<size_t, GaussianParameters> m_childMsgs;
};



}


#endif // GAUSSIAN_H
