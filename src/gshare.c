//
// Created by Fan Jin on 11/7/19.
//

#ifndef GSHARE_H
#define GSHARE_H

#include <string.h>
#include "predictor.h"

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

#define SizeBHT (1u << (unsigned int)ghistoryBits)
static uint8_t *BHT;
static uint32_t globalHistory;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

void init_gshare() {
    BHT = (uint8_t*)malloc(sizeof(uint8_t) * SizeBHT);
    memset(BHT, WeaklyNotTaken, sizeof(uint8_t) * SizeBHT); // initialize to Weakly Not Taken
    globalHistory = NOTTAKEN; // initialize to Not Taken
}

uint8_t predict_gshare(uint32_t pc) {
    uint32_t ghr_low = globalHistory;
    uint32_t add_low = pc & (SizeBHT - 1u);
    uint32_t address = ghr_low ^ add_low;
    return BHT[address] >= WeaklyTaken;
}

void train_gshare(uint32_t pc, uint8_t outcome) {
    uint32_t ghr_low = globalHistory;
    uint32_t add_low = pc & (SizeBHT - 1u);
    uint32_t address = ghr_low ^ add_low;
    if (outcome) {
        if (BHT[address] < StronglyTaken) {
            BHT[address]++;
        }
    } else {
        if (BHT[address] > StronglyNotTaken) {
            BHT[address]--;
        }
    }
    globalHistory = ((globalHistory << 1u) | (outcome & 1u)) & (SizeBHT - 1u);
}

#endif