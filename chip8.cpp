#include "chip8.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <memory>

using namespace std;

const int8_t fontSet[80] =
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


chip8::chip8()
	:memory(new int8_t[4096]),
	registers(new int8_t[16]),
	stack(new int16_t[16]),
	videoBuff(new int8_t[64 * 32]),
	keypad(new bool[16])
{}

chip8::~chip8()
{
	delete[] this->memory;
	delete[] this->registers;
	delete[] this->stack;
	delete[] this->videoBuff;
	delete[] this->keypad;
}

int chip8::loadRomFile(string path)
{
	cout << "[*] Loading rom: " << path << endl;

	ifstream is(path, ifstream::binary);
	is.seekg(0, is.end);
	int len = is.tellg();
	is.seekg(0, is.beg);

	char *buffer = new char[len];
	is.read(buffer, len);

	memcpy(this->memory + 0x200, buffer, sizeof(uint8_t) * len);

	cout << "[*] Total: " << len << " Bytes" << endl;

	is.close();
	delete[] buffer;
	return len;
}

int chip8::initial()
{
	this->PC = 0x200;
	this->I = 0x0;
	this->SP = -1;
	this->ST = 0x0;
	this->DT = 0x0;

	memset(this->videoBuff, 0x0, sizeof(int8_t) * 64 * 32);
	memset(this->stack, 0x0, sizeof(int16_t) * 16);
	memset(this->memory, 0x0, sizeof(int8_t) * 4096);
	memset(this->registers, 0x0, sizeof(int8_t) * 16);
	memset(this->keypad, 0x0, sizeof(bool) * 16);


	memcpy(this->memory, fontSet, sizeof(int8_t) * 80);

	this->videoUpdateFlag = false;

	srand(time(NULL));

	return 0;
}

bool chip8::isVideoUpdated()
{	
	bool tmp = this->videoUpdateFlag;
	this->videoUpdateFlag = false;
	return tmp;
}


void chip8::pressKeypad(int keynum)
{
	this->keypad[keynum] = true;
}

void chip8::releaseKeypad(int keynum)
{
	this->keypad[keynum] = false;
}

void chip8::step()
{
	int16_t opcode = (this->memory[this->PC] << 8) 
					| (this->memory[this->PC + 1] & 0x00FF);

	//printf("PC: 0x%X  |  opcode: 0x%X  \n", this->PC, opcode);
	
	this->PC += 2;

	switch (opcode & 0xF000)
	{
	case 0x0000:
	{
		if (0x00E0 == opcode) {
			this->videoUpdateFlag = true;
			memset(this->videoBuff, 0x0, sizeof(int8_t) * 64 * 32);
		}
		else if (0x00EE == opcode) {
			this->PC = this->stack[this->SP];
			this->SP--;
		}
		else {
			printf("This opcode is ignored!!!!!!! ");
			printf("SYS 0x%X # Jump to a machine code routine at 0x%X \n", (opcode & 0x0FFF), (opcode & 0x0FFF));
		}
		break;
	}
	case 0x1000:
		this->PC = (opcode & 0x0FFF);
		break;

	case 0x2000:
		this->SP++;
		this->stack[this->SP] = this->PC;
		this->PC = (opcode & 0x0FFF);
		break;

	case 0x3000:
		if (this->registers[((opcode & 0x0F00) >> 8)] == (opcode & 0x00FF)) {
			this->PC += 2;
		}
		break;

	case 0x4000:
		if (this->registers[((opcode & 0x0F00) >> 8)] != (opcode & 0x00FF)) {
			this->PC += 2;
		}
		break;

	case 0x5000:
		if (this->registers[((opcode & 0x0F00) >> 8)] == this->registers[((opcode & 0x00F0) >> 4)]) {
			this->PC += 2;
		}
		break;

	case 0x6000:
		this->registers[((opcode & 0x0F00) >> 8)] = (opcode & 0x00FF);
		break;

	case 0x7000:
		this->registers[((opcode & 0x0F00) >> 8)] += (opcode & 0x00FF);
		break;

	case 0x8000:
	{
		switch (opcode & 0x000F)
		{
		case 0:
			this->registers[((opcode & 0x0F00) >> 8)] = this->registers[((opcode & 0x00F0) >> 4)];
			break;

		case 1:
			this->registers[((opcode & 0x0F00) >> 8)] |= this->registers[((opcode & 0x00F0) >> 4)];
			break;

		case 2:
			this->registers[((opcode & 0x0F00) >> 8)] &= this->registers[((opcode & 0x00F0) >> 4)];
			break;

		case 3:
			this->registers[((opcode & 0x0F00) >> 8)] ^= this->registers[((opcode & 0x00F0) >> 4)];
			break;

		case 4:
		{
			unsigned int rlt = static_cast<uint8_t>(this->registers[((opcode & 0x0F00) >> 8)])
							+ static_cast<uint8_t>(this->registers[((opcode & 0x00F0) >> 4)]);

			if (rlt > 255) {
				this->registers[0xF] = 1;
			}
			else {
				this->registers[0xF] = 0;
			}

			this->registers[((opcode & 0x0F00) >> 8)] = static_cast<int8_t>(rlt);
			
			break;
		}
		case 5:
			if (this->registers[((opcode & 0x0F00) >> 8)] > this->registers[((opcode & 0x00F0) >> 4)]) {
				this->registers[0xF] = 1;
			}
			else {
				this->registers[0xF] = 0;
			}

			this->registers[((opcode & 0x0F00) >> 8)] -= this->registers[((opcode & 0x00F0) >> 4)];

			break;

		case 6:
			this->registers[0xF] = this->registers[((opcode & 0x0F00) >> 8)] & 0x01;
			this->registers[((opcode & 0x0F00) >> 8)] /= 2;

			break;

		case 7:
			if (this->registers[((opcode & 0x00F0) >> 4)] > this->registers[((opcode & 0x0F00) >> 8)]) {
				this->registers[0xF] = 1;
			}
			else {
				this->registers[0xF] = 0;
			}

			this->registers[((opcode & 0x0F00) >> 8)] =
				this->registers[((opcode & 0x00F0) >> 4)] - this->registers[((opcode & 0x0F00) >> 8)];

			break;

		case 8:
			this->registers[0xF] = this->registers[((opcode & 0x0F00) >> 8)] & 0x80;
			this->registers[((opcode & 0x0F00) >> 8)] *= 2;
			break;

		default:
			printf("Unknow opcode \n");
			break;
		}

		break;
	}

	case 0x9000:
		if (this->registers[((opcode & 0x0F00) >> 8)] != this->registers[((opcode & 0x00F0) >> 4)]) {
			this->PC += 2;
		}
		break;

	case 0xA000:
		this->I = (opcode & 0x0FFF);
		break;

	case 0xB000:
		
		this->PC += this->registers[0];
		break;

	case 0xC000:
		this->registers[((opcode & 0x0F00) >> 8)] = (int8_t)((unsigned int)rand() % 256) & (opcode & 0x00FF);
		break;

	case 0xD000: {
		

		this->videoUpdateFlag = true;

		int vx = this->registers[((opcode & 0x0F00) >> 8)];
		int vy = this->registers[((opcode & 0x00F0) >> 4)];
		int n = (opcode & 0x000F);

		
		this->registers[0xF] = 0;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < 8; j++) {
				if (this->videoBuff[64 * (vy + i) + vx + j] == 1) {
					this->registers[0xF] = 1;
				}
				int x = vx + j % 64;
				int y = (vy + i) % 32;

				
				this->videoBuff[64 * y + x] ^= (this->memory[this->I + i] >> (7 - j)) & 0x01;
			}
		}

		break;
	}
	case 0xE000:
		if (0x009E == (opcode & 0x00FF)) {
			uint8_t keyvalude = static_cast<uint8_t>(this->registers[((opcode & 0x0F00) >> 8)]);
			if (this->keypad[keyvalude] == true) {
				this->PC += 2;
			}
		}
		else if (0x00A1 == (opcode & 0x00FF)) {
			uint8_t keyvalude = static_cast<uint8_t>(this->registers[((opcode & 0x0F00) >> 8)]);
			if (this->keypad[keyvalude] == false) {
				this->PC += 2;
			}
		}
		else {
			printf("Unknow opcode \n");
		}
		break;

	case 0xF000:
		switch (opcode & 0x00FF)
		{
		case 0x0007:
			this->registers[((opcode & 0x0F00) >> 8)] = this->DT;
			break;

		case 0x000A: {
			bool keypress = false;
			for (int i = 0; i < 16; i++) {
				if (this->keypad[i] == true) {
					this->registers[((opcode & 0x0F00) >> 8)] = i;
					keypress = true;
				}
			}
			if (!keypress) {
				//让下次继续执行此opcode
				this->PC -= 2;
			}
			break;
		}
		case 0x0015:
			this->DT = this->registers[((opcode & 0x0F00) >> 8)];
			break;

		case 0x0018:
			this->ST = this->registers[((opcode & 0x0F00) >> 8)];
			break;

		case 0x001E:
			this->I += this->registers[((opcode & 0x0F00) >> 8)];
			break;

		case 0x0029: {
			uint8_t dig = static_cast<uint8_t>(this->registers[((opcode & 0x0F00) >> 8)]);
			this->I = dig * 5;
			break;
		}
		case 0x0033:
			this->memory[this->I] = this->registers[(opcode & 0x0F00) >> 8] / 100;
			this->memory[this->I + 1] = (this->registers[(opcode & 0x0F00) >> 8] / 10) % 10;
			this->memory[this->I + 2] = (this->registers[(opcode & 0x0F00) >> 8] % 100) % 10;
			break;

		case 0x0055: {
			int x = ((opcode & 0x0F00) >> 8);

			for (int i = 0; i <= x; i++) {
				this->memory[I + i] = this->registers[i];
			}
			break;
		}
		case 0x0065: {
			int t = ((opcode & 0x0F00) >> 8);

			for (int i = 0; i <= t; i++) {
				this->registers[i] = this->memory[I + i];
			}
			break;
		}
		default:
			printf("Unknow opcode \n");
			break;
		}
		break;

	default:
		printf("Unknow opcode \n");
		break;

	}

	if (this->DT > 0) { DT--; }
	if (this->ST > 0) { ST--; }



}