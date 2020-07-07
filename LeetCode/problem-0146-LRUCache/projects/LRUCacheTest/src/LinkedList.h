
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

protected:
    void init() {
        node_type * new_head = new node_type;
        assert(new_head != nullptr);
        head_ = new_head;
        node_type * new_tail = new node_type;
        assert(new_tail != nullptr);
        tail_ = new_tail;

        head_->prev = nullptr;
        head_->next = tail_;
        tail_->prev = head_;
        tail_->next = nullptr;

        // In fact, we needn't initialize the list nodes.
        node_type * new_list = new node_type[capacity_];
        assert(new_list != nullptr);
        list_ = new_list;        
    }

    void remove_internal(node_type * node) {
        assert(node != nullptr);
        node_type * prev = node->prev;
        node_type * next = node->next;
        assert(prev != nullptr);
        prev->next = next;
        assert(next != nullptr);
        next->prev = prev;
    }

    node_type * make_node(key_type key, value_type value) {
        assert(size_ < capacity_);
        node_type * new_item = &list_[size_];
        assert(new_item != nullptr);
        new_item->key   = key;
        new_item->value = value;
        return new_item;
    }

    node_type * push_front_internal(key_type key, value_type value) {
        assert(size_ < capacity_);
        node_type * new_node = make_node(key, value);
        assert(new_node != nullptr);
        push_front_internal(new_node);
        return new_node;
    }

    void push_front_internal(node_type * node) {
        assert(head_ != nullptr);
        assert(node != head_);

        // Insert node into the behind of the head node.
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

public:
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
                    ::memcpy((void *)new_list, (const void *)list_, size_ * sizeof(node_type));
                    delete [] list_;
                }
                list_ = new_list;
                capacity_ = new_capacity;
            }
        }
        else {
            // Shrink the size if necessary.
            if (new_capacity < size_) {
                size_type remove_nodes = (size_ - new_capacity);
                // Remove (size_ - new_capacity) nodes at tail, and adjust the value of tail->prev.
                assert(tail_ != nullptr);
                node_type * node = tail_->prev;
                while (node != nullptr && node->prev != nullptr) {
                    node = node->prev;
                    remove_nodes--;
                    if (remove_nodes == 0) {
                        tail_->prev = node;
                        break;
                    }
                }
                size_ = new_capacity;
            }
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

    void remove(node_type * node) {
        remove_internal(node);

        assert(size_ > 0);
        size_--;
    }

    void remove_safe(node_type * node) {
        assert(node != nullptr);
        node_type * prev = node->prev;
        if (prev != nullptr) {
            prev->next = next;
        }
        node_type * next = node->next;
        if (next != nullptr) {
            next->prev = prev;
        }
        assert(size_ > 0);
        size_--;
    }

    node_type * push_front_fast(key_type key, value_type value) {
        node_type * new_node = push_front_internal(key, value);
        assert(new_node != nullptr);
        size_++;
        assert(size_ <= capacity_);
        return new_node;
    }

    node_type * push_front(key_type key, value_type value) {
        if (size_ < capacity_)
            return push_front_fast(key, value);
        else
            return nullptr;
    }

    void push_front(node_type * node) {
        if (size_ < capacity_) {
            push_front_internal(node);

            size_++;
            assert(size_ <= capacity_);
        }
    }

    void push_back(node_type * node) {
        if (size_ < capacity_) {
            push_back_internal(node);

            size_++;
            assert(size_ <= capacity_);
        }
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

    void pop_front() {
        node_type * node = head_->next;
        assert(node != nullptr);
        assert(node != tail_);
        remove(node);
    }

    void pop_back() {
        node_type * node = tail_->prev;
        assert(node != nullptr);
        assert(node != head_);
        remove(node);
    }

    void bring_to_front(node_type * node) {
        remove_internal(node);
        push_front_internal(node);
    }

    void move_to_back(node_type * node) {
        remove_internal(node);
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
};

} // namespace LeetCode

#endif // LEETCODE_LINKEDLIST_H
