
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>

class Solution {
public:
    int getSecond(unsigned int length) {
        int second = 1;
        while (length > 1) {
            if ((length & 0x01) == 0) {
                length /= 2;
                second++;
            } else {
                length--;
                second++;
            }
        }
        return second;
    }
};

class Solution2 {
public:
    int min_second = 100000000;

    int writeActicle(int second, int len, int max_len) {
        if ((len * 2) == max_len || (len + 1) == max_len)
            return (second + 1);
        if (second > min_second || len >= max_len)
            return 100000000;

        int second1 = 100000000, second2 = 100000000;
        if ((len * 2) <= max_len) {
            second1 = writeActicle(second + 1, len * 2, max_len);
            if (second1 < min_second)
                min_second = second1;
        }
        if ((len + 1) <= max_len) {
            second2 = writeActicle(second + 1, len + 1, max_len);
            if (second2 < min_second)
                min_second = second2;
        }

        second = std::min(second1, second2);
        return second;
    }

    int getSecond(int length) {
        int second = writeActicle(1, 1, length);
        return second;
    }
};

int main(int argc, char * argv[])
{
#ifdef _DEBUG
    int length = 10;

    Solution solution;
    int second = solution.getSecond(length);
    printf("%d\n", second);

    system("pause");
#else
    int length = 0;
    if (scanf("%d", &length) != EOF) {
        Solution solution;
        int second = solution.getSecond(length);
        printf("%d\n", second);
    }
#endif
    return 0;
}
