//
// Created by Fan Jin on 11/8/19.
//

#ifndef CUSTOM_H
#define CUSTOM_H

#include <string.h>
#include "predictor.h"

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

#define SizeChoicePHT (1u << 11u) // Size of Choice PHT: 26K
#define SizeBHT (1u << 13u) // Size of BHT: 16K
#define SizeCache (1u << 10u) // Size of one cache: 10K
#define TagWidth (1u << 8u) // Size of two caches: 20K
// Total size: 62K

static uint64_t globalHistory;

typedef struct {
    int8_t ctr;
    uint16_t tag;
} Entry;
static Entry cacheT[SizeCache];
static Entry cacheN[SizeCache];

static uint16_t choicePHT[SizeChoicePHT];
static int8_t BHT[SizeBHT];

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void init_custom() {
    globalHistory = NOTTAKEN; // initialize to Not Taken
    memset(choicePHT, NOTTAKEN, sizeof(uint16_t) * SizeChoicePHT);
    memset(BHT, WeaklyNotTaken, sizeof(uint8_t) * SizeBHT);
    for (unsigned i = 0; i < SizeCache; i++) {
        cacheT->ctr = WeaklyNotTaken;
        cacheN->ctr = WeaklyNotTaken;
        cacheT->tag = NOTTAKEN;
        cacheN->tag = NOTTAKEN;
    }
}

uint8_t predict_custom(uint32_t pc) {
    uint16_t pc_tag = (uint16_t)pc & (TagWidth - 1u);
    uint16_t pc_low = (uint16_t)pc & (SizeChoicePHT - 1u);
    uint16_t addr = (pc ^ globalHistory) & (SizeCache - 1u);
    int8_t choice = BHT[choicePHT[pc_low]];
    if (choice >= WeaklyTaken) {
        if (cacheN[addr].tag == pc_tag) {
            return cacheN[addr].ctr >= WeaklyTaken;
        } else {
            return TAKEN;
        }
    } else {
        if (cacheT[addr].tag == pc_tag) {
            return cacheT[addr].ctr >= WeaklyTaken;
        } else {
            return NOTTAKEN;
        }
    }
}

void train_custom(uint32_t pc, uint8_t outcome) {
    uint16_t pc_tag = (uint16_t)pc & (TagWidth - 1u);
    uint16_t pc_low = (uint16_t)pc & (SizeChoicePHT - 1u);
    uint16_t addr = (pc ^ globalHistory) & (SizeCache - 1u);
    int8_t *choice = &BHT[choicePHT[pc_low]];

    int updateChoicePHT;
    if (outcome) {
        // increase choice PHT
        updateChoicePHT = 1;
        if (choice[0] <= WeaklyNotTaken && cacheT[addr].tag == pc_tag && cacheT[addr].ctr >= WeaklyTaken) {
            // except for this case
            updateChoicePHT = 0;
        }
    } else {
        // decrease choice PHT
        updateChoicePHT = -1;
        if (choice[0] >= WeaklyTaken && cacheN[addr].tag == pc_tag && cacheN[addr].ctr <= WeaklyNotTaken) {
            // except for this case
            updateChoicePHT = 0;
        }
    }
    if (updateChoicePHT > 0) choice[0] = MIN(choice[0] + 1, 3);
    if (updateChoicePHT < 0) choice[0] = MAX(choice[0] - 1, 0);

    Entry *updateCache = NULL;
    if (choice[0] >= WeaklyTaken) {
        if (!outcome || cacheN[addr].tag == pc_tag) {
            // update N cache
            updateCache = cacheN;
        }
    } else {
        if (outcome || cacheT[addr].tag == pc_tag) {
            // update T cache
            updateCache = cacheT;
        }
    }
    if (updateCache != NULL) {
        if (updateCache[addr].tag == pc_tag) {
            if (outcome)
                updateCache[addr].ctr = MIN(updateCache[addr].ctr + 1, 3);
            else
                updateCache[addr].ctr = MAX(updateCache[addr].ctr - 1, 0);
        } else {
            updateCache[addr].tag = pc_tag;
            if (outcome)
                updateCache[addr].ctr = WeaklyTaken;
            else
                updateCache[addr].ctr = WeaklyNotTaken;
        }
    }

    // update pattern
    choicePHT[pc_low] = ((choicePHT[pc_low] << 1u) | (outcome & 1u)) & (SizeBHT - 1u);
    globalHistory = (globalHistory << 1u) | (outcome & 1u);
}

#endif