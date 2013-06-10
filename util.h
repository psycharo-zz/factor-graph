#ifndef UTIL_H
#define UTIL_H


#include <map>
#include <vector>
#include <iostream>
#include <cassert>

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



}

#endif // UTIL_H
