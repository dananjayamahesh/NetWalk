#ifndef FLOW_UNIT_H_
#define FLOW_UNIT_H_

#include <stdio.h>
#include <stdint.h>
#include "bit_operations.h"
#include "modify_field.h"

#define MTCH_FLD_LEN 45
#define FLW_ENT_LEN  48
#define NO_F_FLWS    20

#define STR_ERR     -10
#define RDNDNT      -2
#define TBL_FULL    -3

typedef struct{
    uint8_t flow_table[FLW_ENT_LEN * NO_F_FLWS];
    uint8_t mask_table[MTCH_FLD_LEN * NO_F_FLWS];
    uint8_t action_table[ACTN_ST_LEN * NO_F_FLWS];
    uint8_t flag_table[2 * NO_F_FLWS];
    int flow_tail;
    int mask_tail;
}tcam_unit;

void write_new_mask(tcam_unit * tcam, uint8_t mask[]);
void write_new_flow_entry(tcam_unit * tcam, uint8_t flow_entry[]);
void write_new_action(tcam_unit * tcam, uint8_t action_set[], uint8_t flag[], int pos);

void delete_restructure_mask(tcam_unit * tcam, int pos);
void restructure_flow_table(tcam_unit * tcam, int pos);
void delete_restructure_flow(tcam_unit * tcam, int pos);
void restructure_action_tables(tcam_unit * flow_unit, int pos);
void delete_restructure_action(tcam_unit * flow_unit, int pos);

int search_within_mask_group(tcam_unit * tcam, uint8_t mask[], uint8_t flow_entry[], int mask_group_start);
int locate_mask_group(tcam_unit * tcam, int mask_no);
int get_mask_pos(tcam_unit * tcam, int pos);
int search_tcam(tcam_unit * tcam, uint8_t new_flow[]);
int search_flow(tcam_unit * flow_unit, uint8_t new_flow[], uint8_t action_set[]);

int add_new_flow_entry(tcam_unit * tcam, uint8_t mask[], uint8_t flow_entry[]);
int add_new_entry(tcam_unit * flow_unit, uint8_t mask[], uint8_t flow_entry[], uint8_t action_set[], uint8_t flag[]);
int modify_entry(tcam_unit * tcam, uint8_t match_fields[], uint8_t flag[], uint8_t new_values[]);
int delete_flow_entry(tcam_unit * tcam, uint8_t flow_entry[]);

int apply_action(tcam_unit * tcam, uint8_t new_flow[]);

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

void write_new_action(tcam_unit * tcam, uint8_t action_set[], uint8_t flag[], int pos){
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
    tcam -> flag_table[2 * pos] = flag[0];
    tcam -> flag_table[2 * pos + 1] = flag[1];
}

void delete_restructure_mask(tcam_unit * tcam, int pos){
    uint8_t dummy_mask[MTCH_FLD_LEN];
    uint8_t i;
    int mask_tail_old = tcam -> mask_tail;
    tcam -> mask_tail = pos;
    while(tcam -> mask_tail < mask_tail_old - 1){
        for(i = 0; i < MTCH_FLD_LEN; i++)
            dummy_mask[i] = tcam -> mask_table[(tcam -> mask_tail + 1) * MTCH_FLD_LEN + i];
        write_new_mask(tcam, dummy_mask);
    }
    tcam -> mask_tail = mask_tail_old - 1;
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

//pos gives the position of the flow entry which is to be deleted
void delete_restructure_flow(tcam_unit * tcam, int pos){
    uint8_t dummy_flow[FLW_ENT_LEN];
    uint8_t i;
    int flow_tail_old = tcam -> flow_tail;
    tcam -> flow_tail = pos;
    while(tcam -> flow_tail < flow_tail_old - 1){
        for(i = 0; i < FLW_ENT_LEN; i++)
            dummy_flow[i] = tcam -> flow_table[(tcam -> flow_tail + 1) * FLW_ENT_LEN + i];
        write_new_flow_entry(tcam, dummy_flow);
    }
    tcam -> flow_tail = flow_tail_old - 1;
}

void restructure_action_tables(tcam_unit * flow_unit, int pos){
    uint8_t dummy_action[ACTN_ST_LEN];
    uint8_t dummy_flag[2];
    uint8_t i;
    int tail = flow_unit -> flow_tail - 1;
    while(tail > pos){
        for(i = 0; i < ACTN_ST_LEN; i++)
            dummy_action[i] = flow_unit -> action_table[(tail - 1) * ACTN_ST_LEN + i];
        dummy_flag[0] = flow_unit -> flag_table[2 * (tail - 1)];
        dummy_flag[1] = flow_unit -> flag_table[2 * (tail - 1) + 1];
        write_new_action(flow_unit, dummy_action, dummy_flag, tail);
        tail--;
    }
}

//Action will always be deleted after the flow entry
void delete_restructure_action(tcam_unit * flow_unit, int pos){
    uint8_t dummy_action[ACTN_ST_LEN];
    uint8_t dummy_flag[2];
    uint8_t i;
    int j = pos;
    while(j < flow_unit -> flow_tail){
        for(i = 0; i < ACTN_ST_LEN; i++)
            dummy_action[i] = flow_unit -> action_table[(j + 1) * ACTN_ST_LEN + i];
        dummy_flag[0] = flow_unit -> flag_table[2 * (j + 1)];
        dummy_flag[1] = flow_unit -> flag_table[2 * (j + 1) + 1];
        write_new_action(flow_unit, dummy_action, dummy_flag, j);
        j++;
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

int add_new_entry(tcam_unit * flow_unit, uint8_t mask[], uint8_t flow_entry[], uint8_t action_set[], uint8_t flag[]){
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
    //pos = -10 indicates an error with the table structure
    //pos = -1 indicates that no matching entry was found
    if(pos < 0)
        return pos;
    uint8_t i;
    for(i = 0; i < ACTN_ST_LEN; i++)
        action_set[i] = flow_unit -> action_table[pos * ACTN_ST_LEN + i];
    return pos;
}

//flag is provided as an array (with two slots) for it to be compatible with functions in the
//bit operations header
int modify_entry(tcam_unit * tcam, uint8_t match_fields[], uint8_t flag[], uint8_t new_values[]){
    //the action set of the matching entry with highest priority is changed
    //may not be the entry that should be changed
    //assumed that this is handled by the SDN controller
    int pos = search_tcam(tcam, match_fields);
    if(pos == STR_ERR)
        return STR_ERR;

    int val_pos = 0;
    //at the worst case, new value will be 6 bytes (MAC Src and Dst)
    uint8_t val[6];

    printf("\nChanges: ");
    //if matching packets are to be dropped, no values will be specified for the action set
    if(!read_bit(flag, 0)){
        //new value for MAC Src
        if(read_bit(flag, 1)){
            printf("\nMAC Src");
            extract_field(new_values, 0, val_pos, val_pos + 47, val);
            replace_field(tcam -> action_table, pos, 0, 47, val);
            val_pos = val_pos + 48;
        }
        //new value for MAC Dst
        if(read_bit(flag, 2)){
            printf("\nMAC Dst");
            extract_field(new_values, 0, val_pos, val_pos + 47, val);
            replace_field(tcam -> action_table, pos, 48, 95, val);
            val_pos = val_pos + 48;
        }
        //new value for VLAN ID
        if(read_bit(flag, 3)){
            printf("\nVLAN ID");
            extract_field(new_values, 0, val_pos, val_pos + 11, val);
            replace_field(tcam -> action_table, pos, 96, 107, val);
            val_pos = val_pos + 12;
        }
        //new value for VLAN Priority
        if(read_bit(flag, 4)){
            printf("\nVLAN Priority");
            extract_field(new_values, 0, val_pos, val_pos + 2, val);
            replace_field(tcam -> action_table, pos, 108, 110, val);
            val_pos = val_pos + 3;
        }
        //new value for MPLS Label
        if(read_bit(flag, 5)){
            printf("\nMPLS Label");
            extract_field(new_values, 0, val_pos, val_pos + 19, val);
            replace_field(tcam -> action_table, pos, 111, 130, val);
            val_pos = val_pos + 20;
        }
        //new value for MPLS Traffic Class
        if(read_bit(flag, 6)){
            printf("\nMPLS Traffic Cls");
            extract_field(new_values, 0, val_pos, val_pos + 2, val);
            replace_field(tcam -> action_table, pos, 131, 133, val);
            val_pos = val_pos + 3;
        }
        //new value for IPv4 Src
        if(read_bit(flag, 7)){
            printf("\nIPv4 Src");
            extract_field(new_values, 0, val_pos, val_pos + 31, val);
            replace_field(tcam -> action_table, pos, 134, 165, val);
            val_pos = val_pos + 32;
        }
        //new value for IPv4 Dst
        if(read_bit(flag, 8)){
            printf("\nIPv4 Dst");
            extract_field(new_values, 0, val_pos, val_pos + 31, val);
            replace_field(tcam -> action_table, pos, 166, 197, val);
            val_pos = val_pos + 32;
        }
        //new value for IPv4 ToS
        if(read_bit(flag, 9)){
            printf("\nIPv4 ToS");
            extract_field(new_values, 0, val_pos, val_pos + 5, val);
            replace_field(tcam -> action_table, pos, 198, 203, val);
            val_pos = val_pos + 6;
        }
        //new value for TCP Src
        if(read_bit(flag, 10)){
            printf("\nTCP Src");
            extract_field(new_values, 0, val_pos, val_pos + 15, val);
            replace_field(tcam -> action_table, pos, 204, 219, val);
            val_pos = val_pos + 16;
        }
        //new value for TCP Dst
        if(read_bit(flag, 11)){
            printf("\nTCP Dst");
            extract_field(new_values, 0, val_pos, val_pos + 15, val);
            replace_field(tcam -> action_table, pos, 220, 235, val);
            val_pos = val_pos + 16;
        }
        //new output port
        if(read_bit(flag, 12)){
            printf("\nOutput Port");
            extract_field(new_values, 0, val_pos, val_pos + 7, val);
            tcam -> action_table[pos * ACTN_ST_LEN + 30] = val[0];
            val_pos = val_pos + 8;
        }
    }

    //priority, counter, timeout fields changing in flow entry
    if(read_bit(flag, 13)){
        printf("\nPriority");
        extract_field(new_values, 0, val_pos, val_pos + 7, val);
        tcam -> flow_table[pos * FLW_ENT_LEN + 45] = val[0];
        val_pos = val_pos + 8;
    }
    if(read_bit(flag, 14)){
        printf("\nCounter");
        extract_field(new_values, 0, val_pos, val_pos + 7, val);
        tcam -> flow_table[pos * FLW_ENT_LEN + 46] = val[0];
        val_pos = val_pos + 8;
        tcam -> flow_table[pos * FLW_ENT_LEN + 46] = tcam -> flow_table[pos * FLW_ENT_LEN + 46] + 1;
    }
    if(read_bit(flag, 15)){
        printf("\nTime Out");
        extract_field(new_values, 0, val_pos, val_pos + 7, val);
        tcam -> flow_table[pos * FLW_ENT_LEN + 47] = val[0];
        val_pos = val_pos + 8;
    }
    //Decrement counter value by 1; as what was searched was not an actual flow
    tcam -> flow_table[pos * FLW_ENT_LEN + 46] = tcam -> flow_table[pos * FLW_ENT_LEN + 46] - 1;
    //Change Flag
    tcam -> flag_table[2 * pos] = (tcam -> flag_table[2 * pos]) | flag[0];
    tcam -> flag_table[2 * pos + 1] = (tcam -> flag_table[2 * pos + 1]) | flag[1];
    //return the position of the modified entry
    return pos;
}

//get the position of the mask that the flow entry belongs to
//tried adding an int field to the structure to save computational time, but it doesn't work
//Using more than 2 int fields in the structure doesn't work
int get_mask_pos(tcam_unit * tcam, int pos){
    int mask_pos = 0;
    int i = 0;
    while(i < tcam -> flow_tail && i <= pos){
        if(read_bit(tcam -> flow_table, (i * FLW_ENT_LEN + 44) * 8 + 4))
            mask_pos++;
        i++;
    }
    return mask_pos;
}

int delete_flow_entry(tcam_unit * tcam, uint8_t flow_entry[]){
    int flow_pos = search_tcam(tcam, flow_entry);
    int mask_pos;
    //Entry to be deleted is the only entry in its mask group
    //then we need to fnd the mask and delete that too
    if(read_bit(tcam -> flow_table, (flow_pos * FLW_ENT_LEN + 44) * 8 + 4) && (flow_pos == tcam -> flow_tail - 1 || read_bit(tcam -> flow_table, ((flow_pos + 1) * FLW_ENT_LEN + 44) * 8 + 4))){
        mask_pos = get_mask_pos(tcam, flow_pos);
        delete_restructure_mask(tcam, mask_pos);
    }
    //Entry to delete is the first (not the only) entry in its mask group
    if(read_bit(tcam -> flow_table, (flow_pos * FLW_ENT_LEN + 44) * 8 + 4) && !read_bit(tcam -> flow_table, ((flow_pos + 1) * FLW_ENT_LEN + 44) * 8 + 4))
        //Have to make the second entry the first entry
        set_bit(tcam -> flow_table, ((flow_pos + 1) * FLW_ENT_LEN + 44) * 8 + 4);
    //delete and restructure flow table and action tables
    delete_restructure_flow(tcam, flow_pos);
    delete_restructure_action(tcam, flow_pos);
    return flow_pos;
}

int apply_action(tcam_unit * tcam, uint8_t new_flow[] /*relevant packet also needed*/){
    uint8_t action_set[ACTN_ST_LEN];
    uint8_t action_flag[2];
    int pos = search_flow(tcam, new_flow, action_set);
    if(pos < 0){
        printf("\nSend to policy controller");
        return pos;
    }
    //get action flag
    action_flag[0] = tcam -> flag_table[2 * pos];
    action_flag[1] = tcam -> flag_table[2 * pos + 1];
    printf("\nActions applied: ");
    if(read_bit(action_flag, 0)){
        printf("\nDrop Packet");
        return 1;
    }
    uint8_t val[6], i;
    if(read_bit(action_flag, 1)){
        printf("\nMAC Src: ");
        extract_field(action_set, 0, 0, 47, val);
        for(i = 0; i < 6; i++)
            printf("%d\t", val[i]);
    }
    if(read_bit(action_flag, 2)){
        printf("\nMAC Dst: ");
        extract_field(action_set, 0, 48, 95, val);
        for(i = 0; i < 6; i++)
            printf("%d\t", val[i]);
    }
    if(read_bit(action_flag, 3)){
        printf("\nVLAN ID: ");
        for(i = 0; i < 6; i++)
            val[i] = 0;
        extract_field(action_set, 0, 96, 107, val);
        for(i = 0; i < 2; i++)
            printf("%d\t", val[i]);
    }
    if(read_bit(action_flag, 4)){
        printf("\nVLAN Priority: ");
        for(i = 0; i < 6; i++)
            val[i] = 0;
        extract_field(action_set, 0, 108, 110, val);
        printf("%d", val[0]);
    }
    if(read_bit(action_flag, 5)){
        printf("\nMPLS Label: ");
        for(i = 0; i < 6; i++)
            val[i] = 0;
        extract_field(action_set, 0, 111, 130, val);
        for(i = 0; i < 3; i++)
            printf("%d\t", val[i]);
    }
    if(read_bit(action_flag, 6)){
        printf("\nMPLS Trff. Cls: ");
        for(i = 0; i < 6; i++)
            val[i] = 0;
        extract_field(action_set, 0, 131, 133, val);
        printf("%d", val[0]);
    }
    if(read_bit(action_flag, 7)){
        printf("\nIPv4 Src: ");
        extract_field(action_set, 0, 134, 165, val);
        for(i = 0; i < 4; i++)
            printf("%d\t", val[i]);
    }
    if(read_bit(action_flag, 8)){
        printf("\nIPv4 Dst: ");
        extract_field(action_set, 0, 166, 197, val);
        for(i = 0; i < 4; i++)
            printf("%d\t", val[i]);
    }
    if(read_bit(action_flag, 9)){
        printf("\nIPv4 ToS: ");
        val[0] = 0;
        extract_field(action_set, 0, 198, 203, val);
        printf("%d\t", val[0]);
    }
    if(read_bit(action_flag, 10)){
        printf("\nTCP Src: ");
        extract_field(action_set, 0, 204, 219, val);
        for(i = 0; i < 2; i++)
            printf("%d\t", val[i]);
    }
    if(read_bit(action_flag, 11)){
        printf("\nTCP Dst: ");
        extract_field(action_set, 0, 220, 235, val);
        for(i = 0; i < 2; i++)
            printf("%d\t", val[i]);
    }
    printf("\nOutput Port: %d", action_set[30]);
    //maybe return output port?
    return 1;
}

#endif
