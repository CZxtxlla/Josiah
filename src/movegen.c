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
    return !isSquareAttacked(pos, kingsq, pos->stm);
}

void generateNoisyMoves(Position* pos, MoveList* noisyMoves) {
    // captures, en passant, promotions
    if (pos->stm == WHITE) {
        Bitboard pawns = pos->pieces[WHITE_PAWN];
        Bitboard promotingPawns = pawns & RANK_7;
        Bitboard normalPawns = pawns & ~RANK_7;
        Bitboard enemies = pos->occupancies[BLACK];

        Bitboard rightCaptures = (normalPawns << 9) & ~A_FILE & enemies;
        Bitboard leftCaptures = (normalPawns << 7) & ~H_FILE & enemies;
        while (rightCaptures) {
            int targetSq = poplsb(&rightCaptures);
            addMove(noisyMoves, EncodeMove(targetSq - 9, targetSq, CAPTURE));
        }
        while (leftCaptures) {
            int targetSq = poplsb(&leftCaptures);
            addMove(noisyMoves, EncodeMove(targetSq - 7, targetSq, CAPTURE));
        }

        // promo captures
        Bitboard rightPromoCaptures = (promotingPawns << 9) & ~A_FILE & enemies;
        Bitboard leftPromoCaptures = (promotingPawns << 7) & ~H_FILE & enemies;

        while (rightPromoCaptures) {
            int targetSq = poplsb(&rightPromoCaptures);
            int fromSq = targetSq - 9;
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_QUEEN_CAPTURE));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_KNIGHT_CAPTURE));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_BISHOP_CAPTURE));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_ROOK_CAPTURE));
        }
        while (leftPromoCaptures) {
            int targetSq = poplsb(&leftPromoCaptures);
            int fromSq = targetSq - 7;
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_QUEEN_CAPTURE));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_KNIGHT_CAPTURE));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_BISHOP_CAPTURE));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_ROOK_CAPTURE));
        }

        // promos
        Bitboard promos = (promotingPawns << 8) & ~pos->occupancies[BOTH];
        while (promos) {
            int targetSq = poplsb(&promos);
            int fromSq = targetSq - 8;
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_QUEEN));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_KNIGHT));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_BISHOP));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_ROOK));
        }

        // ep
        if (pos->ep_square != -1) {
            Bitboard epMask = (1ULL << pos->ep_square);
            Bitboard rightEPCaptures = (normalPawns << 9) & ~A_FILE & epMask;
            Bitboard leftEPCaptures = (normalPawns << 7) & ~H_FILE & epMask;

            while(rightEPCaptures) {
                int targetSq = poplsb(&rightEPCaptures);
                addMove(noisyMoves, EncodeMove(targetSq - 9, targetSq, EP_CAPTURE));
            }
            while(leftEPCaptures) {
                int targetSq = poplsb(&leftEPCaptures);
                addMove(noisyMoves, EncodeMove(targetSq - 7, targetSq, EP_CAPTURE));
            }
        }

    } else {
        Bitboard pawns = pos->pieces[BLACK_PAWN];
        Bitboard promotingPawns = pawns & RANK_2;
        Bitboard normalPawns = pawns & ~RANK_2;
        Bitboard enemies = pos->occupancies[WHITE];

        Bitboard rightCaptures = (normalPawns >> 9) & ~H_FILE & enemies;
        Bitboard leftCaptures = (normalPawns >> 7) & ~A_FILE & enemies;
        while (rightCaptures) {
            int targetSq = poplsb(&rightCaptures);
            addMove(noisyMoves, EncodeMove(targetSq + 9, targetSq, CAPTURE));
        }
        while (leftCaptures) {
            int targetSq = poplsb(&leftCaptures);
            addMove(noisyMoves, EncodeMove(targetSq + 7, targetSq, CAPTURE));
        }

        // promo captures
        Bitboard rightPromoCaptures = (promotingPawns >> 9) & ~H_FILE & enemies;
        Bitboard leftPromoCaptures = (promotingPawns >> 7) & ~A_FILE & enemies;

        while (rightPromoCaptures) {
            int targetSq = poplsb(&rightPromoCaptures);
            int fromSq = targetSq + 9;
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_QUEEN_CAPTURE));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_KNIGHT_CAPTURE));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_BISHOP_CAPTURE));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_ROOK_CAPTURE));
        }
        while (leftPromoCaptures) {
            int targetSq = poplsb(&leftPromoCaptures);
            int fromSq = targetSq + 7;
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_QUEEN_CAPTURE));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_KNIGHT_CAPTURE));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_BISHOP_CAPTURE));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_ROOK_CAPTURE));
        }

        // promos
        Bitboard promos = (promotingPawns >> 8) & ~pos->occupancies[BOTH];
        while (promos) {
            int targetSq = poplsb(&promos);
            int fromSq = targetSq + 8;
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_QUEEN));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_KNIGHT));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_BISHOP));
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, PROMO_ROOK));
        }

        // ep
        if (pos->ep_square != -1) {
            Bitboard epMask = (1ULL << pos->ep_square);
            Bitboard rightEPCaptures = (normalPawns >> 9) & ~H_FILE & epMask;
            Bitboard leftEPCaptures = (normalPawns >> 7) & ~A_FILE & epMask;

            while(rightEPCaptures) {
                int targetSq = poplsb(&rightEPCaptures);
                addMove(noisyMoves, EncodeMove(targetSq + 9, targetSq, EP_CAPTURE));
            }
            while(leftEPCaptures) {
                int targetSq = poplsb(&leftEPCaptures);
                addMove(noisyMoves, EncodeMove(targetSq + 7, targetSq, EP_CAPTURE));
            }
        }

    }   

    // other captures
    int offset = (pos->stm == WHITE) ? 0 : 6;
    Bitboard enemies = pos->occupancies[pos->xstm];

    Bitboard knights = pos->pieces[KNIGHT + offset];
    while (knights) {
        int fromSq = poplsb(&knights);

        Bitboard attacks = getKnightAttacks(fromSq) & enemies;

        while (attacks) {
            int targetSq = poplsb(&attacks);
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, CAPTURE));
        }
    }

    Bitboard kings = pos->pieces[KING + offset];
    while (kings) {
        int fromSq = poplsb(&kings);

        Bitboard attacks = getKingAttacks(fromSq) & enemies;

        while (attacks) {
            int targetSq = poplsb(&attacks);
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, CAPTURE));
        }
    }

    Bitboard bishops = pos->pieces[BISHOP + offset];
    while (bishops) {
        int fromSq = poplsb(&bishops);

        Bitboard attacks = getBishopAttacks(fromSq, pos->occupancies[BOTH]) & enemies;

        while (attacks) {
            int targetSq = poplsb(&attacks);
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, CAPTURE));
        }
    }

    Bitboard rooks = pos->pieces[ROOK + offset];
    while (rooks) {
        int fromSq = poplsb(&rooks);

        Bitboard attacks = getRookAttacks(fromSq, pos->occupancies[BOTH]) & enemies;

        while (attacks) {
            int targetSq = poplsb(&attacks);
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, CAPTURE));
        }
    }

    Bitboard queens = pos->pieces[QUEEN + offset];
    while (queens) {
        int fromSq = poplsb(&queens);

        Bitboard attacks = getQueenAttacks(fromSq, pos->occupancies[BOTH]) & enemies;

        while (attacks) {
            int targetSq = poplsb(&attacks);
            addMove(noisyMoves, EncodeMove(fromSq, targetSq, CAPTURE));
        }
    }
}


void generateQuietMoves(Position* pos, MoveList* quietMoves) {
    // everything but captures, en passant, promotions
    if (pos->stm == WHITE) {
        // pawn pushes
        Bitboard pushes = ((pos->pieces[WHITE_PAWN] & ~RANK_7) << 8) & ~pos->occupancies[BOTH];
        Bitboard doublePushes = ((pushes & RANK_3) << 8) & ~pos->occupancies[BOTH];

        while (pushes) {
            int targetSq = poplsb(&pushes);
            addMove(quietMoves, EncodeMove(targetSq - 8, targetSq, QUIET));
        }

        while(doublePushes) {
            int targetSq = poplsb(&doublePushes);
            addMove(quietMoves, EncodeMove(targetSq - 16, targetSq, DOUBLE_PUSH));
        }

        // castling
        if (pos->castling & WHITE_KS) {
            Bitboard emptyMask = (1ULL << F1) | (1ULL << G1);
            if (!(pos->occupancies[BOTH] & emptyMask)) {
                if (!isSquareAttacked(pos, E1, BLACK) && !isSquareAttacked(pos, F1, BLACK) && !isSquareAttacked(pos, G1, BLACK)) {
                    addMove(quietMoves, EncodeMove(E1, G1, KING_CASTLE));
                }
            }
        }
        if (pos->castling & WHITE_QS) {
            Bitboard emptyMask = (1ULL << D1) | (1ULL << C1) | (1ULL << B1);
            if (!(pos->occupancies[BOTH] & emptyMask)) {
                if (!isSquareAttacked(pos, E1, BLACK) && !isSquareAttacked(pos, D1, BLACK) && !isSquareAttacked(pos, C1, BLACK)) {
                    addMove(quietMoves, EncodeMove(E1, C1, QUEEN_CASTLE));
                }
            }
        }

    } else {
        Bitboard pushes = ((pos->pieces[BLACK_PAWN] & ~RANK_2) >> 8) & ~pos->occupancies[BOTH];
        Bitboard doublePushes = ((pushes & RANK_6) >> 8) & ~pos->occupancies[BOTH];

        while (pushes) {
            int targetSq = poplsb(&pushes);
            addMove(quietMoves, EncodeMove(targetSq + 8, targetSq, QUIET));
        }

        while(doublePushes) {
            int targetSq = poplsb(&doublePushes);
            addMove(quietMoves, EncodeMove(targetSq + 16, targetSq, DOUBLE_PUSH));
        }

        if (pos->castling & BLACK_KS) {
            Bitboard emptyMask = (1ULL << F8) | (1ULL << G8);
            if (!(pos->occupancies[BOTH] & emptyMask)) {
                if (!isSquareAttacked(pos, E8, WHITE) && !isSquareAttacked(pos, F8, WHITE) && !isSquareAttacked(pos, G8, WHITE)) {
                    addMove(quietMoves, EncodeMove(E8, G8, KING_CASTLE));
                }
            }
        }
        if (pos->castling & BLACK_QS) {
            Bitboard emptyMask = (1ULL << D8) | (1ULL << C8) | (1ULL << B8);
            if (!(pos->occupancies[BOTH] & emptyMask)) {
                if (!isSquareAttacked(pos, E8, WHITE) && !isSquareAttacked(pos, D8, WHITE) && !isSquareAttacked(pos, C8, WHITE)) {
                    addMove(quietMoves, EncodeMove(E8, C8, QUEEN_CASTLE));
                }
            }
        }
    }

    // other pieces

    int offset = (pos->stm == WHITE) ? 0 : 6;
    Bitboard occupancy = pos->occupancies[BOTH];

    Bitboard knights = pos->pieces[KNIGHT + offset];
    while (knights) {
        int fromSq = poplsb(&knights);

        Bitboard moves = getKnightAttacks(fromSq) & ~occupancy;

        while (moves) {
            int targetSq = poplsb(&moves);
            addMove(quietMoves, EncodeMove(fromSq, targetSq, QUIET));
        }
    }

    Bitboard kings = pos->pieces[KING + offset];
    while (kings) {
        int fromSq = poplsb(&kings);

        Bitboard moves = getKingAttacks(fromSq) & ~occupancy;

        while (moves) {
            int targetSq = poplsb(&moves);
            addMove(quietMoves, EncodeMove(fromSq, targetSq, QUIET));
        }
    }
    Bitboard bishops = pos->pieces[BISHOP + offset];
    while (bishops) {
        int fromSq = poplsb(&bishops);

        Bitboard moves = getBishopAttacks(fromSq, occupancy) & ~occupancy;

        while (moves) {
            int targetSq = poplsb(&moves);
            addMove(quietMoves, EncodeMove(fromSq, targetSq, QUIET));
        }
    }
    Bitboard rooks = pos->pieces[ROOK + offset];
    while (rooks) {
        int fromSq = poplsb(&rooks);

        Bitboard moves = getRookAttacks(fromSq, occupancy) & ~occupancy;

        while (moves) {
            int targetSq = poplsb(&moves);
            addMove(quietMoves, EncodeMove(fromSq, targetSq, QUIET));
        }
    }
    Bitboard queens = pos->pieces[QUEEN + offset];
    while (queens) {
        int fromSq = poplsb(&queens);

        Bitboard moves = getQueenAttacks(fromSq, occupancy) & ~occupancy;

        while (moves) {
            int targetSq = poplsb(&moves);
            addMove(quietMoves, EncodeMove(fromSq, targetSq, QUIET));
        }
    }



}

void generateLegalMoves(Position* pos, MoveList* legalMoves) {
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
}