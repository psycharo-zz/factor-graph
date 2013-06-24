#ifndef UTIL_H
#define UTIL_H


#include <map>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cassert>
#include <tuple>
#include <memory>

// for-each for tuples
namespace std {
    template<int I, class Tuple, typename F> struct for_each_impl {
        static void for_each(const Tuple& t, F &f) {
            for_each_impl<I - 1, Tuple, F>::for_each(t, f);
            f(get<I>(t));
        }
    };
    template<class Tuple, typename F> struct for_each_impl<0, Tuple, F> {
        static void for_each(const Tuple& t, F &f) {
            f(get<0>(t));
        }
    };
    template<class Tuple, typename F>
    F for_each(const Tuple& t, F &f) {
        for_each_impl<tuple_size<Tuple>::value - 1, Tuple, F>::for_each(t, f);
        return f;
    }


    template<int I, class Tuple, typename F>
    struct for_each_impl_const
    {
        static void for_each(const Tuple& t, const F &f)
        {
            for_each_impl_const<I - 1, Tuple, F>::for_each(t, f);
            f(get<I>(t));
        }
    };
    template<class Tuple, typename F>
    struct for_each_impl_const<0, Tuple, F>
    {
        static void for_each(const Tuple& t, const F &f)
        {
            f(get<0>(t));
        }
    };

    template<class Tuple, typename F>
    F for_each(const Tuple& t, const F &f)
    {
        for_each_impl_const<tuple_size<Tuple>::value - 1, Tuple, F>::for_each(t, f);
        return f;
    }

    template<typename T, typename... Args>
    unique_ptr<T> make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}

namespace vmp
{

/**
 * insert and replace if an entry already exists
 */
template <class TKey, class TVal>
void map_insert(std::map<TKey, TVal> &values, const std::pair<TKey, TVal> &val)
{
    std::pair<typename std::map<TKey,TVal>::iterator, bool> res = values.insert(val);
    if (!res.second)
        res.first->second = val.second;
}


/**
 * print a vector
 */
template <typename T>
inline std::ostream& operator <<(std::ostream &os, const std::vector<T> &v)
{
    for (size_t i = 0; i < v.size(); i++)
        os << v[i] << "\t";
    return os;
}


// TODO: __PRETTY_FUNCTION__ only works for the GCC
#define NotImplementedException std::runtime_error(__PRETTY_FUNCTION__ + std::string(": not implemented "))



} // namespace vmp

#endif // UTIL_H
