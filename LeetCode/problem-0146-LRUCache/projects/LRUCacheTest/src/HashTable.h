
#ifndef LEETCODE_HASHTABLE_H
#define LEETCODE_HASHTABLE_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include <stdint.h>
#include <memory.h>

#include "LRUItem.h"

#include <assert.h>

namespace LeetCode {

template <typename KeyT, typename ValueT>
class LRUHashTable {
public:
    typedef KeyT key_type;
    typedef ValueT value_type;
    typedef uint32_t hash_type;

    struct HashNode {
        key_type   key;
        value_type value;
    };

    static const size_t kDefaultCapacity = 32;
    static const uint32_t kFirstLayerSearchStep = 6;

private:
    size_t size_;
    size_t capacity_;
    size_t mask_;

    HashNode * table1_;
    HashNode * table2_;

public:
    LRUHashTable() : size_(0), capacity_(0), mask_(size_t(-1)),
        table1_(nullptr), table2_(nullptr) {
        size_ = 0;
        capacity_ = kDefaultCapacity;
        mask_ = capacity_ - 1;
        init();
    }

    LRUHashTable(size_t capacity) : size_(0), capacity_(capacity), mask_(size_t(-1)),
        table1_(nullptr), table2_(nullptr) {
        size_ = 0;
        capacity_ = calcCapacity(capacity);
        mask_ = capacity_ - 1;
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

    size_t sizes() const { return size_; }
    size_t capacity() const { return capacity_; }

    bool is_empty() const { return (sizes() == 0); }

protected:
    hash_type getHash1(key_type key) const {
        return (hash_type)(key & mask_);
    }

    hash_type getHash2(key_type key) const {
        // Xor value: 4491719(prime), 4491738, 4491749(prime)
        return (hash_type)((key ^ 4491719) & mask_);
    }

    void init() {
        HashNode * new_table;
        new_table = new HashNode[capacity_];
        if (new_table) {
            table1_ = new_table;
            ::memset(table1_, LRUKey::UnusedKey, sizeof(HashNode) * capacity_);
        }
        new_table = new HashNode[capacity_];
        if (new_table) {
            table2_ = new_table;
            ::memset(table2_, LRUKey::UnusedKey, sizeof(HashNode) * capacity_);
        }
    }

    size_t calcCapacity(size_t capacity) {
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

public:
    HashNode * find(key_type key) {
        // The first layer table.
        uint32_t index1 = (uint32_t)getHash1(key);
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
        uint32_t index2 = (uint32_t)getHash2(key);
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

    void insert(key_type key, value_type value) {
        // The first layer table.
        uint32_t index1 = (uint32_t)getHash1(key);
        assert(index1 < capacity_);

        HashNode * start = &table1_[index1];
        HashNode * end = ((index1 + kFirstLayerSearchStep) <= capacity_) ?
            (start + kFirstLayerSearchStep) : &table1_[capacity_];
        assert(start < end);

        // Search for up to [kFirstLayerSearchStep = 6] steps.
        do {
            // If the key is LRUKey::UnusedKey or LRUKey::EmptyKey,
            // insert the new value to first layer.
            if (unlikely(start->key < LRUKey::ValidKey)) {
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
        uint32_t index2 = (uint32_t)getHash2(key);
        assert(index2 < capacity_);

        start = &table2_[index2];
        end = &table2_[capacity_];
        assert(start < end);

        HashNode * origin = start;
        do {
            // If the key is LRUKey::UnusedKey or LRUKey::EmptyKey,
            // insert the new value to second layer.
            if (unlikely(start->key < LRUKey::ValidKey)) {
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

    inline void remove_fast(HashNode * node) {
        assert(node != nullptr);
        assert((node >= table1_ && node < &table1_[capacity_]) ||
               (node >= table2_ && node < &table2_[capacity_]));
        node->key = LRUKey::EmptyKey;
        assert(size_ > 0);
        size_--;
    }

    void remove_fast(key_type key) {
        HashNode * node = find(key);
        if (node != nullptr) {
            remove_fast(node);
        }
    }

    bool remove(HashNode * node) {
        if (node != nullptr) {
            if ((node >= table1_ && node < &table1_[capacity_]) ||
                (node >= table2_ && node < &table2_[capacity_])) {
                remove_fast(node);
                return true;
            }
        }
        return false;
    }

    bool remove(key_type key) {
        HashNode * node = find(key);
        if (node != nullptr) {
            remove_fast(node);
            return true;
        }
        return false;
    }
};

} // namespace LeetCode

#endif // LEETCODE_HASHTABLE_H
