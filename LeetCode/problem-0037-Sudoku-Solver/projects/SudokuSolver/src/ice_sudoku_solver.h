
#ifndef ICE_SUDOKU_SOLVER_H
#define ICE_SUDOKU_SOLVER_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

void print_a_sudoku(int sudoku[10][9][9]);

void solve_sudoku(int sudo[10][9][9], int sudoext[9][9]);

#ifdef __cplusplus
}
#endif

#endif // ICE_SUDOKU_SOLVER_H
