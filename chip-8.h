#define PROGRAM_LOC 512
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



//Initializes memory and pointers
void initialize();

//Loads ROM into memory.
void loadROM();


void error(const char * msg);