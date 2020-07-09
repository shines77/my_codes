
#ifdef _WIN32
#include <vld.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <ratio>
#include <chrono>

#include "LRUCache.h"
#include "LRUCache_v1.h"
#include "LRUCache_v2.h"
#ifndef __builtin_expect
#define __builtin_expect(expr, f)   (expr)
#endif
#include "LRUCacheOnline.h"

using namespace std;
using namespace std::chrono;

#ifdef NDEBUG
#define MAX_TEST_DATA   1024
#else
#define MAX_TEST_DATA   (16 * 64)
#endif

/* Invalid the debug info print on debug mode. */
#ifndef NDEBUG
#define NDEBUG
#endif

std::vector<int> s_testData[MAX_TEST_DATA];

//
// Problem title: 146. LRU Cache
// Problem URL: https://leetcode.com/problems/lru-cache/
//

/*************************************************************************/
/* Test data                                                             */
/*                                                                       */
/* Input:                                                                */
/*                                                                       */
/*    ["LRUCache","put","put","get","put","get","put","get","get","get"] */
/*    [[2],[1,1],[2,2],[1],[3,3],[2],[4,4],[1],[3],[4]]                  */
/*                                                                       */
/* Output:                                                               */
/*                                                                       */
/*    [null,null,null,1,null,-1,null,-1,3,4]                             */
/*                                                                       */
/*************************************************************************/

void LeetCode_LRUCache_UintTest()
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

    lruCache.display();
    printf("\n");
}

void LeetCode_Online_LRUCache_UintTest()
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

void cpu_warmup(int delayMillsecs)
{
#if defined(NDEBUG)
    double delayTimeLimit = (double)delayMillsecs / 1.0;
    volatile int sum = 0;

    printf("CPU warm-up begin ...\n");
    fflush(stdout);
    high_resolution_clock::time_point startTime, endTime;
    duration<double, std::ratio<1, 1000>> elapsedTime;
    startTime = high_resolution_clock::now();
    do {
        for (int i = 0; i < 500; ++i) {
            sum += i;
            for (int j = 5000; j >= 0; --j) {
                sum -= j;
            }
        }
        endTime = high_resolution_clock::now();
        elapsedTime = endTime - startTime;
    } while (elapsedTime.count() < delayTimeLimit);

    printf("sum = %u, time: %0.3f ms\n", sum, elapsedTime.count());
    printf("CPU warm-up end   ... \n\n");
    fflush(stdout);
#endif // !_DEBUG
}

uint32_t nextUInt32()
{
    uint32_t next;
#if (RAND_MAX == 0x7FFF)
    next = ((uint32_t)rand() << 30)
        | (((uint32_t)rand() & 0x7FFF) << 15)
        |  ((uint32_t)rand() & 0x7FFF);
#else
    next = (uint32_t)rand();
#endif
    return next;
}

int32_t nextInt32()
{
    int32_t next;
#if (RAND_MAX == 0x7FFF)
    next = (int32_t)(((uint32_t)rand() << 30)
                  | (((uint32_t)rand() & 0x7FFF) << 15)
                  |  ((uint32_t)rand() & 0x7FFF));
#else
    next = (int32_t)rand();
#endif
    return next;
}

int randomInt32(int mininum, int maxinum)
{
    int result;
    if (mininum < maxinum) {
        result = mininum + int32_t(nextUInt32() % uint32_t(maxinum - mininum + 1));
    }
    else if (mininum > maxinum) {
        result = maxinum + int32_t(nextUInt32() % uint32_t(mininum - maxinum + 1));
    }
    else {
        result = mininum;
    }
    return result;
}

int randomInt32(int maxinum)
{
    return randomInt32(0, maxinum);
}

void make_test_data()
{
    for (int i = 0; i < MAX_TEST_DATA; i++) {
        std::vector<int> & lruData = s_testData[i];

        int lruCapacity = randomInt32(2, 100);
        lruData.push_back(lruCapacity);
        int basePutSize = randomInt32(lruCapacity * 60 / 100, lruCapacity);

        int multiple = randomInt32(2, 3);
        int maxKey = randomInt32(1, lruCapacity * multiple);
        int maxValue = randomInt32(1, lruCapacity * (multiple + 1));
        
        int lruActionSize = randomInt32(lruCapacity * 60 / 100, lruCapacity * 2);
        lruData.push_back(basePutSize + lruActionSize);

        for (int j = 0; j < basePutSize; j++) {
            // Put
            int key = randomInt32(1, maxKey);
            int value = randomInt32(1, maxValue);
            lruData.push_back(key);
            lruData.push_back(value);
        }

        for (int j = 0; j < lruActionSize; j++) {
            int method = randomInt32(-80, 100);
            if (method >= 0) {
                // Put
                int key = randomInt32(1, maxKey);
                int value = randomInt32(1, maxValue);
                lruData.push_back(key);
                lruData.push_back(value);
            }
            else {
                // Get
                int key = -randomInt32(1, maxKey);
                lruData.push_back(key);
            }
        }
    }
}

void LRUCache_PrefTest()
{
    printf("LRUCache_PrefTest() begin ...\n");
    high_resolution_clock::time_point startTime = high_resolution_clock::now();

    int sumGet = 0, sumVisit = 0;
    for (int i = 0; i < MAX_TEST_DATA; i++) {
        std::vector<int> & lruData = s_testData[i];
        assert(lruData.size() > 0);
        int lruCapacity = lruData[0];
        assert(lruCapacity > 0);
        LeetCode::LRUCache lruCache(lruCapacity);
        assert(lruData.size() > 1);
        int lruActionSize = lruData[1];
        int index = 2;
        for (int j = 0; j < lruActionSize; j++) {
            int key = lruData[index++];
            if (key > 0) {
                // Put
                int value = lruData[index++];
                lruCache.put(key, value);
            }
            else if (key < 0) {
                // Get
                sumGet += lruCache.get(-key);
            }
            else {
                break;
            }
        }

        int order;
        auto node = lruCache.begin();
        for (order = 0; order < lruCapacity; order++) {
            if (node != lruCache.end()) {
                sumVisit += node->key * 256 + node->value * (order + 1);
                node = node->next;
            }
            else {
                break;
            }
        }
#ifndef NDEBUG
        printf("total = %-6d, lruCapacity = %d, lruActionSize = %d\n", order, lruCapacity, lruActionSize);
        if (order == 2) {
            printf("\n");
            lruCache.display();
            for (int i = 0; i < (int)lruData.size(); i++) {
                printf("[%-3d]: %d\n", i, lruData[i]);
            }
            printf("\n");
        }
#endif
    }

    high_resolution_clock::time_point endTime = high_resolution_clock::now();
    duration<double, std::ratio<1, 1000>> elapsedTime = endTime - startTime;

    printf("LRUCache_PrefTest() end   ...\n");
    printf("\n");
    printf("Elapsed time: %0.3f ms, sumGet = %d, sumVisit = %d\n",
           elapsedTime.count(), sumGet, sumVisit);
    printf("\n");
}

void LRUCache_V1_PrefTest()
{
    printf("LRUCache_V1_PrefTest() begin ...\n");
    high_resolution_clock::time_point startTime = high_resolution_clock::now();

    int sumGet = 0, sumVisit = 0;
    for (int i = 0; i < MAX_TEST_DATA; i++) {
        std::vector<int> & lruData = s_testData[i];
        assert(lruData.size() > 0);
        int lruCapacity = lruData[0];
        assert(lruCapacity > 0);
        LeetCode::V1::LRUCache lruCache(lruCapacity);
        assert(lruData.size() > 1);
        int lruActionSize = lruData[1];
        int index = 2;
        for (int j = 0; j < lruActionSize; j++) {
            int key = lruData[index++];
            if (key > 0) {
                // Put
                int value = lruData[index++];
                lruCache.put(key, value);
            }
            else if (key < 0) {
                // Get
                sumGet += lruCache.get(-key);
            }
            else {
                break;
            }
        }

        int order;
        auto node = lruCache.cbegin();
        for (order = 0; order < lruCapacity; order++) {
            if (node != lruCache.cend()) {
                sumVisit += node->first * 256 + node->second * (order + 1);
                node++;
            }
            else break;
        }
#ifndef NDEBUG
        printf("total = %-6d, lruCapacity = %d\n", order, lruCapacity);
#endif
    }

    high_resolution_clock::time_point endTime = high_resolution_clock::now();
    duration<double, std::ratio<1, 1000>> elapsedTime = endTime - startTime;

    printf("LRUCache_V1_PrefTest() end   ...\n");
    printf("\n");
    printf("Elapsed time: %0.3f ms, sumGet = %d, sumVisit = %d\n",
           elapsedTime.count(), sumGet, sumVisit);
    printf("\n");
}

void LRUCache_V2_PrefTest()
{
    printf("LRUCache_V2_PrefTest() begin ...\n");
    high_resolution_clock::time_point startTime = high_resolution_clock::now();

    int sumGet = 0, sumVisit = 0;
    for (int i = 0; i < MAX_TEST_DATA; i++) {
        std::vector<int> & lruData = s_testData[i];
        assert(lruData.size() > 0);
        int lruCapacity = lruData[0];
        assert(lruCapacity > 0);
        LeetCode::V2::LRUCache lruCache(lruCapacity);
        assert(lruData.size() > 1);
        int lruActionSize = lruData[1];
        int index = 2;
        for (int j = 0; j < lruActionSize; j++) {
            int key = lruData[index++];
            if (key > 0) {
                // Put
                int value = lruData[index++];
                lruCache.put(key, value);
            }
            else if (key < 0) {
                // Get
                sumGet += lruCache.get(-key);
            }
            else {
                break;
            }
        }

        int order;
        auto node = lruCache.begin();
        for (order = 0; order < lruCapacity; order++) {
            if (node != lruCache.end()) {
                sumVisit += node->key * 256 + node->value * (order + 1);
                node = node->prev;
            }
            else break;
        }
#ifndef NDEBUG
        printf("total = %-6d, lruCapacity = %d\n", order, lruCapacity);
#endif
    }

    high_resolution_clock::time_point endTime = high_resolution_clock::now();
    duration<double, std::ratio<1, 1000>> elapsedTime = endTime - startTime;

    printf("LRUCache_V2_PrefTest() end   ...\n");
    printf("\n");
    printf("Elapsed time: %0.3f ms, sumGet = %d, sumVisit = %d\n",
           elapsedTime.count(), sumGet, sumVisit);
    printf("\n");
}

void LRUCache_PrefTest2()
{
    printf("LRUCache_PrefTest2() begin ...\n");

    double elapsedMillsecs = 0.0f;

    int sumGet = 0, sumVisit = 0;
    for (int i = 0; i < MAX_TEST_DATA; i++) {
        std::vector<int> & lruData = s_testData[i];
        assert(lruData.size() > 0);
        int lruCapacity = lruData[0];
        assert(lruCapacity > 0);
        LeetCode::LRUCache lruCache(lruCapacity);
        assert(lruData.size() > 1);
        int lruActionSize = lruData[1];

        high_resolution_clock::time_point startTime = high_resolution_clock::now();

        int index = 2;
        for (int j = 0; j < lruActionSize; j++) {
            int key = lruData[index++];
            if (key > 0) {
                // Put
                int value = lruData[index++];
                lruCache.put(key, value);
            }
            else if (key < 0) {
                // Get
                sumGet += lruCache.get(-key);
            }
            else {
                break;
            }
        }

        int order;
        auto node = lruCache.begin();
        for (order = 0; order < lruCapacity; order++) {
            if (node != lruCache.end()) {
                sumVisit += node->key * 256 + node->value * (order + 1);
                node = node->next;
            }
            else {
                break;
            }
        }

        high_resolution_clock::time_point endTime = high_resolution_clock::now();
        duration<double, std::ratio<1, 1000>> elapsedTime = endTime - startTime;

        elapsedMillsecs += elapsedTime.count();

#ifndef NDEBUG
        printf("total = %-6d, lruCapacity = %d, lruActionSize = %d\n", order, lruCapacity, lruActionSize);
        if (order == 2) {
            printf("\n");
            lruCache.display();
            for (int i = 0; i < (int)lruData.size(); i++) {
                printf("[%-3d]: %d\n", i, lruData[i]);
            }
            printf("\n");
        }
#endif
    }

    printf("LRUCache_PrefTest2() end   ...\n");
    printf("\n");
    printf("Elapsed time: %0.3f ms, sumGet = %d, sumVisit = %d\n",
           elapsedMillsecs, sumGet, sumVisit);
    printf("\n");
}

void LRUCache_V1_PrefTest2()
{
    printf("LRUCache_V1_PrefTest2() begin ...\n");

    double elapsedMillsecs = 0.0f;

    int sumGet = 0, sumVisit = 0;
    for (int i = 0; i < MAX_TEST_DATA; i++) {
        std::vector<int> & lruData = s_testData[i];
        assert(lruData.size() > 0);
        int lruCapacity = lruData[0];
        assert(lruCapacity > 0);
        LeetCode::V1::LRUCache lruCache(lruCapacity);
        assert(lruData.size() > 1);
        int lruActionSize = lruData[1];

        high_resolution_clock::time_point startTime = high_resolution_clock::now();

        int index = 2;
        for (int j = 0; j < lruActionSize; j++) {
            int key = lruData[index++];
            if (key > 0) {
                // Put
                int value = lruData[index++];
                lruCache.put(key, value);
            }
            else if (key < 0) {
                // Get
                sumGet += lruCache.get(-key);
            }
            else {
                break;
            }
        }

        int order;
        auto node = lruCache.cbegin();
        for (order = 0; order < lruCapacity; order++) {
            if (node != lruCache.cend()) {
                sumVisit += node->first * 256 + node->second * (order + 1);
                node++;
            }
            else break;
        }

        high_resolution_clock::time_point endTime = high_resolution_clock::now();
        duration<double, std::ratio<1, 1000>> elapsedTime = endTime - startTime;

        elapsedMillsecs += elapsedTime.count();

#ifndef NDEBUG
        printf("total = %-6d, lruCapacity = %d, lruActionSize = %d\n", order, lruCapacity, lruActionSize);
#endif
    }

    printf("LRUCache_V1_PrefTest2() end   ...\n");
    printf("\n");
    printf("Elapsed time: %0.3f ms, sumGet = %d, sumVisit = %d\n",
           elapsedMillsecs, sumGet, sumVisit);
    printf("\n");
}

void LRUCache_V2_PrefTest2()
{
    printf("LRUCache_V2_PrefTest2() begin ...\n");

    double elapsedMillsecs = 0.0f;

    int sumGet = 0, sumVisit = 0;
    for (int i = 0; i < MAX_TEST_DATA; i++) {
        std::vector<int> & lruData = s_testData[i];
        assert(lruData.size() > 0);
        int lruCapacity = lruData[0];
        assert(lruCapacity > 0);
        LeetCode::V2::LRUCache lruCache(lruCapacity);
        assert(lruData.size() > 1);
        int lruActionSize = lruData[1];

        high_resolution_clock::time_point startTime = high_resolution_clock::now();

        int index = 2;
        for (int j = 0; j < lruActionSize; j++) {
            int key = lruData[index++];
            if (key > 0) {
                // Put
                int value = lruData[index++];
                lruCache.put(key, value);
            }
            else if (key < 0) {
                // Get
                sumGet += lruCache.get(-key);
            }
            else {
                break;
            }
        }

        int order;
        auto node = lruCache.begin();
        for (order = 0; order < lruCapacity; order++) {
            if (node != lruCache.end()) {
                sumVisit += node->key * 256 + node->value * (order + 1);
                node = node->prev;
            }
            else break;
        }

        high_resolution_clock::time_point endTime = high_resolution_clock::now();
        duration<double, std::ratio<1, 1000>> elapsedTime = endTime - startTime;

        elapsedMillsecs += elapsedTime.count();

#ifndef NDEBUG
        printf("total = %-6d, lruCapacity = %d, lruActionSize = %d\n", order, lruCapacity, lruActionSize);
#endif
    }

    printf("LRUCache_V2_PrefTest2() end   ...\n");
    printf("\n");
    printf("Elapsed time: %0.3f ms, sumGet = %d, sumVisit = %d\n",
           elapsedMillsecs, sumGet, sumVisit);
    printf("\n");
}

void LeetCode_LRUCache_PrefTest()
{
    cpu_warmup(1000);
    make_test_data();

    printf("---------------------------------------------------------------------\n\n");

    LRUCache_PrefTest();
    LRUCache_V2_PrefTest();
    LRUCache_V1_PrefTest();

    printf("---------------------------------------------------------------------\n\n");

    LRUCache_PrefTest2();
    LRUCache_V2_PrefTest2();
    LRUCache_V1_PrefTest2();

    printf("---------------------------------------------------------------------\n\n");
}

int main(int argc, char * argv[])
{
    uint32_t seed = 20200704UL;
    //::srand((uint32_t)::time(NULL));
    ::srand(seed);

    LeetCode_LRUCache_UintTest();
    LeetCode_Online_LRUCache_UintTest();

    LeetCode_LRUCache_PrefTest();

#if defined(WIN32) || defined(_WIN32) || defined(OS_WINDOWS) || defined(_WINDOWS)
    ::system("pause");
#endif
    return 0;
}
