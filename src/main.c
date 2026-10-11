#include <stdio.h>
#include "attacks.h"
#include "uci.h"
#include "zobrist.h"
#include "transposition.h"
#include "nnue/inference.h"


int main() {
    printf("Initializing engine...\n");
    initAttacks();
    initZobrist();
    ttInit(64); // default 64 MB
    model = loadNNUE("src/nnue/768_quant_50_256_v5.1.nnue");
    printf("Engine initialized.\n");
    uciLoop();
    
    return 0;
}