#include "uci.h"
#include "movegen.h"
#include "perft.h"

#include <string.h>
#include <stdlib.h>

#include "search.h"

#define VERSION "0.0"

void uciLoop() {
    char line[4096];

    Position pos;
    parseFen(STARTPOS, &pos);

    setbuf(stdout, NULL);

    while(fgets(line, sizeof(line), stdin)) {
        if (!strncmp(line, "uci", 3)) {
            printf("id name Josiah " VERSION "\n");
            printf("id author Charles Zitella\n");
            printf("uciok\n");

        } else if (!strncmp(line, "perft", 5)) {
            strtok(line, " ");
            char* d = strtok(NULL, " ") ? : "5";
            char* fen = strtok(NULL, "\0") ? : STARTPOS;

            int depth = atoi(d);
            parseFen(fen, &pos);
            perftTest(&pos, depth);
        }
    }
}