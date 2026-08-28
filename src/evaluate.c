#include "evaluate.h"

// piece square tables (copied from wiki)
// must flip rank

const int PAWN_MG_TABLE[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};

const int KNIGHT_TABLE[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

const int BISHOP_TABLE[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

const int ROOK_TABLE[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};

const int QUEEN_TABLE[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
    0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

const int KING_MG_TABLE[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};

const int PAWN_EG_TABLE[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    80, 80, 80, 80, 80, 80, 80, 80,
    50, 50, 50, 50, 50, 50, 50, 50,
    30, 30, 30, 30, 30, 30, 30, 30,
    20, 20, 20, 20, 20, 20, 20, 20,
    10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10,
    0,  0,  0,  0,  0,  0,  0,  0
};

const int KING_EG_TABLE[64] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};

const int* mgTablePointer[] = {PAWN_MG_TABLE, KNIGHT_TABLE, BISHOP_TABLE, ROOK_TABLE, QUEEN_TABLE, KING_MG_TABLE};
const int* egTablePointer[] = {PAWN_EG_TABLE, KNIGHT_TABLE, BISHOP_TABLE, ROOK_TABLE, QUEEN_TABLE, KING_EG_TABLE};

const int PIECE_TO_SCORE[6] = {100, 300, 320, 500, 900, 0};

// https://chessprogramming.org/Tapered_Eval
int gamePhase(Position* pos) {
    int phase = 0;
    phase += __builtin_popcountll(pos->pieces[WHITE_KNIGHT] | pos->pieces[BLACK_KNIGHT]) * 1;
    phase += __builtin_popcountll(pos->pieces[WHITE_BISHOP] | pos->pieces[BLACK_BISHOP]) * 1;
    phase += __builtin_popcountll(pos->pieces[WHITE_ROOK] | pos->pieces[BLACK_ROOK]) * 2;
    phase += __builtin_popcountll(pos->pieces[WHITE_QUEEN] | pos->pieces[BLACK_QUEEN]) * 4;

    if (phase > 24) {
        phase = 24;
    }

    return phase;
}

int evaluateLegalPos(Position* pos) {
    int phase = gamePhase(pos);
    int score = 0;

    for (int sq = 0; sq < 64; sq++) {
        int ptype = pos->squares[sq];
        if (ptype == PIECE_NONE) {
            continue;
        }
        int pieceIndex = ptype % 6;
        int pieceColor = ptype / 6;
        int flipped = (pieceColor == WHITE) ? sq ^ 56 : sq;

        int mgScore = mgTablePointer[pieceIndex][flipped];
        int egScore = egTablePointer[pieceIndex][flipped];
        int psqtScore = ((mgScore * phase) + (24 - phase) * egScore) / 24;

        int pieceValue = PIECE_TO_SCORE[pieceIndex] + psqtScore;

        if (pieceColor == WHITE) {
            score += pieceValue;
        } else {
            score -= pieceValue;
        }
    }

    return (pos->stm == WHITE) ? score : -score;
}