
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

    static const size_t kCapacity;

private:
    size_t capacity_;
    linkedlist_type list_;
    hashtable_type cache_;

public:
    LRUCache() : capacity_(kCapacity), list_(kCapacity), cache_(kCapacity) {
        //
    }

    LRUCache(size_t capacity) : capacity_(capacity), list_(capacity), cache_(capacity) {
        //
    }

    size_t sizes() const { return list_.sizes(); }
    size_t capacity() const { return list_.capacity(); }

    void insert(key_type key, value_type value) {
        item_type * new_item = list_.insert_fast(key, value);
        if (new_item) {
            cache_.insert(key, new_item);
        }
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
            if (list_.sizes() >= capacity_) {
                touch(key, value);
            }
            else {
                insert(key, value);
            }
        }
    }

    void print() {
        list_.print();
    }
};

} // namespace LeetCode

#endif // LEETCODE_LRUCACHE_H
