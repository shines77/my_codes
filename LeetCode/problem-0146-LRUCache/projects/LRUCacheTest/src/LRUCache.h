
#ifndef LEETCODE_LRUCACHE_H
#define LEETCODE_LRUCACHE_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include <stdint.h>

#include "LRUNode.h"
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
    typedef KeyT    key_type;
    typedef ValueT  value_type;

    typedef std::size_t                             size_type;
    typedef LRUNode<key_type, value_type>           node_type;
    typedef LRUHashTable<key_type, node_type *>     hash_table_type;
    typedef typename hash_table_type::HashNode      hash_node_type;
    typedef ContinuousDoubleLinkedList<node_type>   linkedlist_type;

    static const size_type kDefaultCapacity = 32;

private:
    size_type       capacity_;
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

    size_type sizes() const { return list_.sizes(); }
    size_type capacity() const { return list_.capacity(); }

    value_type get(const key_type & key) {
        hash_node_type * node = cache_.find(key);
        if (node != nullptr) {
            node_type * node = node->value;
            assert(node != nullptr);
            assert(key == node->key);
            touch(node);
            return node->value;
        }
        return kFailedValue;
    }

    void put(const key_type & key, const value_type & value) {
        hash_node_type * node = cache_.find(key);
        if (node != nullptr) {
            node_type * node = node->value;
            assert(node != nullptr);
            assert(key == node->key);
            //node->key = key;
            node->value = value;
            touch(node);
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
    void insert(const key_type & key, const value_type & value) {
        node_type * new_node = list_.insert_fast(key, value);
        if (new_node) {
            cache_.insert(key, new_node);
        }
    }

    void touch(node_type * node) {
        assert(node != nullptr);
        list_.move_to_front(node);
    }

    void touch(const key_type & key, const value_type & value) {
        // Pop the last node.
        node_type * last = list_.pop_back();
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
            // Push the last node to head again.
            list_.push_front(last);
        }
    }
};

typedef LRUCacheBase<int, int> LRUCache;

} // namespace LeetCode

#endif // LEETCODE_LRUCACHE_H
