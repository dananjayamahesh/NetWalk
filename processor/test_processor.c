#include <stdio.h>
#include <stdlib.h>
#include "../processor/processor.h"
int main()
{
    printf("Test Processor\n");
     processor p1;
          uint8_t id=1;
      //For Normal Pcaket Flow
      packet_buffer packet_in;
      packet_buffer packet_out;

      //For OpenFLow COntrol Data
      packet_buffer of_data_in;
      packet_buffer of_data_out;

      memory_ram ram1;

      processor_start(&p1,id,&packet_in,&packet_out,&of_data_in,&of_data_out,&ram1);

      processor_print(&p1);

    return 0;
}
