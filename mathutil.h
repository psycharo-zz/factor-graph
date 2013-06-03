#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cmath>
#include <gsl/gsl_sf_psi.h>

namespace vmp
{

// x^2
template <typename T>
inline T sqr(T x) { return x*x; }


// TODO: matlab support?

// scalar digamma function d(ln(Gamma(x)))/dx
inline double digamma(double x)
{
    return gsl_sf_psi(x);
}




}

#endif // MATHUTIL_H
