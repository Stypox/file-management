#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>

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


constexpr char defaultTempPath[] = "temp.tmp";
constexpr char defaultTempExtension[] = ".tmp";




FileState::FileState(bool Open, bool Eof, bool Fail, bool Bad, bool TempErr, bool ExtErr) : open(Open), eof(Eof), fail(Fail), bad(Bad), tempErr(TempErr), extErr(ExtErr) {}
FileState::operator bool() {
	return !open || eof || fail || bad || tempErr || extErr;
}
std::string FileState::str() {
	std::string fileState = "open:";
	fileState += (open) ? "1" : "0";
	fileState += " eofErr:";
	fileState += (eof) ? "1" : "0";
	fileState += " failErr:";
	fileState += (fail) ? "1" : "0";
	fileState += " badErr:";
	fileState += (bad) ? "1" : "0";
	fileState += " tempErr:";
	fileState += (tempErr) ? "1" : "0";
	fileState += " extErr:";
	fileState += (extErr) ? "1" : "0";
	return fileState;
}
void FileState::save(File &file) {
	if (!file.isOpen()) file.open();
	file << str();
}




File::FilePosition::FilePosition(File * file, std::streampos Position) : file(file), position(Position) {}
File::FilePosition::operator char() {
	return file->getChar(position);
}
bool File::FilePosition::operator=(char newChar) {
	return file->replaceChar(position, newChar);
}




uint32 File::countWords(Tstr Text) {
	bool wasSpace = 1;
	uint32 nrWords = 0;

	for (char letter : Text) {
		if (isspace(letter)) wasSpace = 1;
		else {
			if (wasSpace) {
				wasSpace = 0;
				++nrWords;
			}
		}
	}

	return nrWords;
}
void File::truncEndCR(Tstr & Text) {
	while (!Text.empty() && Text.back() == '\r') {
		Text.pop_back();
	}
}
bool File::openTempToModifyFile(Tfstm & TempFile) {
	if (file.is_open()) file.close();
	file.open(path, std::ios_base::binary | std::ios_base::in | std::ios_base::app);
	if (!file.is_open()) return false;

	if (TempFile.is_open()) TempFile.close();
	TempFile.open(tempPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::app);
	if (!TempFile.is_open()) {
		TempError = 1;
		file.close();
		return false;
	}

	return true;
}
void File::moveFileContent(Tfstm & From, Tfstm & To) {
	char tempChar;
	while (1) {
		tempChar = From.get();
		if (From.eof()) return;
		To << tempChar;
	}
}


File::File() : path(""), tempPath(defaultTempPath), TempError(0), ExternalError(0) {}
File::File(Tstr Path) : path(Path), tempPath(Path + defaultTempExtension), TempError(0), ExternalError(0) {}
File::File(Tstr Path, Tstr TempPath) : path(Path), tempPath(TempPath), TempError(0), ExternalError(0) {}


bool File::pointTo(std::streampos Position) {
	if (!file.is_open() && !open()) return false;
	file.seekg(Position);
	//FARE vedere se serve file.seekp
	return true;
}
bool File::pointTo(uint32 Line, uint32 Word, uint32 Char) {
	if (!pointToBeg()) return false;


	if (Line != UINT32_MAX) {
		for (uint32 currentLine = 0; currentLine < Line; ++currentLine) {
			while (file.get() != '\n') {
				if (file.eof()) {
					if (++currentLine < Line) return false;
					break;
				}
			}
		}
	}


	if (Word != UINT32_MAX) {
		bool wasSpace = 1;
		uint32 nrWords = 0;
		++Word;

		while (1) {
			if (file.eof()) return false;
			if (nrWords >= Word) {
				file.seekg(-1, std::ios_base::cur);
				break;
			}
			if (isspace(file.get())) wasSpace = 1;
			else {
				if (wasSpace) {
					wasSpace = 0;
					++nrWords;
				}
			}
		}
	}


	if (Char != UINT32_MAX && Char != 0) {
		std::streampos position = file.tellg() + (std::streampos)Char;
		file.seekg(0, std::ios_base::end);
		if (position >= file.tellg()) return false;

		file.seekg(position);
		file.seekp(position);
	}


	return true;
}
bool File::pointToBeg() {
	if (!file.is_open() && !open()) return false;
	file.clear(file.eofbit);
	file.seekg(0);
	//FARE vedere se serve file.seekp
	return true;
}
bool File::pointToEnd() {
	if (!file.is_open() && !open()) return false;
	file.clear(file.eofbit);
	file.seekg(0, std::ios_base::end);
	//FARE vedere se serve file.seekp
	return true;
}
std::streampos File::getPosition(uint32 Line, uint32 Word, uint32 Char) {
	std::streampos GpointerBeginning = 0, PpointerBeginning = 0;
	if (file.is_open()) {
		GpointerBeginning = file.tellg();
		PpointerBeginning = file.tellp();
	}
	if (!pointToBeg()) return -1;


	if (Line != UINT32_MAX) {
		for (uint32 currentLine = 0; currentLine < Line; ++currentLine) {
			while (file.get() != '\n') {
				if (file.eof()) {
					if (++currentLine < Line) return -2;
					break;
				}
			}
		}
	}


	if (Word != UINT32_MAX) {
		bool wasSpace = 1;
		uint32 nrWords = 0;
		++Word;

		while (1) {
			if (file.eof()) return -2;
			if (nrWords >= Word) {
				file.seekg(-1, std::ios_base::cur);
				break;
			}
			if (isspace(file.get())) wasSpace = 1;
			else {
				if (wasSpace) {
					wasSpace = 0;
					++nrWords;
				}
			}
		}
	}


	std::streampos position = file.tellg();
	if (Char != UINT32_MAX && Char != 0) {
		position += (std::streampos)Char;
		file.seekg(0, std::ios_base::end);
		if (position >= file.tellg()) {
			position = -2;
		}
	}


	file.clear(file.eof());
	file.seekg(GpointerBeginning);
	file.seekp(PpointerBeginning);


	return position;
}


uint32 File::getNrLines() {
	if (!pointToBeg()) return 0;

	uint32 lines = 0;
	Tstr tempStr;
	while (getline(file, tempStr)) lines++;

	return lines;
}
uint32 File::getNrWords() {
	if (!pointToBeg()) return 0;

	uint32 words = 0;
	Tstr tempStr;
	while (file >> tempStr) words++;

	return words;
}
uint32 File::getNrWords(uint32 Line) {
	if (!pointTo(Line, 0, 0)) return 0;

	Tstr tempStr;
	getline(file, tempStr);

	return countWords(tempStr);
}
uint32 File::getNrChars() {
	struct stat buffer;
	if (::stat(path.c_str(), &buffer) != 0) return 0;

	return (uint32)buffer.st_size;
}
uint32 File::getNrChars(uint32 Line) {
	if (!pointTo(Line, 0, 0)) return 0;

	Tstr tempStr;
	getline(file, tempStr);

	truncEndCR(tempStr);
	return (uint32)tempStr.length();
}
uint32 File::getNrChars(uint32 Line, uint32 Word) {
	if (!pointTo(Line, Word, 0)) return 0;

	Tstr tempStr;
	file >> tempStr;

	return (uint32)tempStr.length();
}


Tstr File::getLine(uint32 Line) {
	if (!pointTo(Line, -1, -1)) return "";

	Tstr line;
	getline(file, line);
	truncEndCR(line);

	return line;
}
Tstr File::getWord(uint32 Word) {
	if (!pointTo(-1, Word, -1)) return "";

	Tstr word;
	file >> word;

	return word;
}
Tstr File::getWord(uint32 Line, uint32 Word) {
	if (!pointTo(Line, Word, -1)) return "";

	Tstr word;
	file >> word;

	return word;
}
char File::getChar(uint32 Char) {
	if (!pointTo(-1, -1, Char)) return 0;

	return file.get();
}
char File::getChar(uint32 Line, uint32 Char) {
	if (!pointTo(Line, -1, Char)) return 0;

	return file.get();
}
char File::getChar(uint32 Line, uint32 Word, uint32 Char) {
	if (!pointTo(Line, Word, Char)) return 0;

	return file.get();
}


Tstr File::getLines(uint32 From, uint32 To) {
	if (To < From) {
		if (!pointTo(To, -1, -1)) return "";

		Tstr lines;
		getline(file, lines);
		truncEndCR(lines);

		Tstr tempStr;
		for (uint32 currentLine = To; currentLine < From; ++currentLine) {
			if (!getline(file, tempStr)) break;
			truncEndCR(tempStr);
			lines = tempStr + "\r\n" + lines;
		}

		return lines;
	}
	else {
		if (!pointTo(From, -1, -1)) return "";

		Tstr lines;
		getline(file, lines);
		truncEndCR(lines);

		Tstr tempStr;
		for (uint32 currentLine = From; currentLine < To; ++currentLine) {
			if (!getline(file, tempStr)) break;
			truncEndCR(tempStr);
			lines += "\r\n" + tempStr;
		}

		return lines;
	}
}
Tstr File::getWords(uint32 From, uint32 To) {
	Tstr words;
	if (To < From) {
		if (!pointTo(-1, To, -1)) return "";
		file >> words;

		Tstr tempStr;
		for (uint32 currentWord = To; currentWord < From; ++currentWord) {
			file >> tempStr;
			words = tempStr + " " + words;
		}
	}
	else {
		if (!pointTo(-1, From, -1)) return "";
		file >> words;

		Tstr tempStr;
		for (uint32 currentWord = From; currentWord < To; ++currentWord) {
			file >> tempStr;
			words += " " + tempStr;
		}
	}
	return words;
}
Tstr File::getWords(uint32 Line, uint32 From, uint32 To) {
	Tstr words;
	if (To < From) {
		if (!pointTo(Line, To, -1)) return "";
		file >> words;

		Tstr tempStr;
		for (uint32 currentWord = To; currentWord < From; ++currentWord) {
			file >> tempStr;
			words = tempStr + " " + words;
		}
	}
	else {
		if (!pointTo(Line, From, -1)) return "";
		file >> words;

		Tstr tempStr;
		for (uint32 currentWord = From; currentWord < To; ++currentWord) {
			file >> tempStr;
			words += " " + tempStr;
		}
	}
	return words;
}
Tstr File::getChars(uint32 From, uint32 To) {
	Tstr chars = "";
	if (To < From) {
		if (!pointTo(-1, -1, To)) return "";
		chars += file.get();

		for (uint32 currentChar = To; currentChar < From; ++currentChar) {
			if (file.eof()) break;
			chars = (char)file.get() + chars;
		}
	}
	else {
		if (!pointTo(-1, -1, From)) return "";
		chars += file.get();

		for (uint32 currentChar = From; currentChar < To; ++currentChar) {
			if (file.eof()) break;
			chars += file.get();
		}
	}
	return chars;
}
Tstr File::getChars(uint32 Line, uint32 From, uint32 To) {
	Tstr chars = "";
	if (To < From) {
		if (!pointTo(Line, -1, To)) return "";
		chars += file.get();

		for (uint32 currentChar = To; currentChar < From; ++currentChar) {
			if (file.eof()) break;
			chars = (char)file.get() + chars;
		}
	}
	else {
		if (!pointTo(Line, -1, From)) return "";
		chars += file.get();

		for (uint32 currentChar = From; currentChar < To; ++currentChar) {
			if (file.eof()) break;
			chars += file.get();
		}
	}
	return chars;
}
Tstr File::getChars(uint32 Line, uint32 Word, uint32 From, uint32 To) {
	Tstr chars = "";
	if (To < From) {
		if (!pointTo(Line, Word, To)) return "";
		chars += file.get();

		for (uint32 currentChar = To; currentChar < From; ++currentChar) {
			if (file.eof()) break;
			chars = (char)file.get() + chars;
		}
	}
	else {
		if (!pointTo(Line, Word, From)) return "";
		chars += file.get();

		for (uint32 currentChar = From; currentChar < To; ++currentChar) {
			if (file.eof()) break;
			chars += file.get();
		}
	}
	return chars;
}


bool File::addLine(uint32 Line, Tstr ToAdd) {
	uint32 fileLength = getNrChars();


	if (!pointTo(Line, -1, -1)) return false;
	if (file.tellg() > 0) {
		file.seekg(-1, std::ios_base::cur);
	}


	while (1) {
		if (file.tellg() <= (std::streampos)0) break;
		if (file.get() == '\n') break;
		else {
			file.seekg(-2, std::ios_base::cur);
		}
	}
			
	
	ToAdd += (Tstr)"\r\n";


	for (std::streampos pointerPosition = file.tellg(); pointerPosition < fileLength; pointerPosition += 1) {
		file.seekg(pointerPosition);
		ToAdd.push_back(file.get());

		file.seekp(pointerPosition);
		file.put(ToAdd[0]);
		ToAdd.erase(0, 1);
	}


	pointToEnd();
	file << ToAdd;


	return true;
}
bool File::addWord(uint32 Word, Tstr ToAdd) {
	uint32 fileLength = getNrChars();


	if (!pointTo(-1, Word, -1)) return false;
	if (file.tellg() > 0) {
		file.seekg(-1, std::ios_base::cur);
	}


	while (1) {
		if (file.tellg() <= (std::streampos)0) {
			ToAdd += (Tstr)" ";
			break;
		}
		if (isspace(file.get())) {
			file.seekg(-2, std::ios_base::cur);
		}
		else {
			ToAdd = (Tstr)" " + ToAdd;
			break;
		}
	}


	for (std::streampos pointerPosition = file.tellg(); pointerPosition < fileLength; pointerPosition += 1) {
		file.seekg(pointerPosition);
		ToAdd.push_back(file.get());

		file.seekp(pointerPosition);
		file.put(ToAdd[0]);
		ToAdd.erase(0, 1);
	}


	pointToEnd();
	file << ToAdd;


	return true;
}
bool File::addWord(uint32 Line, uint32 Word, Tstr ToAdd) {
	uint32 fileLength = getNrChars();


	if (!pointTo(Line, Word, -1)) return false;
	if (file.tellg() > 0) {
		file.seekg(-1, std::ios_base::cur);
	}


	while (1) {
		if (file.tellg() <= (std::streampos)0) {
			ToAdd += (Tstr)" ";
			break;
		}
		if (isspace(file.get())) {
			file.seekg(-2, std::ios_base::cur);
		}
		else {
			ToAdd = (Tstr)" " + ToAdd;
			break;
		}
	}


	for (std::streampos pointerPosition = file.tellg(); pointerPosition < fileLength; pointerPosition += 1) {
		file.seekg(pointerPosition);
		ToAdd.push_back(file.get());

		file.seekp(pointerPosition);
		file.put(ToAdd[0]);
		ToAdd.erase(0, 1);
	}


	pointToEnd();
	file << ToAdd;


	return true;
}
bool File::addChar(uint32 Char, char ToAdd) {
	uint32 fileLength = getNrChars();


	if (!pointTo(-1, -1, Char)) return false;


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
	}
	file.put(ToAdd);


	return true;
}
bool File::addChar(uint32 Line, uint32 Char, char ToAdd) {
	uint32 fileLength = getNrChars();


	if (!pointTo(Line, -1, Char)) return false;


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
bool File::addChar(uint32 Line, uint32 Word, uint32 Char, char ToAdd) {
	uint32 fileLength = getNrChars();


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


bool File::replaceLine(uint32 Line, Tstr Replacement) {
	bool fileEndsWithNewline = 0;
	if (!pointToBeg()) return false;
	file.seekg(-1, std::ios_base::end);
	if (file.get() == '\n') fileEndsWithNewline = 1;


	Tfstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	Tstr tempStr;
	uint32 currentLine = 0;
	

	if (getline(file, tempStr)) {
		truncEndCR(tempStr);
	}
	else {
		tempStr = "";
		fileEndsWithNewline = false;
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
			fileEndsWithNewline = false;
		}
		if (currentLine++ == Line) tempFile << "\r\n" << Replacement;
		else tempFile << "\r\n" << tempStr;
	}
	if (fileEndsWithNewline) {
		tempFile << "\r\n";
	}


	file.close();
	file.open( path, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::replaceWord(uint32 Word, Tstr Replacement) {
	std::streampos wordPos = getPosition(-1, Word, -1);
	if (-1 == wordPos || -2 == wordPos) return false;


	Tfstm tempFile;
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
			}
			else {
				tempFile << tempChar;
			}
		}
		else break;
	}

	file.close();
	file.open( path, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);

	moveFileContent(tempFile, file);

	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::replaceWord(uint32 Line, uint32 Word, Tstr Replacement) {
	std::streampos wordPos = getPosition(Line, Word, -1);
	if (-1 == wordPos || -2 == wordPos) return false;



	Tfstm tempFile;
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
	file.open(path, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::replaceChar(uint32 Char, char Replacement) {
	if (!pointTo(-1, -1, Char)) return false;

	file.put(Replacement);

	return true;
}
bool File::replaceChar(uint32 Line, uint32 Char, char Replacement) {
	if (!pointTo(Line, -1, Char)) return false;

	file.put(Replacement);

	return true;
}
bool File::replaceChar(uint32 Line, uint32 Word, uint32 Char, char Replacement) {
	if (!pointTo(Line, Word, Char)) return false;

	file.put(Replacement);

	return true;
}


bool File::deleteLine(uint32 Line) {
	bool fileEndsWithNewline = 0;
	if (!pointToBeg()) return false;
	file.seekg(-1, std::ios_base::end);
	if (file.get() == '\n') fileEndsWithNewline = 1;


	Tfstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	bool firstIteration = 1;
	Tstr tempStr;
	uint32 currentLine = 0;


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
	if (fileEndsWithNewline && currentLine != Line) {
		tempFile << "\r\n";
	}


	file.close();
	file.open(path, std::ios::binary | std::ios::out | std::ios::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::deleteWord(uint32 Word) {
	std::streampos wordPos = getPosition(-1, Word, -1);
	if (-1 == wordPos) return false;
	else if (-2 == wordPos) return true;


	file.seekg(wordPos);
	bool atEndLine = 0;
	char tempChar;
	while (1) {
		tempChar = file.get();
		if (file.eof()) {
			atEndLine = 1;
			if (0 != wordPos) {
				file.seekg(wordPos - (std::streampos)1);
				if (file.get() != '\n') wordPos -= 1;
			}
			break;
		}
		if (isspace(tempChar)) {
			if (tempChar == '\r') {
				atEndLine = 1;
				if (0 != wordPos) {
					file.seekg(wordPos - (std::streampos)1);
					if (file.get() != '\n') wordPos -= 1;
				}
			}
			break;
		}
	}
	

	Tfstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	uint32 currentChar = 0;
	if (atEndLine) {
		while (1) {
			tempChar = file.get();
			if (file.eof()) break;
			if (currentChar++ == wordPos) {
				file.get();
				while (1) {
					if (isspace(file.get())) {
						tempFile << '\r';
						break;
					}
					if (file.eof()) break;
				}
				if (file.eof()) break;
			}
			else {
				tempFile << tempChar;
			}
		}
	}
	else {
		while (1) {
			tempChar = file.get();
			if (file.eof()) break;
			if (currentChar++ == wordPos) {
				while (1) {
					if (isspace(file.get()) || file.eof()) break;
				}
				if (file.eof()) break;
			}
			else {
				tempFile << tempChar;
			}
		}
	}

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
	file.open( path, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::deleteWord(uint32 Line, uint32 Word) {
	std::streampos wordPos = getPosition(Line, Word, -1);
	if (-1 == wordPos) return false;
	else if (-2 == wordPos) return true;


	file.seekg(wordPos);
	bool atEndLine = 0;
	char tempChar;
	while (1) {
		tempChar = file.get();
		if (file.eof()) {
			atEndLine = 1;
			if (0 != wordPos) {
				file.seekg(wordPos - (std::streampos)1);
				if (file.get() != '\n') wordPos -= 1;
			}
			break;
		}
		if (isspace(tempChar)) {
			if (tempChar == '\r') {
				atEndLine = 1;
				if (0 != wordPos) {
					file.seekg(wordPos - (std::streampos)1);
					if (file.get() != '\n') wordPos -= 1;
				}
			}
			break;
		}
	}


	Tfstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	uint32 currentChar = 0;
	if (atEndLine) {
		while (1) {
			tempChar = file.get();
			if (file.eof()) break;
			if (currentChar++ == wordPos) {
				file.get();
				while (1) {
					if (isspace(file.get())) {
						tempFile << '\r';
						break;
					}
					if (file.eof()) break;
				}
				if (file.eof()) break;
			}
			else {
				tempFile << tempChar;
			}
		}
	}
	else {
		while (1) {
			tempChar = file.get();
			if (file.eof()) break;
			if (currentChar++ == wordPos) {
				while (1) {
					if (isspace(file.get()) || file.eof()) break;
				}
				if (file.eof()) break;
			}
			else {
				tempFile << tempChar;
			}
		}
	}

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
	file.open( path, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::deleteChar(uint32 Char) {
	std::streampos charPos = getPosition(-1, -1, Char);
	if (-1 == charPos) return false;
	else if (-2 == charPos) return true;


	Tfstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	char tempChar;
	uint32 currentChar = 0;


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
	file.open( path, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::deleteChar(uint32 Line, uint32 Char) {
	std::streampos charPos = getPosition(Line, -1, Char);
	if (-1 == charPos) return false;
	else if (-2 == charPos) return true;


	Tfstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	char tempChar;
	uint32 currentChar = 0;


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
	file.open(path, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}
bool File::deleteChar(uint32 Line, uint32 Word, uint32 Char) {
	std::streampos charPos = getPosition(Line, Word, Char);
	if (-1 == charPos) return false;
	else if (-2 == charPos) return true;


	Tfstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;


	char tempChar;
	uint32 currentChar = 0;


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
	file.open( path, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}


bool File::appendLine(Tstr ToAppend) {
	if (!pointToEnd()) return false;

	file << "\r\n" << ToAppend;

	return true;
}
bool File::appendWord(Tstr ToAppend) {
	if (!pointToEnd()) return false;

	file << ' ' << ToAppend;

	return true;
}
bool File::appendWord(uint32 Line, Tstr ToAppend) {
	uint32 fileLength = getNrChars();


	if (!pointTo(Line, -1, -1)) return false;


	while (1) {
		if (file.get() == '\r') {
			file.seekg(-1, std::ios_base::cur);
			break;
		}
		if (file.eof()) {
			break;
		}
	}


	ToAppend = (Tstr)" " + ToAppend;


	for (std::streampos pointerPosition = file.tellg(); pointerPosition < fileLength; pointerPosition += 1) {
		file.seekg(pointerPosition);
		ToAppend.push_back(file.get());

		file.seekp(pointerPosition);
		file.put(ToAppend[0]);
		ToAppend.erase(0, 1);
	}


	pointToEnd();
	file << ToAppend;


	return true;
}
bool File::appendChar(char ToAppend) {
	if (!pointToEnd()) return false;

	file << ToAppend;

	return true;
}
bool File::appendChar(uint32 Line, char ToAppend) {
	uint32 fileLength = getNrChars();


	if (!pointTo(Line, -1, -1)) return false;
	while (1) {
		if (file.get() == '\r') {
			file.seekg(-1, std::ios_base::cur);
			break;
		}
		if (file.eof()) {
			pointToEnd();
			break;
		}
	}


	char buffer;
	std::streampos pointerPosition = file.tellg();


	while (pointerPosition < fileLength) {
		file.seekg(pointerPosition);
		buffer = file.get();
		file.seekp(pointerPosition);
		file.put(ToAppend);

		ToAppend = buffer;
		pointerPosition += 1;
	}
	file.put(ToAppend);

	return true;
}
bool File::appendChar(uint32 Line, uint32 Word, char ToAppend) {
	uint32 fileLength = getNrChars();


	if (!pointTo(Line, Word, -1)) return false;
	while (1) {
		if (!isspace(file.get())) {
			file.seekg(-1, std::ios_base::cur);
			break;
		}
		if (file.eof()) {
			pointToEnd();
			break;
		}
	}


	char buffer;
	std::streampos pointerPosition = file.tellg();


	while (pointerPosition < fileLength) {
		file.seekg(pointerPosition);
		buffer = file.get();
		file.seekp(pointerPosition);
		file.put(ToAppend);

		ToAppend = buffer;
		pointerPosition += 1;
	}
	file.put(ToAppend);

	return true;
}


bool File::deleteLastEmptyLines() {
	uint32 fileLength = getNrChars();


	Tfstm tempFile;
	if (!openTempToModifyFile(tempFile)) return false;

	
	uint32 endPosition = 0;
	char tempChar;


	for (uint32 pointer = 0; pointer < fileLength;) {
		tempChar = file.get();
		if (tempChar != '\n' && tempChar != '\r') {
			endPosition = ++pointer;
		}
		else {
			++pointer;
		}
		tempFile << tempChar;
	}


	file.close();
	file.open( path, std::ios::binary | std::ios::out | std::ios::trunc);
	tempFile.seekg(0);


	for (uint32 pointer = 0; pointer < endPosition; ++pointer) {
		file << (char)tempFile.get();
	}


	file.close();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}


bool File::create() {
	file.open(path, std::ios_base::app);
	file.close();
	if (open()) return true;
	return false;
}
bool File::move(Tstr newPath) {
	if (file.is_open()) file.close();
	file.open(newPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
	if (!file.is_open()) return false;
	Tfstm oldFile(path, std::ios_base::binary | std::ios_base::in | std::ios_base::out);
	if (!oldFile.is_open()) {
		file.close();
		return false;
	}

	moveFileContent(oldFile, file);

	oldFile.close();

	std::remove(path.c_str());
	path = newPath;
	return true;
}
bool File::move(File & toOverwrite) {
	if (!toOverwrite.truncate()) {
		ExternalError = 1;
		return false;
	}
	if (!pointToBeg()) return false;

	char tempChar;
	while (1) {
		tempChar = file.get();
		if (file.eof()) break;
		toOverwrite << tempChar;
	}

	return true;
}
bool File::swap(File & Other) {
	if (!pointToBeg()) return false;
	if (!Other.pointToBeg()) {
		ExternalError = 1;
		return false;
	}
	Tfstm tempFile(tempPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::app); //FARE potrebbe essere che il file esista gia'
	if (!tempFile.is_open()) {
		TempError = 1;
		return false;
	}

	moveFileContent(file, tempFile);

	truncate();
	moveFileContent(Other.file, file);

	Other.truncate();
	tempFile.clear(tempFile.eofbit);
	tempFile.seekg(0);
	moveFileContent(tempFile, Other.file);
	return true;
}
bool File::rename(Tstr newName) {
	Tstr newPath = path;
	while (!newPath.empty()) {
		if (newPath.back() == '/') break;
		newPath.pop_back();
	}
	newPath += newName;
	return move(newPath);
}
bool File::truncate() {
	if (file.is_open()) file.close();
	file.open(path, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
	if (file.is_open()) return true;
	return false;
}
void File::remove() {
	if (file.is_open()) file.close();
	std::remove(path.c_str());
}


bool File::open() {
	file.open(path, std::ios_base::binary | std::ios_base::in | std::ios_base::out);
	if (file.is_open()) return true;
	return false;
}
void File::close() {
	if (file.is_open()) file.close();
	clear();
}
bool File::isOpen() const {
	return file.is_open();
}


bool File::exists() const {
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}
struct stat File::info() {
	struct stat buffer;
	stat(path.c_str(), &buffer);
	return buffer;
}


//da testare FARE
bool File::good() const {
	return file.good() && !TempError && !ExternalError;
}
void File::clear() {
	file.clear();
	TempError = 0;
	ExternalError = 0;
}
bool File::eofErr() const {
	return file.eof();
}
void File::eofErr(bool Value) {
	if (Value) {
		file.setstate(file.eofbit);
	}
	else {
		file.clear(file.eofbit);
	}
}
bool File::failErr() const {
	return file.fail();
}
void File::failErr(bool Value) {
	if (Value) {
		file.setstate(file.failbit);
	}
	else {
		file.clear(file.failbit);
	}
}
bool File::badErr() const {
	return file.bad();
}
void File::badErr(bool Value) {
	if (Value) {
		file.setstate(file.badbit);
	}
	else {
		file.clear(file.eofbit);
	}
}
bool File::tempErr() const {
	return TempError;
}
void File::tempErr(bool Value) {
	TempError = Value;
}
bool File::extErr() const {
	return ExternalError;
}
void File::extErr(bool Value) {
	ExternalError = Value;
}
FileState File::state() const {
	return FileState(file.is_open(), file.eof(), file.fail(), file.bad(), TempError, ExternalError);
}


Tstr File::getPath() const {
	return path;
}
void File::setPath(Tstr Path) {
	if (file.is_open()) file.close();
	path = Path;
}
Tstr File::getTempPath() const {
	return tempPath;
}
void File::setTempPath(Tstr TempPath) {
	tempPath = TempPath;
}


File& File::operator>>(File &Out) {
	if (!pointToBeg()) return *this;
	if (Out.path == path || !Out.pointToBeg()) {
		ExternalError = 1;
		//Out.ExternalError = 1; //FARE se metterlo o no, perche' Out ha gia' il suo errore contenuto nel failbit del suo file e l'errore non e' esterno ma interno
		return *this;
	}
	Tfstm tempFile(tempPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::app);
	if (!tempFile.is_open()) {
		TempError = 1;
		return *this;
	}

	moveFileContent(Out.file, tempFile);

	Out.truncate();
	moveFileContent(file, Out.file);

	tempFile.seekg(0);
	moveFileContent(tempFile, Out.file);

	tempFile.close();
	std::remove(tempPath.c_str());
	return *this;
}
File& File::operator>>(Tfstm &Out) {
	if (!pointToBeg()) return *this;
	if (!Out.is_open()) {
		ExternalError = 1;
		return *this;
	}
	Out.clear(Out.eofbit);
	Out.seekg(0);
	Tfstm tempFile(tempPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::app);
	if (!tempFile.is_open()) {
		TempError = 1;
		return *this;
	}

	moveFileContent(Out, tempFile);
	
	Out.clear(Out.eofbit);
	Out.seekg(0);
	char tempChar;
	while (1) {
		tempChar = file.get();
		if (file.eof()) break;
		Out << tempChar;
	}

	tempFile.seekg(0);
	while (1) {
		tempChar = tempFile.get();
		if (tempFile.eof()) break;
		Out << tempChar;
	}

	tempFile.close();
	std::remove(tempPath.c_str());
	return *this;
}
File& File::operator>>(Tstr &Out) {
	file >> Out;
	return *this;
}
File& File::operator>>(char * &Out) {
	file >> Out;
	return *this;
}
File& File::operator>>(char &Out) {
	file >> Out;
	return *this;
}
File& File::operator>>(int64 &Out) {
	file >> Out;
	return *this;
}
File& File::operator>>(double &Out) {
	file >> Out;
	return *this;
}


File& File::operator<<(File &In) {
	if (!pointToEnd()) return *this;
	if (In.path == path || !In.pointToBeg()) {
		ExternalError = 1;
		//In.ExternalError = 1;
		return *this;
	}

	moveFileContent(In.file, file);
	return *this;
}
File& File::operator<<(Tfstm &In) {
	if (!pointToEnd()) return *this;
	if (!In.is_open()) {
		ExternalError = 1;
		return *this;
	}
	In.clear(In.eofbit);
	In.seekg(0);

	moveFileContent(In, file);

	return *this;
}
File& File::operator<<(Tstr In) {
	file << In;
	return *this;
}
File& File::operator<<(const char * In) {
	file << In;
	return *this;
}
File& File::operator<<(char In) {
	file << In;
	return *this;
}
File& File::operator<<(int64 In) {
	file << In;
	return *this;
}
File& File::operator<<(double In) {
	file << In;
	return *this;
}


File& File::operator=(File &toCopy) {
	close();
	path = toCopy.path;
	tempPath = toCopy.tempPath;

	if (toCopy.isOpen()) {
		open();

		eofErr(toCopy.eofErr());
		failErr(toCopy.failErr());
		badErr(toCopy.badErr());
		tempErr(toCopy.tempErr());
		extErr(toCopy.extErr());

		file.seekg(toCopy.file.tellg());
	}
	else {
		eofErr(toCopy.eofErr());
		failErr(toCopy.failErr());
		badErr(toCopy.badErr());
		tempErr(toCopy.tempErr());
		extErr(toCopy.extErr());
	}

	return *this;
}
bool File::operator=(Tstr newText) {
	if (!truncate()) return false;
	
	file << newText;

	return true;
}
Tstr File::operator+(File &toAdd) {
	return str() + toAdd.str();
}
Tstr File::operator+(Tstr toAdd) {
	return str() + toAdd;
}
File& File::operator+=(File &toAppend) {
	if (!pointToEnd()) return *this;
	if (!toAppend.pointToBeg()) {
		ExternalError = 1;
		return *this;
	}
	moveFileContent(toAppend.file, file);
	return *this;
}
File& File::operator+=(Tstr toAppend) {
	if (!pointToEnd()) return *this;
	file << toAppend;
	return *this;
}


File::FilePosition File::operator[](std::streampos Position) {
	return File::FilePosition(this, Position);
}
bool File::operator==(File &toCompare) {
	if (!pointToBeg()) return false;
	if (!toCompare.pointToBeg()) return false;

	char tempCharThis, tempCharCompare;
	while (1) {
		tempCharThis = file.get();
		tempCharCompare = toCompare.file.get();

		if (file.eof()) {
			if (toCompare.file.eof()) break;
			else return false;
		}

		if (tempCharThis != tempCharCompare) return false;
	}
	return true;
}
bool File::operator!=(File &toCompare) {
	return !operator==(toCompare);
}


//FARE non va
File::operator Tstr() {
	if (!pointToBeg()) return "";

	char tempChar;
	Tstr fileStr = "";
	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			fileStr += tempChar;
		}
		else break;
	}

	return fileStr;
}
Tstr File::str() {
	if (!pointToBeg()) return "";

	char tempChar;
	Tstr fileStr = "";
	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			fileStr += tempChar;
		}
		else break;
	}

	return fileStr;
}


File::operator bool() {
	return !(file.eof() || file.fail() || file.bad() || TempError || ExternalError);
}
bool File::operator!() {
	return file.eof() || file.fail() || file.bad() || TempError || ExternalError;
}