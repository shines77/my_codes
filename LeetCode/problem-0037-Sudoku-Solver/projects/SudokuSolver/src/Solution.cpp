
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

#include <cstdint>
#include <cstddef>
#include <vector>
#include <bitset>

#include "SudokuSolver_v1.h"
#include "SudokuSolver_v2.h"
#include "SudokuSolver_v3.h"

#include "CPUWarmUp.h"

#define DIFFICULTY_LEVEL        1

static const char test_board[3][81] = {
    //
    // Normal (filled = 30)
    // https://leetcode-cn.com/problems/sudoku-solver/
    //
    '5', '3', '.',  '.', '7', '.',  '.', '.', '.',
    '6', '.', '.',  '1', '9', '5',  '.', '.', '.',
    '.', '9', '8',  '.', '.', '.',  '.', '6', '.',

    '8', '.', '.',  '.', '6', '.',  '.', '.', '3',
    '4', '.', '.',  '8', '.', '3',  '.', '.', '1',
    '7', '.', '.',  '.', '2', '.',  '.', '.', '6',

    '.', '6', '.',  '.', '.', '.',  '2', '8', '.',
    '.', '.', '.',  '4', '1', '9',  '.', '.', '5',
    '.', '.', '.',  '.', '8', '.',  '.', '7', '9',

    //
    // Advance (filled = 24)
    // https://www.sudoku-cn.com/
    //
    '4', '.', '2',  '.', '.', '.',  '9', '.', '.',
    '.', '.', '.',  '.', '6', '1',  '.', '.', '.',
    '.', '1', '9',  '.', '.', '.',  '.', '.', '.',

    '7', '.', '5',  '.', '.', '.',  '6', '.', '.',
    '2', '.', '4',  '7', '.', '.',  '.', '.', '5',
    '.', '.', '.',  '.', '9', '.',  '7', '.', '.',

    '.', '8', '.',  '2', '.', '9',  '.', '1', '.',
    '.', '.', '7',  '.', '.', '4',  '.', '.', '.',
    '.', '.', '.',  '.', '.', '.',  '.', '5', '2',

    //
    // Hard (filled = 17)
    // http://www.cn.sudokupuzzle.org/play.php
    //
    '5', '.', '.',  '.', '.', '.',  '.', '.', '.',
    '.', '1', '.',  '.', '.', '.',  '3', '2', '.',
    '.', '.', '.',  '8', '4', '.',  '.', '.', '.',

    '.', '.', '.',  '.', '.', '.',  '.', '.', '.',
    '.', '.', '.',  '.', '.', '3',  '1', '.', '.',
    '6', '.', '8',  '5', '.', '.',  '.', '.', '.',

    '.', '.', '7',  '.', '.', '.',  '.', '6', '8',
    '.', '3', '4',  '.', '.', '1',  '.', '.', '.',
    '.', '.', '.',  '.', '.', '.',  '.', '.', '.'
};

int main(int argn, char * argv[])
{
    using namespace LeetCode::Problem_37;

    jtest::CPU::warmup(1000);

#ifdef NDEBUG
    {
        printf("--------------------------------\n\n");
        printf("Sudoku: v1::Solution - Dancing Links\n\n");

        // Test case
        std::vector<std::vector<char>> board;
        for (size_t row = 0; row < SudokuHelper::Rows; row++) {
            std::vector<char> line;
            for (size_t col = 0; col < SudokuHelper::Cols; col++) {
                line.push_back(test_board[DIFFICULTY_LEVEL][row * 9 + col]);
            }
            board.push_back(line);
        }

        v1::Solution solution;
        solution.solveSudoku(board);
    }

    {
        printf("--------------------------------\n\n");
        printf("Sudoku: v2::Solution - Depth first search\n\n");

        // Test case
        std::vector<std::vector<char>> board;
        for (size_t row = 0; row < SudokuHelper::Rows; row++) {
            std::vector<char> line;
            for (size_t col = 0; col < SudokuHelper::Cols; col++) {
                line.push_back(test_board[DIFFICULTY_LEVEL][row * 9 + col]);
            }
            board.push_back(line);
        }

        v2::Solution solution;
        solution.solveSudoku(board);
    }
#endif

#if 1
    {
        printf("--------------------------------\n\n");
        printf("Sudoku: v3::Solution - Depth first search\n\n");

        // Test case
        std::vector<std::vector<char>> board;
        for (size_t row = 0; row < SudokuHelper::Rows; row++) {
            std::vector<char> line;
            for (size_t col = 0; col < SudokuHelper::Cols; col++) {
                line.push_back(test_board[DIFFICULTY_LEVEL][row * 9 + col]);
            }
            board.push_back(line);
        }

        v3::Solution solution;
        solution.solveSudoku(board);
    }
#endif

#if !defined(NDEBUG) && defined(_MSC_VER)
    ::system("pause");
#endif

    return 0;
}
