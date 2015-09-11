#include <stdio.h>
#include <stdlib.h>
#include "../ceyloniac/ceyloniac_processor.h"
#include "../include/openflow.h"

int main(){

    printf("Test Ceyloniac Processor");

    FILE * fp=fopen("program.txt","r+");
     char i[256];
  if(fp==NULL){
      printf("Error");
  }


  printf("\n +++ Start Reading Program File +++ \n");
    fscanf(fp,"%s",i);
    printf("%s \n",i);
    char * str_p=i;
    ISA p;



    isa_converter(str_p,&p);


}
