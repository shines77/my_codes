
#ifndef LEETCODE_SUDOKU_SOLVER_V4_H
#define LEETCODE_SUDOKU_SOLVER_V4_H

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
#include <list>
#include <bitset>

#include "SudokuSolver.h"
#include "StopWatch.h"

#define V4_SEARCH_ALL_STAGE     0

namespace LeetCode {
namespace Problem_37 {
namespace v4 {

class Solution {
public:
    static const size_t Rows = SudokuHelper::Rows;
    static const size_t Cols = SudokuHelper::Cols;
    static const size_t Numbers = SudokuHelper::Numbers;

    struct Position {
        size_t row;
        size_t col;

        Position() = default;
        Position(size_t row, size_t col) : row(row), col(col) {};
        ~Position() = default;
    };

    typedef typename std::list<Position>::const_iterator cmove_iterator;

private:
    SmallBitMatrix2<9, 9>  rows;
    SmallBitMatrix2<9, 9>  cols;
    SmallBitMatrix2<9, 9>  palaces;
    SmallBitMatrix2<81, 9> usable;

#if V4_SEARCH_ALL_STAGE
    std::vector<std::vector<std::vector<char>>> answers;
#endif

public:
    Solution() = default;
    ~Solution() = default;

    bool getNextFillCell(std::list<Position> & valid_moves,
                         cmove_iterator & out_iter) {
        assert(valid_moves.size() > 0);
        size_t minUsable = size_t(-1);
        cmove_iterator min_iter;
        for (cmove_iterator iter = valid_moves.cbegin();
             iter != valid_moves.cend(); ++iter) {
            size_t row = iter->row;
            size_t col = iter->col;
            size_t numUsable = this->usable[row * 9 + col].count();
            if (numUsable < minUsable) {
                if (numUsable == 0) {
                    return false;
                }
                else if (numUsable == 1) {
                    out_iter = iter;
                    return true;
                }
                minUsable = numUsable;
                min_iter = iter;
            }
        }

        out_iter = min_iter;
        return true;
    }

    std::bitset<9> getUsable(size_t row, size_t col) {
        size_t palace = row / 3 * 3 + col / 3;
        return ~(this->rows[row] | this->cols[col] | this->palaces[palace]);
    }

    std::bitset<9> getUsable(size_t row, size_t col, size_t palace) {
        return ~(this->rows[row] | this->cols[col] | this->palaces[palace]);
    }

    void updateUsable(size_t row, size_t col, size_t num) {
        size_t cell_y = row * 9;
        for (size_t x = 0; x < Cols; x++) {
            if (x != col) {
                this->usable[cell_y + x].reset(num);
            }
        }

        size_t cell_x = col;
        for (size_t y = 0; y < Rows; y++) {
            if (y != row) {
                this->usable[y * 9 + cell_x].reset(num);
            }
        }

        size_t palace_row = row / 3 * 3;
        size_t palace_col = col / 3;
        size_t cell = cell_y + cell_x;
        palace_col *= 3;
        size_t pos = palace_row * 9 + palace_col;
        for (size_t y = 0; y < (Rows / 3); y++) {
            for (size_t x = 0; x < (Cols / 3); x++) {
                if (pos != cell) {
                    this->usable[pos].reset(num);
                }
                pos++;
            }
            pos += (9 - 3);
        }
    }

    template <bool isUndo = true>
    void updateUndoUsable(size_t row, size_t col) {
        size_t cell_y = row * 9;
        size_t palace_row = row / 3 * 3;
        for (size_t x = 0; x < Cols; x++) {
            if (isUndo || x != col) {
                size_t palace = palace_row + x / 3;
                this->usable[cell_y + x] = getUsable(row, x, palace);
            }
        }

        size_t cell_x = col;
        size_t palace_col = col / 3;
        for (size_t y = 0; y < Rows; y++) {
            if (y != row) {
                size_t palace = y / 3 * 3 + palace_col;
                this->usable[y * 9 + cell_x] = getUsable(y, col, palace);
            }
        }

        size_t palace = palace_row + palace_col;
        size_t cell = cell_y + cell_x;
        palace_col *= 3;
        size_t pos = palace_row * 9 + palace_col;
        for (size_t y = 0; y < (Rows / 3); y++) {
            for (size_t x = 0; x < (Cols / 3); x++) {
                if (pos != cell) {
                    this->usable[pos] = getUsable(palace_row + y, palace_col + x, palace);
                }
                pos++;
            }
            pos += (9 - 3);
        }
    }

    void fillNum(size_t row, size_t col, size_t num) {
        size_t palace = row / 3 * 3 + col / 3;
        this->rows[row].set(num);
        this->cols[col].set(num);
        this->palaces[palace].set(num);
    }

    void doFillNum(size_t row, size_t col, size_t num) {
        size_t palace = row / 3 * 3 + col / 3;
        this->rows[row].set(num);
        this->cols[col].set(num);
        this->palaces[palace].set(num);
        updateUsable(row, col, num);
    }

    void undoFillNum(size_t row, size_t col, size_t num) {
        size_t palace = row / 3 * 3 + col / 3;
        this->rows[row].reset(num);
        this->cols[col].reset(num);
        this->palaces[palace].reset(num);
        updateUndoUsable<true>(row, col);
    }

    bool solve(std::vector<std::vector<char>> & board,
               std::list<Position> & valid_moves) {
        if (valid_moves.size() == 0) {
#if V4_SEARCH_ALL_STAGE
            this->answers.push_back(board);
#endif
            return true;
        }

        cmove_iterator moveIter;
        bool hasMoves = getNextFillCell(valid_moves, moveIter);
        if (hasMoves) {
            size_t row = moveIter->row;
            size_t col = moveIter->col;
            valid_moves.erase(moveIter);
            const std::bitset<9> & fillNums = this->usable[row * 9 + col];
            for (size_t num = 0; num < fillNums.size(); num++) {
                // Get usable numbers
                if (fillNums.test(num)) {
                    doFillNum(row, col, num);
                    board[row][col] = (char)(num + '1');

                    if (solve(board, valid_moves)) {
#if (V4_SEARCH_ALL_STAGE == 0)
                        return true;
#endif
                    }

                    board[row][col] = '.';
                    undoFillNum(row, col, num);
                }
            }
            valid_moves.emplace_front(row, col);
        }

        return false;
    }

    void solveSudoku(std::vector<std::vector<char>> & board) {
        SudokuHelper::display_board(board, true);

        jtest::StopWatch sw;
        sw.start();

        std::list<Position> valid_moves;
        for (size_t row = 0; row < board.size(); row++) {
            const std::vector<char> & line = board[row];
            for (size_t col = 0; col < line.size(); col++) {
                char val = line[col];
                if (val != '.') {
                    size_t num = val - '1';
                    fillNum(row, col, num);
                }
                else {
                    valid_moves.emplace_back(row, col);
                }
            }   
        }

        for (size_t row = 0; row < board.size(); row++) {
            const std::vector<char> & line = board[row];
            for (size_t col = 0; col < line.size(); col++) {
                char val = line[col];
                if (val == '.') {
                    this->usable[row * 9 + col] = getUsable(row, col);
                }
            }
        }

        this->solve(board, valid_moves);

        sw.stop();

#if V4_SEARCH_ALL_STAGE
        SudokuHelper::display_answers(this->answers);
#else
        SudokuHelper::display_board(board);
#endif
        printf("Elapsed time: %0.3f ms\n\n", sw.getElapsedMillisec());
    }
};

} // namespace v4
} // namespace Problem_37
} // namespace LeetCode

#endif // LEETCODE_SUDOKU_SOLVER_V4_H
