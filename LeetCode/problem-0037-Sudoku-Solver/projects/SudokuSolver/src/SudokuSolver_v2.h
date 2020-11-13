
#ifndef LEETCODE_SUDOKU_SOLVER_V2_H
#define LEETCODE_SUDOKU_SOLVER_V2_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include <cstdint>
#include <cstddef>
#include <vector>
#include <bitset>

#include "SudokuSolver.h"
#include "StopWatch.h"

namespace LeetCode {
namespace Problem_37 {
namespace v2 {

class Solution {
public:
    std::bitset<9> getPossibleStatus(int x, int y)
    {
        return ~(rows[x] | cols[y] | cells[x / 3][y / 3]);
    }

    std::vector<int> getNext(std::vector<std::vector<char>> & board)
    {
        std::vector<int> ret;
        size_t minCnt = 10;
        for (int i = 0; i < (int)board.size(); i++) {
            for (int j = 0; j < (int)board[i].size(); j++) {
                if (board[i][j] != '.')
                    continue;
                auto cur = getPossibleStatus(i, j);
                if (cur.count() >= minCnt)
                    continue;
                ret = { i, j };
                minCnt = cur.count();
            }
        }
        return ret;
    }

    void fillNum(int x, int y, int n, bool fillFlag)
    {
        rows[x][n] = (fillFlag) ? 1 : 0;
        cols[y][n] = (fillFlag) ? 1 : 0;
        cells[x/3][y/3][n] = (fillFlag) ? 1: 0;
    }
    
    bool dfs(std::vector<std::vector<char>> & board, int cnt)
    {
        if (cnt == 0)
            return true;

        auto next = getNext(board);
        auto bits = getPossibleStatus(next[0], next[1]);
        for (int n = 0; n < (int)bits.size(); n++) {
            if (!bits.test(n))
                continue;
            fillNum(next[0], next[1], n, true);
            board[next[0]][next[1]] = n + '1';
            if (dfs(board, cnt - 1))
                return true;
            board[next[0]][next[1]] = '.';
            fillNum(next[0], next[1], n, false);
        }
        return false;
    }

    void solveSudoku(std::vector<std::vector<char>> & board) 
    {
        SudokuHelper::display_board(board, true);

        jtest::StopWatch sw;
        sw.start();

        rows = std::vector<std::bitset<9>>(9, std::bitset<9>());
        cols = std::vector<std::bitset<9>>(9, std::bitset<9>());
        cells = std::vector<std::vector<std::bitset<9>>>(3, std::vector<std::bitset<9>>(3, std::bitset<9>()));

        int cnt = 0;
        for (int i = 0; i < (int)board.size(); i++) {
            for (int j = 0; j < (int)board[i].size(); j++) {
                cnt += (board[i][j] == '.');
                if (board[i][j] == '.')
                    continue;
                int n = board[i][j] - '1';
                rows[i] |= (1ULL << n);
                cols[j] |= (1ULL << n);
                cells[i / 3][j / 3] |= (1ULL << n);
            }
        }
        dfs(board, cnt);

        sw.stop();

        SudokuHelper::display_board(board);
        printf("Elapsed time: %0.3f ms\n\n", sw.getElapsedMillisec());
    }

private:
    std::vector<std::bitset<9>> rows;
    std::vector<std::bitset<9>> cols;
    std::vector<std::vector<std::bitset<9>>> cells;
};

} // namespace v2
} // namespace Problem_37
} // namespace LeetCode

#endif // LEETCODE_SUDOKU_SOLVER_V1_H
