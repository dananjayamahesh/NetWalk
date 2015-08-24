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
void print_data_buffer(uint8_t data_buffer[]);
int packet_to_data_buffer(struct packet* npacket,uint8_t  data_buffer[]);


enum protocol_length{
      ETHERNET2=14,
      IP=20,
      TCP=20

};
enum openflow_field_flag{

    INGRESS_PORT=1,//4
    META_DATA=5,//8
    HEAD=13,
    DST_MAC_ADDR=HEAD,//6 bytes
    SRC_MAC_ADDR=HEAD+6,//6
    ETHER_TYPE=HEAD+12,//2
    IP_VERSION=HEAD+14,//1
    IP_DIFF_SERV=HEAD+15,//1
    IP_LENGTH=HEAD+16,//2
    IP_IDENTIFICATION=HEAD+18,////2
    FRAGMENT_OFFSET=HEAD+20,//2
    TTL=HEAD+22,//1
    PROTOCOL=HEAD+23,//1
    HEADER_CHECKSUM=HEAD+24,//2
    SRC_IP_ADDR=HEAD+26,//4
    DST_IP_ADDR=HEAD+30,//4
    SRC_PORT=HEAD+34,//2
    DST_PORT=HEAD+36,//2
    SEQ_NUM=HEAD+38,//4
    ACK_NUM=HEAD+42,//4
    HEADER_LENGTH=HEAD+46,//1
    TCP_FLAG=HEAD+47,//2
    TCP_WIN_SIZE=HEAD+49,//2
    TCP_CHECKSUM=HEAD+51,//2
    OPENFLOW_INIT=HEAD+53,

};


void print_data_buffer(uint8_t data_buffer[]){
    printf("\n -----------------------------Data Buffer---------------------------------\n\n");
    int length=data_buffer[0];
    int i=0;
    for(i=1;i<length;i++){
        printf("%.2x",data_buffer[i]);
    }
    printf("\n");

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

#endif // PACKET_H_INCLUDED
