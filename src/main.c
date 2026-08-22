#include <stdio.h>
#include "attacks.h"
#include "bits.h"


int main() {
    initPawnAttacks();
    initKnightAttacks();
    initKingAttacks();

    for (int i = 0; i < 64; i++) {
        printBitboard(PAWN_ATTACKS[WHITE][i]);
    }


    return 0;
}