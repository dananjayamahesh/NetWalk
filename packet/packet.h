/*
Basic Packet Aquiring and filtering using pcap.h and winpcap
-------------Author----------------------------
--------------- Mahesh Dananjaya----------

First Set Up Win PCAP libraries in to your compilation
Settings->Compiler&Debugger->Linker and Directories
For VisualStudio users : http://www.codeproject.com/Articles/30234/Introduction-to-the-WinPcap-Networking-Libraries
For CodeBlock Users: http://www.learncpp.com/cpp-tutorial/a3-using-libraries-with-codeblocks/

*/
#ifndef PACKET_H_INCLUDED
#define PACKET_H_INCLUDED

#include <pcap.h>
#include "../packet-buffer/packet_buffer.h"

#define LINE_LEN 16
#define PACKET_COUNT 1

typedef struct packet {
    struct pcap_pkthdr *header;
     u_char *pkt_data;
}packet;

pcap_t* openfile(char * filename);
struct packet* get_packet(const pcap_t * fp);
struct packet* get_next_packet(const pcap_t * fp);//Standard Method to Acceess the PCAP packet;
uint8_t * get_next_byte_array(const pcap_t * fp);//Standard Method to get packet as a byte array;
int print_std_packet(const struct packet * pkt);
void print_byte_array(const uint8_t * byte_array);
void load_next_packet_to_the_buffer(pcap_t * fp,packet_buffer * pb, uint8_t * ingress_port , uint8_t * metadata);

pcap_t* openfile(char * filename){
     char errbuf[PCAP_ERRBUF_SIZE];
     pcap_t * fp = pcap_open_offline(filename, errbuf);//Use any packet dump
    if(fp == NULL)
    {
        printf("Error: Cannot Locate PCAP file\n");
        return 0;
    }else{
        printf("Pcap File Located\n");

    }
    return fp;

}

int get_next_byte_array_2(const pcap_t * fp,uint8_t *byte_array){
int res;
struct pcap_pkthdr *header;
const u_char *pkt_data;
//uint8_t * byte_array=NULL;
 uint8_t length=0;

         int i=0;
   if((res = pcap_next_ex( fp, &header, &pkt_data)) >= 0)
    {
        if(res != 0){

        printf("%ld:%ld (%ld)\n", header->ts.tv_sec, header->ts.tv_usec, header->len);
        length=header->caplen;
      }else{
       return NULL;
      }
    }

   byte_array=malloc((length+1)*sizeof(uint8_t));
   uint8_t* packet_header=byte_array;
            *packet_header=length;

            printf("Testttt %d\n",length);

        for (i=1; (i < header->caplen + 1 ) ; i++)        {
            printf("%.2x ", pkt_data[i-1]);
             *(packet_header+i)=pkt_data[i-1];
            if ( (i % LINE_LEN) == 0) printf("\n");
        }
           printf("\n\n");
           //byte_array=packet_header;
           printf("Test1\n");
         print_byte_array(byte_array);
          printf("Test2\n");

      if(res == -1)
    {
        fprintf(stderr, "Error reading the packets: %s\n", pcap_geterr(fp));
        return -1;
    }
    return 1;
//return packet_header;

}



uint8_t * get_next_byte_array(const pcap_t * fp){
int res;
struct pcap_pkthdr *header;
const u_char *pkt_data;
uint8_t * byte_array=NULL;
 uint8_t length=0;

         int i=0;
   if((res = pcap_next_ex( fp, &header, &pkt_data)) >= 0)
    {
        if(res != 0){

        printf("%ld:%ld (%ld)\n", header->ts.tv_sec, header->ts.tv_usec, header->len);
        length=header->caplen;
      }else{
       return NULL;
      }
    }

     uint8_t packet_header[length+1];
            packet_header[0]=length;
        for (i=1; (i < header->caplen + 1 ) ; i++)        {
            printf("%.2x ", pkt_data[i-1]);
             packet_header[i]=pkt_data[i-1];
            if ( (i % LINE_LEN) == 0) printf("\n");
        }
           printf("\n\n");
           //byte_array=packet_header;
         print_byte_array(packet_header);

      if(res == -1)
    {
        fprintf(stderr, "Error reading the packets: %s\n", pcap_geterr(fp));
        return -1;
    }
    return packet_header;
//return packet_header;

}


struct packet* get_next_packet(const pcap_t * fp){
int res;
struct pcap_pkthdr *header;
const u_char *pkt_data;
struct packet new_packet;
     int i=0;
  if((res = pcap_next_ex( fp, &header, &pkt_data)) >= 0)
    {
        if(res != 0){
                 new_packet.header=header;
                 new_packet.pkt_data=pkt_data;
         printf("%ld:%ld (%ld)\n", header->ts.tv_sec, header->ts.tv_usec, header->len);

        for (i=1; (i < header->caplen + 1 ) ; i++)
        {
            printf("%.2x ", pkt_data[i-1]);
            if ( (i % LINE_LEN) == 0) printf("\n");
        }
        printf("\n\n");
           }else{
           }
    }
      if(res == -1)
    {
        fprintf(stderr, "Error reading the packets: %s\n", pcap_geterr(fp));
        return -1;
    }

    return &new_packet;

}


struct packet* get_packets(const pcap_t * fp){
int res;
struct pcap_pkthdr *header;
const u_char *pkt_data;
  int packet_count=0;
       struct packet new_packet;
     int i=0;
   while((res = pcap_next_ex( fp, &header, &pkt_data)) >= 0)
    {
        if(res == 0)
                 continue;
                 new_packet.header=header;
                 new_packet.pkt_data=pkt_data;
         printf("%ld:%ld (%ld)\n", header->ts.tv_sec, header->ts.tv_usec, header->len);
         printf("Element : %ld:%ld (%ld)\n", new_packet.header->ts.tv_sec, new_packet.header->ts.tv_usec, new_packet.header->len);

        /* Print the packet */
        //printf("CapLen: %d\n",header->caplen);
        for (i=1; (i < header->caplen + 1 ) ; i++)
        {
            printf("%.2x ", pkt_data[i-1]);
            if ( (i % LINE_LEN) == 0) printf("\n");
        }

        printf("\n\n");

            packet_count++;
         if(packet_count>=PACKET_COUNT){
             //printf("Packet Count: %d\n",packet_count);
          break;
         }

    }
     //printf("CapLen: %d\n",new_packet.header->caplen);

      if(res == -1)
    {
        fprintf(stderr, "Error reading the packets: %s\n", pcap_geterr(fp));
        return -1;
    }
    // printf("CapLen: %d\n",new_packet.header->caplen);
    return &new_packet;

}

int print_std_packet(const struct packet* pkt){

    if(pkt==NULL){
     printf("Error");
     return 0;
    }
    struct packet*pkt1=pkt;
 printf("Here %d\n",pkt1->header->caplen );
     printf("%ld:%ld (%ld)\n", pkt1->header->ts.tv_sec, pkt1->header->ts.tv_usec, pkt1->header->len);
     int i=0;
     printf("%d\n",pkt1->header->caplen );
        for (i=1; (i < pkt1->header->caplen + 1 ) ; i++)
        {
            printf("%.2x ", pkt1->pkt_data[i-1]);
            if ( (i % LINE_LEN) == 0) printf("\n");
        }
           printf("\n\n");
           return 1;

}

void print_byte_array(const uint8_t * byte_array){
    printf("-----Byte_array----\n");
    printf("Array: %d %d  %d %d %.2x %.2x\n",byte_array,&byte_array,*byte_array,byte_array[0],*(byte_array+1),byte_array[1]);
    uint8_t *ptr=byte_array;
    printf("ptr %d %d\n",ptr,*ptr);
    int length= *ptr;
     printf("Start Printing The Array of Length %d: \n",length);
     ptr++;
     int i=0;
     for(i=0;i<length;i++){
        printf("%.2x \n",*ptr);
        ptr++;
     }


}


void load_next_packet_to_the_buffer (pcap_t * fp,packet_buffer * pb, uint8_t * ingress_port , uint8_t * metadata){

    int res;
    struct pcap_pkthdr *header;
    const u_char *pkt_data;
    struct packet new_packet;
    int i=0;
    int packet_count =0;
    while((res = pcap_next_ex( fp, &header, &pkt_data)) >= 0)
    {
        if(res != 0){

            new_packet.header=header;
            new_packet.pkt_data=pkt_data;
            printf("%ld:%ld (%ld)\n", header->ts.tv_sec, header->ts.tv_usec, header->len);

            // adding the first byte as the length. (plus 12, because of 4 Byte ingress port and 8 Byte Metadata
            add_a_byte_to_the_packet_buffer(pb,header->len + 12);

            // Adding the given 4 Byte ingress port
            for(i=0;i<4;i++){
                add_a_byte_to_the_packet_buffer(pb,ingress_port[i]);
            }

            // Adding the given 8 Byte Metadata
            for(i=0;i<8;i++){
                add_a_byte_to_the_packet_buffer(pb,metadata[i]);
            }

            // Adding other headers
            for (i=1; (i < header->caplen + 1 ) ; i++)
            {
                add_a_byte_to_the_packet_buffer(pb,pkt_data[i-1]);
                printf("%.2x ", pkt_data[i-1]);
                if ( (i % LINE_LEN) == 0) printf("\n");
            }
                printf("\n\n");
            break;
           }else{
               continue;
           }
    }
      if(res == -1)
    {
        fprintf(stderr, "Error reading the packets: %s\n", pcap_geterr(fp));
    }
}

#endif // PACKET_H_INCLUDED
