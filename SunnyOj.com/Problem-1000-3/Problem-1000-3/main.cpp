
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <algorithm>

#pragma GCC optimize ("O3")

class Solution {
public:
#if 1
    std::string search(const std::string & str) {
        std::string min_str = str;

        uint8_t min_char = 255;
        for (size_t i = 0; i < str.size(); i++) {
            uint8_t ch = str[i];
            if (ch < min_char) {
                min_char = ch;
            }
        }

        for (size_t i = 1; i < str.size(); i++) {
            uint8_t ch = str[i];
            if (ch == min_char) {
                size_t index = i;
                std::string new_str;
                new_str.reserve(str.size());
                for (size_t j = index; j < str.size(); j++) {
                    new_str.push_back(str[j]);
                }
                for (size_t j = 0; j < index; j++) {
                    new_str.push_back(str[j]);
                }

                if (new_str < min_str) {
                    min_str = new_str;
                }
            }
        }

        return min_str;
    }
#else
    std::string search(const std::string & in_str) {
        std::string min_str = in_str;
        std::string str = in_str;

        for (size_t i = 0; i < str.size() - 1; i++) {
            std::string new_str;
            for (size_t j = 1; j < str.size(); j++) {
                new_str.push_back(str[j]);
            }
            new_str.push_back(str[0]);

            str = new_str;
            if (new_str < min_str) {
                min_str = new_str;
            }
        }

        return min_str;
    }
#endif

    std::string minString(std::string & str, std::size_t K) {
        if (K == 1) {
            return search(str);
        } else {
            std::sort(str.begin(), str.end());
            return str;
        }
    }
};

class Solution2 {
public:
    std::set<std::string> searched;
    std::string best;
    std::string min_str;
    std::size_t K;
    int cnt = 0;

    bool search(const std::string & str) {
        bool done = false;
        searched.insert(str);

        char last = 0;
        std::string prefix;
        prefix.reserve(str.size());
        for (size_t i = 0; i < K; i++) {
            prefix.push_back(str[i]);
        }
        std::sort(prefix.begin(), prefix.end());

        for (size_t i = 0; i < K; i++) {
            char cur = prefix[K - i - 1];
            if (cur == last)
                continue;
            last = cur;
            size_t index = size_t(-1);
            for (size_t j = 0; j < K; j++) {
                if (str[j] == cur) {
                    index = j;
                    break;
                }
            }
            if (index == size_t(-1))
                continue;

            std::string new_str;
            size_t j = 0;
            for (; j < K; j++) {
                if (j != index) {
                    new_str.push_back(str[j]);
                }
            }
            for (; j < str.size(); j++) {
                new_str.push_back(str[j]);
            }
            new_str.push_back(str[index]);

            if (searched.count(new_str) == 0) {
                cnt++;
                printf("i = %d, str = %s\n", cnt, new_str.c_str());
                if (new_str == best) {
                    min_str = best;
                    return true;
                }
                if (new_str < min_str) {
                    min_str = new_str;
                }
                done = search(new_str);
                if (done)
                    break;
            }
        }
        return done;
    }

    void minString(const std::string & str) {
        best = str;
        std::sort(best.begin(), best.end());
        search(str);
    }
};

int main(int argc, char * argv[])
{
#ifdef _DEBUG
    int K = 1;
    std::string str = "dcaba";

    Solution solution;
    std::string min_str = solution.minString(str, K);
    printf("%s\n", min_str.c_str());

#if 0
    Solution2 solution2;
    solution2.min_str = str;
    solution2.K = K;
    solution2.minString(str);
    printf("%s\n", solution2.min_str.c_str());
    printf("%d\n", solution2.cnt);
#endif

    system("pause");
#else
    int K = 0;
    std::string str;
    char buf[1024];
    if (scanf("%s", &buf[0]) != EOF) {
        str = buf;
        scanf("%d", &K);

        Solution solution;
        std::string min_str = solution.minString(str, K);
        printf("%s\n", min_str.c_str());
    }
#endif
    return 0;
}
