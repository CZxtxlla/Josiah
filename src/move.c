#include "move.h"
#include "zobrist.h"
#include "attacks.h"

const int pieceType[13] = {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    PIECE_NONE
};

const int pieceColour[13] = {
    WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
    BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,
    BOTH
};

const int castlingRights[64] = {
    13, 15, 15, 15, 12, 15, 15, 14,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
     7, 15, 15, 15,  3, 15, 15, 11
};

int isSquareAttacked(Position* pos, int square, int attackerColour) {
    // pawns
    int reverseColour = (attackerColour == WHITE) ? BLACK : WHITE;
    if (getPawnAttacks(reverseColour, square) & (attackerColour == WHITE ? pos->pieces[WHITE_PAWN] : pos->pieces[BLACK_PAWN])) {
        return 1;
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

int killerIsValid(Position* pos, Move move) {
    // checks move is pseudoLegal, already knowing it is quiet (non capture, non promo)
    if (move == NULL_MOVE) {
        return 0;
    }

    int from = MoveFrom(move);
    int to = MoveTo(move);
    int piece = pos->squares[from];
    int flag = MoveFlag(move);
    int pt = pieceType[piece];

    if (flag == CAPTURE || flag == EP_CAPTURE || flag >= PROMO) {
        return 0;
    }

    if (flag == DOUBLE_PUSH && pt != PAWN) {
        return 0;
    }

    if ((flag == KING_CASTLE || flag == QUEEN_CASTLE) && pt != KING) {
        return 0;
    }
    
    if (piece == PIECE_NONE) {
        return 0;
    }

    int us = pos->stm;

    if (pieceColour[piece] != us) {
        return 0;
    }

    if (pos->squares[to] != PIECE_NONE) {
        return 0;
    }

    Bitboard occ = pos->occupancies[BOTH];

    switch (pt) {
        case KNIGHT:
            return (getKnightAttacks(from) & (1ULL << to)) != 0;

        case BISHOP:
            return (getBishopAttacks(from, occ) & (1ULL << to)) != 0;

        case ROOK:
            return (getRookAttacks(from, occ) & (1ULL << to)) != 0;

        case QUEEN:
            return (getQueenAttacks(from, occ) & (1ULL << to)) != 0;

        case KING:
            if (flag != KING_CASTLE && flag != QUEEN_CASTLE) {
                return (getKingAttacks(from) & (1ULL << to)) != 0;
            }

            int them = pos->xstm;

            if (flag == KING_CASTLE) {
                if (us == WHITE && (pos->castling & WHITE_KS) && from == E1 && to == G1) {
                    if (occ & ((1ULL << F1) | (1ULL << G1))) {
                        return 0;
                    }
                    if (isSquareAttacked(pos, E1, them) || isSquareAttacked(pos, F1, them)) {
                        return 0;
                    }
                    return 1;
                }
                if (us == BLACK && (pos->castling & BLACK_KS) && from == E8 && to == G8) {
                    if (occ & ((1ULL << F8) | (1ULL << G8))) {
                        return 0;
                    }
                    if (isSquareAttacked(pos, E8, them) || isSquareAttacked(pos, F8, them)) {
                        return 0;
                    }
                    return 1;
                }
            } else if (flag == QUEEN_CASTLE) {
                if (us == WHITE && (pos->castling & WHITE_QS) && from == E1 && to == C1) {
                    if (occ & ((1ULL << B1) | (1ULL << C1) | (1ULL << D1))) {
                        return 0;
                    }
                    if (isSquareAttacked(pos, E1, them) || isSquareAttacked(pos, D1, them)) {
                        return 0;
                    }
                    return 1;
                }
                if (us == BLACK && (pos->castling & BLACK_QS) && from == E8 && to == C8) {
                    if (occ & ((1ULL << B8) | (1ULL << C8) | (1ULL << D8))) {
                        return 0;
                    }
                    if (isSquareAttacked(pos, E8, them) || isSquareAttacked(pos, D8, them)) {
                        return 0;
                    }
                    return 1;
                }
            }
            return 0;

        case PAWN:
            int push = (us == WHITE) ? 8 : -8;
            int promoRank = (us == WHITE) ? 7 : 0;
            
            // ensure no promotions
            if ((to / 8) == promoRank) {
                return 0;
            }

            if (from + push == to) {
                return 1;
            }

            int startRank = (us == WHITE) ? 1 : 6;

            if ((from / 8) == startRank && from + (push * 2) == to) {
                return pos->squares[from + push] == PIECE_NONE;
            }
            return 0;
    }
    return 0;
}

int moveIsPseudo(Position* pos, Move move) {
    int from = MoveFrom(move);
    int to = MoveTo(move);
    int piece = pos->squares[from];
    int us = pos->stm;
    int pt = pieceType[piece];
    int pc = pieceColour[piece];
    int flag = MoveFlag(move);

    if ((move == NULL_MOVE || move == 0) || (pc != us) || (piece == PIECE_NONE)) {
        return 0;
    }
    if (flag > EP_CAPTURE && flag < PROMO) {
        return 0;
    }
    if (flag == DOUBLE_PUSH && pt != PAWN) {
        return 0;
    }
    if ((1ULL << to) & pos->occupancies[us]) {
        return 0;
    }

    if (IsEP(move) && pt != PAWN) {
        return 0;
    }

    if (IsPromo(move) && pt != PAWN) {
        return 0;
    }

    if ((flag == KING_CASTLE || flag == QUEEN_CASTLE) && pt != KING) {
        return 0;
    }

    if (IsCapture(move) && !IsEP(move) && ((pos->squares[to] == PIECE_NONE) || (pieceType[pos->squares[to]] == KING))) {
        return 0;
    }

    if (!IsCapture(move) && pos->squares[to] != PIECE_NONE) {
        return 0;
    }


    Bitboard occ = pos->occupancies[BOTH];

    switch (pt) {
        case KNIGHT:
            return !IsEP(move) && !IsPromo(move) && ((getKnightAttacks(from) & (1ULL << to)) != 0);
        case BISHOP:
            return !IsEP(move) && !IsPromo(move) && ((getBishopAttacks(from, occ) & (1ULL << to)) != 0);
        case ROOK:
            return !IsEP(move) && !IsPromo(move) && ((getRookAttacks(from, occ) & (1ULL << to)) != 0);
        case QUEEN:
            return !IsEP(move) && !IsPromo(move) && ((getQueenAttacks(from, occ) & (1ULL << to)) != 0);
        case KING:
            if (flag != KING_CASTLE && flag != QUEEN_CASTLE) {
                return ((getKingAttacks(from) & (1ULL << to)) != 0);
            }

            int them = pos->xstm;

            if (flag == KING_CASTLE) {
                if (us == WHITE && (pos->castling & WHITE_KS) && from == E1 && to == G1) {
                    if (occ & ((1ULL << F1) | (1ULL << G1))) {
                        return 0;
                    }
                    if (isSquareAttacked(pos, E1, them) || isSquareAttacked(pos, F1, them)) {
                        return 0;
                    }
                    return 1;
                }
                if (us == BLACK && (pos->castling & BLACK_KS) && from == E8 && to == G8) {
                    if (occ & ((1ULL << F8) | (1ULL << G8))) {
                        return 0;
                    }
                    if (isSquareAttacked(pos, E8, them) || isSquareAttacked(pos, F8, them)) {
                        return 0;
                    }
                    return 1;
                }
            } else if (flag == QUEEN_CASTLE) {
                if (us == WHITE && (pos->castling & WHITE_QS) && from == E1 && to == C1) {
                    if (occ & ((1ULL << B1) | (1ULL << C1) | (1ULL << D1))) {
                        return 0;
                    }
                    if (isSquareAttacked(pos, E1, them) || isSquareAttacked(pos, D1, them)) {
                        return 0;
                    }
                    return 1;
                }
                if (us == BLACK && (pos->castling & BLACK_QS) && from == E8 && to == C8) {
                    if (occ & ((1ULL << B8) | (1ULL << C8) | (1ULL << D8))) {
                        return 0;
                    }
                    if (isSquareAttacked(pos, E8, them) || isSquareAttacked(pos, D8, them)) {
                        return 0;
                    }
                    return 1;
                }
            }
            return 0;
            
        case PAWN:
            if (IsEP(move)) {
                if (to != pos->ep_square) {
                    return 0;
                }
                int capsq = (us == WHITE) ? to - 8 : to + 8;
                int enemyPawn = (us == WHITE) ? BLACK_PAWN : WHITE_PAWN;
                return (pos->squares[capsq] == enemyPawn) && (getPawnAttacks(us, from) & (1ULL << to));
            }
            if (IsCapture(move)) {
                return (pos->squares[to] != PIECE_NONE) && (getPawnAttacks(us, from) & (1ULL << to)) != 0;

            }

            if (pos->squares[to] != PIECE_NONE) {
                return 0;
            }

            int push = (us == WHITE) ? 8 : -8;

            if (from + push == to) {
                return 1;
            }
            int startRank = (us == WHITE) ? 1 : 6;

            if ((from / 8) == startRank && from + (push * 2) == to) {
                return pos->squares[from + push] == PIECE_NONE;
            }
            return 0;
    }
    return 0;
}

int moveWasLegal(Position* pos) {
    Bitboard oppKing = pos->pieces[(pos->stm == WHITE) ? WHITE_KING : BLACK_KING];
    if (oppKing == 0) {
        return 0;
    }

    Bitboard king = pos->pieces[(pos->xstm == WHITE) ? WHITE_KING : BLACK_KING];
    if (king == 0) {
        return 0;
    }
    int kingsq = __builtin_ctzll(king);
    return !isSquareAttacked(pos, kingsq, pos->stm);
}

int moveIsLegal(Position* pos, Move move) {
    // given it is pseudo legal already
    Undo undo;
    makeMove(pos, move, &undo);
    if (moveWasLegal(pos)) {
        unmakeMove(pos, move, &undo);
        return 1;
    } else {
        unmakeMove(pos, move, &undo);
        return 0;
    }
}

void makeMove(Position* pos, Move move, Undo* undo) {
    // only for fully legal moves (doesn't check king in check)
    int from = MoveFrom(move);
    int to = MoveTo(move);
    int piece = pos->squares[from];
    int captured = IsEP(move) ? Piece(PAWN, pos->xstm) : pos->squares[to];

    pos->history[pos->historyPly] = pos->hash;
    pos->historyPly++;

    undo->capture_piece = captured;
    undo->ep_square = pos->ep_square;
    undo->castling = pos->castling;
    undo->movedPiece = piece;
    undo->hash = pos->hash;
    undo->half_moves = pos->half_moves;

    pos->half_moves++;

    if (pieceType[piece] == PAWN) {
        pos->half_moves = 0;
    }

    // move piece
    FlipBits(pos->pieces[piece], from, to);
    FlipBits(pos->occupancies[pos->stm], from, to);
    FlipBits(pos->occupancies[BOTH], from, to);
    // update hash
    pos->hash ^= ZOBRIST_PIECES[piece][from];
    pos->hash ^= ZOBRIST_PIECES[piece][to];

    pos->squares[from] = PIECE_NONE;
    pos->squares[to] = piece;
    if (IsKingCastle(move)) {
        if (pos->stm == WHITE) {
            FlipBits(pos->pieces[WHITE_ROOK], 5, 7);
            FlipBits(pos->occupancies[WHITE], 5, 7);
            FlipBits(pos->occupancies[BOTH], 5, 7);
            pos->hash ^= ZOBRIST_PIECES[WHITE_ROOK][5];
            pos->hash ^= ZOBRIST_PIECES[WHITE_ROOK][7];
            pos->squares[7] = PIECE_NONE;
            pos->squares[5] = WHITE_ROOK;
        } else {
            FlipBits(pos->pieces[BLACK_ROOK], 61, 63);
            FlipBits(pos->occupancies[BLACK], 61, 63);
            FlipBits(pos->occupancies[BOTH], 61, 63);
            pos->hash ^= ZOBRIST_PIECES[BLACK_ROOK][61];
            pos->hash ^= ZOBRIST_PIECES[BLACK_ROOK][63];
            pos->squares[63] = PIECE_NONE;
            pos->squares[61] = BLACK_ROOK;
        }
    } else if (IsQueenCastle(move)) {
        if (pos->stm == WHITE) {
            FlipBits(pos->pieces[WHITE_ROOK], 0, 3);
            FlipBits(pos->occupancies[WHITE], 0, 3);
            FlipBits(pos->occupancies[BOTH], 0, 3);
            pos->hash ^= ZOBRIST_PIECES[WHITE_ROOK][0];
            pos->hash ^= ZOBRIST_PIECES[WHITE_ROOK][3];
            pos->squares[0] = PIECE_NONE;
            pos->squares[3] = WHITE_ROOK;
        } else {
            FlipBits(pos->pieces[BLACK_ROOK], 56, 59);
            FlipBits(pos->occupancies[BLACK], 56, 59);
            FlipBits(pos->occupancies[BOTH], 56, 59);
            pos->hash ^= ZOBRIST_PIECES[BLACK_ROOK][56];
            pos->hash ^= ZOBRIST_PIECES[BLACK_ROOK][59];
            pos->squares[56] = PIECE_NONE;
            pos->squares[59] = BLACK_ROOK;
        }

    } else if (IsCapture(move)) {
        pos->half_moves = 0;
        int capsq = to;
        if (IsEP(move)) {
            capsq = (pos->stm == WHITE) ? (to - 8) : (to + 8);
            pos->squares[capsq] = PIECE_NONE;
        }
        FlipBit(pos->pieces[captured], capsq); // remove captured piece
        FlipBit(pos->occupancies[pos->xstm], capsq);
        FlipBit(pos->occupancies[BOTH], capsq);
        pos->hash ^= ZOBRIST_PIECES[captured][capsq];
    }

    if (undo->ep_square != -1) {
        pos->hash ^= ZOBRIST_EP[undo->ep_square % 8];
    }
    if (IsPromo(move)) {
        // promotion
        int promotedType = PromoType(move) + ((pos->stm == WHITE) ? 0 : 6);
        FlipBit(pos->pieces[piece], to);
        FlipBit(pos->pieces[promotedType], to);
        pos->hash ^= ZOBRIST_PIECES[piece][to];
        pos->hash ^= ZOBRIST_PIECES[promotedType][to];
        pos->squares[to] = promotedType;
        pos->ep_square = -1;

    } else if (IsDouble(move)) {
        // double pawn push
        pos->ep_square = (pos->stm == WHITE) ? (to - 8) : (to + 8);
        pos->hash ^= ZOBRIST_EP[pos->ep_square % 8];
    } else {
        pos->ep_square = -1;
    }

    pos->hash ^= ZOBRIST_CASTLE[undo->castling];

    // update castling rights
    pos->castling &= castlingRights[from];
    pos->castling &= castlingRights[to];

    pos->hash ^= ZOBRIST_CASTLE[pos->castling];

    pos->full_moves += (pos->stm == BLACK);

    pos->hash^= ZOBRIST_SIDE;
    pos->stm ^= 1;
    pos->xstm ^= 1;

}


void unmakeMove(Position* pos, Move move, Undo* undo) {
    int from = MoveFrom(move);
    int to = MoveTo(move);
    int piece = undo->movedPiece;

    pos->castling = undo->castling;
    pos->ep_square = undo->ep_square;
    pos->hash = undo->hash;
    pos->half_moves = undo->half_moves;

    pos->historyPly--;

    pos->stm ^= 1;
    pos->xstm ^= 1;

    if (IsPromo(move)) {
        int promoted = PromoType(move) + ((pos->stm == WHITE) ? 0 : 6);
        FlipBit(pos->pieces[piece], to);
        FlipBit(pos->pieces[promoted], to);
        pos->squares[to] = piece;
    }

    // move piece back to where it was
    FlipBits(pos->pieces[piece], to, from);
    FlipBits(pos->occupancies[pos->stm], to, from);
    FlipBits(pos->occupancies[BOTH], to, from);

    pos->squares[to] = PIECE_NONE;
    pos->squares[from] = piece;

    if (IsKingCastle(move)) {
        // move rook back to where it was
        if (pos->stm == WHITE) {
            FlipBits(pos->pieces[WHITE_ROOK], 5, 7);
            FlipBits(pos->occupancies[WHITE], 5, 7);
            FlipBits(pos->occupancies[BOTH], 5, 7);
            pos->squares[5] = PIECE_NONE;
            pos->squares[7] = WHITE_ROOK;
        } else {
            FlipBits(pos->pieces[BLACK_ROOK], 61, 63);
            FlipBits(pos->occupancies[BLACK], 61, 63);
            FlipBits(pos->occupancies[BOTH], 61, 63);
            pos->squares[61] = PIECE_NONE;
            pos->squares[63] = BLACK_ROOK;
        }
    } else if (IsQueenCastle(move)) {
        // move rook back
        if (pos->stm == WHITE) {
            FlipBits(pos->pieces[WHITE_ROOK], 0, 3);
            FlipBits(pos->occupancies[WHITE], 0, 3);
            FlipBits(pos->occupancies[BOTH], 0, 3);
            pos->squares[3] = PIECE_NONE;
            pos->squares[0] = WHITE_ROOK;
        } else {
            FlipBits(pos->pieces[BLACK_ROOK], 56, 59);
            FlipBits(pos->occupancies[BLACK], 56, 59);
            FlipBits(pos->occupancies[BOTH], 56, 59);
            pos->squares[59] = PIECE_NONE;
            pos->squares[56] = BLACK_ROOK;
        }
    } else if (IsCapture(move)) {
        // restore captured piece
        int capsq = to;
        if (IsEP(move)) {
            capsq = (pos->stm == WHITE) ? (to - 8) : (to + 8);
        }

        FlipBit(pos->pieces[undo->capture_piece], capsq);
        FlipBit(pos->occupancies[pos->xstm], capsq);
        FlipBit(pos->occupancies[BOTH], capsq);

        pos->squares[capsq] = undo->capture_piece;
    }

    pos->full_moves -= (pos->stm == BLACK);
}

const char* SQ_TO_COORD[64] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
};

char* moveToStr(Move m) {
    static char buffer[6];

    int from = MoveFrom(m);
    int to = MoveTo(m);

    if(IsPromo(m)) {
        char promoChar = "nbrq"[MoveFlag(m) & 0x3];
        sprintf(buffer, "%s%s%c", SQ_TO_COORD[from], SQ_TO_COORD[to], promoChar);
    } else {
        sprintf(buffer, "%s%s", SQ_TO_COORD[from], SQ_TO_COORD[to]);
    }

    return buffer;
}

int makeMovePseudo(Position* pos, Move move, Undo* undo) {
    makeMove(pos, move, undo);
    if (moveWasLegal(pos)) {
        return 1;
    } else {
        unmakeMove(pos, move, undo);
        return 0;
    }
}

void makeNullMove(Position* pos, Undo* undo) {
    undo->ep_square = pos->ep_square;
    undo->hash = pos->hash;
    undo->half_moves = pos->half_moves;

    if (pos->ep_square != -1) {
        pos->hash ^= ZOBRIST_EP[pos->ep_square % 8];
    }
    pos->hash ^= ZOBRIST_SIDE;

    pos->half_moves++;
    pos->ep_square = -1;

    pos->stm ^= 1;
    pos->xstm ^= 1;
}

void unmakeNullMove(Position* pos, Undo* undo) {
    pos->ep_square = undo->ep_square;
    pos->hash = undo->hash;
    pos->half_moves = undo->half_moves;

    pos->stm ^= 1;
    pos->xstm ^= 1;
}