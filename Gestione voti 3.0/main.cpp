#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <conio.h>

#include "fileManagement.h"

using Tstr = sp::Tstr;
using Tfstm = sp::Tfstm;
using si8 = sp::int8;
using si16 = sp::int16;
using si32 = sp::int32;
using si64 = sp::int64;
using ui8 = sp::uint8;
using ui16 = sp::uint16;
using ui32 = sp::uint32;
using ui64 = sp::uint64;

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
	test = sp::File("backup.txt").str();

	test.replaceWord(1, 'c');

	printSpaces(test.str());

	while (!_kbhit()) {}
	return 0;
}