#ifndef TCAM_INTERFACE_H_
#define TCAM_INTERFACE_H_

#include <stdio.h>
#include <stdint.h>
#include "flow_unit.h"

int run_tcam_unit(tcam_unit * tcam /*packet buffer, match field queue?*/){
    //if tcam unit is empty, return error
    //else
        //if match field queue or packet buffer is empty, return error
        //else
            //while(packet buffer is not empty && match field queue is not empty)
                //get one from match field queue and packet buffer
                //port = apply_action(tcam unit, new match field set, new packet)
                //if port < 0, some error (no match error or tcam unit error)
                    //send match field set and/or pakcet to policy controller
                //else
                    //send packet to appropriate port queue
            //return packet buffer empty or match field queue empty
            //in the ideal case, both of them are empty ?
}

#endif
