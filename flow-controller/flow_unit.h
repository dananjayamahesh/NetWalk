#ifndef FLOW_UNIT_H_
#define FLOW_UNIT_H_

#include <stdio.h>
#include <stdint.h>
#include "bit_operations.h"
#include "generate_entry.h"

#define MTCH_FLD_LEN 45
#define FLW_ENT_LEN  48
#define NO_F_FLWS    20

#define STR_ERR     -1
#define RDNDNT      -2
#define TBL_FULL    -3

typedef struct{
    uint8_t flow_table[FLW_ENT_LEN * NO_F_FLWS];
    uint8_t mask_table[MTCH_FLD_LEN * NO_F_FLWS];
    uint8_t action_table[ACTN_ST_LEN * NO_F_FLWS];
    uint16_t flag_table[NO_F_FLWS];
    int flow_tail;
    int mask_tail;
}tcam_unit;

void write_new_mask(tcam_unit * tcam, uint8_t mask[]);
void write_new_flow_entry(tcam_unit * tcam, uint8_t flow_entry[]);
void write_new_action(tcam_unit * tcam, uint8_t action_set[], uint16_t flag, int pos);

void restructure_flow_table(tcam_unit * tcam, int pos);
void restructure_action_tables(tcam_unit * flow_unit, int pos);

int search_within_mask_group(tcam_unit * tcam, uint8_t mask[], uint8_t flow_entry[], int mask_group_start);
int locate_mask_group(tcam_unit * tcam, int mask_no);
int search_tcam(tcam_unit * tcam, uint8_t new_flow[]);
int search_flow(tcam_unit * flow_unit, uint8_t new_flow[], uint8_t action_set[]);

int add_new_flow_entry(tcam_unit * tcam, uint8_t mask[], uint8_t flow_entry[]);
int add_new_entry(tcam_unit * flow_unit, uint8_t mask[], uint8_t flow_entry[], uint8_t action_set[], uint16_t flag);

void write_new_mask(tcam_unit * tcam, uint8_t mask[]){
    reset_bit(mask, 357);
    reset_bit(mask, 358);
    reset_bit(mask, 359);
    FILE * fp;
    fp = fopen("mask_table.txt", "a");
    fprintf(fp, "%d. ", tcam -> mask_tail);
    uint8_t i;
    for(i = 0; i < MTCH_FLD_LEN; i++){
        tcam -> mask_table[tcam -> mask_tail * MTCH_FLD_LEN + i] = mask[i];
        fprintf(fp, "%d ", mask[i]);
    }
    fprintf(fp, "\n");
    fclose(fp);
    tcam -> mask_tail = tcam -> mask_tail + 1;
}

void write_new_flow_entry(tcam_unit * tcam, uint8_t flow_entry[]){
    FILE * fp;
    fp = fopen("flow_table.txt", "a");
    fprintf(fp, "%d. ", tcam -> flow_tail);
    uint8_t i;
    for(i = 0; i <= MTCH_FLD_LEN; i++){
        tcam -> flow_table[tcam -> flow_tail * FLW_ENT_LEN + i] = flow_entry[i];
        fprintf(fp, "%d ", flow_entry[i]);
    }
    tcam -> flow_table[tcam -> flow_tail * FLW_ENT_LEN + i] = 0;
    tcam -> flow_table[tcam -> flow_tail * FLW_ENT_LEN + i + 1] = 255;
    fprintf(fp, "%d %d", 0, 255);
    fprintf(fp, "\n");
    fclose(fp);
    tcam -> flow_tail = tcam -> flow_tail + 1;
}

void write_new_action(tcam_unit * tcam, uint8_t action_set[], uint16_t flag, int pos){
    FILE * fp;
    fp = fopen("action_table.txt", "a");
    fprintf(fp, "%d. ", pos);
    uint8_t i;
    for(i = 0; i < ACTN_ST_LEN; i++){
        tcam -> action_table[pos * ACTN_ST_LEN + i] = action_set[i];
        fprintf(fp, "%d ", action_set[i]);
    }
    fprintf(fp, "\n");
    fclose(fp);
    tcam -> flag_table[pos] = flag;
}

void restructure_flow_table(tcam_unit * tcam, int pos){
    uint8_t dummy_flow[FLW_ENT_LEN];
    uint8_t i;
    int flow_tail_old = tcam -> flow_tail;
    while(tcam -> flow_tail > pos){
        for(i = 0; i < FLW_ENT_LEN; i++)
            dummy_flow[i] = tcam -> flow_table[(tcam -> flow_tail - 1) * FLW_ENT_LEN + i];
        write_new_flow_entry(tcam, dummy_flow);
        tcam -> flow_tail = tcam -> flow_tail - 2;
    }
    tcam -> flow_tail = flow_tail_old + 1;
}

void restructure_action_tables(tcam_unit * flow_unit, int pos){
    uint8_t dummy_action[ACTN_ST_LEN];
    uint16_t dummy_flag;
    uint8_t i;
    int tail = flow_unit -> flow_tail - 1;
    while(tail > pos){
        for(i = 0; i < ACTN_ST_LEN; i++)
            dummy_action[i] = flow_unit -> action_table[(tail - 1) * ACTN_ST_LEN + i];
        dummy_flag = flow_unit -> flag_table[tail - 1];
        write_new_action(flow_unit, dummy_action, dummy_flag, tail);
        tail--;
    }
}

int search_within_mask_group(tcam_unit * tcam, uint8_t mask[], uint8_t flow_entry[], int mask_group_start){
    int i = mask_group_start;
    uint8_t j, k, l;
    k = 0;
    //until the next group starts or all the end of the flow table is reached
    do{
        //match each entry (with mask) with the new entry, to see whether a duplicate entry exists
        for(l = 0; l < MTCH_FLD_LEN; l++){
            for(j = 0; j < 8; j++){
                if(read_bit(mask, l * 8 + j)){
                    if(read_bit(flow_entry, i * 8 + j) != read_bit(tcam -> flow_table, (i * FLW_ENT_LEN + l) * 8 + j)){
                        k++;
                        break;
                    }
                }

            }

            if(k == 1)
                break;
        }

        if(k == 0)
        //New entry is duplicate; another matching entry exists (with the same mask)
            return RDNDNT;
        k = 0;
        i++;
    }while(read_bit(tcam -> flow_table, (i * FLW_ENT_LEN + 44) * 8 + 4) == 0 && i < tcam -> flow_tail);
    return i;
}

int locate_mask_group(tcam_unit * tcam, int mask_no){
    if(mask_no == 0)
        return 0;
    int k, i;
    k = 0;
    i = 0;
    while(i < tcam -> flow_tail){
        if(read_bit(tcam -> flow_table, (i * FLW_ENT_LEN + 44) * 8 + 4)){
            k++;
            if(k == mask_no)
                break;
        }
        i++;
    }
    if(i == tcam -> flow_tail)
        //no group for an existing mask
        return STR_ERR;
    //return the index of the starting entry of that mask's group
    return i;
}

int add_new_flow_entry(tcam_unit * tcam, uint8_t mask[], uint8_t flow_entry[]){
    int i, mask_group_start, pos, ch;
    uint8_t j, k;
    //flow table full
    if(tcam -> flow_tail >= NO_F_FLWS)
        return TBL_FULL;
    k = 0;
    //look whether mask already exists
    for(i = 0; i < tcam -> mask_tail; i++){
        for(j = 0; j < MTCH_FLD_LEN; j++){
            if(tcam -> mask_table[(i * MTCH_FLD_LEN) + j] != mask[j]){
                k++;
                break;
            }
        }
        if(k == 0){
            //mask exists
            //find the flow from which mask's group starts
            mask_group_start = locate_mask_group(tcam, i);
            //search whether the flow already exists inside that group
            //if OK, 'pos' is the start of the next mask group
            pos = search_within_mask_group(tcam, mask, flow_entry, mask_group_start);
            //if it does, return
            if(pos == RDNDNT)
                return RDNDNT;
            //else add the flow to the end(?) of that group
            //need to restructure table for this
            restructure_flow_table(tcam, pos);
            //as this entry is for an existing mask group
            reset_bit(flow_entry, 44 * 8 + 4);
            ch = tcam -> flow_tail;
            tcam -> flow_tail = pos;
            write_new_flow_entry(tcam, flow_entry);
            tcam -> flow_tail = ch;
            //returns the position at which the new flow was entered; for action table writing
            return pos;
        }
        k = 0;
    }
    //mask doesn't exist
    //as this entry is for a new mask group
    //if this is first entry of the entire flow table, mask pointing bit => 0
    if(tcam -> flow_tail == 0)
        reset_bit(flow_entry, 44 * 8 + 4);
    else
        set_bit(flow_entry, 44 * 8 + 4);
    write_new_mask(tcam, mask);
    write_new_flow_entry(tcam, flow_entry);
    //returns the position at which the new flow was entered
    return tcam -> flow_tail - 1;
}

int add_new_entry(tcam_unit * flow_unit, uint8_t mask[], uint8_t flow_entry[], uint8_t action_set[], uint16_t flag){
    int pos = add_new_flow_entry(flow_unit, mask, flow_entry);
    //tcam side errors; tables full or flow entry redundant
    if(pos == TBL_FULL || pos == RDNDNT)
        return pos;
    if(pos < flow_unit -> flow_tail - 1)
        restructure_action_tables(flow_unit, pos);
    write_new_action(flow_unit, action_set, flag, pos);
    return pos;
}

int search_tcam(tcam_unit * tcam, uint8_t new_flow[]){
    int mask_no = 0;
    int flow_no = 0;
    int match_no = -1;

    uint8_t i, j, k;
    k = 0;
    while(flow_no < tcam -> flow_tail){
        if(read_bit(tcam -> flow_table, (flow_no * FLW_ENT_LEN + 44) * 8 + 4))
            mask_no++;
        if(mask_no >= tcam -> mask_tail)
            //more mask groups than there are masks
            return STR_ERR;
        for(i = 0; i < MTCH_FLD_LEN; i++){
            for(j = 0; j < 8; j++){
                if(read_bit(tcam -> mask_table, (mask_no * MTCH_FLD_LEN + i) * 8 + j)){
                    if(read_bit(new_flow, i * 8 + j) != read_bit(tcam -> flow_table, (flow_no * FLW_ENT_LEN + i) * 8 + j)){
                        k++;
                        break;
                    }
                }
            }
            if(k == 1)
                break;
        }
        if(k == 0){
            tcam -> flow_table[(flow_no * FLW_ENT_LEN) + 46] = tcam -> flow_table[(flow_no * FLW_ENT_LEN) + 46] + 1;
            if(match_no >= 0){
                if(tcam -> flow_table[(match_no * FLW_ENT_LEN) + 45] < tcam -> flow_table[(flow_no * FLW_ENT_LEN) + 45])
                    match_no = flow_no;
            }
            else
                match_no = flow_no;
        }
        flow_no++;
        k = 0;
    }
    return match_no;
}

int search_flow(tcam_unit * flow_unit, uint8_t new_flow[], uint8_t action_set[]){
    int pos = search_tcam(flow_unit, new_flow);
    //if action position is larger than the last written action entry,
    //table structure is not proper
    if(pos == STR_ERR)
        return STR_ERR;
    uint8_t i;
    for(i = 0; i < ACTN_ST_LEN; i++)
        action_set[i] = flow_unit -> action_table[pos * ACTN_ST_LEN + i];
    return pos;
}

#endif
