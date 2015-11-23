#ifndef FLOW_UNIT_H_
#define FLOW_UNIT_H_

#include <stdio.h>
#include <stdint.h>
#include "bit_operations.h"
#include "modify_field.h"

#define MTCH_FLD_LEN 45
#define FLW_ENT_LEN  48
#define NO_F_FLWS    20
#define CACHE_SIZE   5

#define STR_ERR     -10
#define RDNDNT      -2
#define TBL_FULL    -3

//structure for the TCAM
//flow table carries flow entries and mask table carries masks
//there is no 1-to-1 mapping between the two; that is, a single mask can have several flow entries
//action table and flag table stores the action flag and action set values for each flow entry (1-to-1 mapping between flow entries and action sets, flags)
//flow and mask tails point to the most recent entry(last entry) of respective tables
typedef struct{
    uint8_t flow_table[FLW_ENT_LEN * NO_F_FLWS];
    uint8_t mask_table[MTCH_FLD_LEN * NO_F_FLWS];
    uint8_t action_table[ACTN_ST_LEN * NO_F_FLWS];
    uint8_t flag_table[2 * NO_F_FLWS];
    int flow_tail;
    int mask_tail;
}tcam_unit;

//flow cache stores the indexes of the most recently matched masks and flow entries, as pairs of integers
//searching through the TCAM is made faster by first checking with the entries referenced in the cache
int flow_cache[CACHE_SIZE][2];
//cache top points to the most recent entry to the cache
int cache_top;

//-----------------------------FUNCTION DEFINITIONS-------------------------------------------------------
void init();

void write_new_mask(tcam_unit * tcam, uint8_t mask[]);
void write_new_flow_entry(tcam_unit * tcam, uint8_t flow_entry[]);
void write_new_action(tcam_unit * tcam, uint8_t action_set[], uint8_t flag[], int pos);

void restructure_flow_table(tcam_unit * tcam, int pos);
void restructure_action_tables(tcam_unit * flow_unit, int pos);

int get_mask_pos(tcam_unit * tcam, int pos);
int locate_mask_group(tcam_unit * tcam, int mask_no);
int search_within_mask_group(tcam_unit * tcam, uint8_t mask[], uint8_t flow_entry[], int mask_group_start);

int add_new_flow_entry(tcam_unit * tcam, uint8_t mask[], uint8_t flow_entry[]);

int search_cache(uint8_t flow[], tcam_unit * tcam, uint8_t actions[]);
int search_tcam(tcam_unit * tcam, uint8_t new_flow[], int mask_pos[]);
int search_flow(tcam_unit * flow_unit, uint8_t new_flow[], uint8_t action_set[], int mask_pos[]);

void delete_restructure_mask(tcam_unit * tcam, int pos);
void delete_restructure_flow(tcam_unit * tcam, int pos);
void delete_restructure_action(tcam_unit * flow_unit, int pos);

//functions to be used when interacting with the flow unit
int add_new_entry(tcam_unit * flow_unit, uint8_t mask[], uint8_t flow_entry[], uint8_t action_set[], uint8_t flag[]);
int delete_flow_entry(tcam_unit * tcam, uint8_t flow_entry[]);
int modify_entry(tcam_unit * tcam, uint8_t match_fields[], uint8_t flag[], uint8_t new_values[]);
int apply_action(tcam_unit * tcam, uint8_t new_flow[] /*relevant packet also needed*/);


//-----------------------------FUNCTION IMPLEMENTATIONS---------------------------------------------------

//initializes the cache memory
//should be called before using any other function
void init(){
    int i;
    for(i = 0; i < CACHE_SIZE; i++){
        flow_cache[i][0] = -1;
    }
    cache_top = 1;
}

//writes a new entry to the mask table of the given TCAM
//at the same time updates a log that is kept as a text file
void write_new_mask(tcam_unit * tcam, uint8_t mask[]){
    //bits 357, 358 and 359 are not used for any purpose
    //so they do not need to be checked(matched)
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

//writes a new entry to the flow table of the given TCAM while updating a log file
void write_new_flow_entry(tcam_unit * tcam, uint8_t flow_entry[]){
    FILE * fp;
    fp = fopen("flow_table.txt", "a");
    fprintf(fp, "%d. ", tcam -> flow_tail);
    uint8_t i;
    for(i = 0; i <= MTCH_FLD_LEN; i++){
        tcam -> flow_table[tcam -> flow_tail * FLW_ENT_LEN + i] = flow_entry[i];
        fprintf(fp, "%d ", flow_entry[i]);
    }
    //counter starts with 0
    tcam -> flow_table[tcam -> flow_tail * FLW_ENT_LEN + i] = 0;
    //a timeout value of 255 is used for every flow entry
    tcam -> flow_table[tcam -> flow_tail * FLW_ENT_LEN + i + 1] = 255;
    fprintf(fp, "%d %d", 0, 255);
    fprintf(fp, "\n");
    fclose(fp);
    tcam -> flow_tail = tcam -> flow_tail + 1;
}

//writes a new action set and an action flag to the respective tables of the TCAM
//action sets are also updated to a log
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
    //writes the action flag, which is a 16 bit value(given as 2 bytes)
    tcam -> flag_table[2 * pos] = flag[0];
    tcam -> flag_table[2 * pos + 1] = flag[1];
}

//to be used when we need to insert a new entry to an existing mask which is not the last mask in the mask table
//here we have to move all the flow entries that does not belong to the said mask and comes after thet mask's group in the flow table down by 1
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

//for the same functionality as 'restructure_flow_table' but this time intended for action tables; flag and action set both
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

//get the position of the mask that the flow entry belongs to
//flow entry is given by its position in the flow table
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

//used to locate the first entry in a group of entries belonging to a mask(given by its position in the mask table as 'mask_no')
int locate_mask_group(tcam_unit * tcam, int mask_no){
    if(mask_no == 0)
        return 0;
    int k, i;
    k = 0;
    i = 0;
    while(i < tcam -> flow_tail){
        //checks for first entries of mask groups
        //indicated by having a high bit at that particular position
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

//to be used when writing a new entry to the TCAM
//checks whether a matching entry exists in the group belonging to the same mask
//if such an entry exists, adding the new one will be redundant and will not be added
int search_within_mask_group(tcam_unit * tcam, uint8_t mask[], uint8_t flow_entry[], int mask_group_start){
    int i = mask_group_start;
    uint8_t j, k, l;
    k = 0;
    //until the next group starts or the end of the flow table is reached
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
        //New entry is duplicate; another matching entry exists(with the same mask)
            return RDNDNT;
        k = 0;
        i++;
    }while(read_bit(tcam -> flow_table, (i * FLW_ENT_LEN + 44) * 8 + 4) == 0 && i < tcam -> flow_tail);
    return i;
}

//function to add a new flow entry
//mask of that flow entry need to be given as well
//if that mask already exists, entry will be added to its group
//else the mask will be written to the mask table
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

//function to search the cache which has the indexes of the most recently matched flow entries
int search_cache(uint8_t flow[], tcam_unit * tcam, uint8_t actions[]){
    int i, j, k, l;
    int p;
    int match_no = -1;

    if(cache_top > 1){
        for(l = cache_top; l > 0; l--){
            k = 0;
            for(i = 0; i < MTCH_FLD_LEN; i++){
                //try matching the flow with an entry in the cache
                for(j = 0; j < 8; j++){
                    if(read_bit(tcam -> mask_table, (flow_cache[CACHE_SIZE - l][0] * MTCH_FLD_LEN + i) * 8 + j)){
                        if(read_bit(flow, i * 8 + j) != read_bit(tcam -> flow_table, (flow_cache[CACHE_SIZE - l][1] * FLW_ENT_LEN + i) * 8 + j)){
                            k++;
                            break;
                        }
                    }
                }
                if(k == 1)
                    break;
            }
            if(k == 0){
                tcam -> flow_table[(flow_cache[CACHE_SIZE - l][1] * FLW_ENT_LEN) + 46] = tcam -> flow_table[(flow_cache[CACHE_SIZE - l][1] * FLW_ENT_LEN) + 46] + 1;
                //compare priorities
                if(match_no >= 0){
                    if(tcam -> flow_table[(match_no * FLW_ENT_LEN) + 45] < tcam -> flow_table[(flow_cache[CACHE_SIZE - l][1] * FLW_ENT_LEN) + 45]){
                        match_no = flow_cache[CACHE_SIZE - l][1];
                    }
                }
                else{
                    match_no = flow_cache[CACHE_SIZE - l][1];
                }
            }
        }
    }
    return match_no;
}

//function to search for a specific flow entry(new_flow) in the given TCAM
//to be used when matching a flow
//returns the position of the matched entry in the flow table
//position of the mask is written to 'mask_pos'
int search_tcam(tcam_unit * tcam, uint8_t new_flow[], int mask_pos[]){
    int mask_no = 0;
    int flow_no = 0;
    int match_no = -1;

    uint8_t i, j, k;
    k = 0;
    while(flow_no < tcam -> flow_tail){
        //check for the start of a new mask group
        if(read_bit(tcam -> flow_table, (flow_no * FLW_ENT_LEN + 44) * 8 + 4))
            mask_no++;
        if(mask_no >= tcam -> mask_tail)
            //more mask groups than there are masks
            return STR_ERR;

        //tries to match the given flow with the entry, guided by the mask
        for(i = 0; i < MTCH_FLD_LEN; i++){
            for(j = 0; j < 8; j++){
                //should this bit position be matched?
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
        //if match found
        if(k == 0){
            //increment the number of hits of that flow entry
            tcam -> flow_table[(flow_no * FLW_ENT_LEN) + 46] = tcam -> flow_table[(flow_no * FLW_ENT_LEN) + 46] + 1;
            if(match_no >= 0){
                //compare priorities
                if(tcam -> flow_table[(match_no * FLW_ENT_LEN) + 45] < tcam -> flow_table[(flow_no * FLW_ENT_LEN) + 45]){
                    match_no = flow_no;
                    mask_pos[0] = mask_no;
                }
            }
            //if first match
            else{
                match_no = flow_no;
                mask_pos[0] = mask_no;
            }
        }

        flow_no++;
        k = 0;
    }
    return match_no;
}

//function to match a flow
//action set of the matching flow entry is written to 'action_set'
//returns the position of that matched entry in the flow table
int search_flow(tcam_unit * flow_unit, uint8_t new_flow[], uint8_t action_set[], int mask_pos[]){
    int pos = search_tcam(flow_unit, new_flow, mask_pos);
    //pos = -10 indicates an error with the table structure
    //pos = -1 indicates that no matching entry was found
    if(pos < 0)
        return pos;
    uint8_t i;
    for(i = 0; i < ACTN_ST_LEN; i++)
        action_set[i] = flow_unit -> action_table[pos * ACTN_ST_LEN + i];
    //write to cache the positions of the matched flow entry and mask
    flow_cache[CACHE_SIZE - cache_top][0] = mask_pos[0];
    flow_cache[CACHE_SIZE - cache_top][1] = pos;
    cache_top++;
    if(cache_top > CACHE_SIZE){
        cache_top = 1;
    }
    return pos;
}

//delete a mask from the mask table; position of the mask to be deleted is given by pos
//masks after the deleted mask are moved up by one position
//that is, there are no blank spaces inside the table, other than at the end
void delete_restructure_mask(tcam_unit * tcam, int pos){
    uint8_t dummy_mask[MTCH_FLD_LEN];
    uint8_t i;
    int mask_tail_old = tcam -> mask_tail;
    tcam -> mask_tail = pos;
    //moves the entries after the deleted mask up by one
    while(tcam -> mask_tail < mask_tail_old - 1){
        for(i = 0; i < MTCH_FLD_LEN; i++)
            dummy_mask[i] = tcam -> mask_table[(tcam -> mask_tail + 1) * MTCH_FLD_LEN + i];
        write_new_mask(tcam, dummy_mask);
    }
    tcam -> mask_tail = mask_tail_old - 1;
}

//to be used to delete a flow entry
//all the flow entries coming after are moved up buy one
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

//same as the one above; deletes the action set and flag of the flow entry deleted
//entries coming after are moved up by one
//action will always be deleted after the flow entry
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

//--------------------------------FUNCTIONS FOR INTERACTING WITH THE FLOW UNIT---------------------------------------------

//function to add a new entry
//flow entry, mask, action set, flag all need to be given
int add_new_entry(tcam_unit * flow_unit, uint8_t mask[], uint8_t flow_entry[], uint8_t action_set[], uint8_t flag[]){
    //first add the flow entry to the flow table
    //if mask is a new one that is added to the mask table as well
    int pos = add_new_flow_entry(flow_unit, mask, flow_entry);
    //tcam side errors; tables full or flow entry redundant
    if(pos == TBL_FULL || pos == RDNDNT)
        return pos;
    //add the action set and flag to respective tables
    //restructure those tables if necessary
    if(pos < flow_unit -> flow_tail - 1)
        restructure_action_tables(flow_unit, pos);
    write_new_action(flow_unit, action_set, flag, pos);
    return pos;
}

//delete an existing flow entry
int delete_flow_entry(tcam_unit * tcam, uint8_t flow_entry[]){
    int mask[2];
    //get the position of the flow entry
    int flow_pos = search_tcam(tcam, flow_entry, mask);
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

//modifies an existing entry in the TCAM
//can specify new action values, modify the priority, time out value and count(?) of the entry
//flag is provided as an array (with two slots) for it to be compatible with functions in the
//bit operations header
int modify_entry(tcam_unit * tcam, uint8_t match_fields[], uint8_t flag[], uint8_t new_values[]){
    //the action set of the matching entry with highest priority is changed
    //may not be the entry that should be changed
    //assumed that this is handled by the SDN controller

    int mask[2];
    //get the position of the flow entry to modify
    int pos = search_tcam(tcam, match_fields, mask);
    if(pos == STR_ERR)
        return STR_ERR;

    int val_pos = 0;
    //at the worst case, new value will be 6 bytes (MAC Src and Dst)
    uint8_t val[6];

    //change or add new values to the action set
    //which fields of the action set to be modified is indicated by the flag
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

//function to search for a given flow(new_flow)
//first checks with the cache
//if no hit, checks with the flow table
//if still no hits, policy controller is notified
//applying the actions(if a match is found) is also done inside this function
int apply_action(tcam_unit * tcam, uint8_t new_flow[] /*relevant packet also needed*/){
    uint8_t action_set[ACTN_ST_LEN];
    int mask[2];
    uint8_t action_flag[2];

    int v;
    //first search in cache
    int pos = search_cache(new_flow, tcam, action_set);
    if(pos >= 0){
        printf("\nCache Hit");
        for(v = 0; v < ACTN_ST_LEN; v++)
            action_set[v] = tcam -> action_table[pos * ACTN_ST_LEN + v];
    }
    //if no hits, search in the full flow_table
    else{
        pos = search_flow(tcam, new_flow, action_set, mask);
    }

    //if still no hit, policy controller should be notified
    if(pos < 0){
        printf("\nSend to policy controller");
        return pos;
    }

    //get action flag
    action_flag[0] = tcam -> flag_table[2 * pos];
    action_flag[1] = tcam -> flag_table[2 * pos + 1];
    //apply the actions
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
