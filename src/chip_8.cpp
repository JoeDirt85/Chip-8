#include "chip_8.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>

uint8_t chip_8_fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};
const unsigned int fontset_address = 0x50;

chip_8::chip_8() {

};

chip_8::~chip_8() {

};


//used to initialize the registers and memory
void chip_8::initialize() {

	pc = 0x200;	//program counter starts at 0x200
	opcode = 0;	//reset the current opcode
	I = 0;	//reset the index register
	sp = 0;	//reset the stack pointer

	//clear display
	for (int i = 0; i < 2048; ++i) {
		display[i] = 0;
	}

	//clear the stack
	for (int i = 0; i < 16; ++i) {
		stack[i] = 0;
	}

	//clear the registers V0-Vf
	for (int i = 0; i < 16; ++i) {
		V[i] = 0;
	}

	//clear the memory
	for (int i = 0; i < 4096; ++i) {
		memory[i] = 0;
	}

	//load the fontset
	for (int i = 0; i < 80; ++i) {
		memory[fontset_address + i] = chip_8_fontset[i];
	}

	//reset timers
	delay_timer = 0;
	sound_timer = 0;

	drawFlag = true;

	srand(time(NULL));

}

bool chip_8::loadROM(const char* filename) {
	
	initialize();

	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open()) {

		//get the size of the file
		std::streamsize size = file.tellg();

		if (size > 3584) {
			std::cerr << "ROM to large: " << size << " bytes (3584) \n";
			return false;
		}

		//allocate a buffer
		std::vector<char> buffer(size);

		//move the file pointer back to the beginning of the file
		file.seekg(0, std::ios::beg);

		//read the file into the buffer
		file.read(buffer.data(), size);

		//close the file
		file.close();

		//load ROM contents into the chip-8 memory (start at 0x200)
		for (int i = 0; i < size; ++i) {
			memory[512 + i] = buffer[i];
		}
	}
	else {
		std::cerr << "Could not opoen ROM: " << filename << '\n';
		return false;
	}
	return true;
}

void chip_8::emulateCycle() {

	//fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	//declare variables for VX and VY

	uint8_t X = (opcode & 0x0F00) >> 8;
	uint8_t Y = (opcode & 0x00F0) >> 4;
	int temp;

	//decode opcode & execute opcode
	switch (opcode & 0xF000)		//isolates the first nibble
	{

	case 0x0000:

		switch (opcode & 0x000F)	//isolates the last nibble
		{

		case 0x0000:		//0x00E0: clears the screen

			//loops through and clears the display
			for (int i = 0; i < 2048; ++i) {
				display[i] = 0x0;
			}
			drawFlag = true;
			pc += 2;
			break;

		case 0x000E:		//0x00EE: returns from the subroutine

			--sp;
			pc = stack[sp];
			pc += 2;
			break;

		default:
			std::cerr << "Unknown opcode [0x0000]: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << opcode << '\n';
			break;
		}
		break;

	case 0x1000:		//0x1NNN: jumps to address NNN

		pc = (opcode & 0x0FFF);
		break;

	case 0x2000:		//0x2NNN: call subroutine at NNN

		stack[sp] = pc;
		++sp;
		pc = (opcode & 0x0FFF);
		break;

	case 0x3000:		//0x3XNN: skip next instruction if VX == NN

		//conditional that checks if VX == NN
		if (V[X] == (opcode & 0x00FF)) {

			//skips next instruction by incrementing pc by 4 instead of 2
			pc += 4;
		}
		else {

			//else condition, increments by 2 to move to next instruction
			pc += 2;
		}
		break;

	case 0x4000:		//0x4NN: skip next instruction if VX != NN

		if (V[X] != (opcode & 0x00FF)) {
			pc += 4;
		}
		else {
			pc += 2;
		}
		break;

	case 0x5000:		//0x5XY0: skip the next instruction if VX == VY

		if (V[X] == V[Y]) {
			pc += 4;
		}
		else {
			pc += 2;
		}
		break;

	case 0x6000:		//0x6XNN: sets VX to NN

		V[X] = (opcode & 0x00FF);
		pc += 2;
		break;

	case 0x7000:		//0x7XNN: adds NN to VX

		V[X] += (opcode & 0x00FF);
		pc += 2;
		break;

	case 0x8000:

		switch (opcode & 0x000F) {		//isolates the last nibble

		case 0x0000:		//0x8XY0: sets VX to the value of VY

			V[X] = V[Y];
			pc += 2;
			break;

		case 0x0001:		//0x8XY1: sets VX to (VX OR VY)

			V[X] |= V[Y];
			pc += 2;
			break;

		case 0x0002:		//0x8XY2: sets VX to (VX & VY)

			V[X] &= V[Y];
			pc += 2;
			break;

		case 0x0003:		//0x8XY3: sets VX to (VX XOR VY)

			V[X] ^= V[Y];
			pc += 2;
			break;

		case 0x0004:		//0x8XY4: adds VY to VX

			if (V[Y] > (0xFF - V[X])) {
				temp = 1; //carry
			}
			else {
				temp = 0;
			}

			V[X] += V[Y];
			V[0xF] = temp;
			pc += 2;
			break;

		case 0x0005:		//0x8XY5: Subtracts VY from VX

			if (V[Y] > V[X]) {
				temp = 0;
			}
			else {
				temp = 1;
			}

			V[X] -= V[Y];
			V[0xF] = temp;
			pc += 2;
			break;

		case 0x0006:		//0x8XY6: shifts VX >> 1, stores least significant bit into VF prior to shift

			//stores least significant bit into VF
			temp = V[X] & 0x1;

			//shift VX by one
			V[X] >>= 1;
			V[0xF] = temp;
			pc += 2;
			break;

		case 0x0007:		//0x8XY7: sets VX equal to VY - VX

			if (V[X] > V[Y]) {
				temp = 0;
			}
			else {
				temp = 1;
			}

			V[X] = (V[Y] - V[X]);
			V[0xF] = temp;
			pc += 2;
			break;

		case 0x000E:		//shifts VX << 1, checks if MSB was set or unset prior to shift

			if ((V[X] & 0x80) == 0x80) {
				temp = 1;
			}
			else {
				temp = 0;
			}
			V[X] <<= 1;
			V[0xF] = temp;
			pc += 2;
			break;

		default:
			std::cerr << "Unknown opcode [0x8000]: 0x" << std::hex << std::uppercase << opcode << '\n';
			break;
		}
		break;

	case 0x9000:		//0x9XY0: skips next instruction if VX != VY

		if (V[X] != V[Y]) {
			pc += 4;
		}
		else {
			pc += 2;
		}
		break;

	case 0xA000:		//0xANNN: sets I to the address of NNN

		I = (opcode & 0x0FFF);
		pc += 2;
		break;

	case 0xB000:		//0xBNN: jumps to address NNN + V0  

		pc = (opcode & 0x0FFF) + V[0];
		break;

	case 0xC000:		//0xCXNN: sets VX to random number & NN

		V[X] = rand() & (opcode & 0x00FF);
		pc += 2;
		break;

	case 0xD000: {	//0xDXYN: Draws sprite at (VX, VY) with N height
		
		//register indices, helps with screen wrap and protects coordiantes from VF reset
		uint8_t xPos = V[X] % 64;
		uint8_t yPos = V[Y] % 32;

		uint8_t height = opcode & 0x000F;		//gets sprite height from N
		uint8_t pixel;							//temp variable to store row of sprite data

		V[0xF] = 0;			//collision flag, sets it to 0
		for (int yRow = 0; yRow < height; yRow++) {
			
			//stops loop if pixels go beyond the screen
			if (yPos + yRow >= 32) {
				break;
			}
			pixel = memory[I + yRow];

			for (int xCol = 0; xCol < 8; xCol++) {

				//stops loop if pixels goe beyond the screen
				if (xPos + xCol >= 64) {
					break;
				}

				if ((pixel & (0x80 >> xCol)) != 0) {

					if (display[(xPos + xCol + ((yPos + yRow) * 64))] == 0xFFFFFFFF) {
						V[0xF] = 1;		//sets VF equal to 1
					}
					display[xPos + xCol + ((yPos + yRow) * 64)] ^= 0xFFFFFFFF;
				}
			}
		}

		drawFlag = true;
		pc += 2;
	}
		break;
 
	case 0xE000: 

		switch (opcode & 0x00FF) {

		case 0x009E:		//0xEX9E: skips next instruction if key stored VX is pressed

			if (keypad[V[X] & 0xF] == 1) {
				pc += 4;
			}
			else {
				pc += 2;
			}
			break;

		case 0x00A1:		//0xEXA1: skips next instruction if key stored in VX isnt pressed

			if (keypad[V[X] & 0xF] == 0) {
				pc += 4;
			}
			else {
				pc += 2;
			}
			break;

		default:
			std::cerr << "Unknown opcode [0xE000]: 0x" << std::hex << std::uppercase << opcode << '\n';
			break;
		}
		break;

	case 0xF000:

		switch (opcode & 0x00FF) {

		case 0x0007:		//0xFX07: sets VX to the value of the delay timer

			V[X] = delay_timer;
			pc += 2;
			break;

		case 0x000A:		//0xFX0A: haults instructions until a key is pressed, timers still continue
		{
			bool keyPress = false;

			for (int i = 0; i < 16; ++i) {
				if (keypad[i] != 0) {
					V[X] = i;
					keyPress = true;
				}
			}
			if (keyPress == true) {
				pc += 2;
			}
		}
		break;

		case 0x0015:		//0xFX15: sets delay timer to VX

			delay_timer = V[X];
			pc += 2;
			break;

		case 0x0018:		//0xFX18: sets the sound timer to VX

			sound_timer = V[X];
			pc += 2;
			break;

		case 0x001E:		//0xFX1E: adds VX to I

			I += V[X];
			pc += 2;
			break;

		case 0x0029:		//sets I to the location of font sprite in VX

			I = fontset_address + ((V[X] & 0xF) * 0x5);
			pc += 2;
			break;

		case 0x0033:		//stores the bcd of VX, hundreds digits at I, tens at I + 1, and omes digits at I + 2

			memory[I] = V[X] / 100;			//stores the hundredth place into I
			memory[I + 1] = (V[X] / 10) % 10;		//stores the tenth place into I + 1
			memory[I + 2] = V[X] % 10;		//stores the ones place into I + 2

			pc += 2;
			break;

		case 0x0055:		//stores V0-VX in memory, starting at address I

			for (int i = 0; i <= X; ++i) {

				memory[I + i] = V[i];
			}
			pc += 2;
			break;

		case 0x0065:		//fills V0 - VX with values from memory starting at I

			for (int i = 0; i <= X; ++i) {

				V[i] = memory[I + i];
			}

			pc += 2;
			break;

		default:
			std::cerr << "Unknown opcode [0xF000]: 0x" << std::hex << std::uppercase << opcode << '\n';
			break;
		}
		break;

	default:
		std::cerr << "Unknown opcode: 0x" << std::hex << std::uppercase << opcode << '\n';
		break;

	}

}

void chip_8::updateTimers() {
	//update timers
	if (delay_timer > 0) {
		--delay_timer;
	}

	if (sound_timer > 0) {
		if (sound_timer == 1) {
			printf("Beep!\n");
		}
		--sound_timer;
	}
}