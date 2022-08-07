
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>

class Solution {
public:
    int buyIceCream(int N, int M, int V,
        std::vector<int> & weights,
        std::vector<int> & prices) {
        int satisfy = 0;
        std::sort(weights.begin(), weights.end());
        std::sort(prices.begin(), prices.end());

        size_t wi = 0, pi = 0;
        while (wi < weights.size()) {
            int weight = weights[wi];
            while (pi < prices.size()) {
                int price = prices[pi];
                if (price > V)
                    return satisfy;
                if (price >= weight) {
                    pi++;
                    V -= price;
                    satisfy++;
                    break;
                }
                pi++;
            }
            wi++;
        }
        return satisfy;
    }
};

int main(int argc, char * argv[])
{
#ifdef _DEBUG
    int N = 3, M = 5, V = 20;
    std::vector<int> weights = { 9, 5, 4 };
    std::vector<int> prices = { 10, 8, 6, 6, 5 };

    Solution solution;
    int satisfy = solution.buyIceCream(N, M, V, weights, prices);
    printf("%d\n", satisfy);

    system("pause");
#else
    int N = 0, M = 0, V = 0;
    std::vector<int> weights;
    std::vector<int> prices;
    if (scanf("%d %d %d", &N, &M, &V) != EOF) {
        int val;
        for (int i=0; i<N; i++) {
            scanf("%d", &val);
            weights.push_back(val);
        }
        for (int i=0; i<M; i++) {
            scanf("%d", &val);
            prices.push_back(val);
        }

        Solution solution;
        int satisfy = solution.buyIceCream(N, M, V, weights, prices);
        printf("%d\n", satisfy);
    }
#endif
    return 0;
}
