#include "inference.h"
#include <math.h>
#include <immintrin.h>

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

    // transpose from [in][out] to [out][in]
    int8_t* tempW = (int8_t*)malloc(wSize);
    fread(tempW, sizeof(int8_t), wSize, file);
    for (int j = 0; j < layer->inFeatures; j++) {
        for (int i = 0; i < layer->outFeatures; i++) {
            layer->weight[i * layer->inFeatures + j] = tempW[j * layer->outFeatures + i];
        }
    }


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

    int16_t* wAcc = pos->nnueAcc.white;
    int16_t* bAcc = pos->nnueAcc.black;
    const int16_t* wWeight = &model->featureTransformer->weight[wIdx * ACC_SIZE];
    const int16_t* bWeight = &model->featureTransformer->weight[bIdx * ACC_SIZE];

    if (isAdding) {
        for (int i = 0; i < ACC_SIZE; i+=16) {
            __m256i vwAcc= _mm256_loadu_si256((__m256i*)&wAcc[i]);
            __m256i vwWeight = _mm256_loadu_si256((__m256i*)&wWeight[i]);
            _mm256_storeu_si256((__m256i*)&wAcc[i], _mm256_add_epi16(vwAcc, vwWeight));

            __m256i vbAcc= _mm256_loadu_si256((__m256i*)&bAcc[i]);
            __m256i vbWeight = _mm256_loadu_si256((__m256i*)&bWeight[i]);
            _mm256_storeu_si256((__m256i*)&bAcc[i], _mm256_add_epi16(vbAcc, vbWeight));
        }
    } else {
        for (int i = 0; i < ACC_SIZE; i+=16) {
            __m256i vwAcc= _mm256_loadu_si256((__m256i*)&wAcc[i]);
            __m256i vwWeight = _mm256_loadu_si256((__m256i*)&wWeight[i]);
            _mm256_storeu_si256((__m256i*)&wAcc[i], _mm256_sub_epi16(vwAcc, vwWeight));

            __m256i vbAcc= _mm256_loadu_si256((__m256i*)&bAcc[i]);
            __m256i vbWeight = _mm256_loadu_si256((__m256i*)&bWeight[i]);
            _mm256_storeu_si256((__m256i*)&bAcc[i], _mm256_sub_epi16(vbAcc, vbWeight));
        }
    }
}


// evaluation

int evaluateNNUEQuantized(const Position* pos, NNUE* model) {
    alignas(32) uint8_t currentBuffer[ACC_SIZE * 2];
    alignas(32) uint8_t nextBuffer[ACC_SIZE * 2];

    uint8_t* inPtr = currentBuffer;
    uint8_t* outPtr = nextBuffer;

    const int16_t* stmAcc = (pos->stm == WHITE) ? pos->nnueAcc.white : pos->nnueAcc.black;
    const int16_t* nstmAcc = (pos->stm == WHITE) ? pos->nnueAcc.black : pos->nnueAcc.white;

    // perspective concat
    for (int i = 0; i < ACC_SIZE; i+=16) {
        __m256i stm = _mm256_load_si256((__m256i*)&stmAcc[i]);
        __m256i nstm = _mm256_load_si256((__m256i*)&nstmAcc[i]);

        __m128i stm_low = _mm256_castsi256_si128(stm);
        __m128i stm_high = _mm256_extracti128_si256(stm, 1);

        __m128i nstm_low = _mm256_castsi256_si128(nstm);
        __m128i nstm_high = _mm256_extracti128_si256(nstm, 1);

        __m128i stm_packed = _mm_packus_epi16(stm_low, stm_high);
        __m128i nstm_packed = _mm_packus_epi16(nstm_low, nstm_high);

        _mm_store_si128((__m128i*)&inPtr[i], stm_packed);
        _mm_store_si128((__m128i*)&inPtr[ACC_SIZE + i], nstm_packed);
    }

    // hidden layers
    int currentDim = ACC_SIZE * 2;
    int32_t finalLogit = 0;

    for (int l = 0; l < model->numHidden; l++) {
        HiddenLayer* hl = model->hiddenLayers[l];
        int32_t sums[256];

        __m256i ones = _mm256_set1_epi16(1);

        for (int i = 0; i < hl->outFeatures; i+=4) {
            __m256i sum32_0 = _mm256_setzero_si256();
            __m256i sum32_1 = _mm256_setzero_si256();
            __m256i sum32_2 = _mm256_setzero_si256();
            __m256i sum32_3 = _mm256_setzero_si256();


            for (int j = 0; j < hl->inFeatures; j += 32) {
                __m256i in = _mm256_loadu_si256((__m256i*)&inPtr[j]);
                
                __m256i weight_0 = _mm256_loadu_si256((__m256i*)&hl->weight[i * hl->inFeatures + j]);
                __m256i res16_0 = _mm256_maddubs_epi16(in, weight_0);
                __m256i res32_0 = _mm256_madd_epi16(res16_0, ones);
                sum32_0 = _mm256_add_epi32(sum32_0, res32_0);

                __m256i weight_1 = _mm256_loadu_si256((__m256i*)&hl->weight[(i + 1) * hl->inFeatures + j]);
                __m256i res16_1 = _mm256_maddubs_epi16(in, weight_1);
                __m256i res32_1 = _mm256_madd_epi16(res16_1, ones);
                sum32_1 = _mm256_add_epi32(sum32_1, res32_1);

                __m256i weight_2 = _mm256_loadu_si256((__m256i*)&hl->weight[(i + 2) * hl->inFeatures + j]);
                __m256i res16_2 = _mm256_maddubs_epi16(in, weight_2);
                __m256i res32_2 = _mm256_madd_epi16(res16_2, ones);
                sum32_2 = _mm256_add_epi32(sum32_2, res32_2);

                __m256i weight_3 = _mm256_loadu_si256((__m256i*)&hl->weight[(i + 3) * hl->inFeatures + j]);
                __m256i res16_3 = _mm256_maddubs_epi16(in, weight_3);
                __m256i res32_3 = _mm256_madd_epi16(res16_3, ones);
                sum32_3 = _mm256_add_epi32(sum32_3, res32_3);
            }

            __m128i vlow_0 = _mm256_castsi256_si128(sum32_0);
            __m128i vhigh_0 = _mm256_extracti128_si256(sum32_0, 1);
            __m128i sum128_0 = _mm_add_epi32(vlow_0, vhigh_0);
            sum128_0 = _mm_add_epi32(sum128_0, _mm_shuffle_epi32(sum128_0, _MM_SHUFFLE(1, 0, 3, 2)));
            sum128_0 = _mm_add_epi32(sum128_0, _mm_shuffle_epi32(sum128_0, _MM_SHUFFLE(2, 3, 0, 1)));
            sums[i + 0] = hl->bias[i + 0] + _mm_cvtsi128_si32(sum128_0);

            __m128i vlow_1 = _mm256_castsi256_si128(sum32_1);
            __m128i vhigh_1 = _mm256_extracti128_si256(sum32_1, 1);
            __m128i sum128_1 = _mm_add_epi32(vlow_1, vhigh_1);
            sum128_1 = _mm_add_epi32(sum128_1, _mm_shuffle_epi32(sum128_1, _MM_SHUFFLE(1, 0, 3, 2)));
            sum128_1 = _mm_add_epi32(sum128_1, _mm_shuffle_epi32(sum128_1, _MM_SHUFFLE(2, 3, 0, 1)));
            sums[i + 1] = hl->bias[i + 1] + _mm_cvtsi128_si32(sum128_1);

            __m128i vlow_2 = _mm256_castsi256_si128(sum32_2);
            __m128i vhigh_2 = _mm256_extracti128_si256(sum32_2, 1);
            __m128i sum128_2 = _mm_add_epi32(vlow_2, vhigh_2);
            sum128_2 = _mm_add_epi32(sum128_2, _mm_shuffle_epi32(sum128_2, _MM_SHUFFLE(1, 0, 3, 2)));
            sum128_2 = _mm_add_epi32(sum128_2, _mm_shuffle_epi32(sum128_2, _MM_SHUFFLE(2, 3, 0, 1)));
            sums[i + 2] = hl->bias[i + 2] + _mm_cvtsi128_si32(sum128_2);

            __m128i vlow_3 = _mm256_castsi256_si128(sum32_3);
            __m128i vhigh_3 = _mm256_extracti128_si256(sum32_3, 1);
            __m128i sum128_3 = _mm_add_epi32(vlow_3, vhigh_3);
            sum128_3 = _mm_add_epi32(sum128_3, _mm_shuffle_epi32(sum128_3, _MM_SHUFFLE(1, 0, 3, 2)));
            sum128_3 = _mm_add_epi32(sum128_3, _mm_shuffle_epi32(sum128_3, _MM_SHUFFLE(2, 3, 0, 1)));
            sums[i + 3] = hl->bias[i + 3] + _mm_cvtsi128_si32(sum128_3);
        }

        if (l < model->numHidden - 1) {

            for (int i = 0; i < hl->outFeatures; i+=16) {
                __m128i v0 = _mm_loadu_si128((__m128i*)&sums[i + 0]);
                __m128i v1 = _mm_loadu_si128((__m128i*)&sums[i + 4]);
                __m128i v2 = _mm_loadu_si128((__m128i*)&sums[i + 8]);
                __m128i v3 = _mm_loadu_si128((__m128i*)&sums[i + 12]);

                v0 = _mm_srai_epi32(v0, 6);
                v1 = _mm_srai_epi32(v1, 6);
                v2 = _mm_srai_epi32(v2, 6);
                v3 = _mm_srai_epi32(v3, 6);

                __m128i p0 = _mm_packs_epi32(v0, v1);
                __m128i p1 = _mm_packs_epi32(v2, v3);

                __m128i packed = _mm_packus_epi16(p0, p1);

                _mm_store_si128((__m128i*)&outPtr[i], packed);
            }
            uint8_t* temp = inPtr;
            inPtr = outPtr;
            outPtr = temp;
        } else {
            finalLogit = sums[0];
        }
    }

    // unquantize, (QA = 255) * (QB = 64) = 16320
    float logit = (float) finalLogit / 16320.0f;

    return (int)roundf(400.0f * logit);
}