#ifndef MODIFY_FIELD_H_
#define MODIFY_FIELD_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "bit_operations.h"

#define MTCH_FLD_LEN 45
#define ACTN_ST_LEN  31

void extract_field(uint8_t table[], int entry_no, int strt_bit, int end_bit, uint8_t val[]);
void replace_field(uint8_t table[], int entry_no, int strt_bit, int end_bit, uint8_t val[]);

//extracts value of a field from a given action; either one saved in the action table, or
//new value set given with the modify entry command
//start and end bit positions are for a single entry, not the table
//they are inclusive
void extract_field(uint8_t table[], int entry_no, int strt_bit, int end_bit, uint8_t val[]){
    int i;
    for(i = strt_bit; i <= end_bit; i++){
        if(read_bit(table, entry_no * ACTN_ST_LEN * 8 + i))
            set_bit(val, i - strt_bit);
        else
            reset_bit(val, i - strt_bit);
    }
}

void replace_field(uint8_t table[], int entry_no, int strt_bit, int end_bit, uint8_t val[]){
    int i;
    for(i = strt_bit; i <= end_bit; i++){
        if(read_bit(val, i - strt_bit))
            set_bit(table, entry_no * ACTN_ST_LEN * 8 + i);
        else
            reset_bit(table, entry_no * ACTN_ST_LEN * 8 + i);
    }
}

#endif
