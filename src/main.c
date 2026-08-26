#include <stdio.h>
#include "attacks.h"
#include "bits.h"
#include "position.h"
#include "uci.h"
#include "movegen.h"


int main() {
    printf("Initializing engine...\n");
    initAttacks();
    printf("Engine initialized.\n");
    uciLoop();
    
    return 0;
}