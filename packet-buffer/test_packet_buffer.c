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

//
// NOTE: remember to include WPCAP and HAVE_REMOTE among your
// preprocessor definitions.
//
#include <pcap.h>
#include "../packet-buffer/packet_buffer.h"
#define LINE_LEN 16
#define PCAP_SRC_IF_STRING 0
#define PCAP_OPENFLAG_PROMISCUOUS 0
#define PACKET_COUNT 2  //Limit the captured packets

typedef struct packet{
    struct pcap_pkthdr *header;
     u_char *pkt_data;
}packet;

int main(){
    pcap_if_t *alldevs, *d;

pcap_t *fp;
u_int inum, i=0;
char errbuf[PCAP_ERRBUF_SIZE];
int res;
struct pcap_pkthdr *header;
const u_char *pkt_data;
initialize_packet_buffer();

 fp = pcap_open_offline("../dump/dump1.pcap", errbuf);//Use any packet dump
    if(fp == NULL)
    {
        printf("Cannot Locate PCAP file\n");
        return 0;
    }else{
        printf("Locate PCAP File\n");
    }
  int packet_count=0;
   while((res = pcap_next_ex( fp, &header, &pkt_data)) >= 0)
    {


        if(res == 0)
            /* Timeout elapsed */
            continue;

                 //Netwalk we can use packet struct/packet queue to save the data.

                 struct packet new_packet;
                 new_packet.header=header;
                 new_packet.pkt_data=pkt_data;
                 /////////Then We can Enqueue the necessary packets to the relevant queue

           packet_count++;
         if(packet_count>PACKET_COUNT){
          break;
         }
        /* print pkt timestamp and pkt len */
        printf("%ld:%ld (%ld)\n", header->ts.tv_sec, header->ts.tv_usec, header->len);
         printf("Element : %ld:%ld (%ld)\n", new_packet.header->ts.tv_sec, new_packet.header->ts.tv_usec, new_packet.header->len);

        // Adding the first byte as the length of the packet to the packet buffer
        add_a_byte_to_the_packet_buffer(header->len);

        /* Print the packet */
        for (i=1; (i < header->caplen + 1 ) ; i++)
        {
            // adding the packet byte by byte to the packet buffer
            add_a_byte_to_the_packet_buffer(pkt_data[i-1]);

            printf("%.2x ", pkt_data[i-1]);
            if ( (i % LINE_LEN) == 0) printf("\n");
        }

        printf("\n\n");
    }

    print_buffer();



   //uint8_t *temp= read_a_packet();

   // uint8_t * a = read_a_packet();
    //uint8_t a[10] ={10,2,3,4,5,6,7,8,9,1};
    //print_packet(a);
   //printf(" sss %.2x",a[2]);

    uint8_t * a = malloc(100);
    read_a_packet(a);

    print_packet(a);





    if(res == -1)
    {
        fprintf(stderr, "Error reading the packets: %s\n", pcap_geterr(fp));
        return -1;
    }

    return 0;


}
