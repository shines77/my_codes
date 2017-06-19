
#ifndef LEETCODE_LINKEDLIST_H
#define LEETCODE_LINKEDLIST_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include <stdint.h>
#include <memory.h>

#include "LRUItem.h"

#include <assert.h>

namespace LeetCode {

template <typename ItemT>
class FixedDoubleLinkedList {
public:
    typedef ItemT item_type;
    typedef typename item_type::key_type key_type;
    typedef typename item_type::value_type value_type;

    static const size_t kDefaultCapacity = 32;

private:
    size_t size_;
    size_t capacity_;

    item_type * head_;
    item_type * tail_;
    item_type * list_;

public:
    FixedDoubleLinkedList()
        : size_(0), capacity_(kDefaultCapacity),
          head_(nullptr), tail_(nullptr), list_(nullptr) {
        init();
    }

    FixedDoubleLinkedList(size_t capacity)
        : size_(0), capacity_(capacity),
          head_(nullptr), tail_(nullptr), list_(nullptr) {
        init();
    }

    ~FixedDoubleLinkedList() {
        free();
    }

    size_t sizes() const { return size_; }
    size_t capacity() const { return capacity_; }

    bool is_empty() const { return (sizes() == 0); }

    void realloc(size_t new_capacity) {
        if (new_capacity > capacity_) {
            item_type * new_list = new item_type[new_capacity];
            if (new_list) {
                assert(size_ <= capacity_);
                // Fill unused items and new alloc items.
                ::memset(new_list + size_, 0, sizeof(item_type) * (new_capacity - size_));
                if (list_) {
                    // Copy first [size_] items.
                    ::memcpy((void *)new_list, (const void *)list_, sizeof(item_type) * size_);
                    delete [] list_;
                }
                list_ = new_list;
            }
        }
        if (new_capacity >= size_) {
            capacity_ = new_capacity;
        }
        else {
            intptr_t remove_items = (size_ - new_capacity);
            // Remove last [remove_items] items, adjust the tail item.
            assert(tail_ != nullptr);
            item_type * item = tail_->prev;
            while (item != nullptr && item->prev != nullptr) {
                item = item->prev;
                remove_items--;
                if (remove_items <= 0) {
                    tail_->prev = item;
                    break;
                }
            }
            size_ = new_capacity;
            capacity_ = new_capacity;
        }
    }

    void resize(size_t new_capacity) {
        if (list_) {
            delete [] list_;
        }
        item_type * new_list = new item_type[new_capacity];
        if (new_list) {
            ::memset(new_list, 0, sizeof(item_type) * new_capacity);
        }
        list_ = new_list;

        size_ = 0;
        capacity_ = new_capacity;

        head_->prev = nullptr;
        head_->next = tail_;
        tail_->prev = head_;
        tail_->next = nullptr;
    }

    item_type * insert_fast(key_type key, value_type value) {
        assert(size_ <= capacity_);
        item_type * new_item = &list_[size_];
        assert(new_item != nullptr);
        new_item->key   = key;
        new_item->value = value;
        assert(head_ != nullptr);
        new_item->prev  = head_;
        new_item->next  = head_->next;

        // Adjust the head item.
        head_->next->prev = new_item;
        head_->next       = new_item;
        size_++;
        return new_item;
    }

    item_type * insert(key_type key, value_type value) {
        assert(size_ <= capacity_);
        if (size_ < capacity_) {
            return insert_fast(key, value);
        }
        return nullptr;
    }

    void remove_fast(item_type * item) {
        assert(item != nullptr);
        item_type * prev = item->prev;
        item_type * next = item->next;
        assert(prev != nullptr);
        prev->next = next;
        assert(next != nullptr);
        next->prev = prev;
        assert(size_ >= 1);
        size_--;
    }

    bool remove(item_type * item) {
        assert(item != nullptr);
        item_type * prev = item->prev;
        item_type * next = item->next;
        if (prev != nullptr && next != nullptr) {
            prev->next = next;
            next->prev = prev;
            size_--;
            return true;
        }
        return false;
    }

    void push_front(item_type * item) {
        assert(head_ != nullptr);
        assert(item != head_);
        item->prev = head_;
        item->next = head_->next;

        // Adjust the head item.
        head_->next->prev = item;
        head_->next       = item;
        size_++;
    }

    void push_back(item_type * item) {
        assert(tail_ != nullptr);
        assert(item != tail_);
        item->prev = tail_->prev;
        item->next = tail_;

        // Adjust the tail item.
        tail_->prev->next = item;
        tail_->prev       = item;
        size_++;
    }

    item_type * pop_front() {
        item_type * item = head_->next;
        assert(item != nullptr);
        if (item != tail_) {
            remove_fast(item);
            return item;
        }
        else {
            return nullptr;
        }
    }

    item_type * pop_back() {
        item_type * item = tail_->prev;
        assert(item != nullptr);
        if (item != head_) {
            remove_fast(item);
            return item;
        }
        else {
            return nullptr;
        }
    }

    void move_to_front(item_type * item) {
        // remove_fast(item);
        assert(item != nullptr);
        item_type * prev = item->prev;
        item_type * next = item->next;
        assert(prev != nullptr);
        prev->next = next;
        assert(next != nullptr);
        next->prev = prev;
        
        // push_front(item);
        assert(head_ != nullptr);
        assert(item != head_);
        item->prev = head_;
        item->next = head_->next;

        // Adjust the head item.
        head_->next->prev = item;
        head_->next       = item;
    }

    void print() {
        item_type * item = head_->next;
        int index = 0;
        printf("LRUCache: (size = %u, capacity = %u)\n", (uint32_t)size_, (uint32_t)capacity_);
        while (item && item->next) {            
            printf("[%4d]  key: %6d, value: %6d\n", index + 1, item->key, item->value);
            index++;
            item = item->next;
        }
        printf("\n\n");
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
        else {
            item_type * new_item1 = new item_type;
            item_type * new_item2 = new item_type;
            if (new_item1 && new_item2) {
                head_ = new_item1;
                tail_ = new_item2;
                head_->prev = nullptr;
                head_->next = tail_;
                tail_->prev = head_;
                tail_->next = nullptr;
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
        size_ = 0;
        capacity_ = 0;
    }
};

} // namespace LeetCode

#endif // LEETCODE_LINKEDLIST_H
