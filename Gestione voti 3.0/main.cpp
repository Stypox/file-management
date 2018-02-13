#include <iostream>
#include <fstream>
#include <Windows.h>

#include "fileManagement.h"
/*
std::fstream fIle("input1.txt", std::ios::binary | std::ios::in | std::ios::out);
fIle >> test;
fIle.seekg(0);
fIle.put('b');

if (!inFile.checkOpen()) return 0;
inFile.file.put('t');
inFile.pointTo(0, 0, 0);
inFile.file >> test;
std::cout << test;
inFile.file >> test;
std::cout << test << inFile.countWords("ciao, come va ?");

inFile.pointTo(1, 1, 0);
inFile.file >> test;
std::cout << test;

inFile.pointTo(0, 0, 0);
std::cout << inFile.file.tellg() << inFile.file.tellp();
inFile.file.put('T');

inFile.file.close();
*/
/*
std::cout << "l, w, c = lines, words, chars | nr = numero(di)\n\n";
std::cout << "Linee:" << inFile.getNrLines() << " | Parole:" << inFile.getNrWords() << " | Byte:" << inFile.getNrChars() << "\n\n";
std::cout << "nrw_l4:" << inFile.getNrWordsLine(3) << " | nrc_l4:" << inFile.getNrCharsLine(3) << " | nrc_w8:" << inFile.getNrCharsWord(7) << " | nrc_l3w5:" << inFile.getNrCharsWord(2, 4) << "\n\n";
std::cout << "l6:" << inFile.getLine(5) << " | w3:" << inFile.getWord(2) << " | l3w2:" << inFile.getWord(2, 1) << " | c1:" << inFile.getChar(0)
<< " | l2c3:" << inFile.getChar(1, 2) << " | l4w3c5:" << inFile.getChar(3, 2, 4) << "\n\n";

std::cout << "c16-50:" << inFile.getChars(15, 49) << " | c50-16:" << inFile.getChars(49, 15) << " | l4c3-7:" << inFile.getChars(3, 2, 6)
<< " | l4c7-3:" << inFile.getChars(3, 6, 2) << " | l4w2c2-5:" << inFile.getChars(3, 1, 1, 4) << " | l4w2c5-2:" << inFile.getChars(3, 1, 4, 1) << "\n\n";
std::cout << "w2-4:" << inFile.getWords(1, 3) << " | w4-2:" << inFile.getWords(3, 1) << " | l2w2-4:" << inFile.getWords(1, 1, 3) << " | l2w4-2:" << inFile.getWords(1, 3, 1) << "\n\n";
*/

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
	File inFile("input.txt");
	char testChar;
	str test;

	test = inFile.getLine(1);
	inFile.replaceLine(1, "Linea-");
	std::cout << inFile.getLine(1);
	inFile.replaceLine(1, test);


	test = inFile.getWord(1);
	inFile.replaceWord(1, "Parola1-");
	std::cout << inFile.getWord(1);
	inFile.replaceWord(1, test);

	test = inFile.getWord(1, 0);
	inFile.replaceWord(1, 0, "Parola2-");
	std::cout << inFile.getWord(1, 0);
	inFile.replaceWord(1, 0, test);
	

	testChar = inFile.getChar(0);
	inFile.replaceChar(0, 'A');
	std::cout << inFile.getChar(0);
	inFile.replaceChar(0, testChar);

	testChar = inFile.getChar(1, 1);
	inFile.replaceChar(1, 1, 'B');
	std::cout << inFile.getChar(1, 1);
	inFile.replaceChar(1, 1, testChar);

	testChar = inFile.getChar(2, 2, 0);
	inFile.replaceChar(2, 2, 0, 'C');
	std::cout << inFile.getChar(2, 2, 0);
	inFile.replaceChar(2, 2, 0, testChar);


	std::cout << inFile.deleteChar(1, 1, 1);
	std::cout << "\n\n" << inFile.string();


	std::cout << '\n';
	std::cout << '\r';
	std::cout << '\r';
	std::cout << '\r';
	std::cout << '\r';
	std::cout << '\r';
	std::cout << '\r';
	std::cout << '\r';
	std::cout << '\r';

	inFile.file.close();
	askEnd();
	return 0;
}