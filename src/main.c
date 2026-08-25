#include <stdio.h>
#include "attacks.h"
#include "bits.h"
#include "position.h"
#include "uci.h"
#include "movegen.h"


int main() {
    initAttacks();

    Position pos;
    parseFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &pos);

    printPosition(&pos);

    MoveList movesl;
    generateLegalMoves(&pos, &movesl);

    for (int i = 0; i < movesl.size; i++) {
        printMove(movesl.moves[i]);
    }
    
    return 0;
}