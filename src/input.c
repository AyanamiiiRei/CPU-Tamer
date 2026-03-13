#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 64

char buffer[BUFFER_SIZE];

int get_input(void){
    printf("CPU-Tamer>");
    if (fgets(buffer,BUFFER_SIZE,stdin)!= NULL){//prevent buffer overflow
        buffer[strcspn(buffer,"\n")]=0;
        return 0;
    }
    return 1;
}


