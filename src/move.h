#ifndef MOVE_H
#define MOVE_H

#include "types.h"
#include "position.h"
#include "bits.h"

typedef uint16_t Move;
// move encoding:
// 0-5 bits for from square, 6-11 bits for to square, 12-15 bits for flag 
/// https://chessprogramming.org/Encoding_Moves#information-required for codes (remaining 4 bits)

#define NULL_MOVE 0

// move flags
#define QUIET 0b0000
#define DOUBLE_PUSH 0b0001
#define KING_CASTLE 0b0010
#define QUEEN_CASTLE 0b0011
#define CAPTURE 0b0100
#define EP_CAPTURE 0b0101

#define PROMO 0b1000
#define PROMO_KNIGHT 0b1000
#define PROMO_BISHOP 0b1001
#define PROMO_ROOK 0b1010
#define PROMO_QUEEN 0b1011
#define PROMO_KNIGHT_CAPTURE 0b1100
#define PROMO_BISHOP_CAPTURE 0b1101
#define PROMO_ROOK_CAPTURE 0b1110
#define PROMO_QUEEN_CAPTURE 0b1111

typedef struct {
    Move moves[MAX_MOVES];
    int size;
} MoveList;

static inline void addMove(MoveList* list, Move move) {
    list->moves[list->size++] = move;
}

void makeMove(Position* pos, Move move, Undo* undo);
void unmakeMove(Position* pos, Move move, Undo* undo);

#define EncodeMove(from, to, flag) (from) | ((to) << 6) | ((flag << 12))
#define MoveFrom(move) (move & 63)
#define MoveTo(move) ((move >> 6) & 63)
#define MoveFlag(move) ((move >> 12) & 15)

#define IsCapture(move) (!!(MoveFlag(move) & CAPTURE))
#define IsEP(move) (MoveFlag(move) == EP_CAPTURE)
#define IsDouble(move) (MoveFlag(move) == DOUBLE_PUSH)
#define IsKingCastle(move) (MoveFlag(move) == KING_CASTLE)
#define IsQueenCastle(move) (MoveFlag(move) == QUEEN_CASTLE)
#define IsPromo(move) (!!(MoveFlag(move) & PROMO))
#define PromoType(move) ((MoveFlag(move) & 0x3) + KNIGHT)

static inline void printMove(Move m) {
    int from = MoveFrom(m);
    int rankFrom = Rank(from);
    int fileFrom = File(from);
    int to = MoveTo(m);
    int rankTo = Rank(to);
    int fileTo = File(to);
    
    if (IsPromo(m)) {
        char promoChar = "nrbq"[MoveFlag(m) & 0x3];
        printf("%c%d%c%d%c", fileFrom + 'a', rankFrom + 1, fileTo + 'a', rankTo + 1, promoChar);

    } else {
        printf("%c%d%c%d", fileFrom + 'a', rankFrom + 1, fileTo + 'a', rankTo + 1);
    }
}

char* moveToStr(Move m);

#endif