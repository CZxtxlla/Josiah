#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint64_t bitboard;
typedef uint32_t move;


typedef struct {
    bitboard occupancies[3]; // 0 = white, 1 = black, 2 = both
    bitboard pieces[12]; // one bitboard for each piece
}



#endif 