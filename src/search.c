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

int nonPawnMaterial(Position* pos) {
    // for NMP to guard Zugzwang
    if (pos->stm == WHITE) {
        return pos->pieces[WHITE_KNIGHT] || pos->pieces[WHITE_BISHOP] || pos->pieces[WHITE_ROOK] || pos->pieces[WHITE_QUEEN];
    } else {
        return pos->pieces[BLACK_KNIGHT] || pos->pieces[BLACK_BISHOP] || pos->pieces[BLACK_ROOK] || pos->pieces[BLACK_QUEEN];
    }
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
    state->pvLength[state->ply] = state->ply;

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

    int kingSq = __builtin_ctzll(pos->pieces[(pos->stm == WHITE) ? WHITE_KING : BLACK_KING]);
    int inCheck = isSquareAttacked(pos, kingSq, pos->xstm);

    int pvNode = (beta - alpha > 1);

    // RFP
    if (!pvNode && !inCheck && depth <= 3) {
        int staticEval = evaluateLegalPos(pos);

        if (depth == 1 && staticEval - PIECE_TO_SCORE[BISHOP] > beta) {
            return beta;
        }
        if (depth == 2 && staticEval - PIECE_TO_SCORE[ROOK] > beta) {
            return beta;
        }
        if (depth == 3 && staticEval - PIECE_TO_SCORE[QUEEN] > beta) {
            depth--; // demote search
        }
    }

    // NMP
    Undo undo;
    if (!pvNode && nonPawnMaterial(pos) && !inCheck) {
        int R = 3; // reduction
        makeNullMove(pos, &undo);
        state->ply++;
        int nullDepth = depth - R - 1;
        if (nullDepth < 0) {
            nullDepth = 0;
        }
        int nullScore = -negaMax(pos, nullDepth, -beta, -beta + 1, state);
        state->ply--;
        unmakeNullMove(pos, &undo);

        if (state->abort) {
            return 0;
        }

        if (nullScore >= beta) {
            return beta;
        }
    }

    MoveList pseudoMoves;
    generatePseudoMoves(pos, &pseudoMoves);

    int legalMovesPlayed = 0;
    int bestValue = -INFINITY_SCORE;
    int bestMove = 0; // used for tt storing
    int originalAlpha = alpha; // used for tt storing

    orderMoves(&pseudoMoves, pos, ttMove);
    for (int i = 0; i < pseudoMoves.size; i++) {
        if (!makeMovePseudo(pos, pseudoMoves.moves[i], &undo)) {
            continue;
        }
        legalMovesPlayed++;
        state->ply++;

        int score;
        if (legalMovesPlayed == 1) {
            // perform full window search
            score = -negaMax(pos, depth - 1, -beta, -alpha, state);
        } else {
            // LMR
            int wasReduced = 0;
            //int oppKingSq = __builtin_ctzll(pos->pieces[(pos->stm == WHITE) ? BLACK_KING : WHITE_KING]);
            //int oppInCheck = isSquareAttacked(pos, oppKingSq, pos->stm);
            if (depth > 3 && legalMovesPlayed > 2 && !inCheck && !IsCapture(pseudoMoves.moves[i]) 
            && !IsPromo(pseudoMoves.moves[i])) {
                int reduction = (legalMovesPlayed > 6) ? 2 : 1;

                score = -negaMax(pos, depth - 1 - reduction, -alpha - 1, -alpha, state);

                if (score <= alpha) {
                    // skip rest of search
                    wasReduced = 1;
                }
            }
            if (!wasReduced) {
                // perform null window search
                score = -negaMax(pos, depth - 1, -alpha - 1, -alpha, state);
                if (score > alpha && score < beta) {
                    // re-search
                    score = -negaMax(pos, depth - 1, -beta, -alpha, state);
                }
            }
        }
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
            
            state->pvTable[state->ply][state->ply] = pseudoMoves.moves[i];

            for (int nextPly = state->ply + 1; nextPly < state->pvLength[state->ply + 1]; nextPly++) {
                state->pvTable[state->ply][nextPly] = state->pvTable[state->ply + 1][nextPly];
            }

            state->pvLength[state->ply] = state->pvLength[state->ply + 1];
        }

        if (alpha >= beta) {
            // cutoff
            break;
        }
    }

    if (legalMovesPlayed == 0) {
        // checkmate or stalemate
        if (inCheck) {
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

    int32_t ttScore = 0;
    Move ttMove = 0; 
    uint8_t ttFlag = 0;

    if (ttProbe(pos->hash, 0, &ttMove, &ttScore, &ttFlag)) {
        if (!moveIsLegal(pos, ttMove)) {
            // collision
            ttMove = 0;
        } 
    }

    MoveList legalMoves;
    generateLegalMoves(pos, &legalMoves);

    if (legalMoves.size == 0) {
        printf("bestmove 0000\n");
        return;
    }

    orderMoves(&legalMoves, pos, ttMove);
    Move bestMoveSoFar = legalMoves.moves[0];

    Undo undo;
    for (int j = 1; j <= maxDepth; j++) {

        int alpha = -INFINITY_SCORE;
        int beta = INFINITY_SCORE;
        Move bestRootMoveThisDepth = legalMoves.moves[0];
        state.pvLength[0] = 0;

        for (int i = 0; i < legalMoves.size; i++) {
            makeMove(pos, legalMoves.moves[i], &undo);
            state.ply++;

            int score;
            if (i == 0) {
                // full search
                score = -negaMax(pos, j - 1, -beta, -alpha, &state);
            } else {
                score =  -negaMax(pos, j - 1, -alpha - 1, -alpha, &state);
                if (score > alpha && score < beta) {
                    score = -negaMax(pos, j - 1, -beta, -alpha, &state);
                }
            }
            state.ply--;
            unmakeMove(pos, legalMoves.moves[i], &undo);
            
            if (state.abort) {
                break; // don't use partial results
            }

            if (score > alpha) {
                alpha = score;
                bestRootMoveThisDepth = legalMoves.moves[i];

                state.pvTable[0][0] = legalMoves.moves[i];
                
                for (int nextPly = 1; nextPly < state.pvLength[1]; nextPly++) {
                    state.pvTable[0][nextPly] = state.pvTable[1][nextPly];
                }

                state.pvLength[0] = state.pvLength[1];
            }
        }

        if (state.abort) {
            break;
        }

        bestMoveSoFar = bestRootMoveThisDepth;
        long long duration = getTimeMS() - state.startTime;
        printf("info depth %d score cp %d time %lld nodes %lld pv ", j, alpha, duration, state.nodes);
        for (int k = 0; k < state.pvLength[0]; k++) {
            printf("%s ", moveToStr(state.pvTable[0][k]));
        }
        printf("\n");

    }
    printf("bestmove %s\n", moveToStr(bestMoveSoFar));
}