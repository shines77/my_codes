
#ifndef LEETCODE_LRUITEM_H
#define LEETCODE_LRUITEM_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include <stdint.h>

#if defined(_MSC_VER)
  #ifndef likely
    #define likely(expr)    (expr)
  #endif
  #ifndef unlikely
    #define unlikely(expr)  (expr)
  #endif
#else
  #ifndef likely
    #define likely(expr)    __builtin_expect(!!(expr), 1)
  #endif
  #ifndef unlikely
    #define unlikely(expr)  __builtin_expect(!!(expr), 0)
  #endif
#endif

#ifndef noif
  #define noif(expr)    ((void)0);
#endif

namespace LeetCode {

struct LRUKey {
    enum  {
        EmptyKey = -2,
        UnusedKey = -1,
        ValidKey = 0
    };
};

struct LRUValue {
    enum  {
        FailedValue = -1
    };
};

template <typename KeyT, typename ValueT>
struct LRUItem {
    typedef KeyT key_type;
    typedef ValueT value_type;
    typedef LRUItem<key_type, value_type> this_type;

    key_type    key;
    value_type  value;
    this_type * prev;
    this_type * next;
};

} // namespace LeetCode

#endif // LEETCODE_LRUITEM_H
