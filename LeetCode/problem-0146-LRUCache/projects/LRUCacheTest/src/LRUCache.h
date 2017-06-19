
#ifndef LEETCODE_LRUCACHE_H
#define LEETCODE_LRUCACHE_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include <stdint.h>

#include "LRUItem.h"
#include "HashTable.h"
#include "LinkedList.h"

namespace LeetCode {

template <typename KeyT, typename ValueT, ValueT kFailedValue = LRUValue::FailedValue>
class LRUCache {
public:
    typedef KeyT key_type;
    typedef ValueT value_type;

    typedef LRUItem<key_type, value_type> item_type;
    typedef LRUHashTable<key_type, item_type *> hash_table_type;
    typedef typename hash_table_type::HashNode hash_node_type;
    typedef FixedDoubleLinkedList<item_type> linkedlist_type;

    static const size_t kDefaultCapacity = 32;

private:
    size_t capacity_;
    linkedlist_type list_;
    hash_table_type cache_;

public:
    LRUCache() : capacity_(kDefaultCapacity), list_(kDefaultCapacity), cache_(kDefaultCapacity) {
        //
    }

    LRUCache(size_t capacity) : capacity_(capacity), list_(capacity), cache_(capacity) {
        //
    }

    size_t sizes() const { return list_.sizes(); }
    size_t capacity() const { return list_.capacity(); }

    value_type get(key_type key) {
        hash_node_type * node = cache_.find(key);
        if (node != nullptr) {
            item_type * item = node->value;
            assert(item != nullptr);
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

protected:
    void insert(key_type key, value_type value) {
        item_type * new_item = list_.insert_fast(key, value);
        if (new_item) {
            cache_.insert(key, new_item);
        }
    }

    void touch(item_type * item) {
        list_.move_to_front(item);
    }

    void touch(item_type * item, value_type value) {
        item->value = value;
        list_.move_to_front(item);
    }

    void touch(key_type key, value_type value) {
        // Pop the last item.
        item_type * last = list_.pop_back();
        if (last != nullptr) {
            // Set the new key and value.
            last->key = key;
            last->value = value;
            // Push the last item to head again.
            list_.push_front(last);
            // Insert the new key and value to hash table.
            cache_.insert(key, last);
        }
    }
};

} // namespace LeetCode

#endif // LEETCODE_LRUCACHE_H
