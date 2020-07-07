
#ifndef LEETCODE_LRUCACHE_H
#define LEETCODE_LRUCACHE_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include <stdint.h>

#include "LRUNode.h"
#include "LRUHashTable.h"
#include "LinkedList.h"

#include <unordered_map>

//
// Problem title: 146. LRU Cache
// Problem URL: https://leetcode.com/problems/lru-cache/
//

namespace LeetCode {

#if 1

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

protected:
    void insert(const key_type & key, const value_type & value) {
        node_type * new_node = list_.push_front_fast(key, value);
        if (new_node) {
            cache_.insert(key, new_node);
        }
    }

    void touch_node(node_type * node) {
        assert(node != nullptr);
        list_.bring_to_front(node);
    }

    void touch(const key_type & key, const value_type & value) {
        // Pop the last node.
        node_type * last = list_.back();
        if (last != nullptr) {
            list_.pop_back();
            if (key != last->key) {
                // Remove the old key from the hash table.
                cache_.remove(last->key);
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

public:
    value_type get(const key_type & key) {
        hash_node_type * hash_node = cache_.find(key);
        if (hash_node != nullptr) {
            node_type * node = hash_node->value;
            assert(node != nullptr);
            assert(key == node->key);
            touch_node(node);
            return node->value;
        }
        return kFailedValue;
    }

    void put(const key_type & key, const value_type & value) {
        hash_node_type * hash_node = cache_.find(key);
        if (hash_node != nullptr) {
            node_type * node = hash_node->value;
            assert(node != nullptr);
            assert(key == node->key);
            node->value = value;
            touch_node(node);
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

#else

template <typename KeyT, typename ValueT, ValueT kFailedValue = LRUValue::FailedValue>
class LRUCacheBase {
public:
    typedef KeyT    key_type;
    typedef ValueT  value_type;

    typedef std::size_t                                 size_type;
    typedef LRUNode<key_type, value_type>               node_type;
    typedef std::unordered_map<key_type, node_type *>   hash_table_type;
    typedef ContinuousDoubleLinkedList<node_type>       linkedlist_type;

    static const size_type kDefaultCapacity = 32;

private:
    size_type       capacity_;
    linkedlist_type list_;
    hash_table_type cache_;

public:
    LRUCacheBase() : capacity_(kDefaultCapacity), list_(kDefaultCapacity) {
    }

    LRUCacheBase(size_t capacity) : capacity_(capacity), list_(capacity) {
    }

    ~LRUCacheBase() {
        capacity_ = 0;
    }

    size_type sizes() const { return list_.sizes(); }
    size_type capacity() const { return list_.capacity(); }

protected:
    void insert(const key_type & key, const value_type & value) {
        node_type * new_node = list_.push_front_fast(key, value);
        if (new_node) {
            cache_.insert(std::make_pair(key, new_node));
        }
    }

    void touch_node(node_type * node) {
        assert(node != nullptr);
        list_.bring_to_front(node);
    }

    void touch_node(const key_type & key, const value_type & value) {
        // Pop the last node.
        node_type * last = list_.back();
        if (last != nullptr) {
            list_.pop_back();
            if (key != last->key) {
                // Remove the old key from the hash table.
                cache_.erase(last->key);
                // Insert the new key and value to the hash table.
                cache_.insert(std::make_pair(key, last));
            }
            // Save the new key and value.
            last->key = key;
            last->value = value;
            // Push the last node to head again.
            list_.push_front(last);
        }
    }

public:
    value_type get(const key_type & key) {
        auto iter = cache_.find(key);
        if (iter != cache_.end()) {
            node_type * node = iter->second;
            assert(node != nullptr);
            assert(key == node->key);
            touch_node(node);
            return node->value;
        }
        return kFailedValue;
    }

    void put(const key_type & key, const value_type & value) {
        auto iter = cache_.find(key);
        if (iter != cache_.end()) {
            node_type * node = iter->second;
            assert(node != nullptr);
            assert(key == node->key);
            node->value = value;
            touch_node(node);
        }
        else {
            if (list_.sizes() >= capacity_) {
                touch_node(key, value);
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

#endif

typedef LRUCacheBase<int, int> LRUCache;

} // namespace LeetCode

#endif // LEETCODE_LRUCACHE_H
