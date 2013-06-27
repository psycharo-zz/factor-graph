#include <gaussian.h>

using namespace vmp;

Gaussian::Gaussian(double _mean, double _prec):
    Variable(TParameters(_mean * _prec, _prec)),
    m_meanParent(new ConstGaussian(_mean)),
    m_precParent(new ConstGamma(_prec)),
    m_hasParents(true)
{
}
