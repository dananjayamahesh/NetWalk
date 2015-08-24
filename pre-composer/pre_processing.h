#include <stdlib.h>
#include <stdio.h>
#include "../packet-buffer/packet_buffer.h"
enum packet_header_lengths{
    ingress_port=32;
    metadata=64;
    mac_src = 48;
    mac_dst = 48;
    eth_typ = 16;
    vlan_id	= 12;
    vlan_priority =3;
    mpls_label =20;
    mpls_traffic_cls =3;
    ipv4_src=32;
    ipv4_dst=32;
    ipv4_protocol=8;
    ipv4_tos=6;
    tcp_src=16;
    tcp_dst=16;
};

static const enum packet_header_lengths pkt_headers[]={ingress_port,metadata,mac_src,mac_dst,eth_typ,vlan_id,vlan_priority,mpls_label,mpls_traffic_cls,ipv4_src,ipv4_dst,ipv4_protocol,ipv4_tos,tcp_src,tcp_dst};

void prepare_match_field(uint8_t * pkt , uint8_t * match_field);
void return_match_field_and_packet (packet_buffer * pb, uint8_t * return_mtch_fld,uint8_t * return_packet);
void set_bit(uint8_t arr[], int pos);
void reset_bit(uint8_t arr[], int pos);
uint8_t read_bit(uint8_t arr[], int pos);
void set_match_field_bits(uint8_t pkt[],uint8_t match_field[], int start_pkt ,int start_mf,int length);
void set_match_field_bits_as_zeros(uint8_t match_field[],int start_mf,int length);
void set_match_field_bits_as_ones(uint8_t match_field[],int start_mf,int length);



void prepare_match_field(uint8_t * pkt , uint8_t * match_field){

    int length = pkt[0];

    int i=0;
    int j=0;
    int mf_length=0;
    int pkt_length=7;
    int no_of_bits_inserting =0;
    int additional_tag_length=0;

    // Adding ingress port and metadata
    no_of_bits_inserting = ingress_port+metadata;
    set_match_field_bits(pkt,match_field,pkt_length,mf_length,no_of_bits_inserting);

    pkt_length = pkt_length + no_of_bits_inserting;
    mf_length = mf_length + no_of_bits_inserting + mac_src;
    no_of_bits_inserting = mac_dst;

    // Adding destination mac
    set_match_field_bits(pkt,match_field,pkt_length,mf_length,no_of_bits_inserting);

    pkt_length = pkt_length + no_of_bits_inserting;
    mf_length = mf_length - mac_src;
    no_of_bits_inserting = mac_src;
    // Adding source mac
    set_match_field_bits(pkt,match_field,pkt_length,mf_length,no_of_bits_inserting);


    pkt_length = pkt_length + no_of_bits_inserting;
    mf_length = mf_length + mac_src + mac_dst;
    no_of_bits_inserting = eth_typ;

    // Adding ethr type
    set_match_field_bits(pkt,match_field,pkt_length,mf_length,no_of_bits_inserting);


    if(pkt[25]==8 && pkt[26]==0){    // Ipv4

        int ipv4_header_length = pkt[29]*16 + pkt[30];

        // Adding vlan,MPLS as zeros
        mf_length = mf_length + no_of_bits_inserting;
        no_of_bits_inserting = vlan_id + vlan_priority + mpls_label + mpls_traffic_cls;
        set_match_field_bits_as_zeros(match_field,mf_length,no_of_bits_inserting);

        additional_tag_length =0;

    }else if(pkt[25]==129 && pkt[26]==0){
             int ipv4_header_length = pkt[33]*16 + pkt[34];

             pkt_length = pkt_length + no_of_bits_inserting + 16;
             mf_length = mf_length + no_of_bits_inserting + vlan_id;
             no_of_bits_inserting = vlan_priority;
             // Setting vlan priority
             set_match_field_bits(pkt,match_field,pkt_length,mf_length,no_of_bits_inserting);

             pkt_length = pkt_length + no_of_bits_inserting + 4;
             mf_length = mf_length - vlan_id;
             no_of_bits_inserting = vlan_id;
             // Setting vlan id
             set_match_field_bits(pkt,match_field,pkt_length,mf_length,no_of_bits_inserting);

             // Adding MPLS as zeros
            mf_length = mf_length + no_of_bits_inserting + vlan_priority;
            no_of_bits_inserting = mpls_label + mpls_traffic_cls;
            set_match_field_bits_as_zeros(match_field,mf_length,no_of_bits_inserting);

            additional_tag_length = 32;
        }

        pkt_length = pkt_length + additional_tag_length;

        pkt_length = pkt_length + no_of_bits_inserting + 8;
        mf_length = mf_length + no_of_bits_inserting + ipv4_src + ipv4_dst + ipv4_protocol;
        no_of_bits_inserting = ipv4_tos;
        // Adding IPv4 Tos
        set_match_field_bits(pkt,match_field,pkt_length,mf_length,no_of_bits_inserting);

        pkt_length = pkt_length + no_of_bits_inserting + 56;
        mf_length = mf_length - ipv4_protocol;
        no_of_bits_inserting = ipv4_protocol;
        // Adding IPv4 Protocol
        set_match_field_bits(pkt,match_field,pkt_length,mf_length,no_of_bits_inserting);


        pkt_length = pkt_length + no_of_bits_inserting + 16;
        mf_length = mf_length - ipv4_src - ipv4_dst ;
        no_of_bits_inserting = ipv4_src;
        // Adding IPv4 source
        set_match_field_bits(pkt,match_field,pkt_length,mf_length,no_of_bits_inserting);


        pkt_length = pkt_length + no_of_bits_inserting;
        mf_length = mf_length + no_of_bits_inserting;
        no_of_bits_inserting = ipv4_dst;
        // Adding IPv4 source
        set_match_field_bits(pkt,match_field,pkt_length,mf_length,no_of_bits_inserting);

        pkt_length = pkt_length + no_of_bits_inserting;
        mf_length = mf_length + no_of_bits_inserting + ipv4_protocol + ipv4_tos ;
        no_of_bits_inserting = tcp_src;
        // Adding TCP source
        set_match_field_bits(pkt,match_field,pkt_length,mf_length,no_of_bits_inserting);

        pkt_length = pkt_length + no_of_bits_inserting;
        mf_length = mf_length + no_of_bits_inserting;
        no_of_bits_inserting = tcp_dst;
        // Adding TCP destination
        set_match_field_bits(pkt,match_field,pkt_length,mf_length,no_of_bits_inserting);

}

void return_match_field_and_packet (packet_buffer * pb, uint8_t * return_mtch_fld,uint8_t * return_packet){

    uint8_t * pkt = malloc(MAX_PACKET_SIZE);
    read_a_packet(pb,pkt);

    int length = pkt[0];





}

void set_bit(uint8_t arr[], int pos){
  int byte_pos = pos / 8;
  uint8_t bit_pos = 7 - (pos % 8);

  uint8_t flag = 1;
  flag = flag << bit_pos;

  arr[byte_pos] = arr[byte_pos] | flag;
}

void reset_bit(uint8_t arr[], int pos){
  int byte_pos = pos / 8;
  uint8_t bit_pos = 7 - (pos % 8);

  uint8_t flag = 1;
  flag = flag << bit_pos;
  flag = ~flag;

  arr[byte_pos] = arr[byte_pos] & flag;
}

uint8_t read_bit(uint8_t arr[], int pos){
  int byte_pos = pos / 8;
  uint8_t bit_pos = 7 - (pos % 8);

  uint8_t flag = 1;
  flag = flag << bit_pos;

  if(flag & arr[byte_pos])
    return 1;
  else
    return 0;
}

void set_match_field_bits(uint8_t pkt[],uint8_t match_field[], int start_pkt ,int start_mf,int length){
  int i =0;

  for(i=0;i<length;i++){

    if(read_bit(pkt,start_pkt+i)==1){
        set_bit(match_field,start_mf+i);
    }else{
        reset_bit(match_field,start_mf+i);
    }
  }
}

void set_match_field_bits_as_zeros(uint8_t match_field[],int start_mf,int length){
  int i =0;

  for(i=0;i<length;i++){
        set_bit(match_field,start_mf+i);
  }
}

void set_match_field_bits_as_ones(uint8_t match_field[],int start_mf,int length){
  int i =0;

  for(i=0;i<length;i++){
        reset_bit(match_field,start_mf+i);
  }
}
