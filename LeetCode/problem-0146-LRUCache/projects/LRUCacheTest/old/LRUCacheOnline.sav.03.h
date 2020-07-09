#include <stdint.h>
#include <memory.h>
#ifndef likely
#define likely(expr)    __builtin_expect(!!(expr), 1)
#endif
#ifndef unlikely
#define unlikely(expr)  __builtin_expect(!!(expr), 0)
#endif
struct LRUKey { enum  { EmptyKey = -2, UnusedKey = -1, ValidKey = 0 }; };
struct LRUValue { enum  { FailedValue = -1 }; };
template <typename KeyT, typename ValueT>
struct LRUItem {
    typedef KeyT key_type;
    typedef ValueT value_type;
    key_type key;
    value_type value;
    LRUItem<key_type, value_type> * prev;
    LRUItem<key_type, value_type> * next;
};
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
    LRUHashTable(size_t capacity) : size_(0), capacity_(capacity), mask_(size_t(-1)) {
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
        uint32_t index2 = (uint32_t)getHash2(key);
        start = &table2_[index2];
        end = &table2_[capacity_];
        HashNode * origin = start;
        do {
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
            if (unlikely(start == origin))
                break;
            if (start == end)
                start = table2_;
        } while (1);
    }
    void remove_fast(key_type key) {
        HashNode * node = find(key);
        if (node != nullptr) {
            node->key = LRUKey::EmptyKey;
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
template <typename ItemT>
class ContinuousDoubleLinkedList {
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
    ContinuousDoubleLinkedList(size_t capacity)
        : size_(0), capacity_(capacity),
          head_(nullptr), tail_(nullptr), list_(nullptr) {
        init();
    }
    ~ContinuousDoubleLinkedList() { free(); }
    size_t sizes() const { return size_; }
    size_t capacity() const { return capacity_; }
    item_type * insert_fast(key_type key, value_type value) {
        item_type * new_item = &list_[size_];
        new_item->key = key;
        new_item->value = value;
        push_front(new_item);
        return new_item;
    }
    void remove_fast(item_type * item) {
        item_type * prev = item->prev;
        item_type * next = item->next;
        prev->next = next;
        next->prev = prev;
        size_--;
    }
    void push_front(item_type * item) {
        item->prev = head_;
        item->next = head_->next;
        head_->next->prev = item;
        head_->next = item;
        size_++;
    }
    item_type * pop_back() {
        item_type * item = tail_->prev;
        if (item != head_) {
            remove_fast(item);
            return item;
        }
        else return nullptr;
    }
    void move_to_front(item_type * item) {
        remove_fast(item);
        push_front(item);
    }
protected:
    void init() {
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
        item_type * new_list = nullptr;
        if (capacity_ > 0)
            new_list = new item_type[capacity_];
        list_ = new_list;
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
template <typename KeyT, typename ValueT>
class LRUCache {
public:
    typedef KeyT key_type;
    typedef ValueT value_type;
    typedef LRUItem<key_type, value_type> item_type;
    typedef LRUHashTable<key_type, item_type *> hash_table_type;
    typedef typename hash_table_type::HashNode hash_node_type;
    typedef ContinuousDoubleLinkedList<item_type> linkedlist_type;
    static const size_t kDefaultCapacity = 32;
private:
    size_t capacity_;
    linkedlist_type list_;
    hash_table_type cache_;
public:
    LRUCache(size_t capacity) : capacity_(capacity), list_(capacity), cache_(capacity) { }
    ~LRUCache() { }
    size_t sizes() const { return list_.sizes(); }
    size_t capacity() const { return list_.capacity(); }
    value_type get(key_type key) {
        hash_node_type * node = cache_.find(key);
        if (node != nullptr) {
            item_type * item = node->value;
            touch(item);
            return item->value;
        }
        return LRUValue::FailedValue;
    }
    void put(key_type key, value_type value) {
        hash_node_type * node = cache_.find(key);
        if (node != nullptr) {
            item_type * item = node->value;
            item->key = key;
            item->value = value;            
            touch(item);
        }
        else {
            if (list_.sizes() >= capacity_)
                touch(key, value);
            else
                insert(key, value);
        }
    }
protected:
    void insert(key_type key, value_type value) {
        item_type * new_item = list_.insert_fast(key, value);
        if (new_item)
            cache_.insert(key, new_item);
    }
    void touch(item_type * item) {
        list_.move_to_front(item);
    }
    void touch(key_type key, value_type value) {
        item_type * last = list_.pop_back();
        if (last != nullptr) {
            if (key != last->key) {
                cache_.remove_fast(last->key);
                cache_.insert(key, last);
            }
            last->key = key;
            last->value = value;
            list_.push_front(last);
        }
    }
};