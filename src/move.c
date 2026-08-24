#include "move.h"

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

void makeMove(Position* pos, Move move, Undo* undo) {
    int from = MoveFrom(move);
    int to = MoveTo(move);
    int piece = pos->squares[from];
    int captured = IsEP(move) ? Piece(PAWN, pos->xstm) : pos->squares[to];

    undo->capture_piece = captured;
    undo->ep_square = pos->ep_square;
    undo->castling = pos->castling;

    // move piece
    FlipBits(pos->pieces[piece], from, to);
    FlipBits(pos->occupancies[pos->stm], from, to);
    FlipBits(pos->occupancies[BOTH], from, to);

    pos->squares[from] = PIECE_NONE;
    pos->squares[to] = piece;

    if (IsKingCastle(move)) {
        if (pos->stm == WHITE) {
            FlipBits(pos->pieces[WHITE_ROOK], 5, 7);
            FlipBits(pos->occupancies[WHITE], 5, 7);
            FlipBits(pos->occupancies[BOTH], 5, 7);
            pos->squares[7] = PIECE_NONE;
            pos->squares[5] = WHITE_ROOK;
        } else {
            FlipBits(pos->pieces[BLACK_ROOK], 61, 63);
            FlipBits(pos->occupancies[BLACK], 61, 63);
            FlipBits(pos->occupancies[BOTH], 61, 63);
            pos->squares[63] = PIECE_NONE;
            pos->squares[61] = BLACK_ROOK;
        }
    } else if (IsQueenCastle(move)) {
        if (pos->stm == WHITE) {
            FlipBits(pos->pieces[WHITE_ROOK], 0, 3);
            FlipBits(pos->occupancies[WHITE], 0, 3);
            FlipBits(pos->occupancies[BOTH], 0, 3);
            pos->squares[0] = PIECE_NONE;
            pos->squares[3] = WHITE_ROOK;
        } else {
            FlipBits(pos->pieces[BLACK_ROOK], 56, 59);
            FlipBits(pos->occupancies[BLACK], 56, 59);
            FlipBits(pos->occupancies[BOTH], 56, 59);
            pos->squares[56] = PIECE_NONE;
            pos->squares[59] = BLACK_ROOK;
        }

    } else if (IsCapture(move)) {
        int capsq = to;
        if (IsEP(move)) {
            capsq = (pos->stm == WHITE) ? (to - 8) : (to + 8);
        }
        FlipBit(pos->pieces[captured], capsq); // remove captured piece
        FlipBit(pos->occupancies[pos->xstm], capsq);
        FlipBit(pos->occupancies[BOTH], capsq);
        pos->squares[capsq] = PIECE_NONE;
    }
    if (IsPromo(move)) {
        // promotion
        int promotedType = PromoType(move) + ((pos->stm == WHITE) ? 0 : 8);
        FlipBit(pos->pieces[piece], to);
        FlipBit(pos->pieces[promotedType], to);
        pos->squares[to] = promotedType;

    } else if (IsDouble(move)) {
        // double pawn push
        pos->ep_square = (pos->stm == WHITE) ? (to - 8) : (to + 8);
    } else {
        pos->ep_square = -1;
    }

    // update castling rights
    pos->castling &= castlingRights[from];
    pos->castling &= castlingRights[to];

    pos->stm ^= 1;
    pos->xstm ^= 1;

}


int unmakeMove(Position* pos, Move move, Undo* undo) {

}