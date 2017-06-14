
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
    typedef LRUItem<key_type, value_type> item_type;

    static const size_t kCapacity = 16;

private:
    size_t size_;
    size_t capacity_;

    item_type * head_;
    item_type * tail_;
    item_type * list_;

public:
    FixedDoubleLinkedList()
        : size_(0), capacity_(kCapacity), list_(nullptr),
          head_(nullptr), tail_(nullptr) {
        init();
    }

    FixedDoubleLinkedList(size_t capacity)
        : size_(0), capacity_(capacity), list_(nullptr),
          head_(nullptr), tail_(nullptr) {
        init();
    }

    ~FixedDoubleLinkedList() {
        free();
    }

    size_t sizes() const { return size_; }
    size_t capacity() const { return capacity_; }

    bool is_empty() const {
        return (sizes() == 0);
    }

protected:
    void init() {
        if (capacity_ > 0) {
            item_type * new_item1 = new item_type;
            item_type * new_item2 = new item_type;
            item_type * new_list = new item_type[capacity_];
            if (new_item1 && new_item2 && new_list) {
                head_ = new_item1;
                tail_ = new_item2;
                head_->prev = nullptr;
                head_->next = tail_;
                tail_->prev = head_;
                tail_->next = nullptr;

                list_ = new_list;
                ::memset(list_, 0, sizeof(item_type) * capacity_);
            }
        }
    }

    void free() {
        if (head_) {
            delete head_;
            head_ = nullptr;
        }
        if (tail_) {
            delete tail_;
            tail_ = nullptr;
        }
        if (list_) {
            delete [] list_;
            list_ = nullptr;
        }
    }
};

} // namespace LeetCode

#endif // LEETCODE_LINKEDLIST_H
