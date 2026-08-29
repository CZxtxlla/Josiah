#include <stdio.h>
#include "attacks.h"
#include "uci.h"
#include "zobrist.h"
#include "transposition.h"


int main() {
    printf("Initializing engine...\n");
    initAttacks();
    initZobrist();
    ttInit(256); // default 256 MB
    printf("Engine initialized.\n");
    uciLoop();
    
    return 0;
}