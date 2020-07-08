
#ifndef LEETCODE_LRUHASHTABLE_H
#define LEETCODE_LRUHASHTABLE_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include <stdint.h>
#include <memory.h>
#include <cstdint>

#include "LRUNode.h"

#include <assert.h>

namespace LeetCode {

template <typename KeyT, typename ValueT>
class LRUHashTable {
public:
    typedef KeyT            key_type;
    typedef ValueT          value_type;
    typedef std::size_t     size_type;
    typedef std::uint32_t   hash_type;

    struct HashNode {
        key_type   key;
        value_type value;
    };

    static const size_type kDefaultCapacity = 32;
    static const uint32_t kFirstLayerSearchStep = 6;

private:
    size_type  size_;
    size_type  capacity_;
    hash_type  mask_;

    HashNode * table1_;
    HashNode * table2_;

public:
    LRUHashTable() : size_(0), capacity_(0), mask_(hash_type(-1)),
        table1_(nullptr), table2_(nullptr) {
        size_ = 0;
        capacity_ = kDefaultCapacity;
        mask_ = capacity_ - 1;
        init();
    }

    LRUHashTable(size_t capacity) : size_(0), capacity_(capacity), mask_(hash_type(-1)),
        table1_(nullptr), table2_(nullptr) {
        size_ = 0;
        capacity_ = calc_capacity(capacity);
        mask_ = (hash_type)(capacity_ - 1);
        init();
    }

    ~LRUHashTable() {
        if (table1_) {
            delete [] table1_;
            table1_ = nullptr;
        }
        if (table2_) {
            delete [] table2_;
            table2_ = nullptr;
        }
        size_= 0;
        capacity_ = 0;
    }

    size_type sizes() const { return size_; }
    size_type capacity() const { return capacity_; }

    bool is_empty() const { return (sizes() == 0); }

protected:
    hash_type getHash1(const key_type & key) const {
        return (hash_type)(key & mask_);
    }

    hash_type getHash2(const key_type & key) const {
        // Xor value: 4491719(prime), 4491738, 4491749(prime)
        return (hash_type)((key ^ 4491719) & mask_);
    }

    void init() {
        HashNode * new_table;
        assert(capacity_ > 0);
        new_table = new HashNode[capacity_];
        if (new_table) {
            ::memset(new_table, LRUKey::UnusedKey, capacity_ * sizeof(HashNode));
            table1_ = new_table;
        }
        new_table = new HashNode[capacity_];
        if (new_table) {
            ::memset(new_table, LRUKey::UnusedKey, capacity_ * sizeof(HashNode));
            table2_ = new_table;
        }
    }

    size_t calc_capacity(size_t capacity) {
        size_t newCapacity;
        // The kDefaultCapacity is 32.
        if (capacity > kDefaultCapacity) {
            newCapacity = kDefaultCapacity;
            while (newCapacity < capacity)
                newCapacity *= 2;
            if ((newCapacity >= capacity) && (newCapacity - capacity) < (newCapacity / 4))
                newCapacity *= 2;
        }
        else {
            newCapacity = kDefaultCapacity;
        }
        return newCapacity;
    }

    inline void remove_node_internal(HashNode * node) {
        assert(node != nullptr);
        assert((node >= table1_ && node < &table1_[capacity_]) ||
               (node >= table2_ && node < &table2_[capacity_]));
        assert(node->key != LRUKey::UnusedKey);
        node->key = LRUKey::UnusedKey;
        assert(size_ > 0);
        size_--;
    }

    bool remove_node(HashNode * node) {
        if (node != nullptr) {
            if ((node >= table1_ && node < &table1_[capacity_]) ||
                (node >= table2_ && node < &table2_[capacity_])) {
                remove_node_internal(node);
                return true;
            }
        }
        return false;
    }

public:
    HashNode * find(const key_type & key) {
        // The first layer table.
        hash_type index1 = getHash1(key);
        assert(index1 < capacity_);

        HashNode * start = &table1_[index1];
        HashNode * end = ((index1 + kFirstLayerSearchStep) <= capacity_) ?
            (start + kFirstLayerSearchStep) : &table1_[capacity_];
        assert(start < end);

        // Search for up to [kFirstLayerSearchStep = 6] steps.
        do {
            if (unlikely(start->key == LRUKey::UnusedKey)) {
                break;
            }
            if (unlikely(start->key == key)) {
                return start;
            }
            start++;
        } while (start != end);

        // The second layer table.
        hash_type index2 = getHash2(key);
        assert(index2 < capacity_);

        start = &table2_[index2];
        end = &table2_[capacity_];
        assert(start < end);

        HashNode * origin = start;
        do {
            if (unlikely(start->key == LRUKey::UnusedKey)) {
                break;
            }
            if (unlikely(start->key == key)) {
                return start;
            }
            start++;
            // Reach back to the original node.
            if (unlikely(start == origin)) {
                break;
            }
            // Reroll to the beginning of the table2.
            if (start == end) {
                start = table2_;
            }
        } while (1);

        return nullptr;
    }

    void insert(const key_type & key, const  value_type & value) {
        // The first layer table.
        hash_type index1 = getHash1(key);
        assert(index1 < capacity_);

        HashNode * start = &table1_[index1];
        HashNode * end = ((index1 + kFirstLayerSearchStep) <= capacity_) ?
            (start + kFirstLayerSearchStep) : &table1_[capacity_];
        assert(start < end);

        // Search for up to [kFirstLayerSearchStep = 6] steps.
        do {
            // If the key is LRUKey::UnusedKey or LRUKey::EmptyKey,
            // insert the new value to first layer.
            if (unlikely(start->key == LRUKey::UnusedKey)) {
                start->key = key;
                start->value = value;
                size_++;
                return;
            }
            if (unlikely(start->key == key)) {
                start->value = value;
                return;
            }
            start++;
        } while (start != end);

        // The second layer table.
        hash_type index2 = getHash2(key);
        assert(index2 < capacity_);

        start = &table2_[index2];
        end = &table2_[capacity_];
        assert(start < end);

        HashNode * origin = start;
        do {
            // If the key is LRUKey::UnusedKey or LRUKey::EmptyKey,
            // insert the new value to second layer.
            if (unlikely(start->key == LRUKey::UnusedKey)) {
                start->key = key;
                start->value = value;
                size_++;
                return;
            }
            if (unlikely(start->key == key)) {
                start->value = value;
                return;
            }
            start++;
            // Reach back to the original node.
            if (unlikely(start == origin)) {
                break;
            }
            // Reroll to the beginning of the table2.
            if (start == end) {
                start = table2_;
            }
        } while (1);
    }

    void remove(const key_type & key) {
        HashNode * node = find(key);
        if (node != nullptr) {
            remove_node_internal(node);
        }
    }

    bool remove_if(const key_type & key) {
        HashNode * node = find(key);
        if (node != nullptr) {
            remove_node_internal(node);
            return true;
        }
        return false;
    }
};

} // namespace LeetCode

#endif // LEETCODE_LRUHASHTABLE_H
