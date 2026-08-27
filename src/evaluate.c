#include "evaluate.h"

const int PIECE_TO_SCORE[5] = {100, 300, 320, 500, 900};

int evaluateLegalPos(Position* pos) {
    // from perspective of stm
    int white_score = 0;
    for (int p_type = WHITE_PAWN; p_type <= WHITE_QUEEN; p_type++) {
        white_score += PIECE_TO_SCORE[p_type % 6] * __builtin_popcountll(pos->pieces[p_type]);
    }
    for (int p_type = BLACK_PAWN; p_type <= BLACK_QUEEN; p_type++) {
        white_score -= PIECE_TO_SCORE[p_type % 6] * __builtin_popcountll(pos->pieces[p_type]);
    }

    return (pos->stm == WHITE) ? white_score : -white_score;
}