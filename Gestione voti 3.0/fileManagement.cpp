#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include "fileManagement.h"

/*
bool File::checkOpen() {
	file.open(path.c_str());
	if (!file) {
		return 0;
	}
	return 1;
}
bool File::closeAndOpen() {
	file.close();

	return checkOpen();
}


//I valori partono da 0 (0 = 1a riga/parola/carattere, 1 = 2a riga/parola/carattere...)
//
//Line e word vengono usati come offsets, se sono a 0 si parte dall'inizio del file.
//Es: pointTo(3, 2, 4) punta al 5o carattere della 3a parola della 4a linea.
//	pointTo(3, 0, 0) punta alla 4a linea
//	pointTo(3, 0, 4) punta al 5o carattere della 4a linea
//	pointTo(0, 0, 7) punta al 8o carattere dall'inizio del file
//	pointTo(0, 2, 0) punta alla 3a parola dall'inizio del file

bool File::pointTo(uint16 Line, uint16 Word, uint16 Char) {
	if (!pointToBeg()) return 0;

	Ustr tempStr;
	for (uint16 currentLine = 0; currentLine < Line; ++currentLine) {
		getline(file, tempStr);
	}
	for (uint16 currentWord = 0; currentWord < Word; ++currentWord) {
		file >> tempStr;
	}
	if (Word != 0) ++Char;
	for (uint16 currentChar = 0; currentChar < Char; ++currentChar) {
		file.ignore();
	}

	return 1;
}

//Porta il puntatore all'inizio del file.
//Se il file non e' gia' aperto lo apre.

bool File::pointToBeg() {
	if (!file) {
		std::cout << "non_aperto" << file.eof() << file.fail() << file.bad();
		file.open(path);

		if (!file) {
			std::cout << "NON_APERTO";
			return 0;
		}

		return 1;
	}

	file.seekg(0, std::ios::beg);

	file.clear(file.eofbit);

	return 1;
}

uint16 File::countWords(Ustr String) {
	bool wasSpace = 1;
	uint16 nrWords = 0;

	for (uint16 currentChar = 0; currentChar < String.length(); ++currentChar) {
		if (!isspace(String[currentChar])) {
			if (wasSpace) {
				wasSpace = 0;
				++nrWords;
			}
		}
		else {
			wasSpace = 1;
		}
	}

	return nrWords;
}


File::File() {
	path = "";
}
File::File(Ustr Path) {
	path = Path;
}


uint16 File::getNrLines() {
	if (!pointToBeg()) return 0;
	uint16 lines = 0;
	Ustr tempStr;
	while (getline(file, tempStr)) {
		lines++;
	}
	return lines;
}
uint16 File::getNrWords() {
	if (!pointToBeg()) return 0;
	uint16 words = 0;
	Ustr tempStr;
	while (file >> tempStr) {
		words++;
	}
	return words;
}
uint16 File::getNrChars() {
	if (!pointToBeg()) return 0;

	file.seekg(0, std::ios_base::end);
	return (uint16)file.tellg();
}
uint16 File::getNrWordsLine(uint16 Line) {
	if (!pointTo(Line, 0, 0)) return 0;

	Ustr tempStr;
	getline(file, tempStr);

	return countWords(tempStr);
}
uint16 File::getNrCharsLine(uint16 Line) {
	if (!pointTo(Line, 0, 0)) return 0;

	Ustr tempStr;
	getline(file, tempStr);

	return tempStr.length();
}
uint16 File::getNrCharsWord(uint16 Word) {
	if (!pointTo(0, Word, 0)) return 0;

	Ustr tempStr;
	file >> tempStr;

	return tempStr.length();
}
uint16 File::getNrCharsWord(uint16 Line, uint16 Word) {	
	if (!pointTo(Line, Word, 0)) return 0;

	Ustr tempStr;
	file >> tempStr;

	return tempStr.length();
}


Ustr File::getLine(uint16 Line) {
	if (!pointTo(Line, 0, 0)) return "";

	Ustr strLine;
	getline(file, strLine);

	return strLine;
}
Ustr File::getWord(uint16 Word) {
	if (!pointTo(0, Word, 0)) return "";

	Ustr strWord;
	file >> strWord;

	return strWord;
}
Ustr File::getWord(uint16 Line, uint16 Word) {
	if (!pointTo(Line, Word, 0)) return "";

	Ustr strWord;
	file >> strWord;

	return strWord;
}
char File::getChar(uint16 Char) {
	if (!pointTo(0, 0, Char)) return 0;

	return file.get();
}
char File::getChar(uint16 Line, uint16 Char) {
	if (!pointTo(Line, 0, Char)) return 0;

	return file.get();
}
char File::getChar(uint16 Line, uint16 Word, uint16 Char) {
	if (!pointTo(Line, Word, Char)) return 0;

	return file.get();
}


Ustr File::getLines(uint16 From, uint16 To) {
	Ustr lines;
	if (To < From) {
		if (!pointTo(To, 0, 0)) return "";
		getline(file, lines);

		Ustr tempStr;
		for (uint16 currentLine = To; currentLine < From; ++currentLine) {
			getline(file, tempStr);
			lines = tempStr + "\n" + lines;
		}
	}
	else {
		if (!pointTo(From, 0, 0)) return "";
		getline(file, lines);

		Ustr tempStr;
		for (uint16 currentLine = From; currentLine < To; ++currentLine) {
			getline(file, tempStr);
			lines += "\n" + tempStr;
		}
	}

	return lines;
}
Ustr File::getWords(uint16 From, uint16 To) {
	Ustr words;
	if (To < From) {
		if (!pointTo(0, To, 0)) return "";
		file >> words;

		Ustr tempStr;
		for (uint16 currentWord = To; currentWord < From; ++currentWord) {
			file >> tempStr;
			words = tempStr + " " + words;
		}
	}
	else {
		if (!pointTo(0, From, 0)) return "";
		file >> words;

		Ustr tempStr;
		for (uint16 currentWord = From; currentWord < To; ++currentWord) {
			file >> tempStr;
			words += " " + tempStr;
		}
	}
	return words;
}
Ustr File::getWords(uint16 Line, uint16 From, uint16 To) {
	Ustr words;
	if (To < From) {
		if (!pointTo(Line, To, 0)) return "";
		file >> words;

		Ustr tempStr;
		for (uint16 currentWord = To; currentWord < From; ++currentWord) {
			file >> tempStr;
			words = tempStr + " " + words;
		}
	} else {
		if (!pointTo(Line, From, 0)) return "";
		file >> words;

		Ustr tempStr;
		for (uint16 currentWord = From; currentWord < To; ++currentWord) {
			file >> tempStr;
			words += " " + tempStr;
		}
	}
	return words;
}
Ustr File::getChars(uint16 From, uint16 To) {
	Ustr chars = "";
	if (To < From) {
		if (!pointTo(0, 0, To)) return "";
		chars += file.get();

		for (uint16 currentChar = To; currentChar < From; ++currentChar) {
			if (file.eof()) break;
			chars = (char)file.get() + chars;
		}
	} else {
		if (!pointTo(0, 0, From)) return "";
		chars += file.get();

		for (uint16 currentChar = From; currentChar < To; ++currentChar) {
			if (file.eof()) break;
			chars += file.get();
		}
	}
	return chars;
}
Ustr File::getChars(uint16 Line, uint16 From, uint16 To) {
	Ustr chars = "";
	if (To < From) {
		if (!pointTo(Line, 0, To)) return "";
		chars += file.get();

		for (uint16 currentChar = To; currentChar < From; ++currentChar) {
			if (file.eof()) break;
			chars = (char)file.get() + chars;
		}
	} else {
		if (!pointTo(Line, 0, From)) return "";
		chars += file.get();

		for (uint16 currentChar = From; currentChar < To; ++currentChar) {
			if (file.eof()) break;
			chars += file.get();
		}
	}
	return chars;
}
Ustr File::getChars(uint16 Line, uint16 Word, uint16 From, uint16 To) {
	Ustr chars = "";
	if (To < From) {
		if (!pointTo(Line, Word, To)) return "";
		chars += file.get();

		for (uint16 currentChar = To; currentChar < From; ++currentChar) {
			if (file.eof()) break;
			chars = (char)file.get() + chars;
		}
	} else {
		if (!pointTo(Line, Word, From)) return "";
		chars += file.get();

		for (uint16 currentChar = From; currentChar < To; ++currentChar) {
			if (file.eof()) break;
			chars += file.get();
		}
	}
	return chars;
}

bool File::replaceLine(uint16 Line, Ustr Replacement) {
	pointTo(Line, 0, 0);

	bool endLine = 0;
	char readChar;

	while (!Replacement.empty()) {
		if (file.eof()) break;

		readChar = file.get();
		


		if (readChar == '\n' || readChar == '\n') {
			endLine = 1;
		}
	}
	char readNow;

	
	

	return 1;
}
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

bool File::checkOpen() {
	file.open(path);
	if (file) return true;
	return false;
}

bool File::pointTo(uint16 Line, uint16 Word, uint16 Char) {
	if (!pointToBeg()) return 0;

	str tempStr;
	for (uint16 currentLine = 0; currentLine < Line; ++currentLine) {
		getline(file, tempStr);
	}
	for (uint16 currentWord = 0; currentWord < Word; ++currentWord) {
		file >> tempStr;
	}
	if (Word != 0) ++Char;
	for (uint16 currentChar = 0; currentChar < Char; ++currentChar) {
		file.ignore();
	}

	return 1;
}
bool File::pointToBeg() {
	return false;
}

uint16 File::countWords(str String) {
	return uint16();
}

File::File() : path("") {}
File::File(str Path) : path(Path) {}
