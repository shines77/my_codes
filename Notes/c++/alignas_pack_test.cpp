#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>

#include <memory.h>

// every object of type sse_t will be aligned to 16-byte boundary
struct alignas(16) sse_t
{
    float sse_data[4];
};
 
// the array "cacheline" will be aligned to 128-byte boundary
alignas(128) char cacheline[128];

#pragma pack(push, 1)

struct AAA
{
    char  m1;
    short m2;
    int   m3;
};

#pragma pack(pop)

#pragma pack(push, 2)

struct alignas(8) BBB
{
    char  m1;
    short m2;
    int   m3;
};

#pragma pack(pop)

void alignas_test()
{
    alignas(4) AAA a;
    a.m1 = 1;
    a.m2 = 2;
    a.m3 = 3;
}

void alignas_bbb_test()
{
    alignas(4) AAA a;
    a.m1 = 1;
    a.m2 = 2;
    a.m3 = 3;

    alignas(8) BBB b;
    b.m1 = 1;
    b.m2 = 2;
    b.m3 = 3;

    // b1 的首地址是 1024 字节对齐的
    //BBB * b1 = (BBB *)_aligned_malloc(sizeof(BBB) * 100, 1024);
    BBB * b1 = (BBB *)new BBB[100];
    // 偏移到后面的 11 个字节位置
    BBB * b2 = (BBB *)(((char *)b1) + 11);
    // 之所以这样写, 更明显能看出来是从 b1 偏移了 11 字节
    //memcpy((void *)((char *)b1 + 11), (void *)&b, sizeof(BBB));
    memcpy((void *)b2, (void *)&b, sizeof(BBB));

    printf("\nb2->m1: %d\nb2->m2: %d\nb2->m3: %d\n",
        b2->m1, b2->m2, b2->m3);

    b2->m1 = 4;
    b2->m2 = 5;
    b2->m3 = 6;

    printf("\nb2->m1: %d\nb2->m2: %d\nb2->m3: %d\n",
        b2->m1, b2->m2, b2->m3);

    printf("\nb1     addr: 0x%p\n", (void *)b1);
    printf(  "b2     addr: 0x%p\n", (void *)b2);
    printf("\nb2->m1 addr: 0x%p\nb2->m2 addr: 0x%p\nb2->m3 addr: 0x%p\n",
        (void *)&b2->m1, (void *)&b2->m2, (void *)&b2->m3);
    printf("\n");

    if (b1)
        delete[] b1;
}

#pragma pack(push, 8)

struct CCC
{
    char  m1;
    short m2;
    int   m3;
    char  m4;
};

#pragma pack(pop)

void alignas_ccc_test()
{
    CCC * c1 = new CCC[3];
    CCC * c2 = new CCC;

    printf(  "c1     addr: 0x%p, sizeof() = %u\n", (void *)c1, sizeof(CCC));
    printf("\nc1->m1 addr: 0x%p\nc1->m2 addr: 0x%p\nc1->m3 addr: 0x%p\nc1->m3 addr: 0x%p\n",
        (void *)&c1->m1, (void *)&c1->m2, (void *)&c1->m3, (void *)&c1->m4);
    printf("\n");

    printf(  "c2     addr: 0x%p, sizeof() = %u\n", (void *)c2, sizeof(CCC));
    printf("\nc2->m1 addr: 0x%p\nc2->m2 addr: 0x%p\nc2->m3 addr: 0x%p\nc2->m4 addr: 0x%p\n",
        (void *)&c2->m1, (void *)&c2->m2, (void *)&c2->m3, (void *)&c2->m4);
    printf("\n");

    if (c1)
        delete[] c1;
    if (c2)
        delete[] c2;
}

int main(int argc, char * argv[])
{
    alignas_test();
    alignas_bbb_test();
    alignas_ccc_test();
    return 0;
}