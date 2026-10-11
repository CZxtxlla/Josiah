#include "inference.h"
#include <math.h>

NNUE* model = NULL;

HiddenLayer* loadHiddenLayer(FILE* file) {
    HiddenLayer* layer = (HiddenLayer*)malloc(sizeof(HiddenLayer));
    if (fread(&layer->inFeatures, sizeof(int), 1, file) != 1) {
        return NULL;
    }
    if (fread(&layer->outFeatures, sizeof(int), 1, file) != 1) {
        return NULL;
    }

    int wSize = layer->inFeatures * layer->outFeatures;
    int bSize = layer->outFeatures;

    layer->weight = (int8_t*)malloc(wSize * sizeof(int8_t));
    layer->bias = (int32_t*)malloc(bSize * sizeof(uint32_t));

    fread(layer->weight, sizeof(int8_t), wSize, file);
    fread(layer->bias, sizeof(int32_t), bSize, file);

    return layer;
}

FeatureTransformer* loadFeatureTransformer(FILE* file) {
    FeatureTransformer* layer = (FeatureTransformer*)malloc(sizeof(FeatureTransformer));
    if (fread(&layer->inFeatures, sizeof(int), 1, file) != 1) {
        return NULL;
    }
    if (fread(&layer->outFeatures, sizeof(int), 1, file) != 1) {
        return NULL;
    }

    int wSize = layer->inFeatures * layer->outFeatures;
    int bSize = layer->outFeatures;

    layer->weight = (int16_t*)malloc(wSize * sizeof(int16_t));
    layer->bias = (int16_t*)malloc(bSize * sizeof(uint16_t));

    fread(layer->weight, sizeof(int16_t), wSize, file);
    fread(layer->bias, sizeof(int16_t), bSize, file);

    return layer;
}

NNUE* loadNNUE(const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        printf("Error: Could not open %s\n", filepath);
        return NULL;
    }

    uint32_t magic;
    if (fread(&magic, sizeof(uint32_t), 1, file) != 1 || magic != NNUE_MAGIC_NUMBER) {
        printf("Error: Invalid NNUE file format or magic number.\n");
        return NULL;
    }

    NNUE* model = (NNUE*)malloc(sizeof(NNUE));
    fread(&model->numHidden, sizeof(int), 1, file);

    model->featureTransformer = loadFeatureTransformer(file);

    model->hiddenLayers = (HiddenLayer**)malloc(model->numHidden * sizeof(HiddenLayer*));
    for (int i = 0; i < model->numHidden; i++) {
        model->hiddenLayers[i] = loadHiddenLayer(file);
    }

    fclose(file);

    return model;
}

// cleanup

void freeHiddenLayer(HiddenLayer* layer) {
    if (layer) {
        free(layer->weight);
        free(layer->bias);
        free(layer);
    }
}

void freeFeatureTransformer(FeatureTransformer* layer) {
    if (layer) {
        free(layer->weight);
        free(layer->bias);
        free(layer);
    }
}

void freeNNUE(NNUE* model) {
    if (model) {
        freeFeatureTransformer(model->featureTransformer);

        for (int i = 0; i < model->numHidden; i++) {
            freeHiddenLayer(model->hiddenLayers[i]);
        }

        free((void*)model->hiddenLayers);
        free(model);
    }
}

// helpers for accumulator

static inline int flipSq(int sq) {
    return sq ^ 56;
}

static inline int flipPiece(int pType) {
    return (pType + 6) % 12;
}

static inline int16_t clippedReluInt(int32_t x) {
    if (x < 0) {
        return 0;
    }
    if (x > 255) {
        return 255;
    }
    return (uint8_t)x;
}

// accumulator stuff

void initAccumulator(Position* pos, NNUE* model) {
    // bias
    for (int i = 0; i < ACC_SIZE; i++) {
        pos->nnueAcc.white[i] = model->featureTransformer->bias[i];
        pos->nnueAcc.black[i] = model->featureTransformer->bias[i];
    }

    // pieces on board
    for (int p = 0; p < 12; p++) {
        Bitboard bb = pos->pieces[p];
        while(bb) {
            int sq = poplsb(&bb);

            int wIdx = (p * 64) + sq;
            int bIdx = (flipPiece(p) * 64) + flipSq(sq);

            for (int i = 0; i < ACC_SIZE; i++) {
                pos->nnueAcc.white[i] += model->featureTransformer->weight[wIdx * ACC_SIZE + i];
                pos->nnueAcc.black[i] += model->featureTransformer->weight[bIdx * ACC_SIZE + i];
            }
        }
    }
}

void updateAccumulator(Position* pos, NNUE* model, int piece, int sq, int isAdding) {
    int wIdx = (piece * 64) + sq;
    int bIdx = (flipPiece(piece) * 64) + flipSq(sq);

    int sign = isAdding ? 1 : -1;

    for (int i = 0; i < ACC_SIZE; i++) {
        pos->nnueAcc.white[i] += sign * model->featureTransformer->weight[wIdx * ACC_SIZE + i];
        pos->nnueAcc.black[i] += sign * model->featureTransformer->weight[bIdx * ACC_SIZE + i];
    }
}


// evaluation

int evaluateNNUEQuantized(const Position* pos, NNUE* model) {
    uint8_t currentInput[ACC_SIZE * 2];
    uint8_t nextInput[ACC_SIZE * 2];

    const int16_t* stmAcc = (pos->stm == WHITE) ? pos->nnueAcc.white : pos->nnueAcc.black;
    const int16_t* nstmAcc = (pos->stm == WHITE) ? pos->nnueAcc.black : pos->nnueAcc.white;

    // perspective concat
    for (int i = 0; i < ACC_SIZE; i++) {
        currentInput[i] = clippedReluInt(stmAcc[i]);
        currentInput[ACC_SIZE + i] = clippedReluInt(nstmAcc[i]);
    }

    // hidden layers
    int currentDim = ACC_SIZE * 2;
    int32_t finalLogit = 0;

    for (int l = 0; l < model->numHidden; l++) {
        HiddenLayer* hl = model->hiddenLayers[l];

        int32_t sums[256];
        for (int i = 0; i < hl->outFeatures; i++) {
            sums[i] = hl->bias[i];
        }
        for (int j = 0; j < hl->inFeatures; j++) {
            int32_t inVal = (int32_t)currentInput[j];

            for (int i = 0; i < hl->outFeatures; i++) {
                sums[i] += inVal * hl->weight[(j * hl->outFeatures) + i];
            }
        }

        for (int i = 0; i < hl->outFeatures; i++) {
            if (l < model->numHidden - 1) {
                nextInput[i] = clippedReluInt(sums[i] >> 6);
            } else {
                finalLogit = sums[i];
            }
        }

        if (l < model->numHidden - 1) {
            currentDim = hl->outFeatures;
            for (int i = 0; i < currentDim; i++) {
                currentInput[i] = nextInput[i];
            }
        }
    }

    // unquantize, (QA = 255) * (QB = 64) = 16320
    float logit = (float) finalLogit / 16320.0f;

    return (int)roundf(400.0f * logit);
}

