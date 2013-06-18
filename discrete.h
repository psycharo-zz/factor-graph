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

inline ostream &operator<<(ostream &out, const Parameters<Discrete> &params)
{
    out << "Discrete(" << params.logProb << ")";
    return out;
}

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

    // TODO: unnormalized distribution, not cool
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
    assert(params.logProb.size() == moments.probs.size());
    return std::inner_product(params.logProb.begin(), params.logProb.end(), moments.probs.begin(), 0.0);
}





/**
 * variable only taking integer values
 * TODO: template-based verification
 */
class Discrete : public Variable<Discrete>,
                 public HasParent<Dirichlet>
{
public:
    Discrete(const vector<double> &logProbs):
        Variable(Parameters<Discrete>(logProbs)),
        m_parent(NULL),
        m_dims(logProbs.size()),
        m_parentMsg(logProbs)
    {
            // TODO: finish
//            throw NotImplementedException;
    }

    Discrete(Dirichlet *parent):
        Variable(Parameters<Discrete>(parent->dims())),
        m_parent(parent),
        m_dims(parent->dims()),
        m_parentMsg(Moments<Dirichlet>(m_dims))
    {}


    virtual ~Discrete() {}

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
    inline virtual Parameters<Discrete> parameters() const
    {
        Parameters<Discrete> result = Variable::parameters();
        result.logProb -= lognorm(result.logProb);
        return result;
    }

    //! override Variable
    double logNormalization() const
    {
        return vmp::lognorm(parameters().logProb);
    }

    //! override Variable
    double logNormalizationParents() const { return 0.0; }

    //! override Variable
    Moments<Discrete> moments() const
    {
        if (isObserved())
        {
            vector<double> probs(dims(), 0);
            probs[m_value] = 1.0;
            return Moments<Discrete>(probs);
        }
        else
            // TODO: update these quantities when updating the posterior
            return Moments<Discrete>(expv(parameters().logProb - lognorm(parameters().logProb)));
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
        assert(hasParents() && parent == m_parent);
        m_parentMsg = msg;
    }

    //! override HasParent<Dirichlet>
    Parameters<Dirichlet> messageToParent(Dirichlet *parent) const
    {
        assert(hasParents() && parent == m_parent);
        // TODO: isn't it weird that this messages are computed ?FROM MOMENTS?
        return Parameters<Dirichlet>(moments().probs);
    }


protected:
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
