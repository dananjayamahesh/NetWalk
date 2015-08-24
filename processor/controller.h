#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED
//#include"../processor/processor.h"

typedef struct controller{
    int id;
    uint8_t *ip_addr;
    uint8_t * mac_addr;
    uint8_t *port;
    int is_available;
    int connection_established;
}controller;


void start_controller(controller *c,uint8_t id,uint8_t *mac,uint8_t *ip,uint8_t *port,int is_available,int connection_established){

}



#endif // CONTROLLER_H_INCLUDED
