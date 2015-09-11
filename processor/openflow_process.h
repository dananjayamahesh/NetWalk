#ifndef OPENFLOW_PROCESS_H_INCLUDED
#define OPENFLOW_PROCESS_H_INCLUDED
#include "../include/openflow.h"
#include "processor.h"
#include "../packet/packet.h"


int  openflow_packet_process(processor *p1,uint8_t *data_buffer);
int openflow_hello_process(processor *p1,uint8_t *data_buffer);
int openflow_packet_out_process(processor *p1,uint8_t *data_buffer);
int table_missed_packet_process(processor *p1,uint8_t *data_buffer);


int openflow_packet_process(processor * p1,uint8_t *data_buffer){
    printf("\n----------------------OpenFlow Packet Recieved-----------------------------\n");

   print_data_buffer(data_buffer);
   printf("\n+++ Extracting OpenFlow Header +++\n");
   //Extract the OpenFlow Header
   //uint8_t openflow_data[8]={0x01,0x00,0x00,0x08,0xbb,0xd5,0x35,0x42};
   uint8_t openflow_data[8]={0x01,0x0e,0x00,0x08,0xbb,0xd5,0x35,0x42};
      data_buffer=&openflow_data;

   int i=0;
   uint8_t version=*(data_buffer+i);
   i++;
   uint8_t type=*(data_buffer+i);
   i++;
   uint16_t length=(*(data_buffer+i)*256)+*(data_buffer+i+1);
   i++;i++;


   printf("Version : %.2x\nType: %.2x\nLength: %.4x\n",version,type,length);
   enum ofp_type t=type;

    switch(type){


    case OFPT_HELLO :
                        printf("\n--------------------------OpenFlow Hello Message------------------------\n");
                        openflow_hello_process(p1,data_buffer);
                            break;
    case OFPT_ERROR : break;
    case OFPT_ECHO_REQUEST:   //printf("OpenFlow Echo Request");
                                break;
    case OFPT_ECHO_REPLY  :   printf("OpenFlow Echo Reply\n"); break;
    case OFPT_EXPERIMENTER  :    break;

    /* Switch configuration messages. */
    case OFPT_FEATURES_REQUEST:   printf("OpenFlow Feature Request\n"); break;
    case OFPT_FEATURES_REPLY   :    break;
    case OFPT_GET_CONFIG_REQUEST:    break;
    case OFPT_GET_CONFIG_REPLY   :    break;
    case OFPT_SET_CONFIG         :  printf("OpenFlow Set Config\n");  break;

    /* Asynchronous messages. */
    case OFPT_PACKET_IN          :    printf("OpenFlow Packet in Message\n");break;
    case OFPT_FLOW_REMOVED       :    break;
    case OFPT_PORT_STATUS       :    break;

    /* Controller command messages. */
    case OFPT_PACKET_OUT         :  printf("OpenFlow Packet Out Packet Out");
                                       // openflow_packet_out_process();
                                        break;
    case OFPT_FLOW_MOD           :  printf("\n-----------------OpenFLow Flow Mod Packet----------------------\n");

                                        openflow_flow_mod_process(p1,data_buffer);
                                        break;
    case OFPT_GROUP_MOD         :    break;
    case OFPT_PORT_MOD           :    break;
    case OFPT_TABLE_MOD         :    break;

    /* Multipart messages. */
    case OFPT_MULTIPART_REQUEST :    break;
    case OFPT_MULTIPART_REPLY   :    break;

    /* Barrier messages. */
    case OFPT_BARRIER_REQUEST    :    break;
    case OFPT_BARRIER_REPLY      :    break;

    /* Controller role change request messages. */
    case OFPT_ROLE_REQUEST       :    break;
    case OFPT_ROLE_REPLY         :    break;

    /* Asynchronous message configuration. */
    case OFPT_GET_ASYNC_REQUEST  :    break;
    case OFPT_GET_ASYNC_REPLY    :    break;
    case OFPT_SET_ASYNC          :    break;
    /* Meters and rate limiters configuration messages. */
    case OFPT_METER_MOD          :    break;

    /* Controller role change event messages. */
    case OFPT_ROLE_STATUS        :    break;
    /* Asynchronous messages. */
    case OFPT_TABLE_STATUS      :    break;

    /* Request forwarding by the switch. */
    case OFPT_REQUESTFORWARD     :    break;
    /* Bundle operations (multiple messages as a single operation). */
    case OFPT_BUNDLE_CONTROL     :    break;
    case OFPT_BUNDLE_ADD_MESSAGE :    break;

   }

}

int openflow_hello_process(processor *p1,uint8_t *data_buffer){
    printf("\n-----------------------Process OpenFlow Hello Message--------------------\n");

    print_processor(p1);

   return 1;
}
int openflow_packet_out_process(processor *p1,uint8_t *data_buffer){
    //Place necessary Control Data Bundle in the openflow data buffer out

    return 1;
}

int table_missed_packet_process(processor *p1,uint8_t *data_buffer){
       printf("\n-----------------------Table Missed Other Packet----------------------\n");
       //Buffer Id,Store to access them back
       print_mac_addr(&data_buffer[DST_MAC_ADDR]);
        print_ip_addr(&data_buffer[IP_DST_ADDR]);
                print_port(&data_buffer[TCP_DST_PORT]);

                //int buff_id=make_buffer_packet(data_buffer);//Tagging Buffer ID Information
                //1 Byte: for Bypass Openflow Data Plane
                //i Byte:length
                //4 Byte: Egress Port//Most Probably Physical PortPort 3;
                //14,20,20,OPENFLOW HEADER{}

                uint8_t packet_in_data[1000];
                generate_openflow_packet_in(p1,data_buffer,packet_in_data);




       return 1;
}

int generate_buffer_packet(uint8_t * data_buffer){
    return 1;
}

int generate_openflow_packet_in(processor * p1,uint8_t *data_buffer, uint8_t *packet_in_data_buffer){
  printf("\n++Generate open Flow Packet++\n");
     switchx s=p1->swt[0];
     print_switch(&s);
     print_controller(p1->con);
   printf("\n++Generating Packet-In Message++\n");
   uint8_t type_ip[2]={0x08,0x00};
   uint8_t ip_version=0x45;
    uint8_t ip_diff_serv=0x00;
    uint8_t ip_total_length[2]={0x00,0x29};
    uint8_t ip_id[2]={0x16,0x81};
    uint8_t ip_flag=0x40;
    uint8_t ip_fragment_offset=0x00;
    uint8_t ip_ttl=0x80;


    uint8_t ip_protocol=0x00;
    uint8_t ip_checksum[2]={0x00,0x00};
    uint8_t tcp_seq[4]={0x00,0x00,0x00,0x01};
    uint8_t tcp_ack[4]={0x00,0x00,0x00,0x00};
    uint8_t tcp_header_len=0x50;
     uint8_t tcp_flag[4]={0x00,0x00,0x00,0x00};
     uint8_t tcp_window_size[2]={0xfc,0xab};
     uint8_t tcp_checksum[2]={0x3c,0x38};
        uint8_t zero=0x00;

        uint8_t ofpt_version=0x04;
        int8_t ofpt_type=0x0a;
        int8_t ofpt_length[2]={0x00,0x3c};
        int8_t ofpt_xid[4]={0x00,0x00,0x00,0x00};

         // int8_t ofpt_xid[4]={0x00,0x00,0x00,0x00};
            int8_t ofpt_buffer_id[4]={0x00,0x00,0x00,0x01};
              int8_t ofpt_total_length[2]={0x00,0x2a};
                int8_t ofpt_in_port[2]={0x00,0x01};
                 int8_t ofpt_reason=0x00;

      uint8_t  i=5;
     uint8_t length=0;
      write_length(&length,&packet_in_data_buffer[i]);
      print_length(&packet_in_data_buffer[i]);
      i++;
      write_mac_addr(p1->con->mac_addr,&packet_in_data_buffer[i]);
      print_mac_addr(&packet_in_data_buffer[i]);
     i =i+6;
      write_mac_addr(p1->swt->mac_addr,&packet_in_data_buffer[i]);
        print_mac_addr(&packet_in_data_buffer[i]);
       i =i+6;
      write_ether_type(type_ip,&packet_in_data_buffer[i]);
         print_ether_type(&packet_in_data_buffer[i]);
      i=i+2;
      write_ip_version(&ip_version,&packet_in_data_buffer[i]);
   print_ip_version(&packet_in_data_buffer[i]);
        i=i+1;

      write_ip_diff_serv(&ip_diff_serv,&packet_in_data_buffer[i]);
      print_ip_diff_serv(&packet_in_data_buffer[i]);
      i=i+1;

       write_ip_total_length(&ip_total_length,&packet_in_data_buffer[i]);
      print_ip_total_length(&packet_in_data_buffer[i]);
      i=i+2;

        write_ip_id(&ip_id,&packet_in_data_buffer[i]);
      print_ip_id(&packet_in_data_buffer[i]);
      i=i+2;

      write_ip_flag(&ip_flag,&packet_in_data_buffer[i]);
      print_ip_flag(&packet_in_data_buffer[i]);
      i=i+1;

      write_ip_fragment_offset(&ip_fragment_offset,&packet_in_data_buffer[i]);
      print_ip_fragment_offset(&packet_in_data_buffer[i]);
      i=i+1;

    write_ip_ttl(&ip_ttl,&packet_in_data_buffer[i]);
      print_ip_ttl(&packet_in_data_buffer[i]);
      i=i+1;



 write_ip_protocol(&ip_protocol,&packet_in_data_buffer[i]);
      print_ip_protocol(&packet_in_data_buffer[i]);
      i=i+1;


        write_ip_checksum(&ip_checksum,&packet_in_data_buffer[i]);
      print_ip_checksum(&packet_in_data_buffer[i]);
      i=i+2;

       write_ip_addr(p1->swt->ip_addr,&packet_in_data_buffer[i]);
      print_ip_addr(&packet_in_data_buffer[i]);
      i=i+4;

       write_ip_addr(p1->con->ip_addr,&packet_in_data_buffer[i]);
      print_ip_addr(&packet_in_data_buffer[i]);
      i=i+4;

      write_tcp_port(p1->swt->port,&packet_in_data_buffer[i]);
      print_tcp_port(&packet_in_data_buffer[i]);
      i=i+2;


      write_tcp_port(p1->con->port,&packet_in_data_buffer[i]);
      print_tcp_port(&packet_in_data_buffer[i]);
      i=i+2;

      write_tcp_seq(&tcp_seq,&packet_in_data_buffer[i]);
      print_tcp_seq(&packet_in_data_buffer[i]);
      i=i+4;

     write_tcp_ack(&tcp_ack,&packet_in_data_buffer[i]);
      print_tcp_ack(&packet_in_data_buffer[i]);
      i=i+4;

      write_tcp_header_len(&tcp_header_len,&packet_in_data_buffer[i]);
      print_tcp_header_len(&packet_in_data_buffer[i]);
      i=i+1;

    write_tcp_flag(&tcp_flag,&packet_in_data_buffer[i]);
      print_tcp_flag(&packet_in_data_buffer[i]);
      i=i+1;

        write_tcp_window_size(&tcp_window_size,&packet_in_data_buffer[i]);
      print_tcp_window_size(&packet_in_data_buffer[i]);
      i=i+2;

        write_tcp_checksum(&tcp_checksum,&packet_in_data_buffer[i]);
      print_tcp_checksum(&packet_in_data_buffer[i]);
      i=i+2;

        write_zero(&zero,&packet_in_data_buffer[i]);
        i++;
         write_zero(&zero,&packet_in_data_buffer[i]);
         i++;

//OpenFlow Header
           write_ofpt_version(&ofpt_version,&packet_in_data_buffer[i]);
      print_ofpt_version(&packet_in_data_buffer[i]);
      i=i+1;

            write_ofpt_type(&ofpt_type,&packet_in_data_buffer[i]);
      print_ofpt_type(&packet_in_data_buffer[i]);
      i=i+1;

            write_ofpt_length(&ofpt_length,&packet_in_data_buffer[i]);
      print_ofpt_length(&packet_in_data_buffer[i]);
      i=i+2;

            write_ofpt_xid(&ofpt_xid,&packet_in_data_buffer[i]);
      print_ofpt_xid(&packet_in_data_buffer[i]);
      i=i+4;
//OpenFlow Packet-In Header
            write_ofpt_buffer_id(&ofpt_buffer_id,&packet_in_data_buffer[i]);
      print_ofpt_buffer_id(&packet_in_data_buffer[i]);
      i=i+4;


                write_ofpt_total_length(&ofpt_total_length,&packet_in_data_buffer[i]);
      print_ofpt_total_length(&packet_in_data_buffer[i]);
      i=i+2;

          write_ofpt_in_port(&ofpt_in_port,&packet_in_data_buffer[i]);
      print_ofpt_in_port(&packet_in_data_buffer[i]);
      i=i+2;

           write_ofpt_reason(&ofpt_reason,&packet_in_data_buffer[i]);
      print_ofpt_reason(&packet_in_data_buffer[i]);
      i=i+1;


length=i-5;
       write_length(&length,&packet_in_data_buffer[5]);
         print_length(&packet_in_data_buffer[5]);


    print_data_buffer(&packet_in_data_buffer[5]);

}



int openflow_flow_mod_process(processor *p,uint8_t *data_buffer){

          printf("\n++Processing OpenFlow Mod Packet++\n");
          //Dummy
    uint8_t openflow_data[80]={0x01,0x0e,0x00,0x50,0xbb,0xd5,0x35,0x42 ,0x00,0x00,0x00,0x00,0x00,0x02,0x0a,0xf9,0xaf,0xca,0xd1,0xa4,0x6a,0x6b,0x85,0x74,0x6d,0xcf,0xff,0xff,0x00,0x00,0x08,0x06,0x00,0x02,0x00,0x00,0x0a,0x00,0x00,0x02,0x0a,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,  0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08, 0x00,0x01,0x00,0x00 };
      data_buffer=&openflow_data;
   int i=0;

//write_length(&length,&packet_in_data_buffer[i]);
      print_ofpt_version(&data_buffer[i]);
      i++;
       print_ofpt_type(&data_buffer[i]);
       i++;
        print_ofpt_length(&data_buffer[i]);
        i=i+2;
        print_ofpt_xid(&data_buffer[i]);
        i=i+4;

       print_ofpt_wildcard(&data_buffer[i]);
        i=i+4;

       print_ofpt_in_port(&data_buffer[i]);
        i=i+2;

       print_mac_addr(&data_buffer[i]);
       i=i+6;

        print_mac_addr(&data_buffer[i]);
       i=i+6;

        print_vlan_vid(&data_buffer[i]);
         i=i+2;

         print_vlan_pcp(&data_buffer[i]);
         i=i+1;

         i=i+1;


          print_ether_type(&data_buffer[i]);
         i=i+2;

            print_ip_diff_serv(&data_buffer[i]);
          i=i+1;

           print_ip_protocol (&data_buffer[i]);
          i=i+1;

          i=i+2;

        print_ip_addr(&data_buffer[i]);
          i=i+4;

            print_ip_addr(&data_buffer[i]);
          i=i+4;

          print_tcp_port(&data_buffer[i]);
          i=i+2;


          print_tcp_port(&data_buffer[i]);
          i=i+2;

          print_ofpt_cookie(&data_buffer[i]);
          i=i+8;


          //When Ever Having a Cookie Mask

  //printf("Command\n");
          print_oftp_command(&data_buffer[i]);
          i=i+2;


          print_oftp_idle_timeout(&data_buffer[i]);
          i=i+2;

          print_oftp_hard_timeout(&data_buffer[i]);
                    i=i+2;

         print_oftp_priority(&data_buffer[i]);
         i=i+2;


         print_oftp_buffer_id(&data_buffer[i]);
         i=i+4;

        print_oftp_out_port(&data_buffer[i]);
        i=i+2;

       printf_oftp_flag(&data_buffer[i]);
       i=i+2;

       //Actions Output
       print_oftp_action_type(&data_buffer[i]);
       i=i+2;

       print_oftp_length(&data_buffer[i]);
       i=i+2;
        print_oftp_action_port(&data_buffer[i]);
        i=i+2;

        print_oftp_max_length(&data_buffer[i]);
        i=i+2;

}
#endif // OPENFLOW_PROCESS_H_INCLUDED
