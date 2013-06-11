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


inline Parameters<Dirichlet> operator+(const Parameters<Dirichlet> &a,
                                       const Parameters<Dirichlet> &b)
{
    return Parameters<Dirichlet>(a.U + b.U);
}

inline Parameters<Dirichlet> operator-(const Parameters<Dirichlet> &a,
                                       const Parameters<Dirichlet> &b)
{
    return Parameters<Dirichlet>(a.U - b.U);
}


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


// dot product
inline double operator*(const Parameters<Dirichlet> &params,
                        const Moments<Dirichlet> &moments)
{
    assert(params.U.size() == moments.logProb.size());
    double result = 0;
    for (size_t i = 0; i < params.U.size(); ++i)
        result += (params.U[i] - 1) * moments.logProb[i];
    return result;
}



/**
 * dirichlet-distributed simplex
 */
class Dirichlet : public Variable<Dirichlet>
{
public:
    /**
     * @param _dims the dimensionality
     */
    Dirichlet(const vector<double> &u):
        Variable(Parameters<Dirichlet>(u)),
        m_priorU(u),
        m_dims(u.size())
    {}

    //! get the number of dimensions
    inline size_t dims() const { return m_dims; }

    //! override Variable. TODO: in current implementation cannot have a
    inline bool hasParents() const { return false; }

    //! override Variable
    inline double logNormalization() const
    {
        return lngamma(sumv(parameters().U)) - sumv(lngammav(parameters().U));
    }

    //! override Variable
    inline double logNormalizationParents() const
    {
        Parameters<Dirichlet> params = parametersFromParents();
        return lngamma(sumv(params.U)) - sumv(lngammav(params.U));
    }


    //! override HasForm<Dirichlet>
    inline Moments<Dirichlet> moments() const
    {
        Moments<Dirichlet> result(dims());
        // lambda functions/matrices?
        double dgSumU = digamma(sumv(parameters().U));
        for (size_t i = 0; i < dims(); ++i)
            result.logProb[i] = digamma(parameters().U[i]) - dgSumU;
        // TODO: is normalization required here?
        return result;
    }

    //! override HasForm<Dirichlet>
    inline Parameters<Dirichlet> parametersFromParents() const
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
