
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
    //

private:
    BitMartix<9, 9>  rows;
    BitMartix<9, 9>  cols;
    BitMartix<9, 9>  used;
    BitMartix<9, 9>  filled;
    BitMartix<81, 9> canFills;

public:
    Solution() {
        this->rows.clear();
        this->cols.clear();
        this->used.clear();
        this->filled.clear();
        this->canFills.set();
    }

    ~Solution() = default;

    void doFillNum(size_t row, size_t col, size_t num) {
        size_t palace = row / 3 * 3 + col / 3;
        size_t pos = (row % 3) * 3 + (col % 3);
        this->rows[row].set(num);
        this->cols[col].set(num);
        this->used[palace].set(num);
        this->filled[palace].set(pos);
        this->canFills[row * 9 + col].reset();
    }

    void undoFillNum(size_t row, size_t col, size_t num) {
        size_t palace = row / 3 * 3 + col / 3;
        size_t pos = (row % 3) * 3 + (col % 3);
        this->rows[row].reset(num);
        this->cols[col].reset(num);
        this->used[palace].reset(num);
        this->filled[palace].reset(pos);
        this->canFills[row * 9 + col].set();
    }

    void solveSudoku(std::vector<std::vector<char>> & board) {
        SudokuHelper::display_board(board, true);

        jtest::StopWatch sw;
        sw.start();

        for (size_t row = 0; row < SudokuHelper::Rows; row++) {
            for (size_t col = 0; col < SudokuHelper::Cols; col++) {
                char val = board[row][col];
                if (val != '.') {
                    size_t num = val - '1';
                    doFillNum(row, col, num);
                }
            }   
        }

        sw.stop();

        SudokuHelper::display_board(board);
        printf("Elapsed time: %0.3f ms\n\n", sw.getElapsedMillisec());
    }
};

} // namespace v3
} // namespace Problem_0037
} // namespace LeetCode

#endif // LEETCODE_SUDOKU_SOLVER_V3_H
