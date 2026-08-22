#include <stdio.h>
#include "attacks.h"
#include "bits.h"


int main() {
    initPawnAttacks();

    printBitboard(PAWN_ATTACKS[WHITE][A1]);
    printBitboard(PAWN_ATTACKS[BLACK][D4]);
    printBitboard(PAWN_ATTACKS[WHITE][D4]);


    return 0;
}