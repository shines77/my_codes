
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <cstdio>

#include "LRUCache.h"

using namespace LeetCode;

int main(int argc, char * argv[])
{
    LRUCache<int, int> lruCache(100);

    lruCache.put(1, 1);
    lruCache.put(2, 2);
    lruCache.get(1);       // returns 1
    lruCache.put(3, 3);    // evicts key 2
    lruCache.get(2);       // returns -1 (not found)
    lruCache.put(4, 4);    // evicts key 1
    lruCache.get(1);       // returns -1 (not found)
    lruCache.get(3);       // returns 3
    lruCache.get(4);       // returns 4

    ::system("pause");
    return 0;
}
