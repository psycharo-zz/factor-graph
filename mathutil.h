#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cmath>
#include <vector>
#include <map>

#include <cfloat>
#include <random>
#include <numeric>
#include <functional>
#include <algorithm>
using namespace std;


// x^2
template <typename T>
inline T sqr(T x) { return x*x; }


// TODO: matlab support?
#ifdef GSL

inline double digamma(double x)
{
    return gsl_sf_psi(x);
}

inline double lngamma(double x)
{
    return gsl_sf_lngamma(x);
}


#else
double Rand(void);
double RandN(void);
double GammaRand(double a);
double BetaRand(double a, double b);
int BinoRand(double p, int n);

void ResetSeed(void);
void SetSeed(long new_ix, long new_iy, long new_iz);
void GetSeed(long *ix_out, long *iy_out, long *iz_out);

//! sum in log domain, 1-st order taylor expansion
double logSum(double a, double b);

//! compute the jacobian \returns a pair(dg/ds, dg/dn)
inline pair<double,double> logSumJacobian(double s, double n)
{
    double tmp = exp(n - s);
    tmp /= (1 + tmp);
    return make_pair(1 - tmp, tmp);
}



double pochhammer(double x, int n);
double di_pochhammer(double x, int n);
double tri_pochhammer(double x, int n);
double gammaln2(double x, double d);
double gammaln(double x);
double digamma(double x);
double trigamma(double x);
double tetragamma(double x);

#endif


namespace vmp {



//! random vector of integers i in [0;max)
inline vector<size_t> randomv(size_t _size, size_t _max)
{
    vector<size_t> result(_size, 0);
    for (size_t i = 0; i < _size; ++i)
        result[i] = i % _max;
    return result;
}

//! vector digamma
inline vector<double> digammav(const vector<double> &v)
{
    vector<double> result(v.size(), 0);
    transform(v.begin(), v.end(), result.begin(), digamma);
    return result;
}




inline vector<double> gammalnv(const vector<double> &v)
{
    vector<double> result(v.size(), 0);
    transform(v.begin(), v.end(), result.begin(), gammaln);
    return result;
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


inline vector<double> logv(const vector<double> &v)
{
    vector<double> result(v);
    for (size_t i = 0; i < result.size(); i++)
        result[i] = log(result[i]);
    return result;
}



//! normalize the vector to sum up to one
inline void normalize(vector<double> &v)
{
    double norm = sumv(v);
    for (vector<double>::iterator it = v.begin(); it != v.end(); ++it)
        *it /= norm;
}

//! get the normalization factor (summand) for the vector in log-domain
inline double lognorm(const vector<double> &v)
{
    double result = 0.0;
    for (vector<double>::const_iterator it = v.begin(); it != v.end(); ++it)
        result += exp(*it);
    return log(result);
}


template <typename T>
inline vector<T> &operator+=(vector<T> &a, const vector<T> &other)
{
    transform(a.begin(), a.end(), other.begin(), a.begin(), std::plus<double>());
    return a;
}


template <typename T>
inline vector<T> &operator-=(vector<T> &a, const vector<T> &other)
{
    transform(a.begin(), a.end(), other.begin(), a.begin(), std::minus<double>());
    return a;
}


template <typename T>
inline vector<T> &operator+=(vector<T> &v, T val)
{
    for (vector<double>::iterator it = v.begin(); it != v.end(); ++it)
        *it += val;
    return v;
}

template <typename T>
inline vector<T> &operator-=(vector<T> &v, T val)
{
    for (vector<double>::iterator it = v.begin(); it != v.end(); ++it)
        *it -= val;
    return v;
}


template <typename T>
inline vector<T> &operator*=(vector<T> &v, T val)
{
    for (vector<double>::iterator it = v.begin(); it != v.end(); ++it)
        *it *= val;
    return v;
}

template <typename T>
inline vector<T> &operator/=(vector<T> &v, T val)
{
    for (vector<double>::iterator it = v.begin(); it != v.end(); ++it)
        *it /= val;
    return v;
}


template <typename T>
inline vector<T> operator+(const vector<T> &a, const vector<T> &b)
{
    vector<T> result(a);
    result += b;
    return result;
}

template <typename T>
inline vector<T> operator-(const vector<T> &a, const vector<T> &b)
{
    vector<T> result(a);
    result -= b;
    return result;
}

template <typename T>
inline vector<T> operator+(const vector<T> &a, T b)
{
    vector<T> result(a);
    for (size_t i = 0; i < result.size(); ++i)
        result[i] = a[i] + b;
    return result;
}

template <typename T>
inline vector<T> operator-(const vector<T> &a, T b)
{
    vector<T> result(a);
    for (size_t i = 0; i < result.size(); ++i)
        result[i] = a[i] - b;
    return result;
}



// ln(2*PI)
const double LN_2PI = 1.837877066409345;


// EPSILON
const double EPSILON = 1e-3;


// initial value for the lower bound
const double LB_INIT = -DBL_MAX;

}



#endif // MATHUTIL_H
