#include "search.h"
#include "evaluate.h"
#include "movegen.h"
#include <limits.h>
#include "utils.h"
#include "movepicker.h"

int timeLimit = 100000000;

int quiescence(Position* pos, int alpha, int beta, SearchState* state) {
    state->nodes++;

    if (state->nodes % 2048 == 0) {
        if ((getTimeMS() - state->startTime) >= timeLimit) {
            state->abort = 1;
        }
    }

    if (state->abort) {
        return 0;
    }

    int staticEval = evaluateLegalPos(pos);
    int bestValue = staticEval;
    if (bestValue >= beta) {
        return bestValue;
    }
    if (bestValue > alpha) {
        alpha = bestValue;
    }

    MoveList noisyMoves;
    noisyMoves.size = 0;
    generateNoisyMoves(pos, &noisyMoves);

    orderMoves(&noisyMoves, pos);

    Undo undo;
    for (int i = 0; i < noisyMoves.size; i++) {
        makeMove(pos, noisyMoves.moves[i], &undo);
         if (!moveWasLegal(pos)) {
            unmakeMove(pos, noisyMoves.moves[i], &undo);
            continue;
        }
        state->ply++;
        int score = -quiescence(pos, -beta, -alpha, state);
        state->ply--;
        unmakeMove(pos, noisyMoves.moves[i], &undo);

        if (state->abort) {
            return 0;
        }

        if (score > bestValue) {
            bestValue = score;
        }

        if (score > alpha) {
            alpha = score;
        }

        if (alpha >= beta) {
            // cutoff
            break;
        }
    }
    return bestValue;
}


int negaMax(Position* pos, int depth, int alpha, int beta, SearchState* state) {
    state->nodes++;

    if (state->nodes % 2048 == 0) {
        if ((getTimeMS() - state->startTime) >= timeLimit) {
            state->abort = 1;
        }
    }

    if (state->abort) {
        return 0;
    }

    if (depth == 0) {
        return evaluateLegalPos(pos); //quiescence(pos, alpha, beta, state);
    }

    MoveList legalMoves;
    generateLegalMoves(pos, &legalMoves);

    if (legalMoves.size == 0) {
        // checkmate or stalemate
        int kingsq = __builtin_ctzll(pos->pieces[(pos->stm == WHITE) ? WHITE_KING : BLACK_KING]);
        if (isSquareAttacked(pos, kingsq, pos->xstm)) {
            return -MATE_SCORE + state->ply;
        }
        return 0; // stalemate
    }

    int bestValue = -INFINITY_SCORE;

    orderMoves(&legalMoves, pos);

    Undo undo;
    for (int i = 0; i < legalMoves.size; i++) {
        makeMove(pos, legalMoves.moves[i], &undo);
        state->ply++;
        int score = -negaMax(pos, depth - 1, -beta, -alpha, state);
        state->ply--;
        unmakeMove(pos, legalMoves.moves[i], &undo);

        if (state->abort) {
            return 0;
        }

        if (score > bestValue) {
            bestValue = score;
        }

        if (score > alpha) {
            alpha = score;
        }

        if (alpha >= beta) {
            // cutoff
            break;
        }

    }
    return bestValue;
}

void iterativeDeepening(Position* pos, int maxDepth, int searchTimeLimit) {
    SearchState state;
    state.ply = 0;
    state.nodes = 0;
    state.startTime = getTimeMS();
    state.abort = 0;

    timeLimit = (searchTimeLimit > 0) ? searchTimeLimit : 99999999;
    maxDepth = (maxDepth > 0) ? maxDepth : MAX_SEARCH_DEPTH;

    MoveList legalMoves;
    generateLegalMoves(pos, &legalMoves);

    if (legalMoves.size == 0) {
        printf("bestmove 0000\n");
        return;
    }

    orderMoves(&legalMoves, pos);
    Move bestMoveSoFar = legalMoves.moves[0];

    Undo undo;
    for (int j = 1; j <= maxDepth; j++) {

        int alpha = -INFINITY_SCORE;
        int beta = INFINITY_SCORE;
        Move bestRootMoveThisDepth = legalMoves.moves[0];

        for (int i = 0; i < legalMoves.size; i++) {
            makeMove(pos, legalMoves.moves[i], &undo);
            state.ply++;
            int score = -negaMax(pos, j - 1, -beta, -alpha, &state);
            state.ply--;
            unmakeMove(pos, legalMoves.moves[i], &undo);

            if (score > alpha) {
                alpha = score;
                bestRootMoveThisDepth = legalMoves.moves[i];
            }
        }
        if (state.abort) {
            break; // don't use partial results
        }
        bestMoveSoFar = bestRootMoveThisDepth;
        long long duration = getTimeMS() - state.startTime;
        printf("info depth %d score cp %d time %lld nodes %lld\n", j, alpha, duration, state.nodes);

    }
    printf("bestmove %s\n", moveToStr(bestMoveSoFar));
}