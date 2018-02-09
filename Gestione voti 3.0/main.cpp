#include <iostream>

#include "fileManagement.h"

using str = std::string;
using fstm = std::fstream;
using sstm = std::stringstream;
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


int main() {
	File inFile("input1.txt");
	
	str test;
	if (inFile.checkOpen()) test = "";
	inFile.file >> test;
	std::cout << test;

	/*
	std::cout << "l, w, c = lines, words, chars | nr = numero(di)\n\n";
	std::cout << "Linee:" << Fin.getNrLines() << " | Parole:" << Fin.getNrWords() << " | Byte:" << Fin.getNrChars() << "\n\n";
	std::cout << "nrw:l4= " << Fin.getNrWordsLine(3) << " | nrc:l4= " << Fin.getNrCharsLine(3) << " | nrc:w8= " << Fin.getNrCharsWord(7) << " | nrc:l3w5= " << Fin.getNrCharsWord(2, 4) << "\n\n";

	std::cout << "l6:" << Fin.getLine(5) << " | w3:" << Fin.getWord(2) << " | l3w2:" << Fin.getWord(2, 1) << " | c1:" << Fin.getChar(0)
		<< " | l2c3:" << Fin.getChar(1, 2) << " | l4w3c5:" << Fin.getChar(3, 2, 4) << "\n\n";
		
	std::cout << "c15-50:" << Fin.getChars(14, 49) << " | c50-15:" << Fin.getChars(49, 14) << " | l4c3-7:" << Fin.getChars(3, 2, 6)
		<< " | l4c7-3:" << Fin.getChars(3, 6, 2) << " | l4w2c2-5:" << Fin.getChars(3, 1, 1, 4) << " | l4w2c5-2:" << Fin.getChars(3, 1, 4, 1) << "\n\n";
	std::cout << "w2-4:" << Fin.getWords(1, 3) << " | w4-2:" << Fin.getWords(3, 1) << " | l2w2-4:" << Fin.getWords(1, 1, 3) << " | l2w4-2:" << Fin.getWords(1, 3, 1) << "\n\n";
	std::cout << "l4-3:" << Fin.getLines(3, 2) << " | l3-4:" << Fin.getLines(2, 3) << "\n\n";
		;
	*/
	askEnd();
	return 0;
}