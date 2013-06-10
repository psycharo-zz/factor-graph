#ifndef DISCRETE_H
#define DISCRETE_H

#include <variable.h>
#include <dirichlet.h>


#include <vector>
#include <stdexcept>
using namespace std;

namespace vmp
{

class Discrete;


/**
 * stores natural parameters of discrete distribution
 */
template<>
class Parameters<Discrete>
{
public:
    Parameters(size_t _dims):
        logProb(_dims, log(1./_dims))
    {
    }

    Parameters(const vector<double> &_logProb):
        logProb(_logProb)
    {}

    Parameters &operator+=(const Parameters &other)
    {
        logProb += other.logProb;
        return *this;
    }

    //! log-probabilities of the discrete choices
    vector<double> logProb;
};

typedef Parameters<Discrete> DiscreteParameters;


inline Parameters<Discrete> operator+(const Parameters<Discrete> &a,
                                      const Parameters<Discrete> &b)
{
    return Parameters<Discrete>(a.logProb + b.logProb);
}


inline Parameters<Discrete> operator-(const Parameters<Discrete> &a,
                                      const Parameters<Discrete> &b)
{
    return Parameters<Discrete>(a.logProb - b.logProb);
}



/**
 * stores expectation of the natural statistics vector of
 * the discrete distribution
 */
template<>
class Moments<Discrete>
{
public:
    Moments(const vector<double> &_p):
        probs(_p)
    {}

    Moments(size_t _size):
        probs(_size)
    {}

    //! probabilities of the discrete choices
    vector<double> probs;
};

typedef Moments<Discrete> DiscreteMoments;


// dot product
inline double operator*(const Parameters<Discrete> &params,
                        const Moments<Discrete> &moments)
{
    throw std::runtime_error("double operator*(const Parameters<Discrete> &,const Moments<Discrete> &): not implemented");
}








/**
 * variable only taking integer values
 * TODO: template-based verification
 */
class Discrete : public Variable<Discrete>,
                 public HasParent<Dirichlet>
{
public:
    Discrete(Dirichlet *parent):
        Variable(Parameters<Discrete>(parent->dims())),
        m_parent(parent),
        m_dims(parent->dims()),
        m_parentMsg(Moments<Dirichlet>(m_dims))
    {}

    //! get the dimensionality
    inline size_t dims() const { return m_dims; }

    //! set a fixed value
    inline void observe(size_t _value)
    {
        if (!validate(_value))
            throw std::runtime_error("DiscreteVariable::observe(size_t): the value out of bounds");
        m_observed = true;
        m_value = _value;
    }

    //! draw a random integer sample
    inline size_t sample() const { return rand() % m_dims; }

    //! override Variable
    inline bool hasParents() const { return m_parent != NULL; }

    //! override Variable
    double logNormalization() const
    {
        throw std::runtime_error("Discrete::logNormalization(): not implemented");
    }

    //! override Variable
    double logNormalizationParents() const
    {
        throw std::runtime_error("Discrete::logNormalization(): not implemented");
    }



    //! override HasForm<Discrete>
    Moments<Discrete> moments() const
    {
        if (isObserved())
        {
            vector<double> probs(dims(), 0);
            probs[m_value] = 1.0;
            return Moments<Discrete>(probs);
        }
        else
            return Moments<Discrete>(expv(m_params.logProb));
    }

    //! override HasForm<Discrete>
    Parameters<Discrete> parametersFromParents() const
    {
        // TODO: check correctness
        return Parameters<Discrete>(m_parentMsg.logProb);
    }

    //! override HasParent<Dirichlet>
    void receiveFromParent(const Moments<Dirichlet> &msg, Dirichlet *parent)
    {
        assert(parent == m_parent);
        m_parentMsg = msg;
    }

    //! override HasParent<Dirichlet>
    Parameters<Dirichlet> messageToParent(Dirichlet */*parent*/) const
    {
        // TODO: isn't it weird that this messages are computed ?FROM MOMENTS?
        return Parameters<Dirichlet>(moments().probs);
    }


    //! override HasForm<Discrete>
    virtual void updatePosterior()
    {
        m_observed = false;
        Variable<Discrete>::updatePosterior();
        double norm = sumv(expv(m_params.logProb));
        m_params.logProb = m_params.logProb - log(norm);
    }


protected:
    //! override Variable
    virtual double logEvidenceLowerBoundObserved() const
    {
        throw std::runtime_error("Discrete::logEvidenceLowerBoundObserved(): not implemented");
    }

    //! override Variable
    virtual double logEvidenceLowerBoundHidden() const
    {
        throw std::runtime_error("Discrete::logEvidenceLowerBoundHidden(): not implemented");
    }



    //! check whether the value is within the limits TODO: rename the function
    inline bool validate(size_t _value) { return _value < m_dims; }

    //! the (only) dirichlet parent
    Dirichlet *m_parent;

    //! the maximum allowed number of choices
    size_t m_dims;

    //! the message from its dirichlet parent
    Moments<Dirichlet> m_parentMsg;

    //! stored value in case it is observed
    size_t m_value;

};


}


#endif // DISCRETE_H
