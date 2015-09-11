#include <stdio.h>
#include <stdlib.h>
#include "../processor/processor.h"
#include "../packet/packet.h"
#include "../include/openflow.h"
#include "openflow_process.h"
//#include "../packet-buffer/packet_buffer.h"
//Optional Function till we develop packet_buffer as seperate module

int main()
{
    printf("\n++++ Starting NetWalk Switch ++++\n");

switchx s1;
    uint8_t switch_id=1;
//    uint8_t switch_mac_addr[6]={0x90,0x61,0x0c,0x0f,0x04,0x02};
//    uint8_t switch_ip_addr[4]={0x4a,0x79,0x8d,0x02};//{0xc0,0xa8,0x01,0x02};
//    uint8_t switch_port[2]={0x00,0x02};//{0x19,0xfd};

    uint8_t switch_mac_addr[6]={0x90,0x61,0x0c,0x0f,0x04,0x01};
    uint8_t switch_ip_addr[4]={0x4a,0x79,0x8d,0x01};//{0xc0,0xa8,0x01,0x02};
    uint8_t switch_port[2]={0x00,0x02};//{0x19,0xfd};



    s1.id=switch_id;
    s1.mac_addr=switch_mac_addr;
     s1.ip_addr=switch_ip_addr;
      s1.port=switch_port;

 print_mac_addr(switch_mac_addr);
   print_ip_addr(switch_ip_addr);
    print_tcp_port(switch_port);

    enum openflow_field_flag ff;

    printf("\n+++ Starting Processor +++\n");
     processor p1;
          uint8_t id=1;
      //For Normal Pcaket Flow
      packet_buffer packet_in;
      initialize_packet_buffer(&packet_in);
      packet_buffer packet_out;
       initialize_packet_buffer(&packet_out);

      //For OpenFLow COntrol Data
      packet_buffer of_data_in;
       initialize_packet_buffer(&of_data_in);
      packet_buffer of_data_out;
       initialize_packet_buffer(&of_data_out);

      packet_buffer openflow_buffer;

      memory_ram ram1;
      processor_start(&p1,id,&packet_in,&packet_out,&of_data_in,&of_data_out,&ram1);
      processor_print(&p1);
      p1.swt=&s1;


      //Controller Functions
      printf("++ Establish Connection to Controller ++ \n");
      //oprnflow_controller_estbblish();Send Hello
    uint8_t controller_mac_addr[6]={0x90,0x61,0x0c,0x0f,0x04,0x01};
    uint8_t controller_ip_addr[4]={0x4a,0x79,0x8d,0x01};//{0xc0,0xa8,0x01,0x02};
    uint8_t controller_port[2]={0xf4,0x47};//{0x19,0xfd};
  controller con1;
      con1.id=1;
      con1.mac_addr=controller_mac_addr;
      con1.ip_addr=controller_ip_addr;
      con1.port=controller_port;
      con1.is_available=1;
      con1.connection_established=1;
    print_controller(&con1);
      p1.con=&con1;


  int length=0;
    int i=0;
      uint8_t ingress_port1[4] = {1,2,3,4};
    uint8_t metadata1[8] = {1,2,3,4,5,6,7,8};
//Packet Buffer
   //packet_buffer pb;
   // initialize_packet_buffer(&pb);


    pcap_t * fp= openfile("../dump/dump1.pcap");
    struct packet* npacket;
   // npacket=get_next_packet(fp);

   // print_std_packet(npacket);

     // load_a_packet_to_the_packet_buffer(npacket,&pb,ingress_port1,metadata1);
    load_next_packet_to_the_buffer(fp,p1.packet_in, ingress_port1 , metadata1);

       //print_std_packet(npacket);



        printf("Initialize Packet Buffer\n");


    print_buffer(p1.packet_in);

    uint8_t *data_buffer= p1.packet_in->packet_buffer;

    printf("\n");
     //printf("%.2x ",data_buffer[ff]);
    // printf("%.2x %.2x\n",data_buffer[ff],*switch_mac_addr);
     ff=DST_MAC_ADDR;
     int mac_addr_matched = compare_mac_addr(&data_buffer[ff],switch_mac_addr);
     //printf("Mac Address Mathced: %d\n",mac_addr_matched);

     ff=IP_DST_ADDR;
    int ip_addr_matched=compare_ip_addr(&data_buffer[ff],switch_ip_addr);
   //  printf("Ip Address Matched: %d\n",ip_addr_matched);
    int packet_for_switch=0;
    if(mac_addr_matched && ip_addr_matched){
         packet_for_switch=1;

    }

    if(packet_for_switch==1){

        printf("\n+++ Processing Packet For This Swicth +++\n");
        ff=TCP_SRC_PORT;

        int tcp_port_matched=compare_tcp_port(&data_buffer[ff],&controller_port);
        printf("TCP PORT MATCHED: %d\n",tcp_port_matched);

        if(tcp_port_matched){
               printf("\n+++ Processing OpenFlow Packets +++\n");
               openflow_packet_process(&p1,data_buffer);
            }
    }

    else{
        //call for OpenFlow
         //Table Missed Packet to be buffered and send notification to controller
           table_missed_packet_process(&p1,data_buffer);

    }
     //Then


    return 0;
}


