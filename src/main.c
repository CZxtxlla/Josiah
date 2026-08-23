#include <stdio.h>
#include "attacks.h"
#include "bits.h"


int main() {
    initAttacks();

    testBishopAttacks();
    testRookAttacks();


    return 0;
}