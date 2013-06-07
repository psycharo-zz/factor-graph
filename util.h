#ifndef UTIL_H
#define UTIL_H


#include <map>

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



}

#endif // UTIL_H
