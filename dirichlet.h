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
    Parameters(size_t _dims, double value = 1):
        U(_dims)
    {
        U.fill(value);
    }

    Parameters(const vec &_U):
        U(_U)
    {}

    Parameters &operator+=(const Parameters &other)
    {
        U += other.U;
        return *this;
    }

    inline void clear() { throw NotImplementedException; }

    inline size_t dims() const { return U.size(); }

    //! the concentration parameter
    vec U;
};


inline ostream &operator<<(ostream &out, const Parameters<Dirichlet> &params)
{
    out << "Dirichlet(" << params.U.t() << ")";
    return out;
}



template<>
class Moments<Dirichlet>
{
public:
    Moments()
    {}

    Moments(const Parameters<Dirichlet> &_params):
        logProb(_params.dims())
    {
        fromParameters(*this, _params);
    }

    Moments(const vec _logProb):
        logProb(_logProb)
    {}

    Moments(size_t _dims):
        logProb(_dims)
    {}

    inline size_t dims() const { return logProb.size(); }

    static void fromParameters(Moments &moments, const Parameters<Dirichlet> &params)
    {
        double dgSumU = digamma(sum(params.U));
        for (size_t i = 0; i < moments.dims(); ++i)
            moments.logProb[i] = digamma(params.U[i]) - dgSumU;
    }

    //! log-probabilities TODO: substitute this with vector?
    vec logProb;
};

inline ostream &operator<<(ostream &out, const Moments<Dirichlet> &ms)
{
    out << "Dirichlet(" << ms.logProb.t() << ")";
    return out;
}



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
    Dirichlet(const TParameters &_params):
        Variable(_params), // TODO: decent initialisation
        m_priorU(_params.U)
    {}

    Dirichlet(size_t _dims, const double value):
        Variable(TParameters(_dims, value)),
        m_priorU(_dims)
    {
        m_priorU.fill(value);
    }

    virtual ~Dirichlet() {}

    void updatePosterior()
    {
        assert(!isObserved());
        m_params = parametersFromParents();

        for (MessageIt it = m_messages.begin(); it != m_messages.end(); ++it)
            m_params += it->second;
        updateMoments();

        cout << m_params << endl;
        cout << m_moments << endl;
        cout << exp(m_moments.logProb).t() << endl;
        cout << endl;

    }

    //! get the number of dimensions
    inline size_t dims() const { return m_priorU.size(); }

    //! override Variable
    inline double logNormParents() const
    {
        Parameters<Dirichlet> params = parametersFromParents();
        return gammaln(sum(params.U)) - sum(gammalnv(params.U));
    }

    inline Parameters<Dirichlet> parametersFromParents() const { return m_priorU; }

    inline static double logNorm(const TParameters &ps)
    {
        return gammaln(sum(ps.U)) - sum(gammalnv(ps.U));
    }

    double logPDF(const TMoments &/*moments*/) const { throw NotImplementedException; }


protected:
    //! the prior parameter vector
    vec m_priorU;
};


class ConstDirichlet : public Dirichlet
{
public:
    ConstDirichlet(const vec &_logProb):
        Dirichlet(_logProb),
        m_constMoments(_logProb)
    {}

    inline const Moments<Dirichlet> &moments() const { return m_constMoments; }

private:
    Moments<Dirichlet> m_constMoments;
};


}


#endif // DIRICHLET_H
