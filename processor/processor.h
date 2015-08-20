#ifndef PROCESSOR_H_INCLUDED
#define PROCESSOR_H_INCLUDED
//#include "../include/openflow.h"
//#include "packet_buffer.h"
#include <stdint.h>
typedef struct memory{
        //RAM CODE
}memory_ram;

typedef struct dummy_packet_buffer{

}packet_buffer;

typedef struct decapsulation_module{


}decap;

typedef struct encapsulation_module{


}encap;

//typedef uint8_t packet_buffer;
//typedef uint8_t memory_ram;
typedef struct processor{

      uint8_t id;
      //For Normal Pcaket Flow
      packet_buffer * packet_in;
      packet_buffer * packet_out;

      //For OpenFLow COntrol Data
      packet_buffer * of_data_in;
      packet_buffer * of_data_out;

      //RAM
      memory_ram * ram;

}processor;

int processor_start(processor * p,uint8_t *id,packet_buffer * packet_in,packet_buffer *packet_out,packet_buffer * of_data_in,packet_buffer* of_data_out,memory_ram * ram){
       if(p == NULL){
        printf("ERROR: Processor Undefined.please create the processor first\n") ;
        return 0;
       }else{
           p->id=id;
           p->packet_in=packet_in;
           p->packet_out=packet_out;
           p->of_data_in=of_data_in;
           p->of_data_out=of_data_out;
           p->ram=ram;

       }
         return 1;
}

processor_print(processor *p){
      if(p == NULL){
        printf("ERROR: Processor Undefined.please create the processor first\n") ;
        return 0;
       }else{
           printf("%d\n %d\n %d\n %d\n %d\n %d\n",
           p->id,
           p->packet_in[0],
           p->packet_out[0],
           p->of_data_in[0],
           p->of_data_out[0]);
       }
}

#endif // PROCESSOR_H_INCLUDED
