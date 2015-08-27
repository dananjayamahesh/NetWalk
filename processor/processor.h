#ifndef PROCESSOR_H_INCLUDED
#define PROCESSOR_H_INCLUDED
//#include "../include/openflow.h"
//#include "../packet-buffer/packet_buffer.h"
#include <stdint.h>
#include "../processor/controller.h"
#include "../packet/packet.h"

typedef struct switch_stack{
    //layer 2 Info
    uint8_t     id;
    uint8_t    * mac_addr;
    uint8_t    * ip_addr;
    uint8_t     * port;


}switchx;

typedef struct memory{
        //RAM CODE
}memory_ram;

//typedef struct storage{
//        //RAM CODE
//}packet_buffer;

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
      controller *con;

      switchx * swt;


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

int print_processor(processor *p){
     if(p == NULL){
        printf("ERROR: Processor Undefined.please create the processor first\n") ;
        return 0;
       }else{
           printf("Processor: %d\n",p->id);
       }
       return 1;
}

void processor_print(processor *p){
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




int compare_mac_addr(uint8_t * mac1,uint8_t *mac2){
    int i =0;
    int is_matched=1;
    printf("Start MAC Comparison\n");
    // printf("%.2x %.2x\n",*mac1,*mac2);
    for(i=0;i<6;i++){

         if(*(mac1+i) != *(mac2+i)){
           is_matched=0;
           break;
         }
    }
    return is_matched;

}

int compare_ip_addr(uint8_t * ip1,uint8_t *ip2){
    int i =0;
    int is_matched=1;
    printf("Start IP Comparison\n");
    // printf("%.2x %.2x\n",*ip1,*ip2);
    for(i=0;i<4;i++){
       //printf("IP : %.2x   %.2x",*(ip1+i),*(ip2+i));
         if(*(ip1+i) != *(ip2+i)){
           is_matched=0;
           break;
         }
    }
    return is_matched;

}

int compare_tcp_port(uint8_t * port1,uint8_t *port2){

    int i =0;
    int is_matched=1;
    printf("Start PORT Comparison\n");
    // printf("%.2x %.2x\n",*port1,*port2);
    for(i=0;i<2;i++){

         if(*(port1+i) != *(port2+i)){
           is_matched=0;
           break;
         }
    }
    return is_matched;

}

void print_ip_addr(uint8_t *ip_addr){
    int i=0;
    for(i=0;i<4;i++){
          printf("%d.",*(ip_addr+i));
    }
    printf("\n");
}

void print_mac_addr(uint8_t *mac_addr){
    int i=0;
    for(i=0;i<6;i++){
          printf("%.2x.",*(mac_addr+i));
    }
    printf("\n");
}

void print_port(uint8_t *port){
    int i=0;
    for(i=0;i<2;i++){
          printf("%.2x",*(port+i));
    }
    printf("\n");
}

void print_openflow_port(uint8_t *openflow_port){
    int i=0;
    for(i=0;i<4;i++){
          printf("%.2x",*(openflow_port+i));
    }
    printf("\n");
}

void  print_ether_type(uint8_t *ether_type){
    int i=0;
    for(i=0;i<2;i++){
          printf("%.2x",*(ether_type+i));
    }
    printf("\n");
}

void  print_length(uint8_t *length){
    int i=0;
    for(i=0;i<1;i++){
          printf("%d",*(length+i));
    }
    printf("\n");
}

void  print_ip_version(uint8_t *version){
    int i=0;
    for(i=0;i<1;i++){
          printf("%.2x",*(version+i));
    }
    printf("\n");
}

void print_ip_diff_serv(uint8_t *ip_diff_serv){
     int i=0;
    for(i=0;i<1;i++){
          printf("%.2x",*(ip_diff_serv+i));
    }
    printf("\n");
}

void print_ip_total_length(uint8_t *ip_total_length){
     int i=0;
    for(i=0;i<2;i++){
          printf("%.2x",*(ip_total_length+i));
    }
    printf("\n");
}

void print_ip_id(uint8_t *ip_id){
     int i=0;
    for(i=0;i<2;i++){
          printf("%.2x",*(ip_id+i));
    }
    printf("\n");
}

void print_ip_flag(uint8_t *ip_flag){
     int i=0;
    for(i=0;i<1;i++){
          printf("%.2x",*(ip_flag+i));
    }
    printf("\n");
}
void print_ip_fragment_offset(uint8_t *ip_fragment_offset){
     int i=0;
    for(i=0;i<1;i++){
          printf("%.2x",*(ip_fragment_offset+i));
    }
    printf("\n");
}

void print_ip_ttl(uint8_t *ip_ttl){
     int i=0;
    for(i=0;i<1;i++){
          printf("%.2x",*(ip_ttl+i));
    }
    printf("\n");
}

void print_ip_checksum(uint8_t *ip_checksum){
     int i=0;
    for(i=0;i<2;i++){
          printf("%.2x",*(ip_checksum+i));
    }
    printf("\n");
}


void print_tcp_port(uint8_t *tcp_port){
     int i=0;
    for(i=0;i<2;i++){
          printf("%.2x",*(tcp_port+i));
    }
    printf("\n");
}

void print_tcp_seq(uint8_t *tcp_seq){
     int i=0;
    for(i=0;i<4;i++){
          printf("%.2x",*(tcp_seq+i));
    }
    printf("\n");
}

void print_tcp_ack(uint8_t *tcp_ack){
     int i=0;
    for(i=0;i<4;i++){
          printf("%.2x",*(tcp_ack+i));
    }
    printf("\n");
}

void print_tcp_header_len(uint8_t *tcp_header_len){
     int i=0;
    for(i=0;i<1;i++){
          printf("%.2x",*(tcp_header_len+i));
    }
    printf("\n");
}
void print_tcp_flag(uint8_t *tcp_flag){
     int i=0;
    for(i=0;i<1;i++){
          printf("%.2x",*(tcp_flag+i));
    }
    printf("\n");
}

void print_tcp_window_size(uint8_t *tcp_window_size){
     int i=0;
    for(i=0;i<2;i++){
          printf("%.2x",*(tcp_window_size+i));
    }
    printf("\n");
}

void print_tcp_checksum(uint8_t *tcp_checksum){
     int i=0;
    for(i=0;i<2;i++){
          printf("%.2x",*(tcp_checksum+i));
    }
    printf("\n");
}

void print_ofpt_version(uint8_t *ofpt_version){
     int i=0;
    for(i=0;i<1;i++){
          printf("%.2x",*(ofpt_version+i));
    }
    printf("\n");
}

void print_ofpt_type(uint8_t *ofpt_type){
     int i=0;
    for(i=0;i<1;i++){
          printf("%.2x",*(ofpt_type+i));
    }
    printf("\n");
}

void print_ofpt_length(uint8_t *ofpt_length){
     int i=0;
    for(i=0;i<2;i++){
          printf("%.2x",*(ofpt_length+i));
    }
    printf("\n");
}

void print_ofpt_xid(uint8_t *ofpt_xid){
     int i=0;
    for(i=0;i<4;i++){
          printf("%.2x",*(ofpt_xid+i));
    }
    printf("\n");
}

void print_ofpt_buffer_id(uint8_t *ofpt_buffer_id){
     int i=0;
    for(i=0;i<4;i++){
          printf("%.2x",*(ofpt_buffer_id+i));
    }
    printf("\n");
}

void print_ofpt_total_length(uint8_t *ofpt_total_length){
     int i=0;
    for(i=0;i<2;i++){
          printf("%.2x",*(ofpt_total_length+i));
    }
    printf("\n");
}

void print_ofpt_in_port(uint8_t *ofpt_in_port){
     int i=0;
    for(i=0;i<2;i++){
          printf("%.2x",*(ofpt_in_port+i));
    }
    printf("\n");
}

void print_ofpt_reason(uint8_t *ofpt_reason){
     int i=0;
    for(i=0;i<1;i++){
          printf("%.2x",*(ofpt_reason+i));
    }
    printf("\n");
}





//Write Data To memory
void print_switch(switchx *s){
     if(s == NULL){
        printf("ERROR: Processor Undefined.please create the processor first\n") ;
        return 0;
       }else{
           printf("SWITCH: %d\n",s->id);
           print_mac_addr(s->mac_addr);
           print_ip_addr(s->ip_addr);
           print_port(s->port);
       }
}

int write_length(uint8_t * length,uint8_t * length_n){
     int i=0;
    for(i=0;i<1;i++){
         *(length_n+i)=*(length+i);
    }
return 1;
}
int write_mac_addr(uint8_t * mac_addr,uint8_t * mac_addr_n){
    int i=0;
    for(i=0;i<6;i++){
         *(mac_addr_n+i)=*(mac_addr+i);
    }

 return 1;
}

int write_ether_type(uint8_t * type,uint8_t *type_n){
     int i=0;
    for(i=0;i<2;i++){
         *(type_n+i)=*(type+i);
    }

 return 1;

}

int write_ip_version(uint8_t * version,uint8_t *version_n){
     int i=0;
    for(i=0;i<1;i++){
         *(version_n+i)=*(version+i);
    }

 return 1;

}

int write_ip_addr(uint8_t * ip_addr,uint8_t * ip_addr_n){
    int i=0;
    for(i=0;i<4;i++){
         *(ip_addr_n+i)=*(ip_addr+i);
    }

 return 1;
}

int write_ip_diff_serv(uint8_t * ip_diff_serv,uint8_t * ip_diff_serv_n){
    int i=0;
    for(i=0;i<1;i++){
         *(ip_diff_serv_n+i)=*(ip_diff_serv+i);
    }

 return 1;
}

int write_ip_total_length(uint8_t * ip_total_length,uint8_t * ip_total_length_n){
    int i=0;
    for(i=0;i<2;i++){
         *(ip_total_length_n+i)=*(ip_total_length+i);
    }

 return 1;
}

int write_ip_id(uint8_t * ip_id,uint8_t * ip_id_n){
    int i=0;
    for(i=0;i<2;i++){
         *(ip_id_n+i)=*(ip_id+i);
    }

 return 1;
}

int write_ip_flag(uint8_t * ip_flag,uint8_t * ip_flag_n){
    int i=0;
    for(i=0;i<1;i++){
         *(ip_flag_n+i)=*(ip_flag+i);
    }

 return 1;
}

int write_ip_fragment_offset(uint8_t * ip_fragment_offset,uint8_t * ip_fragment_offset_n){
    int i=0;
    for(i=0;i<1;i++){
         *(ip_fragment_offset_n+i)=*(ip_fragment_offset+i);
    }

 return 1;
}

int write_ip_ttl(uint8_t * ip_ttl,uint8_t * ip_ttl_n){
    int i=0;
    for(i=0;i<1;i++){
         *(ip_ttl_n+i)=*(ip_ttl+i);
    }

 return 1;
}

int write_ip_checksum(uint8_t * ip_checksum,uint8_t * ip_checksum_n){
    int i=0;
    for(i=0;i<2;i++){
         *(ip_checksum_n+i)=*(ip_checksum+i);
    }

 return 1;
}
int write_tcp_port(uint8_t * tcp_port,uint8_t * tcp_port_n){
    int i=0;
    for(i=0;i<2;i++){
         *(tcp_port_n+i)=*(tcp_port+i);
    }

 return 1;
}



int write_tcp_seq(uint8_t * tcp_seq,uint8_t * tcp_seq_n){
    int i=0;
    for(i=0;i<4;i++){
         *(tcp_seq_n+i)=*(tcp_seq+i);
    }

 return 1;
}

int write_tcp_ack(uint8_t * tcp_ack,uint8_t * tcp_ack_n){
    int i=0;
    for(i=0;i<4;i++){
         *(tcp_ack_n+i)=*(tcp_ack+i);
    }

 return 1;
}

int write_tcp_header_len(uint8_t * tcp_header_len,uint8_t * tcp_header_len_n){
    int i=0;
    for(i=0;i<1;i++){
         *(tcp_header_len_n+i)=*(tcp_header_len+i);
    }

 return 1;
}


int write_tcp_flag(uint8_t * tcp_flag,uint8_t * tcp_flag_n){
    int i=0;
    for(i=0;i<1;i++){
         *(tcp_flag_n+i)=*(tcp_flag+i);
    }

 return 1;
}


int write_tcp_window_size(uint8_t * tcp_window_size,uint8_t * tcp_window_size_n){
    int i=0;
    for(i=0;i<2;i++){
         *(tcp_window_size_n+i)=*(tcp_window_size+i);
    }

 return 1;
}

int write_tcp_checksum(uint8_t * tcp_checksum,uint8_t * tcp_checksum_n){
    int i=0;
    for(i=0;i<1;i++){
         *(tcp_checksum_n+i)=*(tcp_checksum+i);
    }

 return 1;
}

int write_zero(uint8_t * zero,uint8_t * zero_n){
    int i=0;
    for(i=0;i<1;i++){
         *(zero_n+i)=*(zero+i);
    }

 return 1;
}

int write_ofpt_version(uint8_t * ofpt_version,uint8_t * ofpt_version_n){
    int i=0;
    for(i=0;i<1;i++){
         *(ofpt_version_n+i)=*(ofpt_version+i);
    }

 return 1;
}


int write_ofpt_type(uint8_t * ofpt_type,uint8_t * ofpt_type_n){
    int i=0;
    for(i=0;i<1;i++){
         *(ofpt_type_n+i)=*(ofpt_type+i);
    }

 return 1;
}


int write_ofpt_length(uint8_t * ofpt_length,uint8_t * ofpt_length_n){
    int i=0;
    for(i=0;i<2;i++){
         *(ofpt_length_n+i)=*(ofpt_length+i);
    }

 return 1;
}


int write_ofpt_xid(uint8_t * ofpt_xid,uint8_t * ofpt_xid_n){
    int i=0;
    for(i=0;i<4;i++){
         *(ofpt_xid_n+i)=*(ofpt_xid+i);
    }

 return 1;
}
//Packet In Messages

int write_ofpt_buffer_id(uint8_t * ofpt_buffer_id,uint8_t * ofpt_buffer_id_n){
    int i=0;
    for(i=0;i<4;i++){
         *(ofpt_buffer_id_n+i)=*(ofpt_buffer_id+i);
    }

 return 1;
}

int write_ofpt_total_length(uint8_t * ofpt_total_length,uint8_t * ofpt_total_length_n){
    int i=0;
    for(i=0;i<2;i++){
         *(ofpt_total_length_n+i)=*(ofpt_total_length+i);
    }

 return 1;
}

int write_ofpt_in_port(uint8_t * ofpt_in_port,uint8_t * ofpt_in_port_n){
    int i=0;
    for(i=0;i<2;i++){
         *(ofpt_in_port_n+i)=*(ofpt_in_port+i);
    }

 return 1;
}

int write_ofpt_reason(uint8_t * ofpt_reason,uint8_t * ofpt_reason_n){
    int i=0;
    for(i=0;i<1;i++){
         *(ofpt_reason_n+i)=*(ofpt_reason+i);
    }

 return 1;
}
void print_controller(controller* c){

     if(c == NULL){
        printf("ERROR: Controller Undefined.Connect to Controller first\n") ;
        return 0;
       }else{
           printf("\nController: %d\n",c->id);
           print_mac_addr(c->mac_addr);
          print_ip_addr(c->ip_addr);
             print_port(c->port);
       }

}


#endif // PROCESSOR_H_INCLUDED
