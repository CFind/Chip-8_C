#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "SDL_Interface.h"
#include "chip-8.h"


#define PROGRAM_LOC 512u
//Register VF
#define VF 0xF
#define VIDEO_WIDTH 64u
#define VIDEO_HEIGHT 32u
#define FONTSET_SIZE 80u

uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


int quit_emulator = 0;
//Chip-8 has 35 opcodes 2 bytes in length.
//This holds the current opcode.
uint16_t opcode;
/*Memory Map as follows
0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
0x200-0xFFF - Program ROM and work RAM
*/
uint8_t memory[4096];
//CPU registers. 0-15
uint8_t v[16];
//Index register.
uint16_t I;
//Program counter
uint16_t pc;
//Pixel array. 2048 pixels.
uint32_t graphics[VIDEO_WIDTH*VIDEO_HEIGHT];
//Counts down at 60hz when set above 0.
uint8_t delay_timer;
//Buzzer sounds when this reaches 0.
uint8_t sound_timer;
//The stack stores the program counter value before a jump is made 
//or a subroutine is called.
uint16_t stack[16];
//The stack pointer stores the current level of the stack.
uint16_t stack_pointer;
//Keypad states. 16 keys 0x0 - 0xF values.
uint8_t keys[16];



int main(int argc, char *argv[]){

    initializeGraphics();
    //Pointer to the rom
    if (argc <= 1)
        error("Need ROM filepath as string");
    initialize();
    loadROM(argv[1]);


    while (!quit_emulator) {
        getEvent(&quit_emulator);
        draw(graphics);
        updateInput(keys);
    }

    quit();

    return 0;
}


void emulateCycle(){
    //fetch opcode
    opcode = memory[pc] << 8u | memory[pc+1];
}


void setQuitFlag(){
    quit_emulator = 1;
}

void initialize(){
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
	{
		memory[80 + i] = fontset[i];
	}
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
    uint8_t * buffer;
    long size;
    long result;

    if (pFile == NULL)
        error("ROM not found!");

    
    fseek(pFile, 0, SEEK_END);
    size = ftell(pFile);
    rewind(pFile);

    buffer = (uint8_t*) malloc(sizeof(uint8_t)*size);
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

////Opcode functions
//Clear the display.
void op_00E0_CLS(){
    memset(graphics, 0, sizeof(graphics));
}
//Return from a subroutine.
void op_00EE_RET(){
    stack_pointer--;
    pc = stack[stack_pointer];
}

//The interpreter sets the program counter to nnn
void op_1nnn_JP(){
    uint16_t address = opcode & 0x0FFFu;
    pc = address;
}

//The interpreter increments the stack pointer, 
//then puts the current PC on the top of the stack. The PC is then set to nnn.
void op_2nnn_CALL(){
    uint16_t address = opcode & 0x0FFFu;
    stack[stack_pointer] = pc;
    stack_pointer++;
    pc = address;
}
//The interpreter compares register Vx to kk, 
//and if they are equal, increments the program counter by 2.
void op_3xkk_SE(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t byte = opcode & 0x00FF;
    if (v[Vx] == byte)
        pc += 2;
}

//The interpreter compares register Vx to kk, 
//and if they are not equal, increments the program counter by 2.
void op_4xkk_SNE(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t byte = opcode & 0x00FF;
    if (v[Vx] != byte)
        pc += 2;
}

//The interpreter compares register Vx to register Vy, 
//and if they are equal, increments the program counter by 2.
void op_5xy0_SE(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t Vy = (opcode >> 4) & 0x000Fu;
    if (v[Vx] == v[Vy])
        pc += 2;
}
//The interpreter puts the value kk into register Vx.
void op_6xkk_LD(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t byte = opcode & 0x00FF;

    v[Vx] = byte;
}
//Adds the value kk to the value of register Vx, then stores the result in Vx. 
void op_7xkk_ADD(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t byte = opcode & 0x00FF;
    v[Vx] += byte;
}
//Stores the value of register Vy in register Vx.
void op_8xy0_LD(){ 
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t Vy = (opcode >> 4) & 0x000Fu;

    v[Vx] = v[Vy];
}
//Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
void op_8xy1_OR(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t Vy = (opcode >> 4) & 0x000Fu;

    v[Vx] = v[Vx] | v[Vy];
}
//Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
void op_8xy2_AND(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t Vy = (opcode >> 4) & 0x000Fu;

    v[Vx] = v[Vx] & v[Vy];
}
//Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
void op_8xy3_XOR(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t Vy = (opcode >> 4) & 0x000Fu;

    v[Vx] = v[Vx] ^ v[Vy];
} 
//The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) 
//VF is set to 1, otherwise 0.
void op_8xy4_ADD(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t Vy = (opcode >> 4) & 0x000Fu;

    uint16_t val = v[Vx] += v[Vx];
    v[15] = (val > 255);

    v[Vx] = val & 0xFFu;

}
//If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
void op_8xy5_SUB(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t Vy = (opcode >> 4) & 0x000Fu;

    if (Vx > Vy)
        v[0xF] = 1;
    v[Vx] = v[Vx] - v[Vy];
}
//If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
void op_8xy6_SHR(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t Vy = (opcode >> 4) & 0x000Fu;

    v[0xF] = (Vx & 1u);

    v[Vx] /= 2;
}
//If Vy > Vx, then VF is set to 1, otherwise 0.
//Then Vx is subtracted from Vy, and the results stored in Vx.
void op_8xy7_SUBN(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t Vy = (opcode >> 4) & 0x000Fu;

    v[VF] = (v[Vy] > v[Vx]);

    v[Vx] = Vy - Vx;
}

void op_8xyE_SHL(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;

    v[VF] = (Vx & 1u);

    v[Vx] *= 2;

}

void op_9xy0_SNE(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t Vy = (opcode >> 4) & 0x000Fu;

    if(v[Vx] != v[Vy])
        pc += 2;
}

void op_Annn_LD(){
    uint16_t address = opcode & 0x0FFF;

    I = address;
}

void op_Bnnn_JP(){
    uint16_t address = opcode & 0x0FFF;

    pc += address + v[0];
}

void op_Cxkk_RND(){
    uint8_t Vx = (opcode >> 12) & 0x000Fu;
    uint8_t byte = (opcode & 0x00FF);

    srand(clock());
    uint8_t rNum = rand() * 255;

    v[Vx] = rNum & byte;
}

void op_Dxyn_DRW(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t Vy = (opcode >> 4) & 0x000Fu;
    uint8_t height = opcode & 0x000Fu;

    uint8_t xPos = v[Vx] % VIDEO_WIDTH;
    uint8_t yPos = v[Vy] % VIDEO_HEIGHT;

    v[VF] = 0;

    for (int row = 0; row < height; row++){
        uint8_t sprite = memory[I + row];
        for (int col = 0; col < 8; col++){
            uint8_t spritePix = sprite & (0x80u >> col);
            uint32_t* screenPix = &graphics[(yPos + row) * VIDEO_WIDTH + (xPos + col)];
            if (spritePix){
                if (*screenPix == 0xFFFFFFFFF){
                    v[VF] = 1;
                }
            }
            *screenPix ^= 0xFFFFFFFFF;
        }
    }

}

void op_Ex9E_SKP(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t key = v[Vx];
    if (keys[key])
        pc += 2; 
}

void op_ExA1_SKNP(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint8_t key = v[Vx];
    if (!keys[key])
        pc += 2; 
}

void op_Fx07_LD(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu; 
    v[Vx] = delay_timer;
}

void op_Fx0A_LD(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    for (int i = 0; i <= 15; i++)
        if (keys[i]){
            v[Vx] = i;
            return;
        }
    pc -= 2;
}

void op_Fx15_LD(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    delay_timer = v[Vx];
}

void op_Fx18_LD(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    sound_timer = v[Vx];
}

void op_Fx1E(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    I += v[Vx];
}

void op_Fx29(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    I = 0x50 + (5* v[Vx]);
}

void op_Fx55(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint16_t address = I;
    for (int i = 0; i <= Vx; i++){
        memory[I+i] = v[i];
    }  
}

void op_Fx33(){
    uint8_t Vx = (opcode >> 12) & 0x000Fu;
    uint8_t val = v[Vx];

    memory[I+2] = val % 10;
    val/=10;
    memory[I+1] = val % 10;
    val/=10;
    memory[I] = val % 10;
}

void op_Fx65(){
    uint8_t Vx = (opcode >> 8) & 0x000Fu;
    uint16_t address = I;
    uint16_t j = 0;
    for (uint16_t i = I; j <= Vx; i++,j++)
    {
         v[j] = memory[i];
    }
}