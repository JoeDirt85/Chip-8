#ifndef CHIP_8_H
#define CHIP_8_H


#include <cstdint>
class chip_8
{
private:

	uint8_t memory[4096]{};		//0x000-0x1ff - was originally reserved for the interpreter. 0x050-0x0A0 - storage space for the 16 built in characters (0-f). 0x200-0xFFF stores the instructions from the ROM
	uint8_t V[16]{};			//V-Registers. 16 8-bit registers, V0-VF. Can hold values from 0x00-0xFF
	uint16_t I{};				//Index register, stores memory addresses used in operations.
	uint16_t pc{};				//Program counter. It is used to hold the next instruction to execute.
	uint16_t stack[16]{};		//used to track the order of execution
	uint8_t sp{};				//Stack pointer. is used to point to where the most recent value was placed in stack
	uint8_t delay_timer{};		//timer that decrements at 60 hz
	uint8_t sound_timer{};		//timer used for sound. decrements at 60 hz
	uint16_t opcode;			//holds the current opcode

	//method to initialize the 
	void initialize();	

public:

	uint8_t keypad[16]{};			//keypad that maps to 0-F
	uint32_t display[64 * 32]{};	//memory for the graphics

	chip_8();
	~chip_8();

	bool drawFlag;

	bool loadROM(char const* filename);		//method to load rom
	void emulateCycle();					//method that handles emulation logic
	void updateTimers();					//method that handles timers
	
};

#endif