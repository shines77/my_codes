
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
#include "SudokuSolver_v4.h"
#include "SudokuSolver_v5.h"
#include "SudokuSolver_v6.h"

#include "ice_sudoku_solver.h"

#include "CPUWarmUp.h"

// Level: [0 - 4]
#define DIFFICULTY_LEVEL        2

static const char test_board[5][81] = {
    //
    // 0 #
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
    // 1 #
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
    // 2 #
    //
    // Hard (filled = 21)
    // https://github.com/tropicalwzc/ice_sudoku.github.io/blob/master/sudoku_solver.c
    //
    '5', '.', '.',  '.', '.', '.',  '3', '.', '.',
    '.', '2', '.',  '1', '.', '.',  '.', '7', '.',
    '.', '.', '8',  '.', '.', '.',  '.', '.', '9',

    '.', '4', '.',  '.', '.', '7',  '.', '.', '.',
    '.', '.', '.',  '8', '2', '1',  '.', '.', '.',
    '.', '.', '.',  '6', '.', '.',  '.', '1', '.',

    '3', '.', '.',  '.', '.', '.',  '8', '.', '.',
    '.', '6', '.',  '.', '.', '4',  '.', '2', '.',
    '.', '.', '9',  '.', '.', '.',  '.', '.', '5',

    // Ice suduku string: 500000300020100070008000009040007000000821000000600010300000800060004020009000005

    //
    // 3 #
    //
    // Hardcore (filled = 17)
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
    '.', '.', '.',  '.', '.', '.',  '.', '.', '.',

    // Ice suduku string: 500000000010000320000840000000000000000003100608500000007000068034001000000000000

    //
    // 4 #
    //
    // Hardcore (filled = 21)
    // http://news.sohu.com/20130527/n377158555.shtml
    // https://baike.baidu.com/reference/13848819/1bf4HJzRCPCNz9Rypz3HpTtnhc2MpcRr5JMIp0032uiuKPQm4eOMuq2WZWxf77V-UBRjIkyDf9CVZDEjlDeHJBaazlstk30qaDtt
    //
    '8', '.', '.',  '.', '.', '.',  '.', '.', '.',
    '.', '.', '3',  '6', '.', '.',  '.', '.', '.',
    '.', '7', '.',  '.', '9', '.',  '2', '.', '.',

    '.', '5', '.',  '.', '.', '7',  '.', '.', '.',
    '.', '.', '.',  '.', '4', '5',  '7', '.', '.',
    '.', '.', '.',  '1', '.', '.',  '.', '3', '.',

    '.', '.', '1',  '.', '.', '.',  '.', '6', '8',
    '.', '.', '8',  '5', '.', '.',  '.', '1', '.',
    '.', '9', '.',  '.', '.', '.',  '4', '.', '.',

    /*************************************************

    '.', '.', '.',  '.', '.', '.',  '.', '.', '.',
    '.', '.', '.',  '.', '.', '.',  '.', '.', '.',
    '.', '.', '.',  '.', '.', '.',  '.', '.', '.',

    '.', '.', '.',  '.', '.', '.',  '.', '.', '.',
    '.', '.', '.',  '.', '.', '.',  '.', '.', '.',
    '.', '.', '.',  '.', '.', '.',  '.', '.', '.',

    '.', '.', '.',  '.', '.', '.',  '.', '.', '.',
    '.', '.', '.',  '.', '.', '.',  '.', '.', '.',
    '.', '.', '.',  '.', '.', '.',  '.', '.', '.',

    **************************************************/
};

int main(int argc, char * argv[])
{
    using namespace LeetCode::Problem_37;

    jtest::CPU::warmup(1000);

#ifdef NDEBUG
    if (1)
    {
        printf("--------------------------------\n\n");
        printf("Sudoku: v1::Solution - Dancing Links\n\n");

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

    if (1)
    {
        printf("--------------------------------\n\n");
        printf("Sudoku: v2::Solution - Depth first search\n\n");

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

    if (1)
    {
        printf("--------------------------------\n\n");
        printf("Sudoku: v3::Solution - Depth first search\n\n");

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

    if (1)
    {
        printf("--------------------------------\n\n");
        printf("Sudoku: v4::Solution - Depth first search\n\n");

        std::vector<std::vector<char>> board;
        for (size_t row = 0; row < SudokuHelper::Rows; row++) {
            std::vector<char> line;
            for (size_t col = 0; col < SudokuHelper::Cols; col++) {
                line.push_back(test_board[DIFFICULTY_LEVEL][row * 9 + col]);
            }
            board.push_back(line);
        }

        v4::Solution solution;
        solution.solveSudoku(board);
    }
#endif

    if (1)
    {
        printf("--------------------------------\n\n");
        printf("Sudoku: v5::Solution - Depth first search\n\n");

        std::vector<std::vector<char>> board;
        for (size_t row = 0; row < SudokuHelper::Rows; row++) {
            std::vector<char> line;
            for (size_t col = 0; col < SudokuHelper::Cols; col++) {
                line.push_back(test_board[DIFFICULTY_LEVEL][row * 9 + col]);
            }
            board.push_back(line);
        }

        v5::Solution solution;
        solution.solveSudoku(board);
    }

    if (1)
    {
        printf("--------------------------------\n\n");
        printf("Sudoku: v6::Solution - Depth first search\n\n");

        std::vector<std::vector<char>> board;
        for (size_t row = 0; row < SudokuHelper::Rows; row++) {
            std::vector<char> line;
            for (size_t col = 0; col < SudokuHelper::Cols; col++) {
                line.push_back(test_board[DIFFICULTY_LEVEL][row * 9 + col]);
            }
            board.push_back(line);
        }

        v6::Solution solution;
        solution.solveSudoku(board);
    }

    if (0)
    {
        printf("--------------------------------\n\n");
        printf("Sudoku: ice suduku\n\n");

        int sudo_in[9][9];
        int sudo_answer[10][9][9];
        for (size_t row = 0; row < SudokuHelper::Rows; row++) {
            for (size_t col = 0; col < SudokuHelper::Cols; col++) {
                char val = test_board[DIFFICULTY_LEVEL][row * 9 + col];
                if (val == '.')
                    sudo_in[row][col] = 0;
                else
                    sudo_in[row][col] = val - '0';
            }
        }

        jtest::StopWatch sw;
        sw.start();

        solve_sudoku(sudo_answer, sudo_in);

        sw.stop();

        print_a_sudoku(sudo_answer);

        printf("\n");
        printf("Elapsed time: %0.3f ms\n\n", sw.getElapsedMillisec());
    }

    printf("--------------------------------\n\n");

#if !defined(NDEBUG) && defined(_MSC_VER)
    ::system("pause");
#endif

    return 0;
}
