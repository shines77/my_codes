
#ifndef LEETCODE_SUDOKU_SOLVER_V3_H
#define LEETCODE_SUDOKU_SOLVER_V3_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stdint.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

#include <cstdint>
#include <cstddef>
#include <vector>
#include <bitset>

#include "SudokuSolver.h"
#include "StopWatch.h"

namespace LeetCode {
namespace Problem_37 {
namespace v3 {

class Solution {
public:
    void solveSudoku(std::vector<std::vector<char>> & board) {
        SudokuHelper::display_board(board, true);

        jtest::StopWatch sw;
        sw.start();

        //

        sw.stop();

        SudokuHelper::display_board(board);
        printf("Elapsed time: %0.3f ms\n\n", sw.getElapsedMillisec());
    }
};

} // namespace v3
} // namespace Problem_0037
} // namespace LeetCode

#endif // LEETCODE_SUDOKU_SOLVER_V3_H
