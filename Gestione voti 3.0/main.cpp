#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <conio.h>

#include "fileManagement.h"

using Tstr = std::string;
using Tfstm = std::fstream;
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

void askEnd() {
	bool end;
	std::cin >> end;
	return;
}
void printSpaces(char letter) {
	switch (letter) {
	case ' ':
		std::cout << "_";
		break;
	case '\t':
		std::cout << "\\t";
		break;
	case '\n':
		std::cout << "\n\\n";
		break;
	case '\v':
		std::cout << "\\v";
		break;
	case '\f':
		std::cout << "\\f";
		break;
	case '\r':
		std::cout << "\\r";
		break;
	default:
		std::cout << letter;
	}
}
void printSpaces(Tstr str) {
	for (char letter : str) printSpaces(letter);
}

using ib = std::ios_base;

int main() {
	Tstr tempString = " \r";
	sp::File test("f1.txt");

	test.add(0, (int8)12);

	printSpaces(test.str());

	while (!_kbhit()) {}
	return 0;
}