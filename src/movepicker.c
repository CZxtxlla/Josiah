#include "movepicker.h"
#include "evaluate.h"

int scoreMove(Move m, Position* pos, Move ttMove, SearchState* state) {
    if (m == ttMove) {
        return 100000;
    }
    if (IsCapture(m)) {
        int capturer = pos->squares[MoveFrom(m)] % 6;
        int captured = IsEP(m) ? PAWN : (pos->squares[MoveTo(m)] % 6);

        return (10 * PIECE_TO_SCORE[captured]) - PIECE_TO_SCORE[capturer];
    } 
    
    return state->history[pos->stm][MoveFrom(m)][MoveTo(m)];
}

void orderMoves(MoveList* movesl, Position* pos, Move ttMove, SearchState* state) {
    int scores[256];

    for (int i = 0; i < movesl->size; i++) {
        scores[i] = scoreMove(movesl->moves[i], pos, ttMove, state);
    }

    for (int i = 0; i < movesl->size - 1; i++) {
        int max_idx = i;

        for (int j = i + 1; j < movesl->size; j++) {
            if (scores[j] > scores[max_idx]) {
                max_idx = j;
            }
        }

        Move tempMove = movesl->moves[i];
        movesl->moves[i] = movesl->moves[max_idx];
        movesl->moves[max_idx] = tempMove;

        int tempScore = scores[i];
        scores[i] = scores[max_idx];
        scores[max_idx] = tempScore;
    }
}