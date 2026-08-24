#include "movegen.h"
#include "attacks.h"

int isSquareAttacked(Position* pos, int square, int attackerColour) {

    // pawns (pawn can only be attacked from square )
    if (attackerColour == WHITE) {
        if (((1ULL << square) >> 7) & ~A_FILE & pos->pieces[WHITE_PAWN]) {
            return 1;
        } 
        if (((1ULL << square) >> 9) & ~H_FILE & pos->pieces[WHITE_PAWN]) {
            return 1;
        } 
    }
    if (attackerColour == BLACK) {
        if (((1ULL << square) << 7) & ~H_FILE & pos->pieces[BLACK_PAWN]) {
            return 1;
        } 
        if (((1ULL << square) << 9) & ~A_FILE & pos->pieces[BLACK_PAWN]) {
            return 1;
        } 
    }

    // knights
    if (getKnightAttacks(square) & ((attackerColour == WHITE) ? pos->pieces[WHITE_KNIGHT] : pos->pieces[BLACK_KNIGHT])) {
        return 1;
    }
    // king
    if (getKingAttacks(square) & ((attackerColour == WHITE) ? pos->pieces[WHITE_KING] : pos->pieces[BLACK_KING])) {
        return 1;
    }
    // bishops / queens
    if (getBishopAttacks(square, pos->occupancies[BOTH]) & (attackerColour == WHITE ? (pos->pieces[WHITE_BISHOP] | pos->pieces[WHITE_QUEEN]) : (pos->pieces[BLACK_BISHOP] | pos->pieces[BLACK_QUEEN]))) {
        return 1;
    }

    // rooks / queens
    if (getRookAttacks(square, pos->occupancies[BOTH]) & (attackerColour == WHITE ? (pos->pieces[WHITE_ROOK] | pos->pieces[WHITE_QUEEN]) : (pos->pieces[BLACK_ROOK] | pos->pieces[BLACK_QUEEN]))) {
        return 1;
    }

    return 0; // not attacked
}

int moveWasLegal(Position* pos) {

    Bitboard king = pos->pieces[(pos->xstm == WHITE) ? WHITE_KING : BLACK_KING];
    int kingsq = __builtin_ctzll(king);
    return !isSquareAttacked(pos, kingsq, pos->xstm);
}

int generateNoisyMoves(Position* pos, MoveList* moves) {
    return 0;
}
int generateQuietMoves(Position* pos, MoveList* moves) {
    return 0;
}

int generateLegalMoves(Position* pos, MoveList* legalMoves) {
    MoveList pseudoMoves;
    pseudoMoves.size = 0;
    legalMoves->size = 0;

    generateNoisyMoves(pos, &pseudoMoves);
    generateQuietMoves(pos, &pseudoMoves);

    Undo undo;
    for (int i = 0; i < pseudoMoves.size; i++) {
        makeMove(pos, pseudoMoves.moves[i], &undo);
        if (moveWasLegal(pos)) {
            addMove(legalMoves, pseudoMoves.moves[i]);
        }
        unmakeMove(pos, pseudoMoves.moves[i], &undo);
    }

    return legalMoves->size;
}