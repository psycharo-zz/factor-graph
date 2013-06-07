#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cmath>
#include <vector>
#include <map>

#include <numeric>
#include <functional>
#include <algorithm>
using namespace std;
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


inline double sumv(const vector<double> &v)
{
    return accumulate(v.begin(), v.end(), 0.0, std::plus<double>());
}


inline vector<double> expv(const vector<double> &v)
{
    vector<double> result(v);
    for (size_t i = 0; i < result.size(); i++)
        result[i] = exp(result[i]);
    return result;
}


template <typename T>
inline vector<T> &operator+=(vector<T> &a, const vector<T> &other)
{
    transform(a.begin(), a.end(), other.begin(), a.begin(), std::plus<double>());
    return a;
}


template <typename T>
inline vector<T> operator+(const vector<T> &a, const vector<T> &b)
{
    vector<T> result(a);
    result += b;
    return result;
}



}

#endif // MATHUTIL_H
