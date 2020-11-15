
#ifndef LEETCODE_SUDOKU_SOLVER_V1_H
#define LEETCODE_SUDOKU_SOLVER_V1_H

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

#define V1_SEARCH_ALL_STAGE     0

namespace LeetCode {
namespace Problem_37 {
namespace v1 {

class DancingLinks;

class SudokuSolver {
public:
    static const size_t Rows = SudokuHelper::Rows;
    static const size_t Cols = SudokuHelper::Cols;
    static const size_t Numbers = SudokuHelper::Numbers;

    static const size_t BoardSize = Rows * Cols * Numbers;

    typedef SmallBitMatrix<BoardSize, Rows * Cols * 4> matrix_type;

private:
    matrix_type matrix;
    size_t rows[BoardSize + 1];
    size_t cols[BoardSize + 1];
    size_t numbers[BoardSize + 1];

public:
    SudokuSolver(const std::vector<std::vector<char>> & board) {
        this->init(board);
    }
    ~SudokuSolver() {}

    matrix_type & getDlkMartix() {
        return matrix;
    }

    const matrix_type & getDlkMartix() const {
        return matrix;
    }

private:
    void init(const std::vector<std::vector<char>> & board) {
        size_t empties = 0;
        for (size_t row = 0; row < Rows; row++) {
            for (size_t col = 0; col < Cols; col++) {
                if (board[row][col] == '.') {
                    empties++;
                }
            }
        }

        matrix.setRows(empties * 8 + 9 * 9);

        size_t index = 0;
        matrix.clear();
        for (size_t row = 0; row < Rows; row++) {
            for (size_t col = 0; col < Cols; col++) {
                size_t m = 0, n = Numbers - 1;
                if (board[row][col] != '.') {
                    m = n = board[row][col] - '1';
                }
                size_t palace = row / 3 * 3 + col / 3;
                for (size_t number = m; number <= n; number++) {
#if (MATRIX_BITSET_MODE == MATRIX_USE_SMALL_BITMAP) || (MATRIX_BITSET_MODE == MATRIX_USE_BITMAP)
                    matrix[index].set(81 * 0 + row * 9 + col);
                    matrix[index].set(81 * 1 + row * 9 + number);
                    matrix[index].set(81 * 2 + col * 9 + number);
                    matrix[index].set(81 * 3 + (row / 3 * 3 + col / 3) * 9 + number);
#else
                    matrix[index][81 * 0 + row * 9 + col] = 1;
                    matrix[index][81 * 1 + row * 9 + number] = 1;
                    matrix[index][81 * 2 + col * 9 + number] = 1;
                    matrix[index][81 * 3 + palace * 9 + number] = 1;
#endif
                    rows[index + 1] = row;
                    cols[index + 1] = col;
                    numbers[index + 1] = number;
                    index++;
                }
            }
        }
    }

public:
    void display_answer(std::vector<std::vector<char>> & board,
                        const DancingLinks * dancingLinks);

    void display_answers(std::vector<std::vector<char>> & board,
                         const DancingLinks * dancingLinks);
};

class DancingLinks {
private:
    struct Node {
        int prev, next;
        int up, down;
        int row, col;
    };
    
    std::vector<Node>   link_list;
    std::vector<int>    col_size;
    int                 free_idx;
    std::vector<int>    answer;
#if V1_SEARCH_ALL_STAGE
    std::vector<std::vector<int>> answers;
#endif

    struct StackInfo {
        int col;
        int i;

        StackInfo() : col(0), i(0) {}
        StackInfo(int col, int i) : col(col), i(i) {}

        void set(int col, int i) {
            this->col = col;
            this->i = i;
        }
    };

    enum StackState {
        SearchNext,
        BackTracking,
        BackTrackingRetry,
        Last
    };

public:
    DancingLinks(typename SudokuSolver::matrix_type & matrix, size_t nodes)
        : link_list(nodes), col_size(matrix.cols() + 1), free_idx((int)matrix.cols() + 1) {
        this->init(matrix);
    }

    ~DancingLinks() {}

    bool is_empty() const { return (link_list[0].next == 0); }

    const std::vector<int> &              get_answer() const  { return this->answer; }
#if V1_SEARCH_ALL_STAGE
    const std::vector<std::vector<int>> & get_answers() const { return this->answers; }
#endif

private:
    void init(const typename SudokuSolver::matrix_type & matrix) {
        int cols = (int)matrix.cols();
        for (int col = 0; col <= cols; col++) {
            link_list[col].prev = col - 1;
            link_list[col].next = col + 1;
            link_list[col].up = col;
            link_list[col].down = col;
        }
        link_list[0].prev = cols;
        link_list[cols].next = 0;

        init_from_martix(matrix);
    }

    void init_from_martix(const typename SudokuSolver::matrix_type & matrix) {
        int rows = (int)matrix.rows();
        int cols = (int)matrix.cols();
        for (int row = rows - 1; row >= 0; row--) {
            int head = free_idx, tail = free_idx;
            for (int col = 0; col < cols; col++) {
                if (matrix[row].test(col)) {
                    tail = this->insert(head, tail, row + 1, col + 1);
                }
            }
        }
    }

public:
    int insert(int head, int tail, int row, int col) {
        link_list[free_idx].prev = tail;
        link_list[free_idx].next = head;
        link_list[free_idx].up = col;
        link_list[free_idx].down = link_list[col].down;
        link_list[free_idx].row = row;
        link_list[free_idx].col = col;

#if 1
        link_list[tail].next = free_idx;
        link_list[head].prev = free_idx;
        link_list[col].down = free_idx;
        link_list[link_list[free_idx].down].up = free_idx;
#else
        link_list[link_list[free_idx].prev].next = free_idx;
        link_list[link_list[free_idx].next].prev = free_idx;
        link_list[link_list[free_idx].up].down = free_idx;
        link_list[link_list[free_idx].down].up = free_idx;
#endif
        col_size[col]++;
        tail = free_idx++;
        return tail;
    }

    void erase(int col) {
        link_list[link_list[col].prev].next = link_list[col].next;
        link_list[link_list[col].next].prev = link_list[col].prev;

        for (int i = link_list[col].down; i != col; i = link_list[i].down) {
            for (int j = link_list[i].next; j != i; j = link_list[j].next) {
                link_list[link_list[j].up].down = link_list[j].down;
                link_list[link_list[j].down].up = link_list[j].up;
                col_size[link_list[j].col]--;
            }
        }
    }

    void recover(int col) {
        for (int i = link_list[col].up; i != col; i = link_list[i].up) {
            for (int j = link_list[i].prev; j != i; j = link_list[j].prev) {
                link_list[link_list[j].down].up = j;
                link_list[link_list[j].up].down = j;
                col_size[link_list[j].col]++;
            }
        }

        link_list[link_list[col].next].prev = col;
        link_list[link_list[col].prev].next = col;
    }

    int get_min_column() const {
        int min_col = link_list[0].next;
        for (int i = link_list[0].next; i != 0 ; i = link_list[i].next) {
            if (col_size[i] < col_size[min_col])
                min_col = i;
        }
        return min_col;
    }

    bool solve() {
        if (this->is_empty()) {
#if V1_SEARCH_ALL_STAGE
            this->answers.push_back(answer);
#endif
            return true;
        }
        
        int col = get_min_column();
        this->erase(col);
        for (int i = link_list[col].down; i != col; i = link_list[i].down) {
            answer.push_back(link_list[i].row);
            for (int j = link_list[i].next; j != i; j = link_list[j].next) {
                this->erase(link_list[j].col);
            }

            if (solve()) {
#if (V1_SEARCH_ALL_STAGE == 0)
                return true;
#endif
            }

            for (int j = link_list[i].prev; j != i; j = link_list[j].prev) {
                this->recover(link_list[j].col);
            }
            answer.pop_back();
        }
        this->recover(col);
        return false;
    }

    bool solve_non_recursive() {
        int state = StackState::SearchNext;
        std::vector<StackInfo> stack;
        StackInfo stack_info;
        int i, col;

        do {       
            if (state == StackState::SearchNext) {
Search_Next:
                if (this->is_empty()) {
#if V1_SEARCH_ALL_STAGE
                    this->answers.push_back(answer);
                    state = StackState::BackTracking;
                    goto BackTracking_Entry;
#else
                    return true;
#endif
                }

                col = get_min_column();
                this->erase(col);

                i = link_list[col].down;

                while (i != col) {
                    stack_info.set(col, i);
                    stack.push_back(stack_info);
                    answer.push_back(link_list[i].row);

                    for (int j = link_list[i].next; j != i; j = link_list[j].next) {
                        this->erase(link_list[j].col);
                    }

                    // SearchNext
                    goto Search_Next;
                }
            }
            else if (state == StackState::BackTrackingRetry) {
BackTracking_Retry:
                while (i != col) {
                    stack_info.set(col, i);
                    stack.push_back(stack_info);
                    answer.push_back(link_list[i].row);
                    for (int j = link_list[i].next; j != i; j = link_list[j].next) {
                        this->erase(link_list[j].col);
                    }

                    state = StackState::SearchNext;
                    goto Search_Next;
                }
            }
            else {
                // StackState::BackTracking
BackTracking_Entry:
                answer.pop_back();
                stack_info = stack.back();
                stack.pop_back();
                i = stack_info.i;
                col = stack_info.col;

                for (int j = link_list[i].prev; j != i; j = link_list[j].prev) {
                    this->recover(link_list[j].col);
                }

                i = link_list[i].down;

                state = StackState::BackTrackingRetry;
                goto BackTracking_Retry;
            }

            this->recover(col);

            if (stack.size() != 0) {
                state = StackState::BackTracking;
                goto BackTracking_Entry;
            }
            else break;
        } while (1);

        return false;
    }
};

class Solution {
public:
    void solveSudoku(std::vector<std::vector<char>> & board);
};

} // namespace v1
} // namespace Problem_37
} // namespace LeetCode

#endif // LEETCODE_SUDOKU_SOLVER_V1_H
