
#ifndef LEETCODE_SUDOKU_SOLVER_V5_H
#define LEETCODE_SUDOKU_SOLVER_V5_H

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

#define V5_SEARCH_ALL_STAGE     0

namespace LeetCode {
namespace Problem_37 {
namespace v5 {

template <typename T, size_t Capacity>
class SmallFixedDualList {
public:
    typedef T                               value_type;
    typedef SmallFixedDualList<T, Capacity> this_type;

    struct Node {
        int        prev;
        int        next;
        value_type value;

        Node() = default;
        Node(int prev, int next) : prev(prev), next(next) {
        }
        Node(const Node & src) = default;
        ~Node() = default;
    };

    typedef Node node_type;

    static const size_t kCapacity = Capacity + 1;

private:
    size_t           size_;
    size_t           capacity_;
    node_type        data_[kCapacity];

    void init() {
        this->data_[0].prev = -1;
        this->data_[0].next = 1;
    }

public:
    SmallFixedDualList() : size_(1), capacity_(1) {
        this->init();
    }
    ~SmallFixedDualList() {}

    int begin() const { return this->data_[0].next; }
    int end() const   { return -1; }

    int next(int index) const {
        return this->data_[index].next;
    }

    size_t size() const { return this->size_; }
    size_t capacity() const { return this->capacity_; }
    size_t max_capacity() const { return this_type::kCapacity; }

    void init_all() {
        for (int i = 0; i < this_type::kCapacity; i++) {
            this->data_[i].prev = i - 1;
            this->data_[i].next = i + 1;
        }
        this->data_[this_type::kCapacity - 1].next = -1;
    }

    void finalize() {
        this->capacity_ = this->size_ + 1;
        this->data_[this->size_ - 1].next = -1;
    }

    template <typename ... Args>
    void insert(int index, Args && ... args) {
        assert(index < this->max_capacity());
        assert(this->size_ < this->max_capacity());
        this->data_[index].prev = index - 1;
        this->data_[index].next = index + 1;
        new (&(this->data_[index].value)) value_type(std::forward<Args>(args)...);
        this->size_++;
    }

    void remove(int index) {
        assert(index < this->capacity());
        assert(this->size_ < this->capacity());
        assert(this->size_ < this->max_capacity());
        node_type & node = this->data_[index];
        this->data_[node.prev].next = node.next;
        this->data_[node.next].prev = node.prev;
        assert(this->size_ > 0);
        this->size_--;
    }

    void push_front(int index) {
        assert(index < this->capacity());
        assert(this->size_ < this->capacity());
        assert(this->size_ < this->max_capacity());
        this->data_[index].prev = 0;
        this->data_[index].next = this->data_[0].next;
        this->data_[this->data_[0].next].prev = index;
        this->data_[0].next = index;
        this->size_++;
    }

    void restore(int index) {
        assert(index < this->capacity());
        assert(this->size_ < this->capacity());
        assert(this->size_ < this->max_capacity());
        node_type & node = this->data_[index];
        this->data_[node.next].prev = index;
        this->data_[node.prev].next = index;
        this->size_++;
    }

    const value_type & operator [] (int index) const {
        assert(index < this->capacity());
        assert(index < this->max_capacity());
        return this->data_[index].value;
    };
};

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

private:
    SmallBitMatrix2<9, 9>  rows;
    SmallBitMatrix2<9, 9>  cols;
    SmallBitMatrix2<9, 9>  palaces;
    SmallBitMatrix2<81, 9> usable;

#if V5_SEARCH_ALL_STAGE
    std::vector<std::vector<std::vector<char>>> answers;
#endif

public:
    Solution() = default;
    ~Solution() = default;

    int getNextFillCell(SmallFixedDualList<Position, 81> & valid_moves) {
        assert(valid_moves.size() > 1);
        size_t minUsable = size_t(-1);
        int min_index = -1;
        for (int index = valid_moves.begin();
             index != valid_moves.end(); index = valid_moves.next(index)) {
            size_t row = valid_moves[index].row;
            size_t col = valid_moves[index].col;
            size_t numUsable = this->usable[row * 9 + col].count();
            if (numUsable < minUsable) {
                if (numUsable == 0) {
                    return -1;
                }
                else if (numUsable == 1) {
                    return index;
                }
                minUsable = numUsable;
                min_index = index;
            }
        }

        return min_index;
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
               SmallFixedDualList<Position, 81> & valid_moves) {
        if (valid_moves.size() == 1) {
#if V5_SEARCH_ALL_STAGE
            this->answers.push_back(board);
#endif
            return true;
        }

        int move_idx = getNextFillCell(valid_moves);
        if (move_idx > 0) {
            size_t row = valid_moves[move_idx].row;
            size_t col = valid_moves[move_idx].col;
            valid_moves.remove(move_idx);
            const std::bitset<9> & fillNums = this->usable[row * 9 + col];
            for (size_t num = 0; num < fillNums.size(); num++) {
                // Get usable numbers
                if (fillNums.test(num)) {
                    doFillNum(row, col, num);
                    board[row][col] = (char)(num + '1');

                    if (solve(board, valid_moves)) {
#if (V5_SEARCH_ALL_STAGE == 0)
                        return true;
#endif
                    }

                    board[row][col] = '.';
                    undoFillNum(row, col, num);
                }
            }
            valid_moves.push_front(move_idx);
        }

        return false;
    }

    void solveSudoku(std::vector<std::vector<char>> & board) {
        SudokuHelper::display_board(board, true);

        jtest::StopWatch sw;
        sw.start();

        int index = 1;
        SmallFixedDualList<Position, 81> valid_moves;
        for (size_t row = 0; row < board.size(); row++) {
            const std::vector<char> & line = board[row];
            for (size_t col = 0; col < line.size(); col++) {
                char val = line[col];
                if (val != '.') {
                    size_t num = val - '1';
                    fillNum(row, col, num);
                }
                else {
                    valid_moves.insert(index, row, col);
                    index++;
                }
            }   
        }
        valid_moves.finalize();

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

#if V5_SEARCH_ALL_STAGE
        SudokuHelper::display_answers(this->answers);
#else
        SudokuHelper::display_board(board);
#endif
        printf("Elapsed time: %0.3f ms\n\n", sw.getElapsedMillisec());
    }
};

} // namespace v5
} // namespace Problem_37
} // namespace LeetCode

#endif // LEETCODE_SUDOKU_SOLVER_V5_H
