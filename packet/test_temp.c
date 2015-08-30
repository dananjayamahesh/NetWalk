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
    uint8_t ingress_port1[4] = {1,2,3,4};
    uint8_t metadata1[8] = {1,2,3,4,5,6,7,8};


    pcap_t* fp = openfile("../dump/dump1.pcap");

    //packet_buffer * pb = malloc(sizeof(packet_buffer));

    pre_processor * pre_proc = malloc(sizeof(pre_processor));

    initialize_preprocessor(pre_proc);

    load_next_packet_to_the_packet_buffer(fp,pre_proc,ingress_port1,metadata1);

    uint8_t ingress_port2[4] = {10,20,30,40};
    uint8_t metadata2[8] = {10,20,30,40,50,60,70,80};
    load_next_packet_to_the_packet_buffer(fp,pre_proc,ingress_port2,metadata2);

    uint8_t ingress_port3[4] = {100,200,300,400};
    uint8_t metadata3[8] = {100,200,300,400,500,600,700,800};
    load_next_packet_to_the_packet_buffer(fp,pre_proc,ingress_port3,metadata3);

    print_packet_buffer(pre_proc);

    print_match_field_buffer(pre_proc);

    prepare_match_field(pre_proc);

    print_match_field_buffer(pre_proc);

    uint8_t * pkt = malloc(MAX_PACKET_SIZE);
    uint8_t * mtch_fld = malloc(MTCH_FLD_LEN);

    return_match_field_and_packet(pre_proc,mtch_fld,pkt);

    print_packet(pkt);

    print_match_field(mtch_fld);

    print_packet_buffer(pre_proc);

    print_match_field_buffer(pre_proc);

    prepare_match_field(pre_proc);

    print_packet_buffer(pre_proc);

    print_match_field_buffer(pre_proc);





   /* uint8_t * match_field = malloc(45);

    prepare_match_field(pkt,match_field);

    int i=0;
    int line_count=0;

    printf("\n\n -------------------Match Field---------------------- \n");

    for(i=0;i<45;i++){

        printf("%.2x ", match_field[i]);
        line_count++;
        if(line_count==LINE_LEN){
            line_count=0;
            printf("\n");
        }
    } */

}
