#include "uci.h"
#include "movegen.h"
#include "perft.h"
#include "transposition.h"

#include <string.h>
#include <stdlib.h>

#include "search.h"

#define VERSION "0.0"

Move parseMove(char* moves, Position* pos) {
    MoveList movesl;
    generateLegalMoves(pos, &movesl);
    for (int i = 0; i < movesl.size; i++) {
        if (!strcmp("O-O", moves) && IsKingCastle(movesl.moves[i])) {
            return movesl.moves[i];
        } else if (!strcmp("O-O-O", moves) && IsQueenCastle(movesl.moves[i])) {
            return movesl.moves[i];
        } else if (!strcmp(moveToStr(movesl.moves[i]), moves)) {
            return movesl.moves[i];
        }
    }

    return NULL_MOVE;
}

void parseGo(char* line, Position* pos) {
    int depth = -1;
    int wtime = -1, btime = -1;
    int winc = 0, binc = 0;
    int movetime = -1;

    line += 3;

    char* ptr = NULL;

    if ((ptr = strstr(line, "depth"))) {
        depth = atoi(ptr + 6);
    }
    if ((ptr = strstr(line, "movetime"))) {
        movetime = atoi(ptr + 9);
    }
    if ((ptr = strstr(line, "wtime"))) {
        wtime = atoi(ptr + 6);
    }
    if ((ptr = strstr(line, "btime"))) {
        btime = atoi(ptr + 6);
    }
    if ((ptr = strstr(line, "winc"))) {
        winc = atoi(ptr + 5);
    }
    if ((ptr = strstr(line, "binc"))) {
        binc = atoi(ptr + 5);
    }

    int thinkingTime = -1;

    if (movetime != -1) {
        thinkingTime = movetime;
    } else if (wtime != -1 && btime != -1) {
        int timeRemaining = (pos->stm == WHITE) ? wtime : btime;
        int increment = (pos->stm == WHITE) ? winc : binc;

        thinkingTime = (timeRemaining / 20) + (increment / 2);

        if (thinkingTime >= timeRemaining) {
            thinkingTime = timeRemaining - 500;
        }
        if (thinkingTime < 0) {
            thinkingTime = 100; // minimum 100 ms
        }
    }

    if (depth == -1 && thinkingTime == -1) {
        depth = 6;
    }

    iterativeDeepening(pos, depth, thinkingTime);
}

void parsePosition(char* line, Position* pos) {
    line += 9; // skip "position "

    char* ptr;

    if (!strncmp(line, "startpos", 8)) {
        parseFen(STARTPOS, pos);
    } else {
        ptr = strstr(line, "fen");
        if (ptr == NULL) {
            parseFen(STARTPOS, pos);
        } else {
            ptr += 4;
            parseFen(ptr, pos);
        }
    }

    ptr = strstr(line, "moves");

    if (ptr == NULL) {
        return;
    }

    ptr += 6;
    Undo undo;
    for (char* moves = strtok(ptr, " \r\n\t"); moves != NULL; moves = strtok(NULL, " \r\n\t")) {
        Move m = parseMove(moves, pos);
        if (!m) {
            break;
        }

        makeMove(pos, m, &undo);
    }
}

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

        } else if (!strncmp(line, "go", 2)) {
            parseGo(line, &pos);
        } else if (!strncmp(line, "isready", 7)) {
            printf("readyok\n");
        } else if (!strncmp(line, "ucinewgame", 10)) {
            ttClear();
            parseFen(STARTPOS, &pos);
        } else if (!strncmp(line, "perft", 5)) {
            strtok(line, " ");
            char* d = strtok(NULL, " ") ? : "5";
            char* fen = strtok(NULL, "\0") ? : STARTPOS;

            int depth = atoi(d);
            parseFen(fen, &pos);
            perftTest(&pos, depth);
        } else if (!strncmp(line, "position", 8)) {
            parsePosition(line, &pos);
        } else if (!strncmp(line, "print", 5)) {
            printPosition(&pos);
        } else if (!strncmp(line, "quit", 4)) {
            break;
        }
    }
}