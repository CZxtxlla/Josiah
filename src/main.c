#include <stdio.h>
#include "attacks.h"
#include "bits.h"


int main() {
    initAttacks();

    printBitboard(PAWN_ATTACKS[WHITE][D4]);
    return 0;
}