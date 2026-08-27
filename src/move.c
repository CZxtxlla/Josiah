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
    // only for fully legal moves (doesn't check king in check)
    int from = MoveFrom(move);
    int to = MoveTo(move);
    int piece = pos->squares[from];
    int captured = IsEP(move) ? Piece(PAWN, pos->xstm) : pos->squares[to];

    undo->capture_piece = captured;
    undo->ep_square = pos->ep_square;
    undo->castling = pos->castling;
    undo->movedPiece = piece;
    undo->hash = pos->hash;
    undo->half_moves = pos->half_moves;

    if (piece % 6 == 0) {
        pos->half_moves = 0;
    }

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
        pos->half_moves = 0;
        int capsq = to;
        if (IsEP(move)) {
            capsq = (pos->stm == WHITE) ? (to - 8) : (to + 8);
            pos->squares[capsq] = PIECE_NONE;
        }
        FlipBit(pos->pieces[captured], capsq); // remove captured piece
        FlipBit(pos->occupancies[pos->xstm], capsq);
        FlipBit(pos->occupancies[BOTH], capsq);
    }
    if (IsPromo(move)) {
        // promotion
        int promotedType = PromoType(move) + ((pos->stm == WHITE) ? 0 : 6);
        FlipBit(pos->pieces[piece], to);
        FlipBit(pos->pieces[promotedType], to);
        pos->squares[to] = promotedType;
        pos->ep_square = -1;

    } else if (IsDouble(move)) {
        // double pawn push
        pos->ep_square = (pos->stm == WHITE) ? (to - 8) : (to + 8);
    } else {
        pos->ep_square = -1;
    }

    // update castling rights
    pos->castling &= castlingRights[from];
    pos->castling &= castlingRights[to];

    pos->full_moves += (pos->stm == BLACK);

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
        char promoChar = "nrbq"[MoveFlag(m) & 0x3];
        sprintf(buffer, "%s%s%c", SQ_TO_COORD[from], SQ_TO_COORD[to], promoChar);
    } else {
        sprintf(buffer, "%s%s", SQ_TO_COORD[from], SQ_TO_COORD[to]);
    }

    return buffer;
}