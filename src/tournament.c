//
// Created by Fan Jin on 11/8/19.
//

#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include <string.h>
#include "predictor.h"

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

#define SizeGlobalBHT (1u << (unsigned int)ghistoryBits)
#define SizeLocalBHT (1u << (unsigned int)lhistoryBits)
#define SizeChoiceBHT (1u << (unsigned int)ghistoryBits)
#define SizePHT (1u << (unsigned int)pcIndexBits)
static uint8_t *globalBHT;
static uint8_t *localBHT;
static uint8_t *choiceBHT;
static uint32_t *PHT;
static uint32_t globalHistory;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

void init_tournament() {
    globalBHT = malloc(sizeof(uint8_t) * SizeGlobalBHT);
    localBHT = malloc(sizeof(uint8_t) * SizeLocalBHT);
    choiceBHT = malloc(sizeof(uint8_t) * SizeChoiceBHT);
    PHT = malloc(sizeof(uint32_t) * SizePHT);
    memset(globalBHT, WeaklyNotTaken, sizeof(uint8_t) * SizeGlobalBHT); // initialize to Weakly Not Taken
    memset(localBHT, WeaklyNotTaken, sizeof(uint8_t) * SizeLocalBHT); // initialize to Weakly Not Taken
    memset(choiceBHT, WeaklyGlobal, sizeof(uint8_t) * SizeChoiceBHT); // initialize to Weakly Global
    memset(PHT, NOTTAKEN, sizeof(uint8_t) * SizePHT); // initialize to Not Taken
    globalHistory = NOTTAKEN; // initialize to Not Taken
}

uint8_t predict_tournament(uint32_t pc) {
    uint32_t ghr_low = globalHistory;
    uint32_t add_low = pc & (SizePHT - 1u);
    uint8_t resultLocal = localBHT[PHT[add_low]] >= WeaklyTaken;
    uint8_t resultGlobal = globalBHT[ghr_low] >= WeaklyTaken;
    uint8_t resultChoice = choiceBHT[ghr_low];
    return (resultChoice >= WeaklyLocal) ? resultLocal : resultGlobal;
}

void train_tournament(uint32_t pc, uint8_t outcome) {
    uint32_t ghr_low = globalHistory;
    uint32_t add_low = pc & (SizePHT - 1u);
    uint8_t *resultLocal = &localBHT[PHT[add_low]];
    uint8_t *resultGlobal = &globalBHT[ghr_low];
    uint8_t *resultChoice = &choiceBHT[ghr_low];

    // update choice table

    if ((*resultLocal >= WeaklyTaken) == outcome && (*resultGlobal >= WeaklyTaken) != outcome) {
        if (*resultChoice < StronglyLocal) {
            (*resultChoice)++;
        }
    }
    if ((*resultLocal >= WeaklyTaken) != outcome && (*resultGlobal >= WeaklyTaken) == outcome) {
        if (*resultChoice > StronglyGlobal) {
            (*resultChoice)--;
        }
    }

    // update global and local table

    if (outcome) {
        if (*resultGlobal < StronglyTaken) {
            (*resultGlobal)++;
        }
        if (*resultLocal < StronglyTaken) {
            (*resultLocal)++;
        }
    } else {
        if (*resultGlobal > StronglyNotTaken) {
            (*resultGlobal)--;
        }
        if (*resultLocal > StronglyNotTaken) {
            (*resultLocal)--;
        }
    }

    // update pattern

    PHT[add_low] = ((PHT[add_low] << 1u) | (outcome & 1u)) & (SizeLocalBHT - 1u);
    globalHistory = ((globalHistory << 1u) | (outcome & 1u)) & (SizeGlobalBHT - 1u);
}

#endif