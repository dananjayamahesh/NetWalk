#include <stdio.h>
#include <stdint.h>
#include "generate_entry.h"
#include "flow_unit.h"

#define MTCH_FLD_LEN 45
#define FLW_ENT_LEN  48
#define ACTN_ST_LEN  31
#define NO_F_FLWS    20

void write_output(uint8_t dummy_action_set[]);
void output_all_actions(tcam_unit * tcam);
void output_all_flow_entries(tcam_unit * tcam);
void output_flags(tcam_unit * tcam);

int main(){

    uint8_t dummy_mask[MTCH_FLD_LEN];
    uint8_t dummy_flow_entry[MTCH_FLD_LEN + 1];
    uint8_t dummy_action[ACTN_ST_LEN];
    uint8_t dummy_flag[2];
    tcam_unit match_unit;
    match_unit.mask_tail = 0;
    match_unit.flow_tail = 0;

    create_mask(dummy_mask);
    create_flow_entry(dummy_flow_entry);
    create_action(dummy_action);
    dummy_flag[0] = rand() % 256;
    dummy_flag[1] = rand() % 256;
    //mask 1 has 1 flow
    add_new_entry(&match_unit, dummy_mask, dummy_flow_entry, dummy_action, dummy_flag);

    uint8_t dummy_flow_entry_2[MTCH_FLD_LEN + 1];

    create_flow_entry(dummy_flow_entry_2);
    create_action(dummy_action);
    dummy_flag[0] = rand() % 256;
    dummy_flag[1] = rand() % 256;
    //mask 1 has 2 flows
    add_new_entry(&match_unit, dummy_mask, dummy_flow_entry_2, dummy_action, dummy_flag);

    uint8_t dummy_mask_2[MTCH_FLD_LEN];

    create_mask(dummy_mask_2);
    create_flow_entry(dummy_flow_entry_2);
    create_action(dummy_action);
    dummy_flag[0] = 65;
    dummy_flag[1] = rand() % 256;
    //mask 2 has 1 flow
    add_new_entry(&match_unit, dummy_mask_2, dummy_flow_entry_2, dummy_action, dummy_flag);

    uint8_t dummy_flow_entry_3[MTCH_FLD_LEN + 1];
    create_flow_entry(dummy_flow_entry_3);
    create_action(dummy_action);
    //mask 2 has 2 flows
    add_new_entry(&match_unit, dummy_mask_2, dummy_flow_entry_3, dummy_action, dummy_flag);

    create_flow_entry(dummy_flow_entry_2);
    create_action(dummy_action);
    //mask 1 has 3 flows
    add_new_entry(&match_unit, dummy_mask, dummy_flow_entry_2, dummy_action, dummy_flag);

    create_flow_entry(dummy_flow_entry_2);
    create_action(dummy_action);
    //mask 2 has 3 flows
    add_new_entry(&match_unit, dummy_mask_2, dummy_flow_entry_2, dummy_action, dummy_flag);

    output_all_actions(&match_unit);
    output_all_flow_entries(&match_unit);
    output_flags(&match_unit);

    int pos = delete_flow_entry(&match_unit, dummy_flow_entry);
    printf("%d\n", pos);
    output_all_actions(&match_unit);
    output_all_flow_entries(&match_unit);
    output_flags(&match_unit);

    uint8_t dummy_action_set[ACTN_ST_LEN];
    //search Mask 1 entry 1
    //this flow entry has been deleted
    pos = search_flow(&match_unit, dummy_flow_entry, dummy_action_set);
    printf("%d %d\n", pos, get_mask_pos(&match_unit, pos));
    if(pos != -1)
        write_output(dummy_action_set);

    //search mask 2 entry 3
    pos = search_flow(&match_unit, dummy_flow_entry_2, dummy_action_set);
    printf("%d %d\n", pos, get_mask_pos(&match_unit, pos));
    if(pos != -1)
        write_output(dummy_action_set);

    //search mask 2 entry 2
    pos = search_flow(&match_unit, dummy_flow_entry_3, dummy_action_set);
    printf("%d %d\n", pos, get_mask_pos(&match_unit, pos));
    if(pos != -1)
        write_output(dummy_action_set);

    /*uint8_t flag_modification[2];
    flag_modification[0] = 64;
    flag_modification[1] = 9;

    uint8_t new_action_val[8];
    uint8_t i;
    printf("New Action Values:\n");
    for(i = 0; i < 8; i++){
        new_action_val[i] = rand() % 256;
        printf("%d\t", new_action_val[i]);
    }

    modify_entry(&match_unit, dummy_flow_entry_2, flag_modification, new_action_val);
    output_all_actions(&match_unit);
    output_all_flow_entries(&match_unit);
    output_flags(&match_unit);*/

    return 0;

}

void write_output(uint8_t dummy_action_set[]){
    uint8_t i;
    FILE * fp;
    fp = fopen("search_output.txt", "a");
    for(i = 0; i < ACTN_ST_LEN; i++)
        fprintf(fp, "%d ", dummy_action_set[i]);
    fprintf(fp, "\n");
    fclose(fp);
}

void output_all_actions(tcam_unit * tcam){
    int i;
    uint8_t j;
    FILE * fp;
    fp = fopen("final_action_table.txt", "a");
    fprintf(fp, "----------\n");
    for(i = 0; i < tcam -> flow_tail; i++){
        fprintf(fp, "%d. ", i);
        for(j = 0; j < ACTN_ST_LEN; j++)
            fprintf(fp, "%d ", tcam -> action_table[i * ACTN_ST_LEN + j]);
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void output_all_flow_entries(tcam_unit * tcam){
    int i;
    uint8_t j;
    FILE * fp;
    fp = fopen("final_flow_table.txt", "a");
    fprintf(fp, "----------\n");
    for(i = 0; i < tcam -> flow_tail; i++){
        fprintf(fp, "%d. ", i);
        for(j = 0; j < FLW_ENT_LEN; j++)
            fprintf(fp, "%d ", tcam -> flow_table[i * FLW_ENT_LEN + j]);
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void output_flags(tcam_unit * tcam){
    int i;
    FILE * fp;
    fp = fopen("final_flag_table.txt", "a");
    fprintf(fp, "----------\n");
    for(i = 0; i < tcam -> flow_tail; i++){
        fprintf(fp, "%d. ", i);
        fprintf(fp, "%d %d", tcam -> flag_table[i * 2], tcam -> flag_table[i * 2 + 1]);
        fprintf(fp, "\n");
    }
    fclose(fp);
}
