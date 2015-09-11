#include "stdio.h"
#include <stdint.h>
#define MTCH_FLD_LEN 45
#define NO_OF_MATCH_FIELDS_IN_THE_BUFFER 20
#define PACKET_BUFFER_LEN 1000
#define LINE_LEN 16

#define MAX_PACKET_SIZE 100


// Packet buffer structure
typedef struct{
    uint8_t packet_buffer[PACKET_BUFFER_LEN];
    int packet_buffer_head;
    int packet_buffer_tail;
    int buffer_full_flag;
    int buffer_empty_flag;
    // This is the head used when preparing macth field
    int buffer_reading_head;
}packet_buffer;

//functions
void initialize_packet_buffer(packet_buffer * pb);
void add_a_byte_to_the_packet_buffer(packet_buffer * pb,uint8_t byte);
//void add_a_packet_to_the_packet_buffer(packet_buffer * pb,packet *pkt);

uint8_t dqueue_from_packet_buffer(packet_buffer * pb);
uint8_t* read_a_packet_and_pass_a_pointer(packet_buffer * pb);
void print_buffer(packet_buffer * pb);
void read_and_print_packet(packet_buffer * pb);
void print_packet(uint8_t * pkt);
void read_a_packet_for_preprocessing(packet_buffer * pb,uint8_t * pk);

// This will initialize the packet buffer
void initialize_packet_buffer(packet_buffer * pb){
	pb->packet_buffer_head = 0;
	pb->packet_buffer_tail = 0;
	pb->buffer_full_flag = 0;
	pb->buffer_empty_flag = 1;

	pb->buffer_reading_head = 0;
}

//This function will insert a new byte to the buffer
void add_a_byte_to_the_packet_buffer(packet_buffer * pb,uint8_t byte){

	if (pb->buffer_full_flag == 1){
		printf("Packet buffer is full. Can't add more bytes.");
	}
	else{
		//Adding new byte to the buffer
		pb->packet_buffer[pb->packet_buffer_tail] = byte;
		pb->buffer_empty_flag = 0;

		//calculating the new tail
		if (pb->packet_buffer_tail == PACKET_BUFFER_LEN - 1){
			pb->packet_buffer_tail = 0;
		}
		else{
			pb->packet_buffer_tail = pb->packet_buffer_tail + 1;
		}

		//checking whether the buffer is full
		if (pb->packet_buffer_tail == pb->packet_buffer_head){
			pb->buffer_full_flag = 1;
		}
	}
}


//void add_a_packet_to_the_packet_buffer(packet_buffer * pb,packet *pkt){


//}
//This function will read and dequeue a byte from the buffer
uint8_t dqueue_from_packet_buffer(packet_buffer * pb){

	uint8_t data;
	if (pb->buffer_empty_flag == 1){
		printf("Packet buffer is empty. Can't read from it");
		return 0;
	}else if(pb->buffer_reading_head==pb->packet_buffer_head){
        printf("Packet is still not pre-processed. Can't deque");
        return 0;
	}
	else{
		//Reading the buffer from the head
		data = pb->packet_buffer[pb->packet_buffer_head];
		pb->buffer_empty_flag = 0;

		//calculating the new head
		if (pb->packet_buffer_head == PACKET_BUFFER_LEN - 1){
			pb->packet_buffer_head = 0;
		}
		else{
			pb->packet_buffer_head = pb->packet_buffer_head + 1;
		}

		//checking whether the buffer is empty
		if (pb->packet_buffer_tail == pb->packet_buffer_head){
			pb->buffer_empty_flag = 1;
		}

		return data;
	}
}

/*
This function will output the firstly added packet to the buffer, a pointer to the first byte
of that packet will be returned
*/

uint8_t* read_a_packet_and_pass_a_pointer(packet_buffer * pb){


	int packet_length = 0;
	int i = 0;
	int k=0;

	if (pb->buffer_empty_flag == 1){
		printf("Packet buffer is empty. Can't read from it");
		return 0;
	}else if(pb->buffer_reading_head==pb->packet_buffer_head){
        printf("Packet is still not pre-processed. Can't deque");
        return 0;
	}else{
		packet_length = dqueue_from_packet_buffer(pb);
		uint8_t output_packet[packet_length];
        output_packet[0] = packet_length;
		for (i = 1; i <= packet_length; i++){
			output_packet[i] =  dqueue_from_packet_buffer(pb);
		}
		printf("\n");
		return output_packet;
	}
}

/*
This function will output the firstly added packet to the buffer, a pointer has to be given
to the function
*/
void read_a_packet(packet_buffer * pb,uint8_t * pk){


	int packet_length = 0;
	int i = 0;
	int k=0;

	if (pb->buffer_empty_flag == 1){
		printf("Packet buffer is empty. Can't read from it");
		*pk = 0;
	}else if(pb->buffer_reading_head==pb->packet_buffer_head){
        printf("Packet is still not pre-processed. Can't deque");
        return 0;
	}else{
		packet_length = dqueue_from_packet_buffer(pb);
		*pk = packet_length;
		for (i = 1; i <= packet_length; i++){
			*(pk+i) =  dqueue_from_packet_buffer(pb);
		}
		printf("\n");
	}

}

// This will print the current buffer
void print_buffer(packet_buffer * pb){
	printf("\n-------------Packet Buffer-------------------\n");
	int line_count=0;
	if (pb->buffer_empty_flag == 1){
		printf("Packet buffer is empty");
	}
	else{
		if (pb->packet_buffer_head < pb->packet_buffer_tail){
			int j;
			for (j = pb->packet_buffer_head; j < pb->packet_buffer_tail; j++){
				printf("%.2x ", pb->packet_buffer[j]);
				line_count++;
				if(line_count==LINE_LEN){
				    line_count=0;
                    printf("\n");
				}
			}
		}
		else{
			int j;
			for (j = pb->packet_buffer_head; j < PACKET_BUFFER_LEN; j++){
				printf("%.2x ", pb->packet_buffer[j]);
				line_count++;
				if(line_count==LINE_LEN){
				    line_count=0;
                    printf("\n");
				}
			}
			for (j = 0; j < pb->packet_buffer_tail; j++){
				printf("%.2x ", pb->packet_buffer[j]);
				line_count++;
				if(line_count==LINE_LEN){
				    line_count=0;
                    printf("\n");
				}
			}
		}
	}

}


//This will read and print the firstly added packet to the buffer
void read_and_print_packet(packet_buffer * pb){

    printf("\n-------------Packet Recieved-------------------\n");
    uint8_t * a = malloc(MAX_PACKET_SIZE);
    read_a_packet(pb,a);

    uint8_t length = a[0];

    printf("length - %.2x \n",length);
    int j =0;
    int line_count=0;
    for(j=1;j<=length;j++){
        printf("%.2x ", a[j]);
        line_count++;
        if(line_count==LINE_LEN){
            line_count=0;
            printf("\n");
        }
    }
    free(a);

}

//This will print the packet when the pointer to the first byte of that packet is given
void print_packet(uint8_t * pkt){

    uint8_t length = pkt[0];
    printf("\n-------------Packet Recieved-------------------\n");
    printf("length - %.2x \n",length);
    int j =0;
    int line_count=0;
    for(j=1;j<=length;j++){
        printf("%.2x ", pkt[j]);
        line_count++;
        if(line_count==LINE_LEN){
            line_count=0;
            printf("\n");
        }
    }

}

//This function will read and dequeue a byte from the buffer for pre processign
uint8_t dqueue_from_packet_buffer_for_preprocessing(packet_buffer * pb){

	uint8_t data;
	if (pb->buffer_empty_flag == 1){
		printf("Packet buffer is empty. Can't read from it");
		return 0;
	}else{
		//Reading the buffer from the head
		data = pb->packet_buffer[pb->buffer_reading_head];
		pb->buffer_empty_flag = 0;

		//calculating the new head
		if (pb->buffer_reading_head == PACKET_BUFFER_LEN - 1){
			pb->buffer_reading_head = 0;
		}
		else{
			pb->buffer_reading_head = pb->buffer_reading_head + 1;
		}

		return data;
	}
}


void read_a_packet_for_preprocessing(packet_buffer * pb,uint8_t * pk){


	int packet_length = 0;
	int i = 0;
	int k=0;

	if (pb->buffer_empty_flag == 1){
		printf("Packet buffer is empty. Can't read from it");
		*pk = 0;
	}else{
		packet_length = dqueue_from_packet_buffer_for_preprocessing(pb);
		*pk = packet_length;
		for (i = 1; i <= packet_length; i++){
			*(pk+i) =  dqueue_from_packet_buffer_for_preprocessing(pb);
		}
		printf("\n");
	}

}
