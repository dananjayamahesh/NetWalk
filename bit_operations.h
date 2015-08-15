#ifndef BIT_OPERATIONS_H_
#define BIT_OPERATIONS_H_

#include <stdio.h>
#include <stdint.h>

void set_bit(uint8_t arr[], int pos);
void reset_bit(uint8_t arr[], int pos);
uint8_t read_bit(uint8_t arr[], int pos);

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

#endif
