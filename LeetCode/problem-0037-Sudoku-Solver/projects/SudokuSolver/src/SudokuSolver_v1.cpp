
#include "SudokuSolver_v1.h"

#include <stdio.h>
#include <stdlib.h>

#include "StopWatch.h"

using namespace LeetCode::Problem_37::v1;

void SudokuSolver::display_answer(std::vector<std::vector<char>> & board,
                                  const DancingLinks * dancingLinks) {
    for (auto idx : dancingLinks->get_answer()) {
        board[rows[idx]][cols[idx]] = (char)numbers[idx] + '1';
    }

    SudokuHelper::display_board(board);
}

#if V1_SEARCH_ALL_STAGE
void SudokuSolver::display_answers(std::vector<std::vector<char>> & board,
                                   const DancingLinks * dancingLinks) {
    printf("Total answers: %d\n\n", (int)dancingLinks->get_answers().size());
    int i = 0;
    for (auto answer : dancingLinks->get_answers()) {
        for (auto idx : answer) {
            board[rows[idx]][cols[idx]] = (char)numbers[idx] + '1';
        }
        SudokuHelper::display_board(board, false, i);
        i++;
    }
}
#endif

void Solution::solveSudoku(std::vector<std::vector<char>> & board)
{
    SudokuHelper::display_board(board, true);

    jtest::StopWatch sw;
    sw.start();

    SudokuSolver solver(board);

    DancingLinks dancingLinks(solver.getDlkMartix(), SudokuSolver::BoardSize * 4 + 1);
    //dancingLinks.solve_non_recursive();
    dancingLinks.solve();

    sw.stop();

#if V1_SEARCH_ALL_STAGE
    solver.display_answers(board, &dancingLinks);
#else
    solver.display_answer(board, &dancingLinks);
#endif
    printf("Elapsed time: %0.3f ms\n\n", sw.getElapsedMillisec());
}
