#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>

#include "fileManagement.h"

using namespace sp;


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




FilePosition::FilePosition(File * file, uint32_t Position) : file(file), position(Position) {}
FilePosition::operator char() {
	return file->getChar(position);
}
bool FilePosition::operator=(char newChar) {
	return file->replaceChar(position, newChar);
}




FileIterator::FileIterator(File * file, uint32_t Position) : file(file), position(Position) {}
bool FileIterator::operator!=(const FileIterator & ToCompare) {
	return position != ToCompare.position;
}
FilePosition FileIterator::operator*() const {
	return file->operator[](position);
}
void FileIterator::operator++() {
	++position;
}






Tstr File::toString(const char * toConvert) {
	return Tstr(toConvert);
}
Tstr File::toString(char toConvert) {
	return Tstr(&toConvert);
}
Tstr File::toString(Tstr & toConvert) {
	return toConvert;
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
bool File::openTempToEditMain(Tfstm & TempFile) {
	if (file.is_open()) {
		file.close();
	}
	file.open(path, std::ios_base::binary | std::ios_base::in);
	if (!file.is_open()) return false;


	struct stat buffer;
	if (stat(tempPath.c_str(), &buffer) == 0) {
		TempFile.open(tempPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
	}
	else {
		TempFile.open(tempPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::app);
	}
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
Tspos File::getPositionMove(uint32 Line, uint32 Word, uint32 Char) {
	if (!pointToBeg()) return -1;


	if (Line != UINT32_MAX) {
		for (uint32 currentLine = 0; currentLine < Line; ++currentLine) {
			while (file.get() != '\n') {
				if (file.eof()) return -2;
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


	Tspos position = file.tellg();
	if (Char != UINT32_MAX && Char != 0) {
		position += (Tspos)Char;
		if (position >= getNrChars()) {
			position = -2;
		}
	}


	return position;
}


bool File::replaceSection(Tspos From, Tspos To, Tstr Replacement) {
	uint32 oldSize = (uint32)To - (uint32)From + 1,
		newSize = Replacement.size();

	
	if (oldSize > newSize) {
		Tfstm tempFile;
		if (!openTempToEditMain(tempFile)) return false;

		char tempChar;
		uint32 currentChar = 0;
		while (1) {
			tempChar = file.get();
			if (file.eof()) break;
			if (currentChar >= From && currentChar <= To) {
				tempFile << Replacement;
				file.seekg(1 + (uint32)To);
				currentChar += 1 + (uint32)To;
			}
			else {
				tempFile << tempChar;
				++currentChar;
			}
		}


		file.close();
		file.open(path, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
		tempFile.seekg(0);


		moveFileContent(tempFile, file);


		file.close();
		tempFile.close();
		std::remove(tempPath.c_str());
	}

	
	else if (oldSize < newSize) {
		uint32 fileLength = getNrChars();
		if (!pointTo(From)) return false;

		file << Replacement.substr(0, oldSize);
		Tstr buffer = Replacement.substr(oldSize, newSize - oldSize);

		for (Tspos pointerPosition = oldSize + From; pointerPosition < fileLength; pointerPosition += 1) {
			file.seekg(pointerPosition);
			buffer.push_back(file.get());

			file.seekg(pointerPosition);
			file.put(buffer[0]);
			buffer.erase(0, 1);
		}

		file << buffer;
	}
	

	else {
		if (!pointTo(From)) return false;
		file << Replacement;
	}


	file.flush();
	return true;
}
bool File::deleteSection(Tspos From, Tspos To) {
	Tfstm tempFile;
	if (!openTempToEditMain(tempFile)) return false;


	uint32 currentPosition = 0;
	char tempChar;
	while (1) {
		tempChar = file.get();
		if (file.eof()) break;
		if (currentPosition < From || currentPosition > To) {
			tempFile << tempChar;
		}
		++currentPosition;
	}


	file.close();
	file.open(path, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.flush();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}


File::File() : path(""), tempPath(defaultTempPath), TempError(0), ExternalError(0) {}
File::File(Tstr Path) : path(Path), tempPath(Path + defaultTempExtension), TempError(0), ExternalError(0) {}
File::File(Tstr Path, Tstr TempPath) : path(Path), tempPath(TempPath), TempError(0), ExternalError(0) {}
File::~File() {
	close();
}


bool File::pointTo(Tspos Position) {
	if (!file.is_open() && !open()) return false;
	file.clear(file.eofbit);
	file.seekg(Position);
	return true;
}
bool File::pointTo(uint32 Line, uint32 Word, uint32 Char) {
	if (!pointToBeg()) return false;


	if (Line != UINT32_MAX) {
		for (uint32 currentLine = 0; currentLine < Line; ++currentLine) {
			while (file.get() != '\n') {
				if (file.eof()) return false;
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
		Tspos position = file.tellg() + (Tspos)Char;
		file.seekg(0, std::ios_base::end);
		if (position >= file.tellg()) return false;

		file.seekg(position);
	}


	return true;
}
bool File::pointToBeg() {
	if (!file.is_open() && !open()) return false;
	file.clear(file.eofbit);
	file.seekg(0);
	return true;
}
bool File::pointToEnd() {
	if (!file.is_open() && !open()) return false;
	file.clear(file.eofbit);
	file.seekg(0, std::ios_base::end);
	return true;
}
Tspos File::getPosition(uint32 Line, uint32 Word, uint32 Char) {
	Tspos GpointerBeginning = 0, PpointerBeginning = 0;
	if (file.is_open()) {
		GpointerBeginning = file.tellg();
		PpointerBeginning = file.tellp();
	}
	if (!pointToBeg()) return -1;


	if (Line != UINT32_MAX) {
		for (uint32 currentLine = 0; currentLine < Line; ++currentLine) {
			while (file.get() != '\n') {
				if (file.eof()) return -2;
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


	Tspos position = file.tellg();
	if (Char != UINT32_MAX && Char != 0) {
		position += (Tspos)Char;
		if (position >= getNrChars()) {
			position = -2;
		}
	}


	file.clear(file.eof());
	file.seekg(GpointerBeginning);


	return position;
}


uint32 File::getNrLines() {
	if (!pointToBeg()) return 0;

	char tempChar;
	uint32 lines = 0;
	while (1) {
		tempChar = file.get();
		if (file.eof()) break;
		if (tempChar == '\n') ++lines;
	}

	return lines;
}
uint32 File::getNrWords() {
	return countWords(str());
}
uint32 File::getNrWords(uint32 Line) {
	if (!pointTo(Line, -1, -1)) return 0;

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
	if (!pointTo(Line, -1, -1)) return 0;

	Tstr tempStr;
	getline(file, tempStr);

	truncEndCR(tempStr);
	return (uint32)tempStr.length();
}
uint32 File::getNrChars(uint32 Line, uint32 Word) {
	if (!pointTo(Line, Word, -1)) return 0;

	Tstr tempStr;
	file >> tempStr;

	return (uint32)tempStr.length();
}


File& File::put(char ToPut) {
	file.put(ToPut);
	return *this;
}
Tstr File::getLine() {
	Tstr line;
	getLine(line);
	return line;
}
Tstr File::getWord() {
	Tstr word;
	getWord(word);
	return word;
}
char File::get() {
	return file.get();
}
bool File::getLine(Tstr &Line) {
	Line = "";
	if (!file.is_open() || file.eof()) return false;

	char tempChar;
	while (1) {
		tempChar = file.get();
		if (tempChar == '\r' || file.eof()) break;
		Line += tempChar;
	}
	file.ignore();

	return true;
}
bool File::getWord(Tstr &Word) {
	Word = "";
	if (!file.is_open() || file.eof()) return false;

	char tempChar;
	while (1) {
		tempChar = file.get();
		if (!isspace(tempChar)) {
			file.seekg(-1, std::ios_base::cur);
			break;
		}
	}

	while (1) {
		tempChar = file.get();
		if (isspace(tempChar) || file.eof()) break;
		Word += tempChar;
	}

	return true;
}
bool File::get(char &Char) {
	Char = file.get();
	return !file.eof();
}
bool File::deleteCurrent() {
	uint32 position = (uint32)file.tellg();
	Tfstm tempFile;
	if (!openTempToEditMain(tempFile)) return false;


	uint32 currentPosition = 0;
	char tempChar;
	while (1) {
		tempChar = file.get();
		if (file.eof()) break;
		if (currentPosition != position) {
			tempFile << tempChar;
		}
		++currentPosition;
	}


	file.close();
	file.open(path, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);
	moveFileContent(tempFile, file);


	file.flush();
	file.seekg(position);
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}


Tstr File::getLine(uint32 Line) {
	if (!pointTo(Line, -1, -1)) return "";

	Tstr line;
	getline(file, line);
	truncEndCR(line);

	return line;
}
Tstr File::getWord(uint32 Word) {
	return getWord(-1, Word);
}
Tstr File::getWord(uint32 Line, uint32 Word) {
	if (!pointTo(Line, Word, -1)) return "";

	Tstr word;
	file >> word;

	return word;
}
char File::getChar(uint32 Char) {
	return getChar(-1, -1, Char);
}
char File::getChar(uint32 Line, uint32 Char) {
	return getChar(Line, -1, Char);
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
	return getWords(-1, From, To);
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
	return getChars(-1, -1, From, To);
}
Tstr File::getChars(uint32 Line, uint32 From, uint32 To) {
	return getChars(Line, -1, From, To);
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


bool File::addChar(uint32 Char, char ToAdd) {
	return addChar(-1, -1, Char, ToAdd);
}
bool File::addChar(uint32 Line, uint32 Char, char ToAdd) {
	return addChar(Line, -1, Char, ToAdd);
}
bool File::addChar(uint32 Line, uint32 Word, uint32 Char, char ToAdd) {
	Tspos position = getPositionMove(Line, Word, Char);
	if (position < (Tspos)0) return false;
	return add(position, Tstr(&ToAdd));
}


bool File::replaceChar(uint32 Char, char Replacement) {
	return replaceChar(-1, -1, Char, Replacement);
}
bool File::replaceChar(uint32 Line, uint32 Char, char Replacement) {
	return replaceChar(Line, -1, Char, Replacement);
}
bool File::replaceChar(uint32 Line, uint32 Word, uint32 Char, char Replacement) {
	if (!pointTo(Line, Word, Char)) return false;

	file << Replacement;

	file.flush();
	return true;
}


bool File::deleteLine(uint32 Line) {
	Tspos from, to;
	from = getPositionMove(Line, -1, -1);
	if (from == (Tspos)-1) return false;
	if (from == (Tspos)-2) return true;
	to = getPositionMove(Line + 1, -1, -1);

	if (to == (Tspos)-2) {
		return deleteSection(from - (Tspos)2, getNrChars() - 1);
	}
	return deleteSection(from, to - (Tspos)1);
}
bool File::deleteWord(uint32 Word) {
	return deleteWord(-1, Word);
}
bool File::deleteWord(uint32 Line, uint32 Word) {
	Tspos from, to;
	from = getPositionMove(Line, Word, -1);
	if (from == (Tspos)-1) return false;
	if (from == (Tspos)-2) return true;

	char tempChar;
	while (1) {
		tempChar = file.get();
		if (file.eof()) return deleteSection(from, getNrChars() - 1);
		if (isspace(tempChar)) {
			if (tempChar == '\r') break;
			while (1) {
				tempChar = file.get();
				if (file.eof()) return deleteSection(from, getNrChars() - 1);
				if (!isspace(tempChar) || tempChar == '\r') break;
			}
			break;
		}
	}
	return deleteSection(from, file.tellg() - (Tspos)2);
}
bool File::deleteChar(uint32 Char) {
	return deleteChar(-1, -1, Char);
}
bool File::deleteChar(uint32 Line, uint32 Char) {
	return deleteChar(Line, -1, Char);
}
bool File::deleteChar(uint32 Line, uint32 Word, uint32 Char) {
	Tspos position = getPositionMove(Line, Word, Char);
	if (position == (Tspos)-1) return false;
	else if (position == (Tspos)-2) return true;


	Tfstm tempFile;
	if (!openTempToEditMain(tempFile)) return false;


	uint32 currentPosition = 0;
	char tempChar;
	while (1) {
		tempChar = file.get();
		if (file.eof()) break;
		if (currentPosition != position) {
			tempFile << tempChar;
		}
		++currentPosition;
	}


	file.close();
	file.open(path, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	moveFileContent(tempFile, file);


	file.flush();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}


bool File::deleteLines(uint32 From, uint32 To) {
	if (From > To) std::swap(From, To);
	Tspos from, to;
	from = getPositionMove(From, -1, -1);
	if (from == (Tspos)-1) return false;
	if (from == (Tspos)-2) return true;
	to = getPositionMove(To + 1, -1, -1);

	if (to == (Tspos)-2) {
		return deleteSection(from - (Tspos)2, getNrChars() - 1);
	}
	return deleteSection(from, to - (Tspos)1);
}
bool File::deleteWords(uint32 From, uint32 To) {
	return deleteWords(-1, From, To);
}
bool File::deleteWords(uint32 Line, uint32 From, uint32 To) {
	Tspos from, to;
	from = getPositionMove(Line, From, -1);
	if (from == (Tspos)-1) return false;
	if (from == (Tspos)-2) return true;

	pointTo(Line, To, -1);
	char tempChar;
	while (1) {
		tempChar = file.get();
		if (file.eof()) return deleteSection(from, getNrChars() - 1);
		if (isspace(tempChar)) {
			if (tempChar == '\r') break;
			while (1) {
				tempChar = file.get();
				if (file.eof()) return deleteSection(from, getNrChars() - 1);
				if (!isspace(tempChar) || tempChar == '\r') break;
			}
			break;
		}
	}
	return deleteSection(from, file.tellg() - (Tspos)2);
}
bool File::deleteChars(uint32 From, uint32 To) {
	return deleteChars(-1, -1, From, To);
}
bool File::deleteChars(uint32 Line, uint32 From, uint32 To) {
	return deleteChars(Line, -1, From, To);
}
bool File::deleteChars(uint32 Line, uint32 Word, uint32 From, uint32 To) {
	Tspos from, to;
	from = getPositionMove(Line, Word, From);
	if (from == (Tspos)-1) return false;
	if (from == (Tspos)-2) return true;
	to = getPositionMove(Line, Word, To);

	if (to == (Tspos)-2) {
		return deleteSection(from - (Tspos)2, getNrChars() - 1);
	}
	return deleteSection(from, to);
}


bool File::appendChar(char ToAppend) {
	return append(Tstr(&ToAppend));
}
bool File::appendChar(uint32 Line, char ToAppend) {
	if (!pointTo(Line, -1, -1)) return false;
	
	char tempChar;
	while (1) {
		tempChar = file.get();
		if (file.eof()) return append(Tstr(&ToAppend));
		if (tempChar == '\r') break;
	}

	return add(file.tellg() - (Tspos)1, Tstr(&ToAppend));
}
bool File::appendChar(uint32 Line, uint32 Word, char ToAppend) {
	if (!pointTo(Line, Word, -1)) return false;

	char tempChar;
	while (1) {
		tempChar = file.get();
		if (file.eof()) return append(Tstr(&ToAppend));
		if (isspace(tempChar)) break;
	}

	return add(file.tellg() - (Tspos)1, Tstr(&ToAppend));
}


bool File::deleteLastEmptyLines() {
	Tfstm tempFile;
	if (!openTempToEditMain(tempFile)) return false;

	
	char tempChar;
	uint32 endPosition = 0, pointer = 0;
	while (1) {
		tempChar = file.get();
		if (file.eof()) break;
		if (tempChar != '\n' && tempChar != '\r') {
			endPosition = ++pointer;
		}
		else {
			++pointer;
		}
		tempFile << tempChar;
	}


	file.close();
	file.open( path, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
	tempFile.seekg(0);


	for (uint32 pointer = 0; pointer < endPosition; ++pointer) {
		file << (char)tempFile.get();
	}


	file.flush();
	tempFile.close();
	std::remove(tempPath.c_str());
	return true;
}


bool File::create() {
	file.open(path, std::ios_base::app);
	file.close();
	return open();
}
bool File::move(Tstr newPath) {
	if (file.is_open()) file.close();
	file.open(newPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
	if (!file.is_open()) {
		ExternalError = 1;
		return false;
	}
	Tfstm oldFile(path, std::ios_base::binary | std::ios_base::in);
	if (!oldFile.is_open()) {
		file.close();
		return false;
	}

	moveFileContent(oldFile, file);

	oldFile.close();
	file.flush();
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

	toOverwrite.update();
	return true;
}
bool File::swap(File & Other) {
	if (!pointToBeg()) return false;
	if (!Other.pointToBeg()) {
		ExternalError = 1;
		return false;
	}
	Tfstm tempFile(tempPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::app);
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

	file.flush();
	Other.update();
	tempFile.close();
	std::remove(tempPath.c_str());
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
	return file.is_open();
}
void File::remove() {
	if (file.is_open()) file.close();
	std::remove(path.c_str());
}


bool File::open() {
	if (file.is_open()) {
		file.seekg(0);
		return true;
	}
	else {
		file.open(path, std::ios_base::binary | std::ios_base::in | std::ios_base::out);
	}
	return file.is_open();
}
void File::close() {
	if (file.is_open()) file.close();
	clear();
}
bool File::isOpen() const {
	return file.is_open();
}
void File::update() {
	file.flush();
}


bool File::exists() const {
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}
struct stat File::info() {
	file.flush();
	struct stat buffer;
	stat(path.c_str(), &buffer);
	return buffer;
}


bool File::good() const {
	return bool(this);
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

	Out.update();
	tempFile.close();
	std::remove(tempPath.c_str());
	return *this;
}
File& File::operator>>(int8 &Out) {
	uint16 output;
	file >> output;
	Out = (int8)output;
	return *this;
}
File& File::operator>>(uint8 &Out) {
	uint16 output;
	file >> output;
	Out = (uint8)output;
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

	file.flush();
	return *this;
}


File& File::operator=(File &Source) {
	close();
	path = Source.path;
	tempPath = Source.tempPath;

	if (Source.isOpen()) {
		open();

		eofErr(Source.eofErr());
		failErr(Source.failErr());
		badErr(Source.badErr());
		tempErr(Source.tempErr());
		extErr(Source.extErr());

		file.seekg(Source.file.tellg());
	}
	else {
		eofErr(Source.eofErr());
		failErr(Source.failErr());
		badErr(Source.badErr());
		tempErr(Source.tempErr());
		extErr(Source.extErr());
	}

	return *this;
}
Tstr File::operator+(File &ToAdd) {
	return str() + ToAdd.str();
}
File& File::operator+=(File &toAppend) {
	if (!pointToEnd()) return *this;
	if (!toAppend.pointToBeg()) {
		ExternalError = 1;
		return *this;
	}
	moveFileContent(toAppend.file, file);

	file.flush();
	return *this;
}


const FilePosition File::operator[](uint32 Position) {
	return FilePosition(this, Position);
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


const char * File::cStr() {
	if (!pointToBeg()) return nullptr;

	char tempChar;
	Tstr fileStr = "";
	while (1) {
		tempChar = file.get();
		if (!file.eof()) {
			fileStr += tempChar;
		}
		else break;
	}

	return fileStr.c_str();
}
File::operator Tstr() {
	return str();
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


FileIterator File::begin() {
	return FileIterator(this, 0);
}
FileIterator File::end() {
	return FileIterator(this, getNrChars());
}


std::string operator+(std::string First, File &Second) {
	return First + Second.str();
}