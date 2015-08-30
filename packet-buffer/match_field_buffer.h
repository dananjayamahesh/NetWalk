
#include "stdio.h"
#include <stdint.h>
#define MTCH_FLD_LEN 45
#define NO_OF_MATCH_FIELDS_IN_THE_BUFFER 20
#define match_field_buffer_LEN 1000
#define LINE_LEN 16
#define MAX_PACKET_SEIZE 100


// Packet buffer structure
typedef struct {
	uint8_t match_field_buffer_array[MTCH_FLD_LEN*NO_OF_MATCH_FIELDS_IN_THE_BUFFER];
    int match_field_buffer_head;
    int match_field_buffer_tail;
    int buffer_full_flag;
    int buffer_empty_flag;
}match_field_buffer;


//functions
void initialize_match_field_buffer(match_field_buffer * mfb);
void add_a_byte_to_the_match_field_buffer(match_field_buffer * mfb,uint8_t byte);
uint8_t dqueue_from_match_field_buffer(match_field_buffer * mfb);
void read_a_match_field( match_field_buffer * mfb,uint8_t * mf);
void print_mtch_field_buffer(match_field_buffer * mfb);
void print_match_field(uint8_t * mf);



// This will initialize the packet buffer
void initialize_match_field_buffer(match_field_buffer * mfb){
	mfb->match_field_buffer_head = 0;
	mfb->match_field_buffer_tail = 0;
	mfb->buffer_full_flag = 0;
	mfb->buffer_empty_flag = 1;
}

//This function will insert a new byte to the buffer
void add_a_byte_to_the_match_field_buffer(match_field_buffer * mfb,uint8_t byte){

	if (mfb->buffer_full_flag == 1){
		printf("Packet buffer is full. Can't add more bytes.");
	}
	else{
		//Adding new byte to the buffer
		mfb->match_field_buffer_array[mfb->match_field_buffer_tail] = byte;
		mfb->buffer_empty_flag = 0;

		//calculating the new tail
		if (mfb->match_field_buffer_tail == (match_field_buffer_LEN - 1)){
			mfb->match_field_buffer_tail = 0;
		}
		else{
			mfb->match_field_buffer_tail = mfb->match_field_buffer_tail + 1;
		}

		//checking whether the buffer is full
		if (mfb->match_field_buffer_tail == mfb->match_field_buffer_head){
			mfb->buffer_full_flag = 1;
		}
	}
}


//This function will read and dequeue a byte from the buffer
uint8_t dqueue_from_match_field_buffer(match_field_buffer * mfb){

	uint8_t data;
	if (mfb->buffer_empty_flag == 1){
		printf("Packet buffer is empty. Can't read from it");
		return 0;
	}
	else{
		//Reading the buffer from the head
		data = mfb->match_field_buffer_array[mfb->match_field_buffer_head];
		mfb->buffer_empty_flag = 0;

		//calculating the new head
		if (mfb->match_field_buffer_head == match_field_buffer_LEN - 1){
			mfb->match_field_buffer_head = 0;
		}
		else{
			mfb->match_field_buffer_head = mfb->match_field_buffer_head + 1;
		}

		//checking whether the buffer is empty
		if (mfb->match_field_buffer_tail == mfb->match_field_buffer_head){
			mfb->buffer_empty_flag = 1;
		}

		return data;
	}
}


/*
This function will output the firstly added packet to the buffer, a pointer has to be given
to the function
*/
void read_a_match_field( match_field_buffer * mfb,uint8_t * mf){


	int packet_length = 0;
	int i = 0;
	int k=0;

	if (mfb->buffer_empty_flag == 1){
		printf("Packet buffer is empty. Can't read from it");
	}
	else{
		for (i = 0; i < MTCH_FLD_LEN; i++){
			*(mf+i) =  dqueue_from_match_field_buffer(mfb);
		}
		printf("\n");
	}

}

// This will print the current buffer
void print_mtch_field_buffer(match_field_buffer * mfb){
	printf("\n-------------Match Field Buffer-------------------\n");
	int line_count=0;
	if (mfb->buffer_empty_flag == 1){
		printf("Match Field buffer is empty");
	}
	else{
		if (mfb->match_field_buffer_head < mfb->match_field_buffer_tail){
			int j;
			for (j = mfb->match_field_buffer_head; j < mfb->match_field_buffer_tail; j++){
				printf("%.2x ", mfb->match_field_buffer_array[j]);
				line_count++;
				if(line_count==LINE_LEN){
				    line_count=0;
                    printf("\n");
				}
			}
		}
		else{
			int j;
			for (j = mfb->match_field_buffer_head; j < match_field_buffer_LEN; j++){
				printf("%.2x ", mfb->match_field_buffer_array[j]);
				line_count++;
				if(line_count==LINE_LEN){
				    line_count=0;
                    printf("\n");
				}
			}
			for (j = 0; j < mfb->match_field_buffer_tail; j++){
				printf("%.2x ", mfb->match_field_buffer_array[j]);
				line_count++;
				if(line_count==LINE_LEN){
				    line_count=0;
                    printf("\n");
				}
			}
		}
	}

}



//This will print the packet when the pointer to the first byte of that packet is given
void print_match_field(uint8_t * mf){

    printf("\n-------------Match Field Recieved-------------------\n");
    int j =0;
    int line_count=0;
    for(j=0;j< MTCH_FLD_LEN;j++){
        printf("%.2x ", mf[j]);
        line_count++;
        if(line_count==LINE_LEN){
            line_count=0;
            printf("\n");
        }
    }

}

