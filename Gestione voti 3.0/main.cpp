#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>

#include "fileManagement.h"
/*
std::fstream fIle("input1.txt", std::ios::binary | std::ios::in | std::ios::out);
fIle >> test;
fIle.seekg(0);
fIle.put('b');

if (!f1.open()) return 0;
f1.file.put('t');
f1.pointTo(0, 0, 0);
f1.file >> test;
std::cout << test;
f1.file >> test;
std::cout << test << f1.countWords("ciao, come va ?");

f1.pointTo(1, 1, 0);
f1.file >> test;
std::cout << test;

f1.pointTo(0, 0, 0);
std::cout << f1.file.tellg() << f1.file.tellp();
f1.file.put('T');

f1.file.close();
*/
/*
std::cout << "l, w, c = lines, words, chars | nr = numero(di)\n\n";
std::cout << "Linee:" << f1.getNrLines() << " | Parole:" << f1.getNrWords() << " | Byte:" << f1.getNrChars() << "\n\n";
std::cout << "nrw_l4:" << f1.getNrWordsLine(3) << " | nrc_l4:" << f1.getNrCharsLine(3) << " | nrc_w8:" << f1.getNrCharsWord(7) << " | nrc_l3w5:" << f1.getNrCharsWord(2, 4) << "\n\n";
std::cout << "l6:" << f1.getLine(5) << " | w3:" << f1.getWord(2) << " | l3w2:" << f1.getWord(2, 1) << " | c1:" << f1.getChar(0)
<< " | l2c3:" << f1.getChar(1, 2) << " | l4w3c5:" << f1.getChar(3, 2, 4) << "\n\n";

std::cout << "c16-50:" << f1.getChars(15, 49) << " | c50-16:" << f1.getChars(49, 15) << " | l4c3-7:" << f1.getChars(3, 2, 6)
<< " | l4c7-3:" << f1.getChars(3, 6, 2) << " | l4w2c2-5:" << f1.getChars(3, 1, 1, 4) << " | l4w2c5-2:" << f1.getChars(3, 1, 4, 1) << "\n\n";
std::cout << "w2-4:" << f1.getWords(1, 3) << " | w4-2:" << f1.getWords(3, 1) << " | l2w2-4:" << f1.getWords(1, 1, 3) << " | l2w4-2:" << f1.getWords(1, 3, 1) << "\n\n";
*/

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
void printSpaces(Tstr str) {
	for (char letter : str) {
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
}


int main() {
	char testChar;
	Tstr test;


	File f1("f1.txt");
	File f2("f2.txt");
	File backup("backup.txt");
	backup.move(f1);
	backup.move(f2);
	f1.close();
	f1.open();
	f2.open();
	

	f1.deleteLines(10, 2);


	std::cout << "\n\n";
	printSpaces(f1);
	std::cout << "\n\n";
	printSpaces(f2);

	////FARE testare meglio
	////add and delete
	//f1.addLine(0, "La-");				//l
	//std::cout << f1.getLine(0);
	//f1.deleteLine(0);
	//f1.addWord(1, "Wa1-");				//w
	//std::cout << f1.getWord(1);
	//f1.deleteWord(1);
	//f1.addWord(2, 2, "Wa2-");			//lw
	//std::cout << f1.getWord(2, 2);
	//f1.deleteWord(2, 2);
	//f1.addChar(0, 'A');					//c
	//std::cout << f1.getChar(0);
	//f1.deleteChar(0);
	//f1.addChar(1, 1, 'B');				//lc
	//std::cout << f1.getChar(1, 1);
	//f1.deleteChar(1, 1);
	//f1.addChar(2, 2, 0, 'C');			//lwc
	//std::cout << f1.getChar(2, 2, 0);
	//f1.deleteChar(2, 2, 0);
	//
	////replace
	//test = f1.getLine(1);				//l
	//f1.replaceLine(1, "Lr-");
	//std::cout << f1.getLine(1);
	//f1.replaceLine(1, test);
	//test = f1.getWord(1);				//w
	//f1.replaceWord(1, "Wr1-");
	//std::cout << f1.getWord(1);
	//f1.replaceWord(1, test);
	//test = f1.getWord(1, 0);			//lw
	//f1.replaceWord(1, 0, "Wr2-");
	//std::cout << f1.getWord(1, 0);
	//f1.replaceWord(1, 0, test);
	//testChar = f1.getChar(0);			//c
	//f1.replaceChar(0, 'D');
	//std::cout << f1.getChar(0);
	//f1.replaceChar(0, testChar);
	//testChar = f1.getChar(1, 1);		//lc
	//f1.replaceChar(1, 1, 'E');
	//std::cout << f1.getChar(1, 1);
	//f1.replaceChar(1, 1, testChar);
	//testChar = f1.getChar(2, 2, 0);		//lwc
	//f1.replaceChar(2, 2, 0, 'F');
	//std::cout << f1.getChar(2, 2, 0);
	//f1.replaceChar(2, 2, 0, testChar);

	///*f1.appendChar(' ');
	//f1.appendWord("");
	//f1.appendLine("\r\n");
	//f1.appendWord(0, "Ciao");*/

	////FARE fa cose strane
	//f1.addLine(7, "Ciao");
	//std::cout << "\n\n                         Intero file: " << f1.getNrChars() << "byte\n";
	//printSpaces(f1);


	std::cout << "";
	std::cout << "";
	std::cout << "";
	std::cout << "";
	std::cout << "";
	std::cout << "";
	std::cout << "";
	std::cout << "";
	std::cout << "";

	f1.close();
	f2.close();
	askEnd();
	return 0;
}