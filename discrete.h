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

    inline size_t dims() const { return logProb.size(); }

    //! log-probabilities of the discrete choices
    vector<double> logProb;
};

inline ostream &operator<<(ostream &out, const Parameters<Discrete> &params)
{
    out << "Discrete(" << params.logProb << ")";
    return out;
}

typedef Parameters<Discrete> DiscreteParameters;



/**
 * stores expectation of the natural statistics vector of
 * the discrete distribution
 */
template<>
class Moments<Discrete>
{
public:
    Moments()
    {}

    Moments(const Parameters<Discrete> &_params):
        probs(_params.dims(), 1.0 / _params.dims())
    {
        fromParameters(*this, _params);
    }

    Moments(const vector<double> &_p):
        probs(_p)
    {}

    // TODO: unnormalized distribution, not cool
    Moments(size_t _size):
        probs(_size, 1.0 / _size)
    {}

    inline size_t dims() const { return probs.size(); }

    //! probabilities of the discrete choices
    vector<double> probs;

    //! needed for speed
    static void fromParameters(Moments<Discrete> &moments, const Parameters<Discrete> &params)
    {
        for (size_t i = 0; i < moments.probs.size(); ++i)
            moments.probs[i] = exp(params.logProb[i]);
    }

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
    Discrete(const vector<double> &_logProbs):
        Variable(Parameters<Discrete>(_logProbs)),
        m_parent(new ConstDirichlet(_logProbs))
    {
        updatePosterior();
    }

    Discrete(Dirichlet *parent):
        Variable(Parameters<Discrete>(parent->dims())),
        m_parent(parent)
    {
        updatePosterior();
    }

    virtual ~Discrete() {}


    //! override Variable
    inline void updatePosterior()
    {
        Variable<Discrete>::updatePosterior();
        m_params.logProb -= lognorm(m_params.logProb);
        updateMoments();
    }


    //! set a fixed value
    inline void observe(size_t value)
    {
        if (!validate(value))
            throw std::runtime_error("DiscreteVariable::observe(size_t): the value out of bounds");
        m_observed = true;
        m_moments.probs.assign(dims(), 0);
        m_moments.probs[value] = 1.0;
    }

    //! draw a random integer sample
    inline size_t sample() const { return rand() % dims(); }

    //! get the dimensionality
    inline size_t dims() const { return m_moments.probs.size(); }

    //! override Variable
    virtual void updateMoments()
    {
        Discrete::updateMoments(m_moments, parameters());
    }

    //! override Variable
    inline bool hasParents() const { return m_parent != NULL; }

    //! override Variable
    inline double logNormalization() const { return 0.0; }
    // TODO: return lognorm(parameters.logProb);

    //! override Variable
    inline double logNormalizationParents() const { return 0.0; }

    //! override HasForm<Discrete>
    inline Parameters<Discrete> parametersFromParents() const
    {
        // TODO: check correctness
        return m_parent->moments().logProb;
    }

    inline void messageToParent(Parameters<Dirichlet> *params) const
    {
        params->U = moments().probs;
    }

    double logProbabilityDensity(const TMoments &/*moments*/) const { throw NotImplementedException; }

    // static versions
    inline static Parameters<Discrete> parametersFromParents(const Moments<Dirichlet> &dirMsg)
    {
        return dirMsg.logProb;
    }

    inline static void updateMoments(Moments<Discrete> &moments, const Parameters<Discrete> &params)
    {
        for (size_t i = 0; i < moments.probs.size(); ++i)
            moments.probs[i] = exp(params.logProb[i]);
    }


    inline static double logNormalization(const TParameters &params) { return 0.0; }

    //! override Variable
    inline static double logNormalizationParents(const Moments<Dirichlet> &moments) { return 0.0; }

protected:
    //! check whether the value is within the limits TODO: rename the function
    inline bool validate(size_t _value) { return _value < dims(); }

    //! the (only) dirichlet parent
    Dirichlet *m_parent;

    //! the message from its dirichlet parent
//    Moments<Dirichlet> m_parentMsg;
};






} // namespace vmp


#endif // DISCRETE_H
