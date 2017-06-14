
## LeetCode

[https://LeetCode.com](https://LeetCode.com)

## Problem - 0146

`Question` 146. **LRU Cache**

URL: [https://leetcode.com/problems/lru-cache/](https://leetcode.com/problems/lru-cache/)

## Description

Design and implement a data structure for [Least Recently Used (LRU) cache](https://en.wikipedia.org/wiki/Cache_replacement_policies#LRU). It should support the following operations: `get` and `put`.

`get(key)` - Get the value (will always be positive) of the key if the key exists in the cache, otherwise return -1.
`put(key, value)` - Set or insert the value if the key is not already present. When the cache reached its capacity, it should invalidate the least recently used item before inserting a new item.

**Follow up:**

Could you do both operations in O(1) time complexity?

## Subscribe

[Subscribe](https://leetcode.com/subscribe/) to see which companies asked this question.

    Total Accepted: 82292
    Total Submissions: 519691
    Difficulty: Hard

(The deadline is 2016/08/15)

    Total Accepted: 124153
    Total Submissions: 721655
    Difficulty: Hard
    Contributor: LeetCode

(The deadline is 2017/06/14)

## Example:

```cpp
    LRUCache cache = new LRUCache( 2 /* capacity */ );

    cache.put(1, 1);
    cache.put(2, 2);
    cache.get(1);       // returns 1
    cache.put(3, 3);    // evicts key 2
    cache.get(2);       // returns -1 (not found)
    cache.put(4, 4);    // evicts key 1
    cache.get(1);       // returns -1 (not found)
    cache.get(3);       // returns 3
    cache.get(4);       // returns 4
```

## C++:

```cpp
class LRUCache {
public:
    LRUCache(int capacity) {
        
    }
    
    int get(int key) {
        
    }
    
    void put(int key, int value) {
        
    }
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache obj = new LRUCache(capacity);
 * int param_1 = obj.get(key);
 * obj.put(key,value);
 */
```

## C:

```c
typedef struct {
    
} LRUCache;

LRUCache * lRUCacheCreate(int capacity) {
    
}

int lRUCacheGet(LRUCache * obj, int key) {
    
}

void lRUCachePut(LRUCache * obj, int key, int value) {
    
}

void lRUCacheFree(LRUCache * obj) {
    
}

/**
 * Your LRUCache struct will be instantiated and called as such:
 * struct LRUCache * obj = lRUCacheCreate(capacity);
 * int param_1 = lRUCacheGet(obj, key);
 * lRUCachePut(obj, key, value);
 * lRUCacheFree(obj);
 */
```

## Go:

```go
type LRUCache struct {
    
}


func Constructor(capacity int) LRUCache {
    
}


func (this *LRUCache) Get(key int) int {
    
}


func (this *LRUCache) Put(key int, value int)  {
    
}


/**
 * Your LRUCache object will be instantiated and called as such:
 * obj := Constructor(capacity);
 * param_1 := obj.Get(key);
 * obj.Put(key,value);
 */
```

## Testcase

```
["LRUCache","put","put","get","put","get","put","get","get","get"]
[[2],[1,1],[2,2],[1],[3,3],[2],[4,4],[1],[3],[4]]
```
