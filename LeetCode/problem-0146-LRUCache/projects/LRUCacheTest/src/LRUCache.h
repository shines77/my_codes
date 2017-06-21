
#ifndef LEETCODE_LRUCACHE_H
#define LEETCODE_LRUCACHE_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include <stdint.h>

#include "LRUItem.h"
#include "HashTable.h"
#include "LinkedList.h"

//
// Problem title: 146. LRU Cache
// Problem URL: https://leetcode.com/problems/lru-cache/
//

namespace LeetCode {

template <typename KeyT, typename ValueT, ValueT kFailedValue = LRUValue::FailedValue>
class LRUCacheBase {
public:
    typedef KeyT key_type;
    typedef ValueT value_type;

    typedef LRUItem<key_type, value_type> item_type;
    typedef LRUHashTable<key_type, item_type *> hash_table_type;
    typedef typename hash_table_type::HashNode hash_node_type;
    typedef ContinuousDoubleLinkedList<item_type> linkedlist_type;

    static const size_t kDefaultCapacity = 32;

private:
    size_t capacity_;
    linkedlist_type list_;
    hash_table_type cache_;

public:
    LRUCacheBase() : capacity_(kDefaultCapacity), list_(kDefaultCapacity), cache_(kDefaultCapacity) {
    }

    LRUCacheBase(size_t capacity) : capacity_(capacity), list_(capacity), cache_(capacity) {
    }

    ~LRUCacheBase() {
        capacity_ = 0;
    }

    size_t sizes() const { return list_.sizes(); }
    size_t capacity() const { return list_.capacity(); }

    value_type get(key_type key) {
        hash_node_type * node = cache_.find(key);
        if (node != nullptr) {
            item_type * item = node->value;
            assert(item != nullptr);
            assert(key == item->key);
            touch(item);
            return item->value;
        }
        return kFailedValue;
    }

    void put(key_type key, value_type value) {
        hash_node_type * node = cache_.find(key);
        if (node != nullptr) {
            item_type * item = node->value;
            assert(item != nullptr);
            assert(key == item->key);
            //item->key = key;
            item->value = value;
            touch(item);
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

protected:
    void insert(key_type key, value_type value) {
        item_type * new_item = list_.insert_fast(key, value);
        if (new_item) {
            cache_.insert(key, new_item);
        }
    }

    void touch(item_type * item) {
        assert(item != nullptr);
        list_.move_to_front(item);
    }

    void touch(key_type key, value_type value) {
        // Pop the last item.
        item_type * last = list_.pop_back();
        if (last != nullptr) {
            if (key != last->key) {
                // Remove the old key from the hash table.
                cache_.remove_fast(last->key);
                // Insert the new key and value to the hash table.
                cache_.insert(key, last);
            }
            // Save the new key and value.
            last->key = key;
            last->value = value;
            // Push the last item to head again.
            list_.push_front(last);
        }
    }
};

typedef LRUCacheBase<int, int> LRUCache;

} // namespace LeetCode

#endif // LEETCODE_LRUCACHE_H
