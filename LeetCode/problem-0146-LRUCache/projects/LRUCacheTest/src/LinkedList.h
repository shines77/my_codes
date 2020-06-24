
#ifndef LEETCODE_LINKEDLIST_H
#define LEETCODE_LINKEDLIST_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include "LRUNode.h"

#include <stdint.h>
#include <memory.h>

#include <assert.h>

namespace LeetCode {

//
// class ContinuousDoubleLinkedList<T>
//

template <typename NodeTy>
class ContinuousDoubleLinkedList {
public:
    typedef NodeTy                          node_type;
    typedef typename node_type::key_type    key_type;
    typedef typename node_type::value_type  value_type;
    typedef std::size_t                     size_type;

    static const size_type kDefaultCapacity = 32;

private:
    size_type size_;
    size_type capacity_;

    node_type * head_;
    node_type * tail_;
    node_type * list_;

public:
    ContinuousDoubleLinkedList()
        : size_(0), capacity_(kDefaultCapacity),
          head_(nullptr), tail_(nullptr), list_(nullptr) {
        init();
    }

    ContinuousDoubleLinkedList(size_type capacity)
        : size_(0), capacity_(capacity),
          head_(nullptr), tail_(nullptr), list_(nullptr) {
        init();
    }

    ~ContinuousDoubleLinkedList() {
        destroy();
    }

    size_type sizes() const { return size_; }
    size_type capacity() const { return capacity_; }

    bool is_empty() const { return (sizes() == 0); }

    void destroy() {
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

    void clear() {
        size_ = 0;

        head_->prev = nullptr;
        head_->next = tail_;
        tail_->prev = head_;
        tail_->next = nullptr;
    }

    void resize(size_type new_capacity) {
        if (new_capacity > capacity_) {
            node_type * new_list = new node_type[new_capacity];
            if (new_list) {
                assert(size_ <= capacity_);
                if (list_) {
                    // Copy first [size_] items.
                    ::memcpy((void *)new_list, (const void *)list_, sizeof(node_type) * size_);
                    delete [] list_;
                }
                list_ = new_list;
            }
        }
        if (new_capacity >= size_) {
            capacity_ = new_capacity;
        }
        else {
            intptr_t remove_nodes = (size_ - new_capacity);
            // Remove last [remove_nodes] nodes, adjust the tail node.
            assert(tail_ != nullptr);
            node_type * node = tail_->prev;
            while (node != nullptr && node->prev != nullptr) {
                node = node->prev;
                remove_nodes--;
                if (remove_nodes <= 0) {
                    tail_->prev = node;
                    break;
                }
            }
            size_ = new_capacity;
            capacity_ = new_capacity;
        }
    }

    void reset(size_type new_capacity) {
        if (list_) {
            delete [] list_;
        }
        node_type * new_list = new node_type[new_capacity];
        list_ = new_list;

        size_ = 0;
        capacity_ = new_capacity;

        head_->prev = nullptr;
        head_->next = tail_;
        tail_->prev = head_;
        tail_->next = nullptr;
    }

    node_type * insert_fast(key_type key, value_type value) {
        return insert_internal(key, value);
    }

    node_type * insert(key_type key, value_type value) {
        assert(size_ <= capacity_);
        if (size_ < capacity_) {
            return insert_internal(key, value);
        }
        return nullptr;
    }

    void remove_fast(node_type * node) {
        remove_fast_internal(node);

        assert(size_ >= 1);
        size_--;
    }

    bool remove(node_type * node) {
        assert(node != nullptr);
        node_type * prev = node->prev;
        node_type * next = node->next;
        if (prev != nullptr && next != nullptr) {
            prev->next = next;
            next->prev = prev;
            assert(size_ >= 1);
            size_--;
            return true;
        }
        return false;
    }

    void push_front(node_type * node) {
        push_front_internal(node);

        size_++;
        assert(size_ <= capacity_);
    }

    void push_back(node_type * node) {
        push_back_internal(node);

        size_++;
        assert(size_ <= capacity_);
    }

    node_type * front() {
        node_type * node = head_->next;
        assert(node != nullptr);
        if (node != tail_)
            return node;
        else
            return nullptr;
    }

    node_type * back() {
        node_type * node = tail_->prev;
        assert(node != nullptr);
        if (node != head_)
            return node;
        else
            return nullptr;
    }

    node_type * pop_front() {
        node_type * node = head_->next;
        assert(node != nullptr);
        if (node != tail_) {
            remove_fast(node);
            return node;
        }
        else {
            return nullptr;
        }
    }

    node_type * pop_back() {
        node_type * node = tail_->prev;
        assert(node != nullptr);
        if (node != head_) {
            remove_fast(node);
            return node;
        }
        else {
            return nullptr;
        }
    }

    void bring_to_front(node_type * node) {
        remove_fast_internal(node);
        push_front_internal(node);
    }

    void move_to_back(node_type * node) {
        remove_fast_internal(node);
        push_back_internal(node);
    }

    void print() {
        node_type * node = head_->next;
        int index = 0;
        printf("LRUCache: (size = %u, capacity = %u)\n\n", (uint32_t)size_, (uint32_t)capacity_);
        while (node && node->next) {            
            printf("[%-3d]  key: %-06d, value: %-6d\n", index, node->key, node->value);
            index++;
            node = node->next;
        }
        printf("\n");
    }

protected:
    void init() {
        node_type * new_node1 = new node_type;
        node_type * new_node2 = new node_type;
        if (new_node1 && new_node2) {
            head_ = new_node1;
            tail_ = new_node2;
            head_->prev = nullptr;
            head_->next = tail_;
            tail_->prev = head_;
            tail_->next = nullptr;
        }

        node_type * new_list = nullptr;
        if (capacity_ > 0) {
            new_list = new node_type[capacity_];
            // In fact, we needn't initialize the list items.
        }
        list_ = new_list;
    }

    node_type * insert_internal(key_type key, value_type value) {
        assert(size_ < capacity_);
        node_type * new_item = &list_[size_];
        assert(new_item != nullptr);
        new_item->key   = key;
        new_item->value = value;
        push_front(new_item);
        return new_item;
    }

    void remove_fast_internal(node_type * node) {
        assert(node != nullptr);
        node_type * prev = node->prev;
        node_type * next = node->next;
        assert(prev != nullptr);
        prev->next = next;
        assert(next != nullptr);
        next->prev = prev;
    }

    void push_front_internal(node_type * node) {
        assert(head_ != nullptr);
        assert(node != head_);

        // Inserted into the behind of the head node.
        node->prev = head_;
        node->next = head_->next;

        // Adjust the head node.
        head_->next->prev = node;
        head_->next       = node;
    }

    void push_back_internal(node_type * node) {
        assert(tail_ != nullptr);
        assert(node != tail_);

        // Inserted into the front of the tail node.
        node->prev = tail_->prev;
        node->next = tail_;

        // Adjust the tail node.
        tail_->prev->next = node;
        tail_->prev       = node;
    }
};

} // namespace LeetCode

#endif // LEETCODE_LINKEDLIST_H
