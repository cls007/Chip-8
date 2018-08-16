#pragma once

#include <cstdint>
#include <string>

class chip8
{
public:
	chip8();
	~chip8();

	int loadRomFile(std::string path);
	int initial();
	void step();
	bool isVideoUpdated();
	void pressKeypad(int keynum);
	void releaseKeypad(int keynum);


//public:
	
	int8_t * videoBuff;

private:

	
	bool videoUpdateFlag;

	//store the currently executing address
	int16_t PC;
	//point to the topmost level of the stack.
	int8_t SP;
	//used to store the address that the interpreter shoud return to when finished with a subroutine
	int16_t *stack;

	int8_t *memory;
	int8_t *registers;

	//This register is generally used to store memory addresses
	int16_t I;

	//When these registers are non-zero, they are automatically decremented at a rate of 60Hz.
	int8_t DT;	//Delay timer register
	int8_t ST;	//sound timer register

	bool *keypad;




};

