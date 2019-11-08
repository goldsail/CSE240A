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

#define GshareSizeBHT (1u << (unsigned int)ghistoryBits)
static uint8_t *BHT;
static uint32_t globalHistory;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

void init_gshare() {
    BHT = (uint8_t*)malloc(sizeof(uint8_t) * GshareSizeBHT);
    memset(BHT, 1, sizeof(uint8_t) * GshareSizeBHT); // initialize to Weakly Not Taken
    globalHistory = 0; // initialize to Not Taken
}

uint8_t predict_gshare(uint32_t pc) {
    uint32_t ghr_low = globalHistory;
    uint32_t add_low = pc & (GshareSizeBHT - 1u);
    uint32_t address = ghr_low ^ add_low;
    return BHT[address] > 1;
}

void train_gshare(uint32_t pc, uint8_t outcome) {
    uint32_t ghr_low = globalHistory;
    uint32_t add_low = pc & (GshareSizeBHT - 1u);
    uint32_t address = ghr_low ^ add_low;
    if (outcome) {
        if (BHT[address] < 3) {
            BHT[address]++;
        }
    } else {
        if (BHT[address] > 0) {
            BHT[address]--;
        }
    }
    globalHistory = ((globalHistory << 1u) | (outcome & 1u)) & (GshareSizeBHT - 1u);
}

#endif