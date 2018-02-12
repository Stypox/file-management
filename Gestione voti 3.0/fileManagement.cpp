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
*/
/*
VECCHIO REPLACELINE NON FUNZIONANTE
bool File::replaceLine(uint16 Line, str Replacement) {
if (!pointTo(Line, 0, 0)) return false;

str buffer = Replacement;
bool endLine = 1, eof = 0;
char readChar;

while ((readChar = file.get()) != '\n') {
file.put(buffer[0]);
buffer.erase(0);
if (buffer.empty()) {
endLine = 0;

while ((readChar = file.get()) != '\n') {
if (file.eof()) {
eof = 1;
break;
}
}

break;
}
}

if (eof && !endLine) {
file.seekg(file.tellp());
}

while (!buffer.empty()) {
buffer += readChar;
file.put(buffer[0]);
buffer.erase(0);

if (file.eof()) {
eof = 1;
break;
}

readChar = file.get();
}

if (eof) {
for (char letter : buffer) {
file.putback(letter);
}
}


return true;
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

/*
Returns false if the file couldn't be opened, otherwise true
*/
bool File::checkOpen() {
	file.open(path, std::ios::binary | std::ios::in | std::ios::out | std::ios::app); //FARE va bene app
	if (file.is_open()) return true;
	return false;
}

/*
The two pointers inside the file are moved to the new position
The file is opened, if it wasn't already
Line and word are used as offsets, if set to 0 it starts from the beginning
Eg: pointTo(3, 2, 4) points to the 5th char of the 3rd word of the 4th line
	pointTo(3, 0, 0) points to the 4th line
	pointTo(3, 0, 4) points to the 5th char of the 4th line
	pointTo(0, 0, 7) points to the 8th char from the beginning
	pointTo(0, 2, 0) points to the 3rd word from the beginning
Returns false if the file couldn't be opened or
	if EOF has been reached (!), otherwise true
*/
bool File::pointTo(uint16 Line, uint16 Word, uint16 Char) {
	if (!pointToBeg()) return false;

	str tempStr;
	for (uint16 currentLine = 0; currentLine < Line; ++currentLine)	getline(file, tempStr);
	for (uint16 currentWord = 0; currentWord < Word; ++currentWord) file >> tempStr;
	if (Word != 0) ++Char;
	for (uint16 currentChar = 0; currentChar < Char; ++currentChar) file.ignore();

	file.seekp(file.tellg());

	return !file.eof();
}
/*
The two pointers inside the file are moved to the beginning
The file is opened, if it wasn't already
The file's eofbit is cleared
Returns true if the file opened correctly, otherwise false
*/
bool File::pointToBeg() {
	if (!file.is_open() && !checkOpen()) return false;
	file.clear(file.eofbit);
	file.seekg(0);
	file.seekp(0);
	return true;
}
/*
Retrieves the (start) position of the requested line/word/char
Returns -1 if the file couldn't be opened or if
	the specified position is out of bounds.
*/
std::streampos File::getPosition(uint16 Line, uint16 Word, uint16 Char) {
	std::streampos GpointerBeginning = 0, PpointerBeginning = 0;
	if (file.is_open()) {
		GpointerBeginning = file.tellg();
		PpointerBeginning = file.tellp();
	}
	if (!pointToBeg()) return -1;

	str tempStr;
	for (uint16 currentLine = 0; currentLine < Line; ++currentLine)	getline(file, tempStr);
	for (uint16 currentWord = 0; currentWord < Word; ++currentWord) file >> tempStr;
	if (Word != 0) ++Char;

	std::streampos position = file.tellg() + (std::streampos)Char;
	file.seekg(0, std::ios_base::end);
	std::streampos pos = file.tellg();
	if (position > file.tellg()) {	//FARE vedere se c'e' bisogno di >= ma penso di si'
		position = -1;
	}

	file.clear(file.eof());
	file.seekg(GpointerBeginning);
	file.seekp(PpointerBeginning);

	return position;
}

/*
Counts the number of words in a string,
	based on the spaces between them.
*/
uint16 File::countWords(str String) {
	bool wasSpace = 1;
	uint16 nrWords = 0;

	for (uint16 currentChar = 0; currentChar < String.length(); ++currentChar) {
		if (!isspace(String[currentChar])) {
			if (wasSpace) {
				wasSpace = 0;
				++nrWords;
			}
		}
		else wasSpace = 1;
	}

	return nrWords;
}
/*
Deletes all '\r' (Carriage Return) at the end of the string
*/
void File::truncEndCR(str & String) {
	while (!String.empty() && String.back() == '\r') {
		String.pop_back();
	}
}
/*
Gets all the content from the 1st file and copies it to the 2nd
Files must be already open
*/
void File::moveFileContent(fstm & From, fstm & To) {
	char tempChar;
	while (1) {
		tempChar = From.get();
		if (!From.eof()) {
			To << tempChar;
		}
		else return;
	}
}


/*
default constructor
initializes path to an empty string
	and the tempPath to its default value
*/
File::File() : path(""), tempPath(defaultTempPath) {}
/*
constructor with path
initializes path to the corresponding parameter
	and the tempPath to path + ".tmp"
*/
File::File(str Path) : path(Path), tempPath(Path + ".tmp") {}
/*
constructor with path and tempPath
initializes path and tempPath to the corresponding parameters
*/
File::File(str Path, str TempPath) : path(Path), tempPath(TempPath) {}


uint16 File::getNrLines() {
	if (!pointToBeg()) return 0;

	uint16 lines = 0;
	str tempStr;
	while (getline(file, tempStr)) lines++;

	return lines;
}
uint16 File::getNrWords() {
	if (!pointToBeg()) return 0;

	uint16 words = 0;
	str tempStr;
	while (file >> tempStr) words++;

	return words;
}
uint16 File::getNrChars() {
	if (!pointToBeg()) return 0;

	file.seekg(0, std::ios_base::end);

	return (uint16)file.tellg();
}
uint16 File::getNrWordsLine(uint16 Line) {
	if (!pointTo(Line, 0, 0)) return 0;

	str tempStr;
	getline(file, tempStr);

	return countWords(tempStr);
}
uint16 File::getNrCharsLine(uint16 Line) {
	if (!pointTo(Line, 0, 0)) return 0;

	str tempStr;
	getline(file, tempStr);

	return (uint16)tempStr.length();
}
uint16 File::getNrCharsWord(uint16 Word) {
	if (!pointTo(0, Word, 0)) return 0;

	str tempStr;
	file >> tempStr;

	return (uint16)tempStr.length();
}
uint16 File::getNrCharsWord(uint16 Line, uint16 Word) {
	if (!pointTo(Line, Word, 0)) return 0;

	str tempStr;
	file >> tempStr;

	return (uint16)tempStr.length();
}

/*
Removes '\r' at the end of the line
*/
str File::getLine(uint16 Line) {
	if (!pointTo(Line, 0, 0)) return "";

	str line;
	getline(file, line);
	truncEndCR(line);

	return line;
}
str File::getWord(uint16 Word) {
	if (!pointTo(0, Word, 0)) return "";

	str word;
	file >> word;

	return word;
}
str File::getWord(uint16 Line, uint16 Word) {
	if (!pointTo(Line, Word, 0)) return "";

	str word;
	file >> word;

	return word;
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


/*
Removes '\r' at the end of lines and adds '\n' between them
*/
str File::getLines(uint16 From, uint16 To) {
	if (To < From) {
		if (!pointTo(To, 0, 0)) return "";

		str lines;
		getline(file, lines);
		truncEndCR(lines);

		str tempStr;
		for (uint16 currentLine = To; currentLine < From; ++currentLine) {
			if (!getline(file, tempStr)) break;
			truncEndCR(tempStr);
			lines = tempStr + "\r\n" + lines;
		}

		return lines;
	}
	else {
		if (!pointTo(From, 0, 0)) return "";

		str lines;
		getline(file, lines);
		truncEndCR(lines);

		str tempStr;
		for (uint16 currentLine = From; currentLine < To; ++currentLine) {
			if (!getline(file, tempStr)) break;
			truncEndCR(tempStr);
			lines += "\r\n" + tempStr;
		}

		return lines;
	}
}
str File::getWords(uint16 From, uint16 To) {
	str words;
	if (To < From) {
		if (!pointTo(0, To, 0)) return "";
		file >> words;

		str tempStr;
		for (uint16 currentWord = To; currentWord < From; ++currentWord) {
			file >> tempStr;
			words = tempStr + " " + words;
		}
	}
	else {
		if (!pointTo(0, From, 0)) return "";
		file >> words;

		str tempStr;
		for (uint16 currentWord = From; currentWord < To; ++currentWord) {
			file >> tempStr;
			words += " " + tempStr;
		}
	}
	return words;
}
str File::getWords(uint16 Line, uint16 From, uint16 To) {
	str words;
	if (To < From) {
		if (!pointTo(Line, To, 0)) return "";
		file >> words;

		str tempStr;
		for (uint16 currentWord = To; currentWord < From; ++currentWord) {
			file >> tempStr;
			words = tempStr + " " + words;
		}
	}
	else {
		if (!pointTo(Line, From, 0)) return "";
		file >> words;

		str tempStr;
		for (uint16 currentWord = From; currentWord < To; ++currentWord) {
			file >> tempStr;
			words += " " + tempStr;
		}
	}
	return words;
}
str File::getChars(uint16 From, uint16 To) {
	str chars = "";
	if (To < From) {
		if (!pointTo(0, 0, To)) return "";
		chars += file.get();

		for (uint16 currentChar = To; currentChar < From; ++currentChar) {
			if (file.eof()) break;
			chars = (char)file.get() + chars;
		}
	}
	else {
		if (!pointTo(0, 0, From)) return "";
		chars += file.get();

		for (uint16 currentChar = From; currentChar < To; ++currentChar) {
			if (file.eof()) break;
			chars += file.get();
		}
	}
	return chars;
}
str File::getChars(uint16 Line, uint16 From, uint16 To) {
	str chars = "";
	if (To < From) {
		if (!pointTo(Line, 0, To)) return "";
		chars += file.get();

		for (uint16 currentChar = To; currentChar < From; ++currentChar) {
			if (file.eof()) break;
			chars = (char)file.get() + chars;
		}
	}
	else {
		if (!pointTo(Line, 0, From)) return "";
		chars += file.get();

		for (uint16 currentChar = From; currentChar < To; ++currentChar) {
			if (file.eof()) break;
			chars += file.get();
		}
	}
	return chars;
}
str File::getChars(uint16 Line, uint16 Word, uint16 From, uint16 To) {
	str chars = "";
	if (To < From) {
		if (!pointTo(Line, Word, To)) return "";
		chars += file.get();

		for (uint16 currentChar = To; currentChar < From; ++currentChar) {
			if (file.eof()) break;
			chars = (char)file.get() + chars;
		}
	}
	else {
		if (!pointTo(Line, Word, From)) return "";
		chars += file.get();

		for (uint16 currentChar = From; currentChar < To; ++currentChar) {
			if (file.eof()) break;
			chars += file.get();
		}
	}
	return chars;
}

/*
Replaces a line using a temp file
If the specified line is out of bounds some newlines get created
Closes the file before returning, since it has been opened in a different way
Returns false if the files couldn't be opened, otherwise true
*/
bool File::replaceLine(uint16 Line, str Replacement) {
	bool fileEndsWithNewline = 0;
	if (!pointToBeg()) return false;
	file.seekg(-1, std::ios_base::end);
	if (file.get() == '\n') fileEndsWithNewline = 1;

	file.close();
	file.open(path, std::ios::binary | std::ios::in | std::ios::app);
	if (!file.is_open()) return false;
	fstm tempFile(tempPath.c_str(), std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
	if (!tempFile.is_open()) return false;

	str tempStr;
	int16 currentLine = 0;
	truncEndCR(Replacement);
	
	if (!getline(file, tempStr)) {
		tempStr = "";
	}
	else {
		truncEndCR(tempStr);
	}
	if (currentLine++ == Line) tempFile << Replacement;
	else tempFile << tempStr;
	while (1) {
		if (!getline(file, tempStr)) {
			if (currentLine > Line) break;
			tempStr = "";
		}
		else {
			truncEndCR(tempStr);
		}
		if (currentLine++ == Line) tempFile << "\r\n" << Replacement;
		else tempFile << "\r\n" << tempStr;
	}
	if (fileEndsWithNewline) {
		tempFile << "\r\n";
	}

	file.close();
	file.open(path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
	tempFile.seekg(0);

	moveFileContent(tempFile, file);

	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
/*
Replaces a word using a temp file
Closes the file before returning, since it has been opened in a different way
Returns false if the files couldn't be opened or if the
	specified word is out of bounds, otherwise true
*/
bool File::replaceWord(uint16 Word, str Replacement) {
	std::streampos wordPos = getPosition(0, Word, 0);
	if (-1 == wordPos) return false;

	if (file.is_open()) file.close();
	file.open(path, std::ios::binary | std::ios::in | std::ios::app);
	if (!file.is_open()) return false;
	fstm tempFile(tempPath.c_str(), std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
	if (!tempFile.is_open()) {
		file.close();
		return false;
	}

	char tempChar;
	int16 currentChar = 0;

	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			if (currentChar++ == wordPos) {
				if (tempChar == '\n') {
					tempFile << '\n';
				}
				while (!isspace(file.get())) {}
				file.seekg(-1, std::ios::cur);

				for (char letter : Replacement) {
					tempFile << letter;
				}
			}
			else {
				tempFile << tempChar;
			}
		}
		else break;
	}

	file.close();
	file.open(path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
	tempFile.seekg(0);

	moveFileContent(tempFile, file);

	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
/*
Replaces a word in a line using a temp file
Closes the file before returning, since it has been opened in a different way
Returns false if the files couldn't be opened or if the
	specified word is out of bounds, otherwise true
*/
bool File::replaceWord(uint16 Line, uint16 Word, str Replacement) {
	std::streampos wordPos = getPosition(Line, Word, 0);
	if (-1 == wordPos) return false;

	if (file.is_open()) file.close();
	file.open(path, std::ios::binary | std::ios::in | std::ios::app);
	if (!file.is_open()) return false;
	fstm tempFile(tempPath.c_str(), std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
	if (!tempFile.is_open()) {
		file.close();
		return false;
	}

	char tempChar;
	int16 currentChar = 0;

	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			if (currentChar++ == wordPos) {
				if (tempChar == '\n') {
					tempFile << '\n';
				}
				while (!isspace(file.get())) {}
				file.seekg(-1, std::ios::cur);

				for (char letter : Replacement) {
					tempFile << letter;
				}

				tempFile << (char)file.get();
			}
			else {
				tempFile << tempChar;
			}
		}
		else break;
	}

	file.close();
	file.open(path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
	tempFile.seekg(0);

	moveFileContent(tempFile, file);

	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
/*
Replaces a char using a temp file
Closes the file before returning, since it has been opened in a different way
Returns false if the files couldn't be opened or if the
	specified char is out of bounds, otherwise true
*/
bool File::replaceChar(uint16 Char, char Replacement) {
	std::streampos charPos = getPosition(0, 0, Char);
	if (-1 == charPos) return false;

	if (file.is_open()) file.close();
	file.open(path, std::ios::binary | std::ios::in | std::ios::app);
	if (!file.is_open()) return false;
	fstm tempFile(tempPath.c_str(), std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
	if (!tempFile.is_open()) {
		file.close();
		return false;
	}

	char tempChar;
	int16 currentChar = 0;

	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			if (currentChar++ == charPos) {
				tempFile << Replacement;
			}
			else {
				tempFile << tempChar;
			}
		}
		else break;
	}

	file.close();
	file.open(path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
	tempFile.seekg(0);

	moveFileContent(tempFile, file);

	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::replaceChar(uint16 Line, uint16 Char, char Replacement) {
	return false;
}
bool File::replaceChar(uint16 Line, uint16 Word, uint16 Char, char Replacement) {
	return false;
}



str File::string() {
	pointToBeg();

	str fileStr = "";
	while (!file.eof()) {
		fileStr += file.get();
	}

	return fileStr;
}
