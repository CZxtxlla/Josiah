#include "search.h"
#include "evaluate.h"
#include "movegen.h"
#include "utils.h"
#include "movepicker.h"
#include "transposition.h"
#include <limits.h>

int timeLimit = 100000000;

int isRepetition(Position* pos) {
    if (pos->half_moves < 4) {
        return 0;
    }

    for (int i = pos->historyPly - 2; i >= pos->historyPly - pos->half_moves; i -=2) {
        if (pos->history[i] == pos->hash) {
            return 1;
        }
    }
    return 0;
}

int quiescence(Position* pos, int alpha, int beta, SearchState* state) {
    state->nodes++;

    if (state->nodes % 2048 == 0) {
        if ((getTimeMS() - state->startTime) >= timeLimit) {
            state->abort = 1;
        }
    }

    if (state->abort) {
        return 0;
    }

    if (isRepetition(pos) || pos->half_moves >= 100) {
        return 0;
    }

    if (state->ply >= 128) {
        // arbitrary, potentially could remove
        return evaluateLegalPos(pos);
    }

    int staticEval = evaluateLegalPos(pos);
    int bestValue = staticEval;
    if (bestValue >= beta) {
        return bestValue;
    }
    if (bestValue > alpha) {
        alpha = bestValue;
    }

    MoveList noisyMoves;
    noisyMoves.size = 0;
    generateNoisyMoves(pos, &noisyMoves);

    orderMoves(&noisyMoves, pos, 0);

    Undo undo;
    for (int i = 0; i < noisyMoves.size; i++) {
        if (!makeMovePseudo(pos, noisyMoves.moves[i], &undo)) {
            continue;
        }
        state->ply++;
        int score = -quiescence(pos, -beta, -alpha, state);
        state->ply--;
        unmakeMove(pos, noisyMoves.moves[i], &undo);

        if (state->abort) {
            return 0;
        }

        if (score > bestValue) {
            bestValue = score;
        }

        if (score > alpha) {
            alpha = score;
        }

        if (alpha >= beta) {
            // cutoff
            break;
        }
    }
    return bestValue;
}


int negaMax(Position* pos, int depth, int alpha, int beta, SearchState* state) {
    state->nodes++;

    if (state->nodes % 2048 == 0) {
        if ((getTimeMS() - state->startTime) >= timeLimit) {
            state->abort = 1;
        }
    }

    if (state->abort) {
        return 0;
    }

    if (isRepetition(pos) || pos->half_moves >= 100) {
        return 0;
    }

    if (depth == 0) {
        return quiescence(pos, alpha, beta, state);
    }

    int32_t ttScore = 0;
    Move ttMove = 0; 
    uint8_t ttFlag = 0;

    if (ttProbe(pos->hash, depth, &ttMove, &ttScore, &ttFlag)) {
        if (!moveIsLegal(pos, ttMove)) {
            // collision
            ttMove = 0;
        } else {
            if (ttFlag == EXACT || (ttFlag == LOWERBOUND && ttScore >= beta) || (ttFlag == UPPERBOUND && ttScore <= alpha)) {
                return ttScore;
            }
        }
    }


    MoveList pseudoMoves;
    generatePseudoMoves(pos, &pseudoMoves);

    int legalMovesPlayed = 0;
    int bestValue = -INFINITY_SCORE;
    int bestMove = 0; // used for tt storing
    int originalAlpha = alpha; // used for tt storing

    orderMoves(&pseudoMoves, pos, ttMove);

    Undo undo;
    for (int i = 0; i < pseudoMoves.size; i++) {
        if (!makeMovePseudo(pos, pseudoMoves.moves[i], &undo)) {
            continue;
        }
        legalMovesPlayed++;
        state->ply++;
        int score = -negaMax(pos, depth - 1, -beta, -alpha, state);
        state->ply--;
        unmakeMove(pos, pseudoMoves.moves[i], &undo);

        if (state->abort) {
            return 0;
        }

        if (score > bestValue) {
            bestValue = score;
            bestMove = pseudoMoves.moves[i];
        }

        if (score > alpha) {
            alpha = score;
        }

        if (alpha >= beta) {
            // cutoff
            break;
        }
    }

    if (legalMovesPlayed == 0) {
        // checkmate or stalemate
        int kingsq = __builtin_ctzll(pos->pieces[(pos->stm == WHITE) ? WHITE_KING : BLACK_KING]);
        if (isSquareAttacked(pos, kingsq, pos->xstm)) {
            return -MATE_SCORE + state->ply;
        }
        return 0; // stalemate
    }

    uint8_t flag;
    if (bestValue <= originalAlpha) {
        flag = UPPERBOUND;
    } else if (bestValue >= beta) {
        flag = LOWERBOUND;
    } else {
        flag = EXACT;
    }
    ttStore(pos->hash, depth, bestMove, bestValue, flag);
    return bestValue;
}

void iterativeDeepening(Position* pos, int maxDepth, int searchTimeLimit) {
    SearchState state;
    state.ply = 0;
    state.nodes = 0;
    state.startTime = getTimeMS();
    state.abort = 0;

    timeLimit = (searchTimeLimit > 0) ? searchTimeLimit : 99999999;
    maxDepth = (maxDepth > 0) ? maxDepth : MAX_SEARCH_DEPTH;

    MoveList legalMoves;
    generateLegalMoves(pos, &legalMoves);

    if (legalMoves.size == 0) {
        printf("bestmove 0000\n");
        return;
    }

    orderMoves(&legalMoves, pos, 0);
    Move bestMoveSoFar = legalMoves.moves[0];

    Undo undo;
    for (int j = 1; j <= maxDepth; j++) {

        int alpha = -INFINITY_SCORE;
        int beta = INFINITY_SCORE;
        Move bestRootMoveThisDepth = legalMoves.moves[0];

        for (int i = 0; i < legalMoves.size; i++) {
            makeMove(pos, legalMoves.moves[i], &undo);
            state.ply++;
            int score = -negaMax(pos, j - 1, -beta, -alpha, &state);
            state.ply--;
            unmakeMove(pos, legalMoves.moves[i], &undo);
            
            if (state.abort) {
                break; // don't use partial results
            }

            if (score > alpha) {
                alpha = score;
                bestRootMoveThisDepth = legalMoves.moves[i];
            }
        }

        if (state.abort) {
            break;
        }

        bestMoveSoFar = bestRootMoveThisDepth;
        long long duration = getTimeMS() - state.startTime;
        printf("info depth %d score cp %d time %lld nodes %lld\n", j, alpha, duration, state.nodes);

    }
    printf("bestmove %s\n", moveToStr(bestMoveSoFar));
}