#include <stdio.h>
#include <stdlib.h>
#include "../processor/processor.h"
#include "../packet/packet.h"


//#include "../packet-buffer/packet_buffer.h"

//Optional Function till we develop packet_buffer as seperate module
typedef struct swicth_stack{
    //layer 2 Info
    uint8_t     id;
    uint8_t     mac_addr[6];
    uint8_t     ip_addr[4];

}switchx;

void print_data_buffer(uint8_t data_buffer[]){
    int length=data_buffer[0];
    int i=0;
    for(i=1;i<length;i++){
        printf("%.2x",data_buffer[i]);
    }

}

int packet_to_data_buffer(struct packet* npacket,uint8_t  data_buffer[]){
   int i=0;
   int length=npacket->header->caplen;
   printf("Packet Length : %d\n",length);
      data_buffer[0]=length+12+1;
      printf("Data_Buffer : %d\n",data_buffer[0]);
      data_buffer[1]=0x00;
      data_buffer[2]=0x00;
      data_buffer[3]=0x00;
      data_buffer[4]=0x01;
      printf("Ingress Port Tagged\n");
      for(i=5;i<13;i++){
       data_buffer[i]=0x00;
      }
      printf("Metadata Tagged \n");
     uint8_t * ptr=npacket->pkt_data;
      for(i=0;i<length;i++){
         // printf("%.2x\n",*(data_buffer+i-1));

           data_buffer[i+13]=*(ptr+i);

      }
      printf("Packet Data Obtained\n");
}

int compare_mac_addr(uint8_t * mac1,uint8_t *mac2){
    int i =0;
    int is_matched=1;
    printf("Start Comparison\n");
     printf("%.2x %.2x\n",*mac1,*mac2);
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
    printf("Start Comparison\n");
     printf("%.2x %.2x\n",*ip1,*ip2);
    for(i=0;i<6;i++){

         if(*(ip1+i) != *(ip2+i)){
           is_matched=0;
           break;
         }
    }
    return is_matched;

}
//int layer_2_decapsultion(uint8_t * layer_2_header){
//
//
//
//}

enum field_flag{

    INGRESS_PORT=1,
    META_DATA=5,
    HEAD=13,
    DST_MAC_ADDR=HEAD,//6 bytes
    SRC_MAC_ADDR=HEAD+6,//6
    ETHER_TYPE=HEAD+12,//2
    IP_VERSION=HEAD+14,//1
    IP_DIFF_SERV=HEAD+15,//1
    IP_LENGTH=HEAD+16,//2
    IP_IDENTIFICATION=HEAD+18,////2
    FRAGMENT_OFFSET=HEAD+20,//1
    TTL=HEAD+21,//1
    PROTOCOL=22,//1
    HEADER_CHECKSUM=23,//2
    SRC_IP_ADDR=25,//4
    DST_IP_ADDR=29,//4
    SRC_PORT=33,//2
    DST_PORT=35,//2
    SEQ_NUM=37,//4
    ACK_NUM=41,//4
    HEADER_LENGTH=45,//1
    TCP_FLAG=46,//2
    TCP_WIN_SIZE=48,//2
    TCP_CHECKSUM=50,//2

};
int main()
{

    switchx switch1;
    switch1.id=1;
    uint8_t swicth_id=1;
    uint8_t switch1_mac_addr[6]={0x90,0x61,0x0c,0x0f,0x04,0x0e6};
    uint8_t switch_ip_addr[4]={0xc0,0xa8,0x01,0x02};
    enum field_flag ff;

    printf("Test Processor\n");
     processor p1;
          uint8_t id=1;
      //For Normal Pcaket Flow
      packet_buffer packet_in;
      packet_buffer packet_out;

      //For OpenFLow COntrol Data
      packet_buffer of_data_in;
      packet_buffer of_data_out;

      memory_ram ram1;

      processor_start(&p1,id,&packet_in,&packet_out,&of_data_in,&of_data_out,&ram1);

      processor_print(&p1);

     int length=0;
    int i=0;
    pcap_t * fp= openfile("D:/FYP/OpenNode/VisualStudeo/dump1.pcap");
    struct packet* npacket;
    npacket=get_next_packet(fp);
    length=npacket->header->caplen;
    uint8_t data_buffer[length+13];
    printf("Call Packet to Data Buffer %d %d\n ",length,sizeof(data_buffer));
    packet_to_data_buffer(npacket,data_buffer);
    print_data_buffer(data_buffer);
    ff=DST_MAC_ADDR;
    printf("\n");
     //printf("%.2x ",data_buffer[ff]);
     printf("%.2x %.2x\n",data_buffer[ff],*switch1_mac_addr);

     int packet_4_host = compare_mac_addr(&data_buffer[ff],switch1_mac_addr);
     printf("%d",packet_4_host);




    return 0;
}


