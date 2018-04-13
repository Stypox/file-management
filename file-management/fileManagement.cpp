#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include "fileManagement.h"

namespace sp {
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




	FilePosition::FilePosition(File * file, uint32 Position) : file(file), position(Position) {}
	FilePosition::operator char() {
		return file->getChar(position);
	}
	bool FilePosition::operator=(char newChar) {
		return file->replaceChar(position, newChar);
	}




	FileIterator::FileIterator(File * file, uint32 Position) : file(file), position(Position) {}
	bool FileIterator::operator!=(const FileIterator & ToCompare) {
		return position != ToCompare.position;
	}
	FilePosition FileIterator::operator*() const {
		return file->operator[](position);
	}
	void FileIterator::operator++() {
		++position;
	}





	Tstr File::toString(bool toConvert) {
		return (toConvert) ? "1" : "0";
	}
	Tstr File::toString(char toConvert) {
		return Tstr(&toConvert);
	}
	Tstr File::toString(int8 toConvert) {
		return std::to_string(toConvert);
	}
	Tstr File::toString(int16 toConvert) {
		return std::to_string(toConvert);
	}
	Tstr File::toString(int32 toConvert) {
		return std::to_string(toConvert);
	}
	Tstr File::toString(int64 toConvert) {
		return std::to_string(toConvert);
	}
	Tstr File::toString(uint8 toConvert) {
		return std::to_string(toConvert);
	}
	Tstr File::toString(uint16 toConvert) {
		return std::to_string(toConvert);
	}
	Tstr File::toString(uint32 toConvert) {
		return std::to_string(toConvert);
	}
	Tstr File::toString(uint64 toConvert) {
		return std::to_string(toConvert);
	}
	Tstr File::toString(float toConvert) {
		return std::to_string(toConvert);
	}
	Tstr File::toString(double toConvert) {
		return std::to_string(toConvert);
	}
	Tstr File::toString(long double toConvert) {
		return std::to_string(toConvert);
	}
	Tstr File::toString(Tstr &toConvert) {
		return toConvert;
	}


	uint32 File::countWords(Tstr Text) {
		bool wasSpace = true;
		uint32 nrWords = 0;

		for (char letter : Text) {
			if (isspace(letter)) wasSpace = true;
			else if (wasSpace) {
				wasSpace = false;
				++nrWords;
			}
		}

		return nrWords;
	}
	bool File::openTempToEditMain(Tfstm & TempFile) {
		if (mainFile.is_open()) {
			mainFile.close();
		}
		mainFile.open(mainPath, std::ios_base::binary | std::ios_base::in);
		if (!mainFile.is_open()) return false;


		struct stat buffer;
		if (stat(tempPath.c_str(), &buffer) == 0) {
			TempFile.open(tempPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
		}
		else {
			TempFile.open(tempPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::app);
		}
		if (!TempFile.is_open()) {
			TempError = true;
			mainFile.close();
			return false;
		}

		return true;
	}
	void File::moveFileContent(Tfstm & From, Tfstm & To) {
		char tempChar;
		while (1) {
			tempChar = From.get();
			if (From.eof()) break;
			To << tempChar;
		}
	}
	Tspos File::getPositionMove(uint32 Line, uint32 Word, uint32 Char) {
		if (!pointToBeg()) return fileNotOpen;


		if (Line != dontMove) {
			for (uint32 currentLine = 0; currentLine < Line; ++currentLine) {
				while (mainFile.get() != '\n') {
					if (mainFile.eof()) return outOfBounds;
				}
			}
		}


		if (Word != dontMove) {
			bool wasSpace = true;
			uint32 nrWords = 0;
			++Word;

			while (1) {
				if (mainFile.eof()) return outOfBounds;
				if (nrWords >= Word) {
					mainFile.seekg(-1, std::ios_base::cur);
					break;
				}
				if (isspace(mainFile.get())) wasSpace = true;
				else {
					if (wasSpace) {
						wasSpace = false;
						++nrWords;
					}
				}
			}
		}


		Tspos position = mainFile.tellg();
		if (Char != dontMove && Char != 0) {
			position += (Tspos)Char;
			if (position >= getNrChars()) {
				position = outOfBounds;
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
				tempChar = mainFile.get();
				if (mainFile.eof()) break;
				if (currentChar >= From && currentChar <= To) {
					tempFile << Replacement;
					mainFile.seekg(1 + (uint32)To);
					currentChar += 1 + (uint32)To;
				}
				else {
					tempFile << tempChar;
					++currentChar;
				}
			}


			mainFile.close();
			mainFile.open(mainPath, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
			tempFile.seekg(0);


			moveFileContent(tempFile, mainFile);


			mainFile.close();
			tempFile.close();
			std::remove(tempPath.c_str());
		}

	
		else if (oldSize < newSize) {
			uint32 fileLength = getNrChars();
			if (!pointTo(From)) return false;

			mainFile << Replacement.substr(0, oldSize);
			Tstr buffer = Replacement.substr(oldSize, newSize - oldSize);

			for (Tspos pointerPosition = oldSize + From; pointerPosition < fileLength; pointerPosition += 1) {
				mainFile.seekg(pointerPosition);
				buffer.push_back(mainFile.get());

				mainFile.seekg(pointerPosition);
				mainFile.put(buffer[0]);
				buffer.erase(0, 1);
			}

			mainFile << buffer;
		}
	

		else {
			if (!pointTo(From)) return false;
			mainFile << Replacement;
		}


		mainFile.flush();
		return true;
	}
	bool File::deleteSection(Tspos From, Tspos To) {
		Tfstm tempFile;
		if (!openTempToEditMain(tempFile)) return false;


		uint32 currentPosition = 0;
		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) break;
			if (currentPosition < From || currentPosition > To) {
				tempFile << tempChar;
			}
			++currentPosition;
		}


		mainFile.close();
		mainFile.open(mainPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
		tempFile.seekg(0);


		moveFileContent(tempFile, mainFile);


		mainFile.flush();
		tempFile.close();
		std::remove(tempPath.c_str());
		return true;
	}


	File::File(NLMode Mode) : mainPath(""), tempPath(defaultTempFilePath), TempError(0), ExternalError(0), newlineMode(Mode) {}
	File::File(Tstr MainPath, NLMode Mode) : mainPath(MainPath), tempPath(MainPath + defaultTempFileExtension), TempError(0), ExternalError(0), newlineMode(Mode) {}
	File::File(Tstr MainPath, Tstr TempPath, NLMode Mode) : mainPath(MainPath), tempPath(TempPath), TempError(0), ExternalError(0), newlineMode(Mode) {}
	File::File(File & Source) : mainPath(Source.mainPath), tempPath(Source.tempPath), TempError(Source.TempError), ExternalError(Source.TempError), newlineMode(Source.newlineMode) {
		if (Source.isOpen()) {
			open();
			mainFile.seekg(Source.mainFile.tellg());
		}

		eofErr(Source.eofErr());
		failErr(Source.failErr());
		badErr(Source.badErr());
	}
	File::~File() {
		close();
	}


	bool File::pointMove(Tspos Offset) {
		if (!mainFile.is_open() || mainFile.eof()) return false;
		mainFile.seekg(Offset, std::ios_base::cur);
		return true;
	}
	bool File::pointTo(Tspos Position) {
		if (!mainFile.is_open() && !open()) return false;
		mainFile.clear(mainFile.eofbit);
		mainFile.seekg(Position);
		return true;
	}
	bool File::pointTo(uint32 Line, uint32 Word, uint32 Char) {
		if (!pointToBeg()) return false;


		if (Line != dontMove) {
			for (uint32 currentLine = 0; currentLine < Line; ++currentLine) {
				while (mainFile.get() != '\n') {
					if (mainFile.eof()) return false;
				}
			}
		}


		if (Word != dontMove) {
			bool wasSpace = true;
			uint32 nrWords = 0;
			++Word;

			while (1) {
				if (mainFile.eof()) return false;
				if (nrWords >= Word) {
					mainFile.seekg(-1, std::ios_base::cur);
					break;
				}
				if (isspace(mainFile.get())) wasSpace = true;
				else {
					if (wasSpace) {
						wasSpace = false;
						++nrWords;
					}
				}
			}
		}


		if (Char != dontMove && Char != 0) {
			Tspos position = mainFile.tellg() + (Tspos)Char;
			mainFile.seekg(0, std::ios_base::end);
			if (position >= mainFile.tellg()) return false;

			mainFile.seekg(position);
		}


		return true;
	}
	bool File::pointToBeg() {
		if (!mainFile.is_open() && !open()) return false;
		mainFile.clear(mainFile.eofbit);
		mainFile.seekg(0);
		return true;
	}
	bool File::pointToEnd() {
		if (!mainFile.is_open() && !open()) return false;
		mainFile.clear(mainFile.eofbit);
		mainFile.seekg(0, std::ios_base::end);
		return true;
	}
	Tspos File::getPosition(uint32 Line, uint32 Word, uint32 Char) {
		Tspos pointerBeginning = 0;
		if (mainFile.is_open()) {
			pointerBeginning = mainFile.tellg();
		}
		if (!pointToBeg()) return fileNotOpen;


		if (Line != dontMove) {
			for (uint32 currentLine = 0; currentLine < Line; ++currentLine) {
				while (mainFile.get() != '\n') {
					if (mainFile.eof()) return outOfBounds;
				}
			}
		}


		if (Word != dontMove) {
			bool wasSpace = true;
			uint32 nrWords = 0;
			++Word;

			while (1) {
				if (mainFile.eof()) return outOfBounds;
				if (nrWords >= Word) {
					mainFile.seekg(-1, std::ios_base::cur);
					break;
				}
				if (isspace(mainFile.get())) wasSpace = true;
				else {
					if (wasSpace) {
						wasSpace = false;
						++nrWords;
					}
				}
			}
		}


		Tspos position = mainFile.tellg();
		if (Char != dontMove && Char != 0) {
			position += (Tspos)Char;
			if (position >= getNrChars()) {
				position = outOfBounds;
			}
		}


		mainFile.clear(mainFile.eofbit);
		mainFile.seekg(pointerBeginning);


		return position;
	}


	uint32 File::getNrLines() {
		Tspos pointerBeginning = 0;
		if (mainFile.is_open()) {
			pointerBeginning = mainFile.tellg();
		}
		if (!pointToBeg()) return 0;

		char tempChar;
		uint32 lines = 0;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) break;
			if (tempChar == '\n') ++lines;
		}

		mainFile.clear(mainFile.eofbit);
		mainFile.seekg(pointerBeginning);
		return lines;
	}
	uint32 File::getNrWords() {
		Tspos pointerBeginning = 0;
		if (mainFile.is_open()) {
			pointerBeginning = mainFile.tellg();
		}
		if (!pointToBeg()) return 0;
		uint32 nrWords = 0;

		bool wasSpace = true;
		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) break;
			if (isspace(tempChar)) wasSpace = true;
			else if (wasSpace) {
				wasSpace = false;
				++nrWords;
			}
		}

		mainFile.clear(mainFile.eofbit);
		mainFile.seekg(pointerBeginning);
		return nrWords;
	}
	uint32 File::getNrWords(uint32 Line) {
		Tspos pointerBeginning = 0;
		if (mainFile.is_open()) {
			pointerBeginning = mainFile.tellg();
		}
		if (!pointTo(Line, dontMove, dontMove)) return 0;
		uint32 nrWords = 0;

		bool wasSpace = true;
		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof() || tempChar == '\r') break;
			if (isspace(tempChar)) wasSpace = true;
			else if (wasSpace) {
				wasSpace = false;
				++nrWords;
			}
		}

		mainFile.clear(mainFile.eofbit);
		mainFile.seekg(pointerBeginning);
		return nrWords;
	}
	uint32 File::getNrChars() {
		struct stat buffer;
		if (::stat(mainPath.c_str(), &buffer) != 0) return 0;

		return (uint32)buffer.st_size;
	}
	uint32 File::getNrChars(uint32 Line) {
		Tspos pointerBeginning = 0;
		if (mainFile.is_open()) {
			pointerBeginning = mainFile.tellg();
		}
		if (!pointTo(Line, dontMove, dontMove)) return 0;
		uint32 nrChars = 0;

		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof() || tempChar == '\r') break;
			++nrChars;
		}

		mainFile.clear(mainFile.eofbit);
		mainFile.seekg(pointerBeginning);
		return nrChars;
	}
	uint32 File::getNrChars(uint32 Line, uint32 Word) {
		Tspos pointerBeginning = 0;
		if (mainFile.is_open()) {
			pointerBeginning = mainFile.tellg();
		}
		if (!pointTo(Line, Word, dontMove)) return 0;
		uint32 nrChars = 0;

		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof() || isspace(tempChar)) break;
			++nrChars;
		}

		mainFile.clear(mainFile.eofbit);
		mainFile.seekg(pointerBeginning);
		return nrChars;
	}
	
	Tstr File::getLine() {
		Tstr line = "";

		char tempChar;
		if (newlineMode == NLMode::win) {
			while (1) {
				tempChar = mainFile.get();
				if (mainFile.eof()) break;
				if (tempChar == '\r') {
					tempChar = mainFile.get();
					if (tempChar == '\n') break;
					line.push_back(tempChar);
				}
				line.push_back(tempChar);
			}
		}
		else {
			while (1) {
				tempChar = mainFile.get();
				if (tempChar == '\n' || mainFile.eof()) break;
				line.push_back(tempChar);
			}
		}

		return line;
	}
	bool File::getLine(Tstr &Line) {
		if (!mainFile.is_open() || mainFile.eof()) return false;
		Line = getLine();
		return true;
	}
	Tstr File::getLine(uint32 Line) {
		if (!pointTo(Line, dontMove, dontMove)) return "";
		return getLine();
	}
	Tstr File::getWord() {
		Tstr word = "";

		while (isspace(mainFile.get())) {}
		if (mainFile.eof()) return "";
		mainFile.seekg(-1, std::ios_base::cur);

		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (isspace(tempChar) || mainFile.eof()) break;
			word.push_back(tempChar);
		}

		return word;
	}
	bool File::getWord(Tstr &Word) {
		Word = "";
		if (!mainFile.is_open() || mainFile.eof()) return false;

		while (isspace(mainFile.get())) {}
		if (mainFile.eof()) return false;
		mainFile.seekg(-1, std::ios_base::cur);

		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (isspace(tempChar) || mainFile.eof()) break;
			Word.push_back(tempChar);
		}

		return true;
	}
	Tstr File::getWord(uint32 Word) {
		return getWord(dontMove, Word);
	}
	Tstr File::getWord(uint32 Line, uint32 Word) {
		if (!pointTo(Line, Word, dontMove)) return "";
		return getWord();
	}
	char File::getChar() {
		return mainFile.get();
	}
	bool File::getChar(char &Char) {
		Char = mainFile.get();
		return !mainFile.eof();
	}
	char File::getChar(uint32 Char) {
		return getChar(dontMove, dontMove, Char);
	}
	char File::getChar(uint32 Line, uint32 Char) {
		return getChar(Line, dontMove, Char);
	}
	char File::getChar(uint32 Line, uint32 Word, uint32 Char) {
		if (!pointTo(Line, Word, Char)) return fileNotOpen;
		return mainFile.get();
	}


	Tstr File::getLines(uint32 From, uint32 To) {
		Tstr lines = "";

		if (From < To) {
			if (!pointTo(From, dontMove, dontMove)) return "";

			for (uint32 currentLine = From; currentLine <= To; ++currentLine) {
				lines += "\n" + getLine();
				if (mainFile.eof()) break;
			}
		}
		else {
			if (!pointTo(To, dontMove, dontMove)) return "";

			for (uint32 currentLine = To; currentLine <= From; ++currentLine) {
				lines = getLine() + "\r\n" + lines;
				if (mainFile.eof()) break;
			}
		}

		lines.pop_back();
		return lines;
	}
	Tstr File::getWords(uint32 From, uint32 To) {
		return getWords(dontMove, From, To);
	}
	Tstr File::getWords(uint32 Line, uint32 From, uint32 To) {
		Tstr words = "";

		if (From < To) {
			if (!pointTo(Line, From, dontMove)) return "";

			for (uint32 currentWord = From; currentWord <= To; ++currentWord) {
				words += getWord() + " ";
				if (mainFile.eof()) break;
			}
		}
		else {
			if (!pointTo(Line, To, dontMove)) return "";

			for (uint32 currentWord = To; currentWord <= From; ++currentWord) {
				words = getWord() + " " + words;
				if (mainFile.eof()) break;
			}
		}

		words.pop_back();
		return words;
	}
	Tstr File::getChars(uint32 From, uint32 To) {
		return getChars(dontMove, dontMove, From, To);
	}
	Tstr File::getChars(uint32 Line, uint32 From, uint32 To) {
		return getChars(Line, dontMove, From, To);
	}
	Tstr File::getChars(uint32 Line, uint32 Word, uint32 From, uint32 To) {
		Tstr chars = "";

		if (From < To) {
			if (!pointTo(Line, Word, From)) return "";

			for (uint32 currentChar = From; currentChar < To; ++currentChar) {
				chars += mainFile.get();
				if (mainFile.eof()) break;
			}
		}
		else {
			if (!pointTo(Line, Word, To)) return "";

			for (uint32 currentChar = To; currentChar < From; ++currentChar) {
				chars = static_cast<char>(mainFile.get()) + chars;
				if (mainFile.eof()) break;
			}
		}

		return chars;
	}


	bool File::addChar(uint32 Char, char ToAdd) {
		return addChar(dontMove, dontMove, Char, ToAdd);
	}
	bool File::addChar(uint32 Line, uint32 Char, char ToAdd) {
		return addChar(Line, dontMove, Char, ToAdd);
	}
	bool File::addChar(uint32 Line, uint32 Word, uint32 Char, char ToAdd) {
		Tspos position = getPositionMove(Line, Word, Char);
		if (position < 0) return false;
		return add(position, toString(ToAdd));
	}

	
	File& File::replace(char ToPut) {
		mainFile.put(ToPut);
		return *this;
	}
	bool File::replaceChar(uint32 Char, char Replacement) {
		return replaceChar(dontMove, dontMove, Char, Replacement);
	}
	bool File::replaceChar(uint32 Line, uint32 Char, char Replacement) {
		return replaceChar(Line, dontMove, Char, Replacement);
	}
	bool File::replaceChar(uint32 Line, uint32 Word, uint32 Char, char Replacement) {
		if (!pointTo(Line, Word, Char)) return false;
		mainFile << Replacement;

		mainFile.flush();
		return true;
	}


	bool File::deleteLine(uint32 Line) {
		Tspos from, to;
		from = getPositionMove(Line, dontMove, dontMove);
		if (from == fileNotOpen) return false;
		if (from == outOfBounds) return true;
		to = getPositionMove(Line + 1, dontMove, dontMove);

		if (to == outOfBounds) {
			if (newlineMode == NLMode::win) {
				return deleteSection(from - static_cast<Tspos>(2), getNrChars() - 1);
			}
			else {
				return deleteSection(from - static_cast<Tspos>(1), getNrChars() - 1);
			}
		}
		return deleteSection(from, to - static_cast<Tspos>(1));
	}
	bool File::deleteWord(uint32 Word) {
		return deleteWord(dontMove, Word);
	}
	bool File::deleteWord(uint32 Line, uint32 Word) {
		Tspos from, to;
		from = getPositionMove(Line, Word, dontMove);
		if (from == fileNotOpen) return false;
		if (from == outOfBounds) return true;

		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) return deleteSection(from, getNrChars() - 1);
			if (isspace(tempChar)) {
				if (tempChar == '\r') break;
				while (1) {
					tempChar = mainFile.get();
					if (mainFile.eof()) return deleteSection(from, getNrChars() - 1);
					if (!isspace(tempChar) || tempChar == '\r') break;
				}
				break;
			}
		}
		return deleteSection(from, mainFile.tellg() - static_cast<Tspos>(2));
	}
	bool File::deleteChar() {
		if (!mainFile.is_open()) return false;
		uint32 position = (uint32)mainFile.tellg();
		Tfstm tempFile;
		if (!openTempToEditMain(tempFile)) return false;


		uint32 currentPosition = 0;
		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) break;
			if (currentPosition != position) {
				tempFile << tempChar;
			}
			++currentPosition;
		}


		mainFile.close();
		mainFile.open(mainPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
		tempFile.seekg(0);
		moveFileContent(tempFile, mainFile);


		mainFile.flush();
		mainFile.seekg(position);
		tempFile.close();
		std::remove(tempPath.c_str());
		return true;
	}
	bool File::deleteChar(uint32 Char) {
		return deleteChar(dontMove, dontMove, Char);
	}
	bool File::deleteChar(uint32 Line, uint32 Char) {
		return deleteChar(Line, dontMove, Char);
	}
	bool File::deleteChar(uint32 Line, uint32 Word, uint32 Char) {
		Tspos position = getPositionMove(Line, Word, Char);
		if (position == fileNotOpen) return false;
		if (position == outOfBounds) return true;


		Tfstm tempFile;
		if (!openTempToEditMain(tempFile)) return false;


		uint32 currentPosition = 0;
		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) break;
			if (currentPosition != position) {
				tempFile << tempChar;
			}
			++currentPosition;
		}


		mainFile.close();
		mainFile.open(mainPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
		tempFile.seekg(0);


		moveFileContent(tempFile, mainFile);


		mainFile.flush();
		tempFile.close();
		std::remove(tempPath.c_str());
		return true;
	}


	bool File::deleteLines(uint32 From, uint32 To) {
		if (From > To) std::swap(From, To);
		Tspos from, to;
		from = getPositionMove(From, dontMove, dontMove);
		if (from == fileNotOpen) return false;
		if (from == outOfBounds) return true;
		to = getPositionMove(To + 1, dontMove, dontMove);

		if (to == outOfBounds) {
			if (newlineMode == NLMode::win) {
				return deleteSection(from - static_cast<Tspos>(2), getNrChars() - 1);
			}
			else {
				return deleteSection(from - static_cast<Tspos>(1), getNrChars() - 1);
			}
		}
		return deleteSection(from, to - static_cast<Tspos>(1));
	}
	bool File::deleteWords(uint32 From, uint32 To) {
		return deleteWords(dontMove, From, To);
	}
	bool File::deleteWords(uint32 Line, uint32 From, uint32 To) {
		Tspos from, to;
		from = getPositionMove(Line, From, dontMove);
		if (from == fileNotOpen) return false;
		if (from == outOfBounds) return true;

		pointTo(Line, To, dontMove);
		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) return deleteSection(from, getNrChars() - 1);
			if (isspace(tempChar)) {
				if (tempChar == '\r') break;
				while (1) {
					tempChar = mainFile.get();
					if (mainFile.eof()) return deleteSection(from, getNrChars() - 1);
					if (!isspace(tempChar) || tempChar == '\r') break;
				}
				break;
			}
		}
		return deleteSection(from, mainFile.tellg() - static_cast<Tspos>(2));
	}
	bool File::deleteChars(uint32 From, uint32 To) {
		return deleteChars(dontMove, dontMove, From, To);
	}
	bool File::deleteChars(uint32 Line, uint32 From, uint32 To) {
		return deleteChars(Line, dontMove, From, To);
	}
	bool File::deleteChars(uint32 Line, uint32 Word, uint32 From, uint32 To) {
		Tspos from, to;
		from = getPositionMove(Line, Word, From);
		if (from == fileNotOpen) return false;
		if (from == outOfBounds) return true;
		to = getPositionMove(Line, Word, To);

		if (to == outOfBounds) {
			return deleteSection(from, getNrChars() - 1);
		}
		return deleteSection(from, to);
	}


	bool File::appendChar(char ToAppend) {
		return append(Tstr(&ToAppend));
	}
	bool File::appendChar(uint32 Line, char ToAppend) {
		if (!pointTo(Line, dontMove, dontMove)) return false;
	
		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) return append(Tstr(&ToAppend));
			if (tempChar == '\r') break;
		}

		return add(mainFile.tellg() - (Tspos)1, Tstr(&ToAppend));
	}
	bool File::appendChar(uint32 Line, uint32 Word, char ToAppend) {
		if (!pointTo(Line, Word, dontMove)) return false;

		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) return append(Tstr(&ToAppend));
			if (isspace(tempChar)) break;
		}

		return add(mainFile.tellg() - (Tspos)1, Tstr(&ToAppend));
	}


	bool File::deleteLastEmptyLines() {
		Tfstm tempFile;
		if (!openTempToEditMain(tempFile)) return false;

	
		char tempChar;
		uint32 endPosition = 0, pointer = 0;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) break;
			if (tempChar != '\n' && tempChar != '\r') {
				endPosition = ++pointer;
			}
			else {
				++pointer;
			}
			tempFile << tempChar;
		}


		mainFile.close();
		mainFile.open( mainPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
		tempFile.seekg(0);


		for (uint32 pointer = 0; pointer < endPosition; ++pointer) {
			mainFile << (char)tempFile.get();
		}


		mainFile.flush();
		tempFile.close();
		std::remove(tempPath.c_str());
		return true;
	}


	bool File::create() {
		mainFile.open(mainPath, std::ios_base::app);
		mainFile.close();
		return open();
	}
	bool File::move(Tstr newPath) {
		if (mainFile.is_open()) mainFile.close();
		mainFile.open(newPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
		if (!mainFile.is_open()) {
			ExternalError = 1;
			return false;
		}
		Tfstm oldFile(mainPath, std::ios_base::binary | std::ios_base::in);
		if (!oldFile.is_open()) {
			mainFile.close();
			return false;
		}

		moveFileContent(oldFile, mainFile);

		oldFile.close();
		mainFile.flush();
		std::remove(mainPath.c_str());
		mainPath = newPath;
		return true;
	}
	bool File::moveContent(File & toOverwrite) {
		if (!toOverwrite.truncate()) {
			ExternalError = 1;
			return false;
		}
		if (!pointToBeg()) return false;

		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) break;
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

		moveFileContent(mainFile, tempFile);

		truncate();
		moveFileContent(Other.mainFile, mainFile);

		Other.truncate();
		tempFile.clear(tempFile.eofbit);
		tempFile.seekg(0);
		moveFileContent(tempFile, Other.mainFile);

		mainFile.flush();
		Other.update();
		tempFile.close();
		std::remove(tempPath.c_str());
		return true;
	}
	bool File::rename(Tstr newName) {
		Tstr newPath = mainPath;
		while (!newPath.empty()) {
			if (newPath.back() == '/') break;
			newPath.pop_back();
		}
		newPath += newName;
		return move(newPath);
	}
	bool File::truncate() {
		if (mainFile.is_open()) mainFile.close();
		mainFile.open(mainPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
		return mainFile.is_open();
	}
	void File::remove() {
		if (mainFile.is_open()) mainFile.close();
		std::remove(mainPath.c_str());
	}


	bool File::open() {
		if (mainFile.is_open()) {
			mainFile.clear(mainFile.eofbit);
			mainFile.seekg(0);
			return true;
		}
		else {
			mainFile.open(mainPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out);
		}
		return mainFile.is_open();
	}
	void File::close() {
		if (mainFile.is_open()) mainFile.close();
		clear();
	}
	bool File::isOpen() const {
		return mainFile.is_open();
	}
	void File::update() {
		mainFile.flush();
	}


	bool File::exists() const {
		struct stat buffer;
		return (stat(mainPath.c_str(), &buffer) == 0);
	}
	struct stat File::info() {
		mainFile.flush();
		struct stat buffer;
		stat(mainPath.c_str(), &buffer);
		return buffer;
	}


	bool File::good() const {
		return bool(this);
	}
	void File::clear() {
		mainFile.clear();
		TempError = 0;
		ExternalError = 0;
	}
	bool File::eofErr() const {
		return mainFile.eof();
	}
	void File::eofErr(bool Value) {
		if (Value) {
			mainFile.setstate(mainFile.eofbit);
		}
		else {
			mainFile.clear(mainFile.eofbit);
		}
	}
	bool File::failErr() const {
		return mainFile.fail();
	}
	void File::failErr(bool Value) {
		if (Value) {
			mainFile.setstate(mainFile.failbit);
		}
		else {
			mainFile.clear(mainFile.failbit);
		}
	}
	bool File::badErr() const {
		return mainFile.bad();
	}
	void File::badErr(bool Value) {
		if (Value) {
			mainFile.setstate(mainFile.badbit);
		}
		else {
			mainFile.clear(mainFile.eofbit);
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
		return FileState(mainFile.is_open(), mainFile.eof(), mainFile.fail(), mainFile.bad(), TempError, ExternalError);
	}


	Tstr File::getPath() const {
		return mainPath;
	}
	void File::setPath(Tstr Path) {
		if (mainFile.is_open()) mainFile.close();
		mainPath = Path;
	}
	Tstr File::getTempPath() const {
		return tempPath;
	}
	void File::setTempPath(Tstr TempPath) {
		tempPath = TempPath;
	}


	File& File::operator>>(File &Out) {
		if (!pointToBeg()) return *this;
		if (Out.mainPath == mainPath || !Out.pointToBeg()) {
			ExternalError = 1;
			//Out.ExternalError = 1; //FARE se metterlo o no, perche' Out ha gia' il suo errore contenuto nel failbit del suo file e l'errore non e' esterno ma interno
			return *this;
		}
		Tfstm tempFile(tempPath, std::ios_base::binary | std::ios_base::in | std::ios_base::out | std::ios_base::app);
		if (!tempFile.is_open()) {
			TempError = 1;
			return *this;
		}

		moveFileContent(Out.mainFile, tempFile);

		Out.truncate();
		moveFileContent(mainFile, Out.mainFile);

		tempFile.seekg(0);
		moveFileContent(tempFile, Out.mainFile);

		Out.update();
		tempFile.close();
		std::remove(tempPath.c_str());
		return *this;
	}
	File& File::operator>>(int8 &Out) {
		uint16 output;
		mainFile >> output;
		Out = (int8)output;
		return *this;
	}
	File& File::operator>>(uint8 &Out) {
		uint16 output;
		mainFile >> output;
		Out = (uint8)output;
		return *this;
	}


	File& File::operator<<(File &In) {
		if (!pointToEnd()) return *this;
		if (In.mainPath == mainPath || !In.pointToBeg()) {
			ExternalError = 1;
			//In.ExternalError = 1;
			return *this;
		}

		moveFileContent(In.mainFile, mainFile);

		mainFile.flush();
		return *this;
	}


	File& File::operator=(File &Source) {
		close();
		newlineMode = Source.newlineMode;
		mainPath = Source.mainPath;
		tempPath = Source.tempPath;
		TempError = Source.TempError;
		ExternalError = Source.ExternalError;

		if (Source.isOpen()) {
			open();
			mainFile.seekg(Source.mainFile.tellg());
		}
		eofErr(Source.eofErr());
		failErr(Source.failErr());
		badErr(Source.badErr());

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
		moveFileContent(toAppend.mainFile, mainFile);

		mainFile.flush();
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
			tempCharThis = mainFile.get();
			tempCharCompare = toCompare.mainFile.get();

			if (mainFile.eof()) {
				if (toCompare.mainFile.eof()) break;
				else return false;
			}

			if (tempCharThis != tempCharCompare) return false;
		}
		return true;
	}
	bool File::operator!=(File &toCompare) {
		return !operator==(toCompare);
	}


	File::operator Tstr() {
		return str();
	}
	Tstr File::str() {
		if (!pointToBeg()) return "";

		char tempChar;
		Tstr fileStr = "";
		while (1) {
			tempChar = mainFile.get();
			if (!mainFile.eof()) {
				fileStr += tempChar;
			}
			else break;
		}

		return fileStr;
	}
	File::operator bool() {
		return !(mainFile.eof() || mainFile.fail() || mainFile.bad() || TempError || ExternalError);
	}
	bool File::operator!() {
		return mainFile.eof() || mainFile.fail() || mainFile.bad() || TempError || ExternalError;
	}


	FileIterator File::begin() {
		return FileIterator(this, 0);
	}
	FileIterator File::end() {
		return FileIterator(this, getNrChars());
	}


	Tstr operator+(Tstr First, File &Second) {
		return First + Second.str();
	}
}