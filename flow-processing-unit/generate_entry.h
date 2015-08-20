#ifndef GENERATE_ENTRY_H_
#define GENERATE_ENTRY_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MTCH_FLD_LEN 45
#define ACTN_ST_LEN  31

void get_mask(uint8_t mask[]);
void get_flow_entry(uint8_t flow_entry[]);

void create_mask(uint8_t mask[]);
void create_flow_entry(uint8_t flow_entry[]);
void create_action(uint8_t action_set[]);

void get_mask(uint8_t mask[]){
    uint8_t val, i;
    for(i = 0; i < MTCH_FLD_LEN; i++){
        scanf("%d", &val);
        mask[i] = val;
    }
}

void get_flow_entry(uint8_t flow_entry[]){
    uint8_t val, i;
    for(i = 0; i <= MTCH_FLD_LEN; i++){
        scanf("%d", &val);
        flow_entry[i] = val;
    }
}

void create_mask(uint8_t mask[]){
    uint8_t val, i;
    for(i = 0; i < MTCH_FLD_LEN; i++){
        val = rand() % 106;
        mask[i] = val + 150;
    }
}

//generated flow entry has a length of one plus match field length,
//as a field for priority is needed
//field for counter and timeout is created when adding the entry to the table
void create_flow_entry(uint8_t flow_entry[]){
    uint8_t val, i;
    for(i = 0; i <= MTCH_FLD_LEN; i++){
        val = rand() % 256;
        flow_entry[i] = val;
    }
}

void create_action(uint8_t action_set[]){
    uint8_t val, i;
    for(i = 0; i < ACTN_ST_LEN; i++){
        val = rand() % 256;
        action_set[i] = val;
    }
}

#endif
