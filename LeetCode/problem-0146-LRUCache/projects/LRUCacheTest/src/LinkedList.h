
#ifndef LEETCODE_LINKEDLIST_H
#define LEETCODE_LINKEDLIST_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include <stdint.h>
#include <memory.h>

#include "LRUItem.h"

namespace LeetCode {

template <typename KeyT, typename ValueT>
class FixedDoubleLinkedList {
public:
    typedef KeyT key_type;
    typedef ValueT value_type;

private:
    size_t size_;
    size_t capacity_;

public:
    FixedDoubleLinkedList() : size_(0), capacity_(0) {
        init();
    }
    FixedDoubleLinkedList(size_t capacity) : size_(0), capacity_(capacity) {
        init();
    }
    ~FixedDoubleLinkedList() {}

protected:
    void init() {
        //
    }
};

} // namespace LeetCode

#endif // LEETCODE_LINKEDLIST_H
