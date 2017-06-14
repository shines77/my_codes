
#ifndef LEETCODE_HASHTABLE_H
#define LEETCODE_HASHTABLE_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include <stdint.h>
#include <memory.h>

#include "LRUItem.h"

namespace LeetCode {

template <typename KeyT, typename ValueT>
class LRUHashTable {
public:
    typedef KeyT key_type;
    typedef ValueT value_type;
    typedef uint32_t hash_type;
    typedef LRUItem<key_type, value_type> node_type;

    struct HashNode {
        key_type    key;
        node_type * node;
    };

    static const size_t kCapacity = 32;

private:
    size_t capacity_;
    size_t mask_;

    HashNode * table1_;
    HashNode * table2_;

public:
    LRUHashTable() : capacity_(0), mask_(size_t(-1)) {
        capacity_ = kCapacity;
        mask_ = capacity_ - 1;
        init();
    }

    LRUHashTable(size_t capacity) : capacity_(capacity), mask_(size_t(-1)) {
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
    }

protected:
    hash_type getHash1(key_type key) const {
        return (hash_type)(key & mask_);
    }

    hash_type getHash2(key_type key) const {
        return (hash_type)((key ^ 4491738) & mask_);
    }

    void init() {
        HashNode * table;
        table = new HashNode[capacity_];
        if (table) {
            table1_ = table;
            ::memset(table1_, -1, sizeof(HashNode) * capacity_);
        }
        table = new HashNode[capacity_];
        if (table) {
            table2_ = table;
            ::memset(table2_, -1, sizeof(HashNode) * capacity_);
        }
    }

    size_t calcCapacity(size_t capacity) {
        size_t newCapacity;
        // kCapacity is 32.
        if (capacity > kCapacity) {
            newCapacity = kCapacity;
            while (newCapacity < capacity)
                newCapacity *= 2;
            if ((newCapacity >= capacity) && (newCapacity - capacity) < (newCapacity / 4))
                newCapacity *= 2;
        }
        else {
            newCapacity = kCapacity;
        }
        return newCapacity;
    }

public:
    node_type * find(key_type key) {
        node_type * node = nullptr;
        return node;
    }

    void insert(key_type key, node_type * node) {
        //
    }

    void remove(key_type key) {
        //
    }
};

} // namespace LeetCode

#endif // LEETCODE_HASHTABLE_H
