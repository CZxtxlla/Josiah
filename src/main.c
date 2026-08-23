#include <stdio.h>
#include "attacks.h"
#include "bits.h"
#include "position.h"


int main() {
    initAttacks();

    Position pos;
    parseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &pos);

    printPosition(&pos);
    return 0;
}