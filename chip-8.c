#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include "chip-8.h"




int main(int argc, char *argv[]){
    //Pointer to the rom
    if (argc <= 1)
        error("Need ROM filepath as string");
    initialize();
   
    loadROM(argv[1]);

    return 0;
}



void initialize(){
    pc = 0x200;
    opcode = 0;
    I = 0;
    stack_pointer = 0;
}

void loadROM(const char * file){
    FILE * pFile = fopen(file,"rb");
    char * buffer;
    long size;
    long result;

    if (pFile == NULL)
        error("ROM not found!");

    
    fseek(pFile, 0, SEEK_END);
    size = ftell(pFile);
    rewind(pFile);

    buffer = (char*) malloc(sizeof(char)*size);
    if (buffer == NULL)
        error("Load ROM memory allocation error.");
    
    result = fread(buffer,1,size,pFile);
    if (result != size)
        error("File read error!");

    for (int i = 0; i < size; i++)
        memory[i + PROGRAM_LOC] = buffer[i];

    fclose(pFile);
    free(buffer);
        
}

void error(const char * msg){
    printf("\nFATAL ERROR: %s\n", msg);
    printf("%s", "Press ENTER to quit");
    getchar();
    exit(EXIT_FAILURE);
}