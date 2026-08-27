#include "search.h"
#include "evaluate.h"
#include "movegen.h"
#include <limits.h>
#include "utils.h"

int timeLimit = 100000000;


int negaMax(Position* pos, int depth, int alpha, int beta, NodeState* state) {
    state->nodes++;

    if (state->nodes % 2048 == 0) {
        if ((getTimeMS() - state->startTime) >= timeLimit) {
            state->abort = 1;
        }
    }

    if (state->abort) {
        return 0;
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

    if (depth == 0) {
        return evaluateLegalPos(pos);
    }

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

        if (score > alpha) {
            alpha = score;
        }

        if (alpha >= beta) {
            // cutoff
            break;
        }

    }
    return alpha;
}

void searchPosition(Position* pos, int searchTimeLimit) {
    // iterative deepening
    NodeState state;
    state.ply = 0;
    state.nodes = 0;
    state.startTime = getTimeMS();
    state.abort = 0;

    Move bestMoveSoFar = 0;

    MoveList legalMoves;
    generateLegalMoves(pos, &legalMoves);
    timeLimit = searchTimeLimit;

    Undo undo;
    for (int j = 1; j < MAX_SEARCH_DEPTH; j++) {

        int maxScore = INT_MIN;
        int alpha = INT_MIN;
        int beta = INT16_MAX;
        Move bestRootMoveThisDepth = 0;

        for (int i = 0; i < legalMoves.size; i++) {
            makeMove(pos, legalMoves.moves[i], &undo);
            state.ply++;
            int score = -negaMax(pos, j - 1, -beta, -alpha, &state);
            state.ply--;
            unmakeMove(pos, legalMoves.moves[i], &undo);

            if (score > maxScore) {
                maxScore = score;
                bestRootMoveThisDepth = legalMoves.moves[i];
            }
        }
        if (state.abort) {
            break; // don't use partial results
        }
        bestMoveSoFar = bestRootMoveThisDepth;
    }
    printf("bestmove %s\n", moveToStr(bestMoveSoFar));
}