#include <stdio.h>
#include <stdint.h>
#include "flow_unit.h"

#define MTCH_FLD_LEN 45
#define FLW_ENT_LEN  48
#define ACTN_ST_LEN  31
#define NO_F_FLWS    20

void write_output(uint8_t dummy_action_set[]);

int main(){

    uint8_t dummy_mask[MTCH_FLD_LEN];
    uint8_t dummy_flow_entry[MTCH_FLD_LEN + 1];
    uint8_t dummy_action[ACTN_ST_LEN];
    uint16_t dummy_flag;
    tcam_unit match_unit;
    match_unit.mask_tail = 0;
    match_unit.flow_tail = 0;

    create_mask(dummy_mask);
    create_flow_entry(dummy_flow_entry);
    create_action(dummy_action);
    dummy_flag = rand() % 65536;
    //mask 1 has 1 flow
    add_new_entry(&match_unit, dummy_mask, dummy_flow_entry, dummy_action, dummy_flag);

    create_flow_entry(dummy_flow_entry);
    create_action(dummy_action);
    dummy_flag = rand() % 65536;
    //mask 1 has 2 flows
    add_new_entry(&match_unit, dummy_mask, dummy_flow_entry, dummy_action, dummy_flag);

    uint8_t dummy_mask_2[MTCH_FLD_LEN];
    uint8_t dummy_flow_entry_2[MTCH_FLD_LEN + 1];

    create_mask(dummy_mask_2);
    create_flow_entry(dummy_flow_entry_2);
    create_action(dummy_action);
    dummy_flag = rand() % 65536;
    //mask 2 has 1 flow
    add_new_entry(&match_unit, dummy_mask_2, dummy_flow_entry_2, dummy_action, dummy_flag);

    create_flow_entry(dummy_flow_entry_2);
    create_action(dummy_action);
    //mask 2 has 2 flows
    add_new_entry(&match_unit, dummy_mask_2, dummy_flow_entry_2, dummy_action, dummy_flag);

    create_flow_entry(dummy_flow_entry_2);
    create_action(dummy_action);
    //mask 1 has 3 flows
    add_new_entry(&match_unit, dummy_mask, dummy_flow_entry_2, dummy_action, dummy_flag);

    create_flow_entry(dummy_flow_entry_2);
    create_action(dummy_action);
    //mask 2 has 3 flows
    add_new_entry(&match_unit, dummy_mask_2, dummy_flow_entry_2, dummy_action, dummy_flag);

    uint8_t dummy_action_set[ACTN_ST_LEN];
    printf("%d", search_flow(&match_unit, dummy_flow_entry, dummy_action_set));
    write_output(dummy_action_set);

    printf("%d", search_flow(&match_unit, dummy_flow_entry_2, dummy_action_set));
    write_output(dummy_action_set);

    return 0;

}

void write_output(uint8_t dummy_action_set[]){
    uint8_t i;
    FILE * fp;
    fp = fopen("search_output.txt", "a");
    for(i = 0; i < ACTN_ST_LEN; i++)
        fprintf(fp, "%d ", dummy_action_set[i]);
    fprintf(fp, "\n");
}
