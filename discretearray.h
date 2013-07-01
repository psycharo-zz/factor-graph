#ifndef DISCRETEARRAY_H
#define DISCRETEARRAY_H

#include <variable.h>
#include <discrete.h>
#include <dirichlet.h>


namespace vmp {

/**
 * the vector version. TParent can be Dirichlet or Vector<Dirichlet> ??
 */
class DiscreteArray : public VariableArray<Discrete>,
                      public HasParent<Dirichlet>
{
public:
    DiscreteArray(size_t _size, Dirichlet *_parent):
        VariableArray(_size, TBaseParameters(_parent->dims())),
        m_parent(_parent)
    {}

    //! get the number of dimensions
    inline size_t dims() const { return m_parent->dims(); }

    //! set observations
    void observe(const vector<size_t> &values)
    {
        assert(values.size() == size());
        m_observed = true;
        for (size_t i = 0; i < values.size(); ++i)
        {
            m_moments[i].probs.assign(dims(), 0);
            m_moments[i].probs[values[i]] = 1.0;
        }
    }

    //! get moment
    double weight(size_t i, size_t m) const
    {
        return m_moments[i].probs[m];
    }


    //! set initial values
    void initialize(const vector<size_t> &values)
    {
        assert(values.size() == size());
        for (size_t i = 0; i < size(); ++i)
        {
            m_moments[i].probs.assign(dims(), 0);
            m_moments[i].probs[values[i]] = 1.0;
            m_parameters[i].logProb = logv(m_moments[i].probs);
        }
    }

    //! override VariableArray
    void updatePosterior()
    {
        VariableArray<Discrete>::updatePosterior();
        for (size_t i = 0; i < size(); ++i)
            m_parameters[i].logProb -= lognorm(m_parameters[i].logProb);
        updateMoments();
    }

    double logNorm() const { return 0; }
    double logNormParents() const { return 0; }


    //! override VariableArray
    inline TBaseParameters parametersFromParents(size_t /*idx*/) const
    {
        return Discrete::parametersFromParents(m_parent->moments());
    }


    //! override HasParent<Dirichlet>
    void messageToParent(Parameters<Dirichlet> *params) const
    {
        params->U.assign(dims(), 0);
        for (size_t i = 0; i < size(); ++i)
            params->U += moments(i).probs;
    }


private:
    // dirichlet
    Dirichlet *m_parent;
};

} // namespace vmp

#endif // DISCRETEARRAY_H
