#include <stdio.h>
#include "attacks.h"
#include "uci.h"
#include "zobrist.h"


int main() {
    printf("Initializing engine...\n");
    initAttacks();
    initZobrist();
    printf("Engine initialized.\n");
    uciLoop();
    
    return 0;
}