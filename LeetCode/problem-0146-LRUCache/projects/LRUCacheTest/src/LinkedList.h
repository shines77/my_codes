
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

    class free_list {
    private:
        node_type * head_;
        size_type   size_;

    public:
        free_list() : head_(nullptr), size_(0) {}
        free_list(node_type * head) : head_(head), size_(0) {}
        ~free_list() {
#ifndef NDEBUG
            this->clear();
#endif
        }

        node_type * begin() const { return this->head_; }
        node_type * end() const { return nullptr; }

        node_type * head() const { return this->head_; }
        size_type size() const { return this->size_; }

        void set_head(node_type * node) {
            this->head_ = node;
        }
        void set_size(size_type size) {
            this->size_ = size;
        }

        bool is_valid() const { return (this->head() != nullptr); }
        bool is_empty() const { return (this->size() == 0); }

        void clear() {
            this->head_ = nullptr;
            this->size_ = 0;
        }

        void reset(node_type * head) {
            this->head_ = head;
            this->size_ = 0;
        }

        void increase() {
            ++(this->size_);
        }

        void decrease() {
            --(this->size_);
        }

        void push_first(node_type * node) {
            assert(node != nullptr);
            assert(node->next == nullptr);
            this->head_ = node;
            ++(this->size_);
        }

        void push_front(node_type * node) {
            assert(node != nullptr);
            node->next = this->head_;
            this->head_ = node;
            ++(this->size_);
        }

        node_type * pop_front() {
            node_type * node = this->head_;
            assert(node != nullptr);
            this->head_ = node->next;
            assert(this->size_ > 0);
            --(this->size_);
            return node;
        }

        void swap(free_list & right) {
            if (&right != this) {
                node_type * save_head = this->head_;
                size_type save_size = this->size_;
                this->head_ = right.head_;
                this->size_ = right.size_;
                right.head_ = save_head;
                right.size_ = save_size;
            }
        }
    };

    inline void swap(free_list & lhs, free_list & rhs) {
        lhs.swap(rhs);
    }

protected:
    size_type   size_;
    size_type   capacity_;

    node_type * head_;
    node_type * tail_;
    node_type * list_;
    free_list   freelist_;

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

    size_type size() const { return this->size_ - this->freelist_.size(); }
    size_type capacity() const { return capacity_; }

    node_type * head()  { return this->head_; }
    node_type * begin() { return this->head_->next; }
    node_type * end()   { return this->tail_; }

    bool is_empty() const { return (this->size() == 0); }

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

    node_type * create_new_node(key_type key, value_type value) {
        assert(size_ < capacity_);
        node_type * new_node;
        if (freelist_.is_empty()) {
            new_node = &list_[size_];
            size_++;
            assert(size_ <= capacity_);
        }
        else {
            new_node = freelist_.pop_front();
        }
        assert(new_node != nullptr);
        new_node->key   = key;
        new_node->value = value;
        return new_node;
    }

    void remove(node_type * node) {
        assert(node != nullptr);
        node_type * prev = node->prev;
        node_type * next = node->next;
        assert(prev != nullptr);
        prev->next = next;
        assert(next != nullptr);
        next->prev = prev;

        freelist_.push_front(node);
    }

    void remove_safe(node_type * node) {
        assert(node != nullptr);
        node_type * prev = node->prev;
        node_type * next = node->next;
        if (prev != nullptr) {
            prev->next = next;
        }
        if (next != nullptr) {
            next->prev = prev;
        }

        freelist_.push_front(node);
    }

    node_type * push_front_fast(key_type key, value_type value) {
        assert(size_ < capacity_);
        node_type * new_node = create_new_node(key, value);
        assert(new_node != nullptr);
        push_front_internal(new_node);
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
        }
    }

    void push_back(node_type * node) {
        if (size_ < capacity_) {
            push_back_internal(node);
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

    void move_to_front(node_type * node) {
        assert(node != nullptr);
        node_type * prev = node->prev;
        node_type * next = node->next;
        assert(prev != nullptr);
        prev->next = next;
        assert(next != nullptr);
        next->prev = prev;

        assert(head_ != nullptr);
        assert(node != head_);

        // Insert node into the behind of the head node.
        node->prev = head_;
        node->next = head_->next;

        // Adjust the head node.
        head_->next->prev = node;
        head_->next       = node;
    }

    void move_to_back(node_type * node) {
        assert(node != nullptr);
        node_type * prev = node->prev;
        node_type * next = node->next;
        assert(prev != nullptr);
        prev->next = next;
        assert(next != nullptr);
        next->prev = prev;

        assert(tail_ != nullptr);
        assert(node != tail_);

        // Insert node into the front of the tail node.
        node->prev = tail_->prev;
        node->next = tail_;

        // Adjust the tail node.
        tail_->prev->next = node;
        tail_->prev       = node;
    }

    void display() {
        node_type * node = head_->next;
        int index = 0;
        printf("LRUCache: (size = %u, capacity = %u)\n\n", (uint32_t)this->size(), (uint32_t)this->capacity());
        while (node && node->next) {            
            printf("[%-3d]  key: %-6d, value: %-6d\n", index, node->key, node->value);
            index++;
            node = node->next;
        }
        printf("\n");
    }
};

} // namespace LeetCode

#endif // LEETCODE_LINKEDLIST_H
