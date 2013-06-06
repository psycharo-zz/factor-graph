#ifndef DIRICHLET_H
#define DIRICHLET_H


#include <vector>
#include <functional>
#include <numeric>
#include <stdexcept>
using namespace std;


#include <variable.h>
#include <mathutil.h>


namespace vmp
{
// TODO: provide dimensionality as a template parameter?
class Dirichlet;

template<>
class Parameters<Dirichlet>
{
public:
    Parameters(size_t _dims):
        U(_dims)
    {}

    Parameters(const vector<double> &_U):
        U(_U)
    {}

    Parameters &operator+=(const Parameters &other)
    {
        U += other.U;
        return *this;
    }

    //! the concentration parameter
    vector<double> U;
};


template<>
class Moments<Dirichlet>
{
public:
    Moments(size_t _dims):
        logProb(_dims)
    {}

    //! log-probabilities
    vector<double> logProb;
};



/**
 * dirichlet-distributed simplex
 */
class Dirichlet : public Variable,
                  public HasForm<Dirichlet>

{
public:
    /**
     * @param _dims the dimensionality
     */
    Dirichlet(const vector<double> &u):
        HasForm(Parameters<Dirichlet>(u)),
        m_priorU(u),
        m_dims(u.size())
    {}

    //! get the number of dimensions
    inline size_t dims() const { return m_dims; }

    //! override Variable. TODO: in current implementation cannot have a
    virtual bool hasParents() const { return false; }

    //! override Variable. compute the constitute to the lower bound on the log-evidence
    virtual double logEvidenceLowerBound() const
    {
        throw std::runtime_error("Dirichlet::logEvidenceLowerBound(): not implemented");
    }

    //! get the moments TODO: the same as message to children
    virtual Moments<Dirichlet> moments() const
    {
        Moments<Dirichlet> result(m_dims);
        // lambda functions/matrices?
        double dgSumU = digamma(sumv(m_params.U));
        for (size_t i = 0; i < m_dims; ++i)
            result.logProb[i] = digamma(m_params.U[i]) - dgSumU;
        return result;
    }

    //! get the parameters
    virtual Parameters<Dirichlet> parametersFromParents() const
    {
        return Parameters<Dirichlet>(m_priorU);
    }
private:
    //! the prior parameter vector
    const vector<double> m_priorU;

    //! dimensionality
    size_t m_dims;
};

}


#endif // DIRICHLET_H
