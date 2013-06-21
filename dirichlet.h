#ifndef DIRICHLET_H
#define DIRICHLET_H


#include <vector>
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


inline ostream &operator<<(ostream &out, const Parameters<Dirichlet> &params)
{
    out << "Dirichlet(" << params.U << ")";
    return out;
}


//inline Parameters<Dirichlet> operator+(const Parameters<Dirichlet> &a,
//                                       const Parameters<Dirichlet> &b)
//{
//    return Parameters<Dirichlet>(a.U + b.U);
//}

//inline Parameters<Dirichlet> operator-(const Parameters<Dirichlet> &a,
//                                       const Parameters<Dirichlet> &b)
//{
//    return Parameters<Dirichlet>(a.U - b.U);
//}


template<>
class Moments<Dirichlet>
{
public:
    Moments()
    {}

    Moments(const vector<double> _logProb):
        logProb(_logProb)
    {}

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
        Variable(Parameters<Dirichlet>(u),
                 Moments<Dirichlet>(u.size())), // TODO: decent initialisation
        m_priorU(u),
        m_dims(u.size())
    {
        m_moments.logProb.resize(m_dims);
        updatePosterior();
    }

    virtual ~Dirichlet() {}

    //! get the number of dimensions
    inline size_t dims() const { return m_dims; }

    //! override Variable. TODO: in current implementation cannot have a
    inline bool hasParents() const { return false; }

    //! override Variable
    inline double logNormalization() const
    {
        return gammaln(sumv(parameters().U)) - sumv(gammalnv(parameters().U));
    }

    //! override Variable
    inline double logNormalizationParents() const
    {
        Parameters<Dirichlet> params = parametersFromParents();
        return gammaln(sumv(params.U)) - sumv(gammalnv(params.U));
    }


    //! override Variable
    inline void updateMoments()
    {
        // lambda functions/matrices?
        double dgSumU = digamma(sumv(parameters().U));
        for (size_t i = 0; i < dims(); ++i)
            m_moments.logProb[i] = digamma(parameters().U[i]) - dgSumU;
        // TODO: is normalization required here?
        m_moments.logProb -= lognorm(m_moments.logProb);
    }

    inline Parameters<Dirichlet> parametersFromParents() const
    {
        return m_priorU;
    }


protected:
    //! the prior parameter vector
    const vector<double> m_priorU;

    //! dimensionality
    size_t m_dims;
};


class ConstDirichlet : public Dirichlet
{
public:
    ConstDirichlet(const vector<double> &_logProb):
        Dirichlet(_logProb),
        m_constMoments(_logProb)
    {}

    inline const Moments<Dirichlet> &moments() const { return m_constMoments; }

private:
    Moments<Dirichlet> m_constMoments;
};


}


#endif // DIRICHLET_H
