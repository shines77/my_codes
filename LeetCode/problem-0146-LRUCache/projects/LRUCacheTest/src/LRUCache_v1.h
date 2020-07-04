
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
    LRUCache(int capacity) : _capacity(capacity) {
    }
    
    int get(int key) {
        auto it = _table.find(key);
        if (it != _table.end()) {
            _lru.splice(_lru.begin(), _lru, it->second);
            return it->second->second;
        }
        return -1;
    }
    
    void put(int key, int value) {
        auto it = _table.find(key);
        if (it != _table.end()) {
            _lru.splice(_lru.begin(), _lru, it->second);
            it->second->second = value;
            return;
        }
        
        _lru.emplace_front(key, value);
        _table[key] = _lru.begin();
        
        if (_table.size() > size_t(_capacity)) {
            _table.erase(_lru.back().first);
            _lru.pop_back();
        }
    }
private:
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> _table;
    std::list<std::pair<int, int>> _lru;
    int _capacity;
};

} // namespace V1
} // namespace LeetCode

#endif // LEETCODE_LRUCACHE_V1_H
