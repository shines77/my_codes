
## LeetCode

[https://LeetCode.com](https://LeetCode.com)

## Problem - 0146

`Question` 146. **LRU Cache**

URL: [https://leetcode.com/problems/lru-cache/](https://leetcode.com/problems/lru-cache/)

## Description

Design and implement a data structure for Least Recently Used (LRU) cache. It should support the following operations: `get` and `set`.

`get(key)` - Get the value (will always be positive) of the key if the key exists in the cache, otherwise return -1.
`set(key, value)` - Set or insert the value if the key is not already present. When the cache reached its capacity, it should invalidate the least recently used item before inserting a new item.

## Subscribe

[Subscribe](https://leetcode.com/subscribe/) to see which companies asked this question.

    Total Accepted: 82292
    Total Submissions: 519691
    Difficulty: Hard

(The deadline is 2016/08/15)

## Example:

```cpp
    LRUCache cache = new LRUCache( 2 /* capacity */ );

    cache.set(1, 1);
    cache.set(2, 2);
    cache.get(1);       // returns 1
    cache.set(3, 3);    // evicts key 2
    cache.get(2);       // returns -1 (not found)
    cache.set(4, 4);    // evicts key 1
    cache.get(1);       // returns -1 (not found)
    cache.get(3);       // returns 3
    cache.get(4);       // returns 4
```

## C++:

```cpp
    class LRUCache{
    public:
        LRUCache(int capacity) {
            
        }
        
        int get(int key) {
            
        }
        
        void set(int key, int value) {
            
        }
    };

    /**
     * Your LRUCache object will be instantiated and called as such:
     * LRUCache obj = new LRUCache(capacity);
     * int param_1 = obj.get(key);
     * obj.set(key, value);
     */
```
