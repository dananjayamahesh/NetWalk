/*
Basic Packet Aquiring and filtering using pcap.h and winpcap
-------------Author----------------------------
--------------- Mahesh Dananjaya----------

First Set Up Win PCAP libraries in to your compilation
Settings->Compiler&Debugger->Linker and Directories
For VisualStudio users : http://www.codeproject.com/Articles/30234/Introduction-to-the-WinPcap-Networking-Libraries
For CodeBlock Users: http://www.learncpp.com/cpp-tutorial/a3-using-libraries-with-codeblocks/

*/

#include <stdlib.h>
#include <stdio.h>
// NOTE: remember to include WPCAP and HAVE_REMOTE among your
// preprocessor definitions.
//
#include <pcap.h>
#include "../packet-buffer/packet_buffer.h"
#include "../packet/packet.h"
#define LINE_LEN 16
#define PCAP_SRC_IF_STRING 0
#define PCAP_OPENFLAG_PROMISCUOUS 0
#define PACKET_COUNT 2  //Limit the captured packets

void packet_to_buffer(struct packet* npacket){
   int length=npacket->header->caplen;
      add_a_byte_to_the_packet_buffer(length);
      int i=0;
      for(i=1;i<length+1;i++){
           add_a_byte_to_the_packet_buffer(npacket->pkt_data[i-1]);
      }
}

void packet_to_buffer_with_metadata(struct packet* npacket){

   int length=npacket->header->caplen;
   length=length+12;
   printf("%d\n",length);
   uint8_t ingress_port[4]={1,0,0,0};
   uint8_t metadata[8]={0,0,0,0,0,0,0,0};
   printf("AAAAAAAAAAAAAAAA\n");
int i=0;
      add_a_byte_to_the_packet_buffer(length);
      for(i=0;i<4;i++){
                   add_a_byte_to_the_packet_buffer(ingress_port[i]);
      }

       for(i=0;i<8;i++){
                   add_a_byte_to_the_packet_buffer(metadata[i]);
      }
        printf("Befor %d\n",length);
      for(i=1;i<(length+1-12);i++){
            printf("%d\n",i);

           add_a_byte_to_the_packet_buffer(npacket->pkt_data[i-1]);
      }
      printf("BBBBBBBBBBBB\n");
}

int main()
{
    int length=0;
    int i=0;
    pcap_t * fp= openfile("D:/FYP/OpenNode/VisualStudeo/dump1.pcap");
    struct packet* npacket;

    uint8_t *byte_array;
    byte_array=get_next_byte_array(fp);
    printf("Get Byte Array: %d ---- %d\n",byte_array,*byte_array);
    print_byte_array(byte_array);
    printf("Next");

//    npacket=get_next_packet(fp);
//     length=npacket->header->caplen;
//
//      add_a_byte_to_the_packet_buffer(length);
//      for(i=1;i<length+1;i++){
//           add_a_byte_to_the_packet_buffer(npacket->pkt_data[i-1]);
//      }

       npacket=get_next_packet(fp);
      // printf("Start Packet Printing\n");
     // print_std_packet(npacket);

     packet_to_buffer(npacket);
     // packet_to_buffer_with_metadata(npacket);
    print_buffer();



    uint8_t * a = malloc(100);
    read_a_packet(a);
    print_packet(a);

    //Preprocess
    //

    return 0;
}
