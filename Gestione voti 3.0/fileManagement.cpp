//FARE forse si puo' ottimizzare la scrittura su file scrivendo una stringa sola invece che due, es: "file << (str + '\n');" invece che "file << str << '\n';"

#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>

#include "fileManagement.h"

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

bool File::checkOpen() {
	file.open(path, std::ios_base::binary | std::ios_base::in | std::ios_base::out);
	if (file.is_open()) return true;
	return false;
}

bool File::pointTo(uint16 Line, uint16 Word, uint16 Char) {
	if (!pointToBeg()) return false;

	str tempStr;
	for (uint16 currentLine = 0; currentLine < Line; ++currentLine)	getline(file, tempStr);
	for (uint16 currentWord = 0; currentWord < Word; ++currentWord) file >> tempStr;
	if (Word != 0) {
		while (isspace(file.get()));
		file.seekg(-1, std::ios_base::cur);
	}

	std::streampos position = file.tellg() + (std::streampos)Char;
	file.seekg(0, std::ios_base::end);
	if (position >= file.tellg()) return false;
	
	file.seekg(position);
	file.seekp(position);
	return true;
}
bool File::pointToBeg() {
	if (!file.is_open() && !checkOpen()) return false;
	file.clear(file.eofbit);
	file.seekg(0);
	file.seekp(0);
	return true;
}
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
	if (Word != 0) {
		while (isspace(file.get()));
		file.seekg(-1, std::ios_base::cur);
	}

	std::streampos position = file.tellg() + (std::streampos)Char;
	file.seekg(0, std::ios_base::end);
	if (position >= file.tellg()) {
		position = -1;
	}

	file.clear(file.eof());
	file.seekg(GpointerBeginning);
	file.seekp(PpointerBeginning);

	return position;
}

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
void File::truncEndCR(str & String) {
	while (!String.empty() && String.back() == '\r') {
		String.pop_back();
	}
}
bool File::openTempToModifyFile(fstm & TempFile) {
	if (file.is_open()) file.close();
	file.open(path, std::ios_base::binary | std::ios_base::in | std::ios_base::app);
	if (!file.is_open()) return false;

	//FARE controllare se serve ma penso di si'
	if (TempFile.is_open()) file.close();
	TempFile.open(tempPath.c_str(), std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::app);
	if (!TempFile.is_open()) {
		file.close();
		return false;
	}

	return true;
}
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


File::File() : path(""), tempPath(defaultTempPath) {}
File::File(str Path) : path(Path), tempPath(Path + defaultTempExtension) {}
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

//FARE fattibile senza ricopiare tutto il file
bool File::addLine(uint16 Line, str ToAdd) {
	bool fileEndsWithNewline = false;
	if (!pointToBeg()) return false;
	file.seekg(-1, std::ios_base::end);
	if (file.get() == '\n') fileEndsWithNewline = true;


	fstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	str tempStr;
	int16 currentLine = 0;
	truncEndCR(ToAdd);


	if (getline(file, tempStr)) {
		truncEndCR(tempStr);
	}
	else {
		tempStr = "";
		fileEndsWithNewline = false;
	}
	if (currentLine++ == Line) {
		tempFile << ToAdd << "\r\n" << tempStr;
	}
	else {
		tempFile << tempStr;
	}
	while (1) {
		if (getline(file, tempStr)) {
			truncEndCR(tempStr);
		}
		else {
			if (currentLine > Line) break;
			tempStr = "";
			fileEndsWithNewline = false;
		}
		if (currentLine++ == Line) tempFile << "\r\n" << ToAdd;
		tempFile << "\r\n" << tempStr;
	}
	if (fileEndsWithNewline) {
		tempFile << "\r\n";
	}


	file.close();
	file.open(path.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::addWord(uint16 Word, str ToAdd) {
	std::streampos wordPos = getPosition(0, Word, 0);
	if (-1 == wordPos) return false;


	fstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	char tempChar;
	int16 currentChar = 0;


	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			if (currentChar++ == wordPos) {
				for (char letter : ToAdd) {
					tempFile << letter;
				}
				tempFile << ' ' << tempChar;
			}
			else {
				tempFile << tempChar;
			}
		}
		else break;
	}


	file.close();
	file.open(path.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::addWord(uint16 Line, uint16 Word, str ToAdd) {
	std::streampos wordPos = getPosition(Line, Word, 0);
	if (-1 == wordPos) return false;


	fstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	char tempChar;
	int16 currentChar = 0;


	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			if (currentChar++ == wordPos) {
				for (char letter : ToAdd) {
					tempFile << letter;
				}
				tempFile << ' ' << tempChar;
			}
			else {
				tempFile << tempChar;
			}
		}
		else break;
	}


	file.close();
	file.open(path.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::addChar(uint16 Char, char ToAdd) {
	uint16 fileLength = getNrChars();


	if (!pointTo(0, 0, Char)) return false;


	char buffer;
	std::streampos pointerPosition = file.tellg();

	//FARE modo migliore ma sto impazzendo
	while (pointerPosition < fileLength) {
		file.seekg(pointerPosition);
		buffer = file.get();
		file.seekp(pointerPosition);
		file.put(ToAdd);

		ToAdd = buffer;
		pointerPosition += 1;
		if (file.eof()) break;
	}
	file.put(ToAdd);


	return true;
}
bool File::addChar(uint16 Line, uint16 Char, char ToAdd) {
	uint16 fileLength = getNrChars();


	if (!pointTo(Line, 0, Char)) return false;


	char buffer;
	std::streampos pointerPosition = file.tellg();


	while (pointerPosition < fileLength) {
		file.seekg(pointerPosition);
		buffer = file.get();
		file.seekp(pointerPosition);
		file.put(ToAdd);

		ToAdd = buffer;
		pointerPosition += 1;
	}
	file.put(ToAdd);


	return true;
}
bool File::addChar(uint16 Line, uint16 Word, uint16 Char, char ToAdd) {
	uint16 fileLength = getNrChars();


	if (!pointTo(Line, Word, Char)) return false;


	char buffer;
	std::streampos pointerPosition = file.tellg();


	while (pointerPosition < fileLength) {
		file.seekg(pointerPosition);
		buffer = file.get();
		file.seekp(pointerPosition);
		file.put(ToAdd);

		ToAdd = buffer;
		pointerPosition += 1;
	}
	file.put(ToAdd);


	return true;
}

bool File::replaceLine(uint16 Line, str Replacement) {
	bool fileEndsWithNewline = 0;
	if (!pointToBeg()) return false;
	file.seekg(-1, std::ios_base::end);
	if (file.get() == '\n') fileEndsWithNewline = 1;


	fstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	str tempStr;
	int16 currentLine = 0;
	truncEndCR(Replacement);
	

	if (getline(file, tempStr)) {
		truncEndCR(tempStr);
	}
	else {
		tempStr = "";
		fileEndsWithNewline = false; //FARE vedere se va bene
	}
	if (currentLine++ == Line) tempFile << Replacement;
	else tempFile << tempStr;
	while (1) {
		if (getline(file, tempStr)) {
			truncEndCR(tempStr);
		}
		else {
			if (currentLine > Line) break;
			tempStr = "";
			fileEndsWithNewline = false; //FARE vedere se va bene
		}
		if (currentLine++ == Line) tempFile << "\r\n" << Replacement;
		else tempFile << "\r\n" << tempStr;
	}
	if (fileEndsWithNewline) {
		tempFile << "\r\n";
	}


	file.close();
	file.open(path.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::replaceWord(uint16 Word, str Replacement) {
	std::streampos wordPos = getPosition(0, Word, 0);
	if (-1 == wordPos) return false;


	fstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	char tempChar;
	int16 currentChar = 0;


	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			if (currentChar++ == wordPos) {
				//if (tempChar == '\n') {
				//	tempFile << '\n';
				//}
				while (!isspace(file.get())) {}
				file.seekg(-1, std::ios_base::cur);

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
	file.open(path.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);

	moveFileContent(tempFile, file);

	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::replaceWord(uint16 Line, uint16 Word, str Replacement) {
	std::streampos wordPos = getPosition(Line, Word, 0);
	if (-1 == wordPos) return false;


	fstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	char tempChar;
	int16 currentChar = 0;

	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			if (currentChar++ == wordPos) {
				while (!isspace(file.get())) {}
				file.seekg(-1, std::ios_base::cur);

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
	file.open(path.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::replaceChar(uint16 Char, char Replacement) {
	if (!pointTo(0, 0, Char)) return false;

	file.put(Replacement);

	return true;
}
bool File::replaceChar(uint16 Line, uint16 Char, char Replacement) {
	if (!pointTo(Line, 0, Char)) return false;

	file.put(Replacement);

	return true;
}
bool File::replaceChar(uint16 Line, uint16 Word, uint16 Char, char Replacement) {
	if (!pointTo(Line, Word, Char)) return false;

	file.put(Replacement);

	return true;
}

bool File::deleteLine(uint16 Line) {
	bool fileEndsWithNewline = 0;
	if (!pointToBeg()) return false;
	file.seekg(-1, std::ios_base::end);
	if (file.get() == '\n') fileEndsWithNewline = 1;


	fstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	bool firstIteration = 1;
	str tempStr;
	int16 currentLine = 0;


	while (1) {
		if (getline(file, tempStr)) {
			truncEndCR(tempStr);
		}
		else break;
		if (currentLine++ != Line) {
			if (firstIteration) {
				tempFile << tempStr;
				firstIteration = 0;
			}
			else {
				tempFile << "\r\n" << tempStr;
			}
		}
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
bool File::deleteWord(uint16 Word) {
	std::streampos wordPos = getPosition(0, Word, 0);
	if (-1 == wordPos) return true;


	fstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	char tempChar;
	int16 currentChar = 0;


	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			if (currentChar++ == wordPos) {
				while (1) {
					tempChar = file.get();
					if (isspace(tempChar)) {
						if (tempChar == '\r') {
							tempFile << '\r';
						}
						break;
					}
				}
			}
			else {
				tempFile << tempChar;
			}
		}
		else break;
	}


	file.close();
	file.open(path.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::deleteWord(uint16 Line, uint16 Word) {
	std::streampos wordPos = getPosition(Line, Word, 0);
	if (-1 == wordPos) return true;


	fstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	char tempChar;
	int16 currentChar = 0;


	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			if (currentChar++ == wordPos) {
				while (1) {
					tempChar = file.get();
					if (isspace(tempChar)) {
						if (tempChar == '\r') {
							tempFile << '\r';
						}
						break;
					}
				}
			}
			else {
				tempFile << tempChar;
			}
		}
		else break;
	}


	file.close();
	file.open(path.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::deleteChar(uint16 Char) {
	std::streampos charPos = getPosition(0, 0, Char);
	if (-1 == charPos) return true;


	fstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	char tempChar;
	int16 currentChar = 0;


	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			if (currentChar++ != charPos) {
				tempFile << tempChar;
			}
		}
		else break;
	}


	file.close();
	file.open(path.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::deleteChar(uint16 Line, uint16 Char) {
	std::streampos charPos = getPosition(Line, 0, Char);
	if (-1 == charPos) return true;


	fstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	char tempChar;
	int16 currentChar = 0;


	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			if (currentChar++ != charPos) {
				tempFile << tempChar;
			}
		}
		else break;
	}


	file.close();
	file.open(path.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::deleteChar(uint16 Line, uint16 Word, uint16 Char) {
	std::streampos charPos = getPosition(Line, Word, Char);
	if (-1 == charPos) return true;


	fstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	char tempChar;
	int16 currentChar = 0;


	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			if (currentChar++ != charPos) {
				tempFile << tempChar;
			}
		}
		else break;
	}


	file.close();
	file.open(path.c_str(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}




//FARE non va
File::operator std::string() {
	if (!pointToBeg()) return "";

	char tempChar;
	str fileStr = "";
	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			fileStr += tempChar;
		}
		else break;
	}

	return fileStr;
}
str File::string() {
	if (!pointToBeg()) return "";

	char tempChar;
	str fileStr = "";
	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			fileStr += tempChar;
		}
		else break;
	}

	return fileStr;
}

