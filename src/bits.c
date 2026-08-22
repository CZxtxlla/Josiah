#include "bits.h"
#include <stdio.h>

void printBitboard(Bitboard board) {
    for (int i = 0; i < 64; i++) {
        if ((i & 7) == 0) {
            printf(" %d ", 8 - (i >> 3));
        }
        printf(" %d", GetBit(board, i ^ 56) ? 1 : 0);
        
        if ((i & 7) == 7) {
            printf("\n");
        }
    }
    printf("\n    a b c d e f g h\n\n");
}