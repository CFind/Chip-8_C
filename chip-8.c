#include <stdlib.h>
#include <stdio.h>
#include "SDL_Interface.h"
#include "chip-8.h"

int quit_emulator = 0;
//Chip-8 has 35 opcodes 2 bytes in length.
//This holds the current opcode.
unsigned short opcode;
/*Memory Map as follows
0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
0x200-0xFFF - Program ROM and work RAM
*/
unsigned char memory[4096];
//CPU registers. 0-15
unsigned char v[16];
//Index register.
unsigned short I;
//Program counter
unsigned short pc;
//Pixel array. 2048 pixels.
unsigned char graphics[64*32];
//Counts down at 60hz when set above 0.
unsigned char delay_timer;
//Buzzer sounds when this reaches 0.
unsigned char sound_timer;
//The stack stores the program counter value before a jump is made 
//or a subroutine is called.
unsigned short stack[16];
//The stack pointer stores the current level of the stack.
unsigned short stack_pointer;
//Keypad states. 16 keys 0x0 - 0xF values.
unsigned char key[16];



int main(int argc, char *argv[]){

    initializeGraphics();
    //Pointer to the rom
   // if (argc <= 1)
   //     error("Need ROM filepath as string");
    printf("%s", "ssdsd");
    initialize();
    //loadROM(argv[1]);


    while (!quit_emulator) {
        getEvent();
        draw();
        printf("%s","QUIT");
    }

    quit();

    return 0;
}


void setQuitFlag(){
    quit_emulator = 1;
}


void initialize(){
    pc = 0x200;
    opcode = 0;
    I = 0;
    stack_pointer = 0;
}



void quit(){
    killGraphics();
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