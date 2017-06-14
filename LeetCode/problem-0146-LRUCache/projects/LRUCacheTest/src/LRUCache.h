
#ifndef LEETCODE_LRUCACHE_H
#define LEETCODE_LRUCACHE_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include <stdint.h>

#include "HashTable.h"
#include "LinkedList.h"

namespace LeetCode {

template <typename KeyT, typename ValueT, ValueT kFailedValue = -1>
class LRUCache {
public:
    typedef KeyT key_type;
    typedef ValueT value_type;

    typedef LRUItem<key_type, value_type> item_type;
    typedef LRUHashTable<key_type, value_type> hashtable_type;
    typedef FixedDoubleLinkedList<key_type, value_type> linkedlist_type;

private:
    size_t size_;
    size_t capacity_;

    hashtable_type cache_;
    linkedlist_type list_;

public:
    LRUCache() : size_(0), capacity_(0), list_() {
        //
    }

    LRUCache(size_t capacity) : size_(0), capacity_(capacity), list_(capacity) {
        //
    }

    void insert(key_type key, value_type value) {
        //
    }

    void touch(key_type key, value_type value) {
        //
    }

    void touch(item_type * item) {
        //
    }

    void touch(item_type * item, value_type value) {
        //
    }

    value_type get(key_type key) {
        item_type * item = cache_.find(key);
        if (item != nullptr) {
            touch(item);
            return item->value;
        }
        return kFailedValue;
    }

    void put(key_type key, value_type value) {
        item_type * item = cache_.find(key);
        if (item != nullptr) {
            touch(item, value);
        }
        else {
            if (size_ >= capacity_) {
                touch(key, value);
            }
            else {
                insert(key, value);
            }
        }
    }
};

} // namespace LeetCode

#endif // LEETCODE_LRUCACHE_H
