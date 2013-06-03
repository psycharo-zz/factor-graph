#ifndef GAUSSIAN_H
#define GAUSSIAN_H


#include <vector>
#include <cmath>


using namespace std;

#include <factor.h>
#include <variable.h>
#include <variablearray.h>

namespace vmp
{



class GaussianMessage
{
public:

    GaussianMessage(double mp, double p):
        meanPrec(mp),
        precision(p)
    {}

    double meanPrec;
    double precision;
};






/**
 * @brief The GaussianMoments expectation of the natural statistics vector
 * Typically this is to be used to propagate updates from parents to the
 * children.
 */
class GaussianMoments
{
public:
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
    Gaussian(Variable *mean, Variable *precision):
        m_mean(mean),
        m_precision(precision)
    {}

    Gaussian(double mean, double precision):
        m_mean(NULL),
        m_precision(NULL)
    {
        m_meanParent[0] = mean;
        m_meanParent[1] = mean*mean;

        m_precParent[0] = precision;
        m_precParent[1] = log(precision);
    }


    //! check if there are any non-constant parents
    bool hasParents() const
    {
        // TODO:
        return (m_mean != NULL) && (m_precision != NULL);
    }


    //! computing the natural parameter vector from the parent nodes
    Message naturalParamsFromParents() const
    {
        if (hasParents())
        {
            throw std::runtime_error("unimplemented");
        }
        else
        {
            return Message(m_meanParent[0] * m_precParent[0], -0.5 * m_precParent[0]);
        }

    }


    //! compute moment vector according to the current distribution
    Message moments() const
    {
        return Message(0, 0);
    }


    Message messageToChild() const
    {
        // get all the messages from parents
        return Message(0, 0);
    }

    Message messageToParent(Variable *par) const
    {
        if (par == m_mean)
        {
            double expMean = 0.0;
            double expPrec = 1.0;

            return Message(expMean * expPrec, 0);
        }
        else if (par == m_precision)
        {

            return Message(0, 0);
        }
        else
            throw std::runtime_error("Gaussian::messageToParent()");

    }

    //! updating the posterior
    void update()
    {

    }

    virtual ~Gaussian() {}
protected:
    //! this one should be gaussian
    Variable *m_mean;
    //! this one should be gamma or wishart
    Variable *m_precision;

    //! current expectations of
    double m_expMean;
    double m_expMean2;

    //! current values received from parents
    double m_meanParent[2];
    double m_precParent[2];

};


///**
// * @brief The Gaussian factor - corresponds to multivariate gaussian distribution
// */
//class Gaussian : public Factor
//{
//public:
//    static Variable *createVariable(double mean, double variance)
//    {
//        return new GaussianVariable(mean, 1.0 / variance);
//    }


//    static Variable *createVariable(Variable *mean, Variable *variance)
//    {
//        return NULL;
//    }


//    static VariableArray *createArray(Variable *mean, Variable *variance)
//    {
//        return NULL;
//    }

//    // TODO: make a separate function for fixed variance/mean?
//    static VariableArray *createArray(VariableArray *mean, VariableArray *variance)
//    {
//        return NULL;
//    }


//    static Message messageToChild(double mean, double variance)
//    {
//        double u[2];
//        u[0] = mean;
//        u[1] = mean * mean + variance;
//        return Message();
//    }

//    static Message messageToParent(const Message &msgChild, const Message &msgPar, Variable *parent)
//    {
//        // depending on which parent it is,


//        return Message();
//    }
//};





}


#endif // GAUSSIAN_H
