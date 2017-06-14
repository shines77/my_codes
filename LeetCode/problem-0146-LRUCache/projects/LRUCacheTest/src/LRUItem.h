
#ifndef LEETCODE_LRUITEM_H
#define LEETCODE_LRUITEM_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include <stdint.h>

namespace LeetCode {

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
