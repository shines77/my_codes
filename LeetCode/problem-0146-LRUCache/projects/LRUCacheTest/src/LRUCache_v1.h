
#ifndef LEETCODE_LRUCACHE_V1_H
#define LEETCODE_LRUCACHE_V1_H

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#include <stdint.h>

#include <list>
#include <unordered_map>
#include <utility>

//
// Problem title: 146. LRU Cache
// Problem URL: https://leetcode.com/problems/lru-cache/
//

namespace LeetCode {
namespace V1 {

class LRUCache {
public:
    LRUCache(int capacity) : cache_(capacity), capacity_(capacity) {
    }
    
    int get(int key) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            lru_list_.splice(lru_list_.begin(), lru_list_, it->second);
            return it->second->second;
        }
        return -1;
    }
    
    void put(int key, int value) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            lru_list_.splice(lru_list_.begin(), lru_list_, it->second);
            it->second->second = value;
        }
        else {
            lru_list_.emplace_front(key, value);
            cache_[key] = lru_list_.begin();
        
            if (cache_.size() > size_t(capacity_)) {
                cache_.erase(lru_list_.back().first);
                lru_list_.pop_back();
            }
        }
    }

    std::list<std::pair<int, int>>::iterator begin() {
        return lru_list_.begin();
    }

    std::list<std::pair<int, int>>::const_iterator cbegin() const {
        return lru_list_.cbegin();
    }

    std::list<std::pair<int, int>>::iterator end() {
        return lru_list_.end();
    }

    std::list<std::pair<int, int>>::const_iterator cend() const {
        return lru_list_.cend();
    }

private:
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> cache_;
    std::list<std::pair<int, int>> lru_list_;
    int capacity_;
};

} // namespace V1
} // namespace LeetCode

#endif // LEETCODE_LRUCACHE_V1_H
