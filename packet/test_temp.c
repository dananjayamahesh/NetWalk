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

#include "../pre-composer/pre_processing.h"
#include <pcap.h>
#include "packet.h"
#define LINE_LEN 16
#define PCAP_SRC_IF_STRING 0
#define PCAP_OPENFLAG_PROMISCUOUS 0
//#define MAX_PACKET_SIZE 100



int main()
{
    uint8_t ingress_port[4] = {1,2,3,4};
    uint8_t metadata[8] = {1,2,3,4,5,6,7,8};


    pcap_t* fp = openfile("../dump/dump1.pcap");

    packet_buffer * pb = malloc(sizeof(packet_buffer));

    initialize_packet_buffer(pb);

    load_next_packet_to_the_buffer(fp,pb,ingress_port,metadata);

    print_buffer(pb);

    uint8_t * pkt = malloc(MAX_PACKET_SIZE);

    read_a_packet(pb,pkt);

    print_packet(pkt);

    uint8_t * match_field = malloc(45);

    prepare_match_field(pkt,match_field);

    int i=0;
    int line_count=0;

    printf("\n\n---------------Match Field-------------------- \n");

    for(i=0;i<45;i++){

        printf("%.2x ", match_field[i]);
        line_count++;
        if(line_count==LINE_LEN){
            line_count=0;
            printf("\n");
        }
    }

}
