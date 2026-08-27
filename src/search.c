#include "search.h"
#include "evaluate.h"
#include "movegen.h"
#include <limits.h>


int negaMax(Position* pos, int depth, int alpha, int beta, NodeState* state) {
    state->nodes++;

    MoveList legalMoves;
    generateLegalMoves(pos, &legalMoves);

    if (legalMoves.size == 0) {
        // checkmate or stalemate
        int kingsq = __builtin_ctzll(pos->pieces[(pos->stm == WHITE) ? WHITE_KING : BLACK_KING]);
        if (isSquareAttacked(pos, kingsq, pos->xstm)) {
            return -MATE_SCORE - state->ply;
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

Move searchPosition(Position* pos, int depth) {
    NodeState state;
    state.ply = 0;
    state.nodes = 0;

    Move bestRootMove = 0;
    int maxScore = INT_MIN;
    int alpha = INT_MIN;
    int beta = INT16_MAX;

    MoveList legalMoves;
    generateLegalMoves(pos, &legalMoves);

    Undo undo;

    for (int i = 0; i < legalMoves.size; i++) {
        makeMove(pos, legalMoves.moves[i], &undo);
        state.ply++;
        int score = -negaMax(pos, depth - 1, alpha, beta, &state);
        state.ply--;
        unmakeMove(pos, legalMoves.moves[i], &undo);

        if (score > maxScore) {
            maxScore = score;
            bestRootMove = legalMoves.moves[i];
        }
    }
    return bestRootMove;
}