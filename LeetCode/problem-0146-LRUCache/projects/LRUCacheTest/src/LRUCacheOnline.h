#include <stdint.h>
#include <memory.h>
#ifndef likely
#define likely(expr) __builtin_expect(!!(expr), 1)
#endif
#ifndef unlikely
#define unlikely(expr) __builtin_expect(!!(expr), 0)
#endif
struct LRUKey { enum { UnusedKey = -1, ValidKey = 0 }; };
struct LRUValue { enum { FailedValue = -1 }; };
template <typename KeyT, typename ValueT>
struct LRUNode {
    typedef KeyT key_type;
    typedef ValueT value_type;
    key_type key;
    value_type value;
    LRUNode<key_type, value_type> * prev;
    LRUNode<key_type, value_type> * next;
};
template <typename KeyT, typename ValueT>
class LRUHashTable {
public:
    typedef KeyT key_type;
    typedef ValueT value_type;
    typedef uint32_t hash_type;
    struct HashNode {
        key_type key;
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
    LRUHashTable(size_t capacity) : size_(0), capacity_(capacity), mask_(size_t(-1)),
        table1_(nullptr), table2_(nullptr) {
        size_ = 0;
        capacity_ = calcCapacity(capacity);
        mask_ = capacity_ - 1;
        init();
    }
    ~LRUHashTable() {
        if (table1_) {
            delete[] table1_;
            table1_ = nullptr;
        }
        if (table2_) {
            delete[] table2_;
            table2_ = nullptr;
        }
        size_ = 0;
        capacity_ = 0;
    }
    size_t sizes() const { return size_; }
    size_t capacity() const { return capacity_; }
    HashNode * find(key_type key) {
        uint32_t index1 = (uint32_t)getHash1(key);
        HashNode * start = &table1_[index1];
        HashNode * end = ((index1 + kFirstLayerSearchStep) <= capacity_) ? (start + kFirstLayerSearchStep) : &table1_[capacity_];
        do {
            if (unlikely(start->key == LRUKey::UnusedKey))
                break;
            if (unlikely(start->key == key))
                return start;
            start++;
        } while (start != end);
        uint32_t index2 = (uint32_t)getHash2(key);
        start = &table2_[index2];
        end = &table2_[capacity_];
        HashNode * origin = start;
        do {
            if (unlikely(start->key == LRUKey::UnusedKey))
                break;
            if (unlikely(start->key == key))
                return start;
            start++;
            if (unlikely(start == origin))
                break;
            if (start == end)
                start = table2_;
        } while (1);
        return nullptr;
    }
    void insert(key_type key, value_type value) {
        uint32_t index1 = (uint32_t)getHash1(key);
        HashNode * start = &table1_[index1];
        HashNode * end = ((index1 + kFirstLayerSearchStep) <= capacity_) ? (start + kFirstLayerSearchStep) : &table1_[capacity_];
        do {
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
        uint32_t index2 = (uint32_t)getHash2(key);
        start = &table2_[index2];
        end = &table2_[capacity_];
        HashNode * origin = start;
        do {
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
            if (unlikely(start == origin))
                break;
            if (start == end)
                start = table2_;
        } while (1);
    }
    void remove(key_type key) {
        HashNode * node = find(key);
        if (node != nullptr) {
            node->key = LRUKey::UnusedKey;
            size_--;
        }
    }
protected:
    hash_type getHash1(key_type key) const { return (hash_type)(key & mask_); }
    hash_type getHash2(key_type key) const { return (hash_type)((key ^ 4491719) & mask_); }
    void init() {
        HashNode * new_table = new HashNode[capacity_];
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
        if (capacity > kDefaultCapacity) {
            newCapacity = kDefaultCapacity;
            while (newCapacity < capacity)
                newCapacity *= 2;
            if ((newCapacity >= capacity) && (newCapacity - capacity) < (newCapacity / 4))
                newCapacity *= 2;
        }
        else newCapacity = kDefaultCapacity;
        return newCapacity;
    }
};
template <typename NodeT>
class ContinuousDoubleLinkedList {
public:
    typedef NodeT node_type;
    typedef typename node_type::key_type key_type;
    typedef typename node_type::value_type value_type;
    static const size_t kDefaultCapacity = 32;
private:
    size_t size_;
    size_t capacity_;
    node_type * head_;
    node_type * tail_;
    node_type * list_;
public:
    ContinuousDoubleLinkedList(size_t capacity)
        : size_(0), capacity_(capacity),
        head_(nullptr), tail_(nullptr), list_(nullptr) {
        init();
    }
    ~ContinuousDoubleLinkedList() { destroy(); }
    size_t sizes() const { return size_; }
    size_t capacity() const { return capacity_; }
    node_type * insert_fast(key_type key, value_type value) {
        node_type * new_node = &list_[size_];
        new_node->key = key;
        new_node->value = value;
        push_front(new_node);
        return new_node;
    }
    void remove_fast(node_type * node) {
        node_type * prev = node->prev;
        node_type * next = node->next;
        prev->next = next;
        next->prev = prev;
        size_--;
    }
    void push_front(node_type * node) {
        node->prev = head_;
        node->next = head_->next;
        head_->next->prev = node;
        head_->next = node;
        size_++;
    }
    node_type * pop_back() {
        node_type * node = tail_->prev;
        if (node != head_) {
            remove_fast(node);
            return node;
        }
        else return nullptr;
    }
    void bring_to_front(node_type * node) {
        remove_fast(node);
        push_front(node);
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
        if (capacity_ > 0)
            new_list = new node_type[capacity_];
        list_ = new_list;
    }
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
            delete[] list_;
            list_ = nullptr;
        }
        size_ = 0;
        capacity_ = 0;
    }
};
template <typename KeyT, typename ValueT>
class LRUCacheBase {
public:
    typedef KeyT key_type;
    typedef ValueT value_type;
    typedef LRUNode<key_type, value_type> node_type;
    typedef LRUHashTable<key_type, node_type *> hash_table_type;
    typedef typename hash_table_type::HashNode hash_node_type;
    typedef ContinuousDoubleLinkedList<node_type> linkedlist_type;
    static const size_t kDefaultCapacity = 32;
private:
    size_t capacity_;
    linkedlist_type list_;
    hash_table_type cache_;
public:
    LRUCacheBase(size_t capacity) : capacity_(capacity), list_(capacity), cache_(capacity) { }
    ~LRUCacheBase() { }
    size_t sizes() const { return list_.sizes(); }
    size_t capacity() const { return list_.capacity(); }
protected:
    void insert(key_type key, value_type value) {
        node_type * new_node = list_.insert_fast(key, value);
        if (new_node)
            cache_.insert(key, new_node);
    }
    void touch_node(node_type * node) {
        list_.bring_to_front(node);
    }
    void touch(key_type key, value_type value) {
        node_type * last = list_.pop_back();
        if (last != nullptr) {
            if (key != last->key) {
                cache_.remove(last->key);
                cache_.insert(key, last);
            }
            last->key = key;
            last->value = value;
            list_.push_front(last);
        }
    }
public:
    value_type get(key_type key) {
        hash_node_type * hash_node = cache_.find(key);
        if (hash_node != nullptr) {
            node_type * node = hash_node->value;
            touch_node(node);
            return node->value;
        }
        return LRUValue::FailedValue;
    }
    void put(key_type key, value_type value) {
        hash_node_type * hash_node = cache_.find(key);
        if (hash_node != nullptr) {
            node_type * node = hash_node->value;
            node->value = value;
            touch_node(node);
        }
        else {
            if (list_.sizes() >= capacity_)
                touch(key, value);
            else
                insert(key, value);
        }
    }
};
typedef LRUCacheBase<int, int> LRUCache;
