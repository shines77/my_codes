
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <cstdio>

#include "LRUCache.h"
#ifndef __builtin_expect
#define __builtin_expect(expr, f)   (expr)
#endif
#include "LRUCacheOnline.h"

//
// Problem title: 146. LRU Cache
// Problem URL: https://leetcode.com/problems/lru-cache/
//

void LeetCode_LRUCache_test()
{
    printf("\n");
    printf("LRUCacheTest()\n\n");

    LeetCode::LRUCache lruCache(2);

    lruCache.put(1, 1);
    lruCache.put(2, 2);
    lruCache.get(1);       // returns 1
    lruCache.put(3, 3);    // evicts key 2
    lruCache.get(2);       // returns -1 (not found)
    lruCache.put(4, 4);    // evicts key 1
    lruCache.get(1);       // returns -1 (not found)
    lruCache.get(3);       // returns 3
    lruCache.get(4);       // returns 4
    lruCache.put(2, 12);   // evicts key 3

    lruCache.print();
}

void LeetCode_Online_LRUCache_test()
{
    printf("LeetCode Online LRUCacheTest()\n\n");

    LRUCache lruCache(2);

    lruCache.put(1, 1);
    lruCache.put(2, 2);
    lruCache.get(1);       // returns 1
    lruCache.put(3, 3);    // evicts key 2
    lruCache.get(2);       // returns -1 (not found)
    lruCache.put(4, 4);    // evicts key 1
    lruCache.get(1);       // returns -1 (not found)
    lruCache.get(3);       // returns 3
    lruCache.get(4);       // returns 4
    lruCache.put(2, 12);   // evicts key 3

    printf("size = %u, capacity = %u\n\n", (uint32_t)lruCache.sizes(), (uint32_t)lruCache.capacity());
}

int main(int argc, char * argv[])
{
    LeetCode_LRUCache_test();

    LeetCode_Online_LRUCache_test();

#if defined(WIN32) || defined(_WIN32) || defined(OS_WINDOWS) || defined(_WINDOWS)
    ::system("pause");
#endif
    return 0;
}
