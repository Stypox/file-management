#ifndef FILE_MANAGEMENT_CPP
#define FILE_MANAGEMENT_CPP

#include <iostream>
#include <string>
#include <fstream>
#include <experimental/filesystem>

#ifdef __linux__
#include <sys/stat.h>
#endif

#include "fileManagement.h"

namespace sp {
	sp::Tstr spc(char letter) {
		switch (letter) {
		case '\0':
			return "\\0";
		case ' ':
			return "_";
		case '\t':
			return "\\t";
		case '\n':
			return "\\n\n";
		case '\v':
			return "\\v";
		case '\f':
			return "\\f";
		case '\r':
			return "\\r";
		case -1:
			return "-1";
		default:
			return &letter;
		}
	}
	sp::Tstr spc(sp::uint32 number) {
		return std::to_string(number);
	}
	sp::Tstr spc(char letter, sp::int32 & line, sp::Newline mode = sp::Newline::LF, char previousLetter = 0) {
		switch (letter) {
		case '\0':
			return "\\0";
		case ' ':
			return "_";
		case '\t':
			return "\\t";
		case '\n':
			if (line == 0) return "\\n\n";
			if (previousLetter == '\r' || mode == sp::Newline::LF) return sp::Tstr("\\n\n") + std::to_string(line++) + sp::Tstr(" ");
			return sp::Tstr("\\n");
		case '\v':
			return "\\v";
		case '\f':
			return "\\f";
		case '\r':
			return "\\r";
		case -1:
			return "-1";
		default:
			return &letter;
		}
	}
	sp::Tstr spc(sp::Tstr str, sp::Newline mode = sp::Newline::LF) {
		sp::Tstr returnStr = "";
		sp::int32 line = 2;
		char previousLetter = 0;
		for (char letter : str) {
			returnStr += spc(letter, line, mode, previousLetter);
			previousLetter = letter;
		}
		if (line != 2) returnStr = "\n1 " + returnStr + "\n";

		return returnStr;
	}

	FileState::FileState(bool Open, bool Eof, bool Fail, bool Bad, bool ExtErr) : open(Open), eofError(Eof), failError(Fail), badError(Bad), externalError(ExtErr) {}
	FileState::operator bool() {
		return eofError || failError || badError || externalError;
	}
	std::string FileState::str() {
		std::string fileState = "open:";
		fileState += (open) ? "1" : "0";
		fileState += " eofErr:";
		fileState += (eofError) ? "1" : "0";
		fileState += " failErr:";
		fileState += (failError) ? "1" : "0";
		fileState += " badErr:";
		fileState += (badError) ? "1" : "0";
		fileState += " extErr:";
		fileState += (externalError) ? "1" : "0";
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
		return Tstr(1, toConvert);
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
			if (newlineMode == Newline::CRLF) {
				for (uint32 currentLine = 0; currentLine < Line; ++currentLine) {
					char tempChar;
					while (1) {
						tempChar = mainFile.get();
					falseNewline:
						if (tempChar == '\r') {
							tempChar = mainFile.get();
							if (tempChar == '\n') break;
							goto falseNewline;
						}
						if (mainFile.eof()) return outOfBounds;
					}
				}
			}
			else {
				for (uint32 currentLine = 0; currentLine < Line; ++currentLine) {
					while (mainFile.get() != '\n') {
						if (mainFile.eof()) return outOfBounds;
					}
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
			position += static_cast<Tspos>(Char);
			if (position >= getNrChars()) {
				position = outOfBounds;
			}
		}


		return position;
	}
	
	
	bool File::replaceSection(Tspos From, Tspos To, Tstr Replacement) {
		uint32 oldSize = static_cast<uint32>(To) - static_cast<uint32>(From) + 1,
			newSize = static_cast<uint32>(Replacement.length());
		
		if (oldSize > newSize) {
			if (To >= getNrChars() || !pointTo(From)) return false;
			mainFile << Replacement;

			char tempChar;
			for (Tspos getPosition = To + static_cast<Tspos>(1), putPosition = From + static_cast<Tspos>(newSize);; getPosition += static_cast<Tspos>(1), putPosition += static_cast<Tspos>(1)) {
				mainFile.seekg(getPosition);
				tempChar = mainFile.get();
				if (mainFile.eof()) return resize(static_cast<uint32>(putPosition));
				mainFile.seekg(putPosition);
				mainFile.put(tempChar);
			}
		}
		
		else if (oldSize < newSize) {
			if (To >= getNrChars() || !open()) return false;
			uint32 fileSize = getNrChars();

			char tempChar;
			for (Tspos getPosition = fileSize - 1, putPosition = fileSize + newSize - oldSize - 1; getPosition > To; getPosition -= static_cast<Tspos>(1), putPosition -= static_cast<Tspos>(1)) {
				mainFile.seekg(getPosition);
				tempChar = mainFile.get();
				mainFile.seekg(putPosition);
				mainFile.put(tempChar);
			}

			mainFile.seekg(From);
			mainFile << Replacement;
			mainFile.flush();
		}
		
		else {
			if (To >= getNrChars() || !pointTo(From)) return false;
			mainFile << Replacement;
			mainFile.flush();
		}
		
		return true;
	}
	bool File::deleteSection(Tspos From, Tspos To) {
		if (!mainFile.is_open() && !open()) return false;
		uint32 fileSize = getNrChars();
		if (From >= fileSize) return true;
		if (To >= fileSize - 1) return resize(static_cast<uint32>(From));

		char tempChar;
		Tspos getPosition = To + static_cast<Tspos>(1), putPosition = From;
		while (1) {
			mainFile.seekg(getPosition);
			tempChar = mainFile.get();
			if (mainFile.eof()) break;
			mainFile.seekg(putPosition);
			mainFile.put(tempChar);

			getPosition += static_cast<Tspos>(1);
			putPosition += static_cast<Tspos>(1);
		}

		return resize(static_cast<uint32>(putPosition));
	}


	File::File(Newline Mode) : mainPath(""), ExternalError(0), newlineMode(Mode) {}
	File::File(Tstr MainPath, Newline Mode) : mainPath(MainPath), ExternalError(0), newlineMode(Mode) {}
	File::File(File & Source) : mainPath(Source.mainPath), ExternalError(Source.ExternalError), newlineMode(Source.newlineMode) {
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
		return Position <= getNrChars();
	}
	bool File::pointTo(uint32 Line, uint32 Word, uint32 Char) {
		if (!pointToBeg()) return false;


		if (Line != dontMove && Line != 0) {
			if (newlineMode == Newline::CRLF) {
				for (uint32 currentLine = 0; currentLine < Line; ++currentLine) {
					char tempChar;
					while (1) {
						tempChar = mainFile.get();
						falseNewline:
						if (tempChar == '\r') {
							tempChar = mainFile.get();
							if (tempChar == '\n') break;
							goto falseNewline;
						}
						if (mainFile.eof()) return false;
					}
				}
			}
			else {
				for (uint32 currentLine = 0; currentLine < Line; ++currentLine) {
					while (mainFile.get() != '\n') {
						if (mainFile.eof()) return false;
					}
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
			Tspos position = mainFile.tellg() + static_cast<Tspos>(Char);
			mainFile.seekg(0, std::ios_base::end);
			if (position >= mainFile.tellg()) return false; //TODO keep position and remove seekg->tellg to get the end point.

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
			if (newlineMode == Newline::CRLF) {
				for (uint32 currentLine = 0; currentLine < Line; ++currentLine) {
					char tempChar;
					while (1) {
						tempChar = mainFile.get();
					falseNewline:
						if (tempChar == '\r') {
							tempChar = mainFile.get();
							if (tempChar == '\n') break;
							goto falseNewline;
						}
						if (mainFile.eof()) return outOfBounds;
					}
				}
			}
			else {
				for (uint32 currentLine = 0; currentLine < Line; ++currentLine) {
					while (mainFile.get() != '\n') {
						if (mainFile.eof()) return outOfBounds;
					}
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
			position += static_cast<Tspos>(Char);
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
		if (newlineMode == Newline::CRLF) {
			while (1) {
				tempChar = mainFile.get();
				falseNewline:
				if (mainFile.eof()) break;
				if (tempChar == '\r') {
					tempChar = mainFile.get();
					if (tempChar == '\n') ++lines;
					else goto falseNewline;
				}
			}
		}
		else {
			while (1) {
				tempChar = mainFile.get();
				if (mainFile.eof()) break;
				if (tempChar == '\n') ++lines;
			}
		}

		mainFile.clear(mainFile.eofbit);
		mainFile.seekg(pointerBeginning);
		return lines + 1;
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
		if (newlineMode == Newline::CRLF) {
			while (1) {
				tempChar = mainFile.get();
				falseNewline:
				if (mainFile.eof()) break;
				if (isspace(tempChar)) {
					wasSpace = true;
					if (tempChar == '\r') {
						tempChar = mainFile.get();
						if (tempChar == '\n') break;
						else goto falseNewline;
					}
				}
				else if (wasSpace) {
					wasSpace = false;
					++nrWords;
				}
			}
		}
		else {
			while (1) {
				tempChar = mainFile.get();
				if (tempChar == '\n' || mainFile.eof()) break;
				if (isspace(tempChar)) wasSpace = true;
				else if (wasSpace) {
					wasSpace = false;
					++nrWords;
				}
			}
		}

		mainFile.clear(mainFile.eofbit);
		mainFile.seekg(pointerBeginning);
		return nrWords;
	}
	uint32 File::getNrChars() {
		struct stat buffer;
		if (stat(mainPath.c_str(), &buffer) != 0) return 0;

		return static_cast<uint32>(buffer.st_size);
	}
	uint32 File::getNrChars(uint32 Line) {
		Tspos from = getPosition(Line, dontMove, dontMove);
		if (from < 0) return 0;
		Tspos to = getPosition(Line + 1, dontMove, dontMove);
		if (to == outOfBounds) return getNrChars() - static_cast<uint32>(from);

		return static_cast<uint32>(to) - static_cast<uint32>(from) - (newlineMode == Newline::CRLF ? 2 : 1);
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
		if (newlineMode == Newline::CRLF) {
			while (1) {
				tempChar = mainFile.get();
				falseNewline:
				if (mainFile.eof()) break;
				if (tempChar == '\r') {
					tempChar = mainFile.get();
					if (tempChar == '\n') break;
					line.push_back('\r');
					goto falseNewline;
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
				lines += getLine() + "\n";
				if (mainFile.eof()) break;
			}
		}
		else {
			if (!pointTo(To, dontMove, dontMove)) return "";

			for (uint32 currentLine = To; currentLine <= From; ++currentLine) {
				lines = getLine() + "\n" + lines;
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
		if (words.back() == ' ') words.pop_back();
		else if (words[0] == ' ') words.erase(0, 1);
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

			for (uint32 currentChar = From; currentChar <= To; ++currentChar) {
				chars += mainFile.get();
				if (mainFile.eof()) {
					chars.pop_back();
					break;
				}
			}
		}
		else {
			if (!pointTo(Line, Word, To)) return "";

			for (uint32 currentChar = To; currentChar <= From; ++currentChar) {
				chars = static_cast<char>(mainFile.get()) + chars;
				if (mainFile.eof()) {
					chars.erase(0, 1);
					break;
				}
			}
		}

		return chars;
	}


	bool File::addChar(char ToAdd) {
		if (!mainFile.is_open()) return false;
		Tspos pointerPosition = mainFile.tellg();
		if (pointerPosition < 0) return false;
		return add(pointerPosition, ToAdd);
	}
	bool File::addChar(uint32 Char, char ToAdd) {
		return add(Char, ToAdd);
	}
	bool File::addChar(uint32 Line, uint32 Char, char ToAdd) {
		return addChar(Line, dontMove, Char, ToAdd);
	}
	bool File::addChar(uint32 Line, uint32 Word, uint32 Char, char ToAdd) {
		Tspos position = getPositionMove(Line, Word, Char);
		if (position < 0) return false;
		return add(position, ToAdd);
	}

	
	bool File::replaceChar(char Replacement) {
		mainFile.put(Replacement);
		mainFile.flush();
		return !mainFile.eof();
	}
	bool File::replaceChar(uint32 Char, char Replacement) {
		if (!open()) return false;
		mainFile.seekg(Char);
		mainFile.put(Replacement);
		mainFile.flush();
		return true;
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
			if (newlineMode == Newline::CRLF) {
				if (from > static_cast<Tspos>(1)) from -= static_cast<Tspos>(2);
			}
			else {
				if (from > static_cast<Tspos>(0)) from -= static_cast<Tspos>(1);
			}
			return resize(static_cast<uint32>(from));
		}
		return deleteSection(from, to - static_cast<Tspos>(1));
	}
	bool File::deleteWord(uint32 Word) {
		return deleteWord(dontMove, Word);
	}
	bool File::deleteWord(uint32 Line, uint32 Word) {
		Tspos from = getPositionMove(Line, Word, dontMove), to;
		if (from == fileNotOpen) return false;
		if (from == outOfBounds) return true;

		char tempChar;
		if (newlineMode == Newline::CRLF) {
			bool endLine = true;
			while (1) {
				tempChar = mainFile.get();
				if (mainFile.eof()) {
					mainFile.clear(mainFile.eofbit);
					to = maxFileSize;
					break;
				}
				if (isspace(tempChar)) {
					while (1) {
						if (tempChar == '\r') {
							tempChar = mainFile.get();
							if (tempChar == '\n') {
								to = mainFile.tellg() - static_cast<Tspos>(3);
								break;
							}
							else goto falseNewlineA;
						}

						tempChar = mainFile.get();
						falseNewlineA:
						if (mainFile.eof()) {
							mainFile.clear(mainFile.eofbit);
							to = maxFileSize;
							break;
						}
						if (!isspace(tempChar)) {
							to = mainFile.tellg() - static_cast<Tspos>(2);
							endLine = false;
							break;
						}
					}
					break;
				}
			}
			Tspos currentPosition = from;
			while (1) {
				currentPosition -= static_cast<Tspos>(1);
				if (currentPosition < static_cast<Tspos>(0)) {
					from = static_cast<Tspos>(0);
					break;
				}
				mainFile.seekg(currentPosition);
				tempChar = mainFile.get();

				falseNewlineB:
				if (!isspace(tempChar)) {
					if (endLine) from = currentPosition + static_cast<Tspos>(1);
					break;
				}
				if (tempChar == '\n') {
					currentPosition -= static_cast<Tspos>(1);
					if (currentPosition < static_cast<Tspos>(0)) {
						from = static_cast<Tspos>(0);
						break;
					}
					mainFile.seekg(currentPosition);
					tempChar = mainFile.get();

					if (tempChar == '\r') {
						from = currentPosition + static_cast<Tspos>(2);
						break;
					}
					else goto falseNewlineB;
				}
			}
		}
		else {
			bool endLine = true;
			while (1) {
				tempChar = mainFile.get();
				if (mainFile.eof()) {
					mainFile.clear(mainFile.eofbit);
					to = maxFileSize;
					break;
				}
				if (isspace(tempChar)) {
					while (1) {
						if (tempChar == '\n') {
							to = mainFile.tellg() - static_cast<Tspos>(2);
							break;
						}

						tempChar = mainFile.get();
						if (mainFile.eof()) {
							mainFile.clear(mainFile.eofbit);
							to = maxFileSize;
							break;
						}
						if (!isspace(tempChar)) {
							to = mainFile.tellg() - static_cast<Tspos>(2);
							endLine = false;
							break;
						}
					}
					break;
				}
			}
			Tspos currentPosition = from;
			while (1) {
				currentPosition -= static_cast<Tspos>(1);
				if (currentPosition < static_cast<Tspos>(0)) {
					from = static_cast<Tspos>(0);
					break;
				}
				mainFile.seekg(currentPosition);
				tempChar = mainFile.get();

				if (!isspace(tempChar)) {
					if (endLine) from = currentPosition + static_cast<Tspos>(1);
					break;
				}
				if (tempChar == '\n') {
					from = currentPosition + static_cast<Tspos>(1);
					break;
				}
			}
		}

		return deleteSection(from, to);
	}
	bool File::deleteChar() {
		if (!mainFile.is_open()) return false;
		Tspos position = mainFile.tellg();
		return deleteSection(position, position);
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
		else if (position == outOfBounds) return true;
		return deleteSection(position, position);
	}


	bool File::deleteLines(uint32 From, uint32 To) {
		if (From > To) std::swap(From, To);
		Tspos from, to;
		from = getPositionMove(From, dontMove, dontMove);
		if (from == fileNotOpen) return false;
		if (from == outOfBounds) return true;
		to = getPositionMove(To + 1, dontMove, dontMove);

		if (to == outOfBounds) {
			if (newlineMode == Newline::CRLF) {
				if (from > static_cast<Tspos>(1)) from -= static_cast<Tspos>(2);
			}
			else {
				if (from > static_cast<Tspos>(0)) from -= static_cast<Tspos>(1);
			}
			return resize(static_cast<uint32>(from));
		}
		
		return deleteSection(from, to - static_cast<Tspos>(1));
	}
	bool File::deleteWords(uint32 From, uint32 To) {
		return deleteWords(dontMove, From, To);
	}
	bool File::deleteWords(uint32 Line, uint32 From, uint32 To) {
		if (From > To) std::swap(From, To);
		Tspos from = getPositionMove(Line, From, dontMove), to;
		if (from == fileNotOpen) return false;
		if (from == outOfBounds) return true;
		
		char tempChar;
		Tstr lines = "";
		if (newlineMode == Newline::CRLF) {
			bool wasSpace = true, firstWordEndLine = false, lastWordEndLine = false;

			for (uint32 currentWord = From; currentWord <= To;) {
				tempChar = mainFile.get();

				falseNewlineA:
				if (mainFile.eof()) {
					firstWordEndLine = true;
					to = getNrChars() - 1;
					break;
				}
				if (isspace(tempChar)) {
					wasSpace = true;
					if (tempChar == '\r') {
						tempChar = mainFile.get();
						if (tempChar == '\n') {
							if (currentWord == From + 1) {
								firstWordEndLine = true;
							}
							lines += "\r\n";
						}
						else goto falseNewlineA;
					}
				}
				else if (wasSpace) {
					wasSpace = false;
					++currentWord;
				}
			}

			tempChar = mainFile.get();
			if (mainFile.eof()) {
				firstWordEndLine = true;
				mainFile.clear(mainFile.eofbit);
				to = getNrChars() - 1;
			}
			else {
				while (1) {
					if (isspace(tempChar)) {
						while (1) {
							if (tempChar == '\r') {
								tempChar = mainFile.get();
								if (tempChar == '\n') {
									to = mainFile.tellg() - static_cast<Tspos>(3);
									lastWordEndLine = true;
									break;
								}
								else goto falseNewlineB;
							}

							tempChar = mainFile.get();

							falseNewlineB:
							if (mainFile.eof()) {
								mainFile.clear(mainFile.eofbit);
								to = getNrChars() - 1;
								break;
							}
							if (!isspace(tempChar)) {
								to = mainFile.tellg() - static_cast<Tspos>(2);
								break;
							}
						}
						break;
					}

					tempChar = mainFile.get();
					if (mainFile.eof()) {
						mainFile.clear(mainFile.eofbit);
						to = getNrChars() - 1;
						break;
					}
				}
			}

			Tspos currentPosition = from;
			while (1) {
				currentPosition -= static_cast<Tspos>(1);
				if (currentPosition < static_cast<Tspos>(0)) {
					from = static_cast<Tspos>(0);
					break;
				}
				mainFile.seekg(currentPosition);
				tempChar = mainFile.get();

				falseNewlineC:
				if (!isspace(tempChar)) {
					if (firstWordEndLine || (lastWordEndLine && lines == "")) from = currentPosition + static_cast<Tspos>(1);
					break;
				}
				if (tempChar == '\n') {
					currentPosition -= static_cast<Tspos>(1);
					if (currentPosition < static_cast<Tspos>(0)) {
						from = static_cast<Tspos>(0);
						break;
					}
					mainFile.seekg(currentPosition);
					tempChar = mainFile.get();

					if (tempChar == '\r') {
						from = currentPosition + static_cast<Tspos>(2);
						break;
					}
					else goto falseNewlineC;
				}
			}
		}
		else {
			bool wasSpace = true, firstWordEndLine = false, lastWordEndLine = false;

			for (uint32 currentWord = From; currentWord <= To;) {
				tempChar = mainFile.get();

				if (mainFile.eof()) {
					firstWordEndLine = true;
					to = getNrChars() - 1;
					break;
				}
				if (isspace(tempChar)) {
					wasSpace = true;
					if (tempChar == '\n') {
						if (currentWord == From + 1) {
							firstWordEndLine = true;
						}
						lines += '\n';
					}
				}
				else if (wasSpace) {
					wasSpace = false;
					++currentWord;
				}
			}

			tempChar = mainFile.get();
			if (mainFile.eof()) {
				firstWordEndLine = true;
				mainFile.clear(mainFile.eofbit);
				to = getNrChars() - 1;
			}
			else {
				while (1) {
					if (isspace(tempChar)) {
						while (1) {
							if (tempChar == '\n') {
								to = mainFile.tellg() - static_cast<Tspos>(2);
								lastWordEndLine = true;
								break;
							}

							tempChar = mainFile.get();

							if (mainFile.eof()) {
								mainFile.clear(mainFile.eofbit);
								to = getNrChars() - 1;
								break;
							}
							if (!isspace(tempChar)) {
								to = mainFile.tellg() - static_cast<Tspos>(2);
								break;
							}
						}
						break;
					}

					tempChar = mainFile.get();
					if (mainFile.eof()) {
						mainFile.clear(mainFile.eofbit);
						to = getNrChars() - 1;
						break;
					}
				}
			}

			Tspos currentPosition = from;
			while (1) {
				currentPosition -= static_cast<Tspos>(1);
				if (currentPosition < static_cast<Tspos>(0)) {
					from = static_cast<Tspos>(0);
					break;
				}
				mainFile.seekg(currentPosition);
				tempChar = mainFile.get();

				if (!isspace(tempChar)) {
					if (firstWordEndLine || (lastWordEndLine && lines == "")) from = currentPosition + static_cast<Tspos>(1);
					break;
				}
				if (tempChar == '\n') {
					from = currentPosition + static_cast<Tspos>(1);
					break;
				}			
			}
		}

		return replaceSection(from, to, lines);
	}
	bool File::deleteChars(uint32 From, uint32 To) {
		return deleteChars(dontMove, dontMove, From, To);
	}
	bool File::deleteChars(uint32 Line, uint32 From, uint32 To) {
		return deleteChars(Line, dontMove, From, To);
	}
	bool File::deleteChars(uint32 Line, uint32 Word, uint32 From, uint32 To) {
		if (From > To) std::swap(From, To);
		Tspos from, to;
		from = getPositionMove(Line, Word, From);
		if (from == fileNotOpen) return false;
		if (from == outOfBounds) return true;
		to = getPositionMove(Line, Word, To);

		if (to == outOfBounds) {
			return resize(static_cast<uint32>(from));
		}
		return deleteSection(from, to);
	}


	bool File::appendChar(char ToAppend) {
		return append(ToAppend);
	}
	bool File::appendChar(uint32 Line, char ToAppend) {
		Tspos pos = getPositionMove(Line + 1, dontMove, dontMove);
		if (pos == fileNotOpen) return false;
		if (pos == outOfBounds) {
			if (Line >= getNrLines()) return false;
			return append(ToAppend);
		}

		return add(pos - static_cast<Tspos>(newlineMode == Newline::CRLF ? 2 : 1), ToAppend);
	}
	bool File::appendChar(uint32 Line, uint32 Word, char ToAppend) {
		if (!pointTo(Line, Word, dontMove)) return false;

		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) return append(ToAppend);
			if (isspace(tempChar)) break;
		}

		return add(mainFile.tellg() - static_cast<Tspos>(1), ToAppend);
	}


	bool File::deleteLastEmptyLines() {
		if (!pointToBeg()) return false;
		char tempChar;
		Tspos endPosition = 0;
		if (newlineMode == Newline::CRLF) {
			while (1) {
				tempChar = mainFile.get();
				if (mainFile.eof()) break;

				falseNewline:
				if (tempChar == '\r') {
					tempChar = mainFile.get();
					if (mainFile.eof()) return true;
					if (tempChar != '\n') {
						endPosition = mainFile.tellg() - static_cast<Tspos>(1);
						goto falseNewline;
					}
				}
				else endPosition = mainFile.tellg();
			}
		}
		else {
			while (1) {
				tempChar = mainFile.get();
				if (mainFile.eof()) break;
				if (tempChar != '\n') endPosition = static_cast<uint32>(mainFile.tellg());
			}
		}
		
		return resize(static_cast<uint32>(endPosition));
	}


	bool File::create() {
		if (exists()) return open();
		if (mainFile.is_open()) mainFile.close();
		mainFile.open(mainPath, std::ios_base::app);
		if (!mainFile.is_open()) return false;
		mainFile.close();
		return open();
	}
	bool File::move(Tstr newPath) {
		if (mainFile.is_open()) mainFile.close();
		std::error_code e;
		std::experimental::filesystem::rename(mainPath, newPath, e);
		if (e) {
			ExternalError = true;
			return false;
		}

		mainPath = newPath;
		return open();
	}
	bool File::copy(Tstr copyPath) {
		File toOverwrite(copyPath);
		return copy(toOverwrite);
	}
	bool File::copy(File & toOverwrite) {
		std::error_code e;
		if (std::experimental::filesystem::equivalent(mainPath, toOverwrite.mainPath, e)) return true;
		else if (e) return false;

		if (toOverwrite.exists()) {
			if (!toOverwrite.truncate()) {
				ExternalError = 1;
				return false;
			}
			toOverwrite.pointToBeg();
		}
		else {
			if (!toOverwrite.create()) {
				ExternalError = 1;
				return false;
			}
		}
		if (!pointToBeg()) return false;

		moveFileContent(mainFile, toOverwrite.mainFile);

		toOverwrite.update();
		toOverwrite.newlineMode = newlineMode;
		return true;
	}
	bool File::swap(Tstr swapPath) {
		File toSwap(swapPath);
		return swap(toSwap);
	}
	bool File::swap(File & Other) {
		if (!pointToBeg()) return false;
		if (!Other.pointToBeg()) {
			ExternalError = 1;
			return false;
		}

		char tempCharThis, tempCharOther;
		uint32 sizeThis = getNrChars(), sizeOther = Other.getNrChars();
		if (sizeThis > sizeOther) {
			for (uint32 currentPosition = 0; currentPosition < sizeOther; ++currentPosition) {
				mainFile.seekg(currentPosition);
				tempCharThis = mainFile.get();
				Other.mainFile.seekg(currentPosition);
				tempCharOther = Other.mainFile.get();

				mainFile.seekg(currentPosition);
				mainFile.put(tempCharOther);
				Other.mainFile.seekg(currentPosition);
				Other.mainFile.put(tempCharThis);
			}

			for (uint32 currentPosition = sizeOther; currentPosition < sizeThis; ++currentPosition) {
				mainFile.seekg(currentPosition);
				Other.mainFile.seekg(currentPosition);
				Other.mainFile.put(mainFile.get());
			}

			resize(sizeOther);
		}
		else {
			for (uint32 currentPosition = 0; currentPosition < sizeThis; ++currentPosition) {
				mainFile.seekg(currentPosition);
				tempCharThis = mainFile.get();
				Other.mainFile.seekg(currentPosition);
				tempCharOther = Other.mainFile.get();

				mainFile.seekg(currentPosition);
				mainFile.put(tempCharOther);
				Other.mainFile.seekg(currentPosition);
				Other.mainFile.put(tempCharThis);
			}

			for (uint32 currentPosition = sizeThis; currentPosition < sizeOther; ++currentPosition) {
				mainFile.seekg(currentPosition);
				Other.mainFile.seekg(currentPosition);
				mainFile.put(Other.mainFile.get());
			}

			Other.resize(sizeThis);
		}

		std::swap(newlineMode, Other.newlineMode);
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
		if (!resize(0)) return false;
		if (mainFile.is_open()) {
			mainFile.flush();
			mainFile.clear(mainFile.eofbit);
			mainFile.seekg(0);
		}
		return true;
	}
	void File::remove() {
		if (mainFile.is_open()) mainFile.close();
		std::remove(mainPath.c_str());
	}


	uint32 File::size() {
		return getNrChars();
	}
	bool File::resize(uint32 newSize) {
		std::error_code error;
		std::experimental::filesystem::resize_file(mainPath, newSize, error);
		if (mainFile.is_open()) mainFile.flush();
		return !error;
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
		std::error_code e;
		bool returnValue = std::experimental::filesystem::exists(mainPath, e);
		if (e) return false;
		return returnValue;
	}
	struct stat File::info() {
		mainFile.flush();
		struct stat buffer;
		stat(mainPath.c_str(), &buffer);
		return buffer;
	}


	bool File::good() const {
		return this->operator bool();
	}
	void File::clear() {
		mainFile.clear();
		ExternalError = false;
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
	bool File::extErr() const {
		return ExternalError;
	}
	void File::extErr(bool Value) {
		ExternalError = Value;
	}
	FileState File::state() const {
		return FileState(mainFile.is_open(), mainFile.eof(), mainFile.fail(), mainFile.bad(), ExternalError);
	}


	Tstr File::getPath() const {
		return mainPath;
	}
	void File::setPath(Tstr Path) {
		if (mainFile.is_open()) mainFile.close();
		mainPath = Path;
	}


	File& File::operator>>(File &Out) {
		if (!isOpen() && !open()) return *this;
		if (!Out.isOpen() && !Out.open()) {
			ExternalError = true;
			return *this;
		}
		uint32 sizeThis = getNrChars(), sizeOut = Out.getNrChars();
		
		char tempChar;		
		for (Tspos pointerGet = static_cast<Tspos>(sizeOut) - static_cast<Tspos>(1), pointerPut = static_cast<Tspos>(sizeThis + sizeOut) - static_cast<Tspos>(1); pointerGet > -1; pointerGet -= 1, pointerPut -= 1) {
			Out.mainFile.seekg(pointerGet);
			tempChar = Out.mainFile.get();
			Out.mainFile.seekg(pointerPut);
			Out.mainFile.put(tempChar);
		}

		Out.pointTo(0);
		moveFileContent(mainFile, Out.mainFile);

		Out.mainFile.flush();
		return *this;
	}
	File& File::operator>>(int8 &Out) {
		uint16 output;
		mainFile >> output;
		Out = static_cast<int8>(output);
		return *this;
	}
	File& File::operator>>(uint8 &Out) {
		uint16 output;
		mainFile >> output;
		Out = static_cast<uint8>(output);
		return *this;
	}


	File& File::operator<<(File &In) {
		if (!pointToEnd()) return *this;

		if (this == &In) {
			char tempChar;
			uint32 size = getNrChars();
			for (Tspos pointerGet = 0, pointerPut = size; pointerGet < size; pointerGet += 1, pointerPut += 1) {
				mainFile.seekg(pointerGet);
				tempChar = mainFile.get();

				mainFile.seekg(pointerPut);
				mainFile << tempChar;
			}
		}
		else {
			if (!In.pointToBeg()) {
				ExternalError = 1;
				return *this;
			}
			else {
				moveFileContent(In.mainFile, mainFile);
			}
		}

		mainFile.flush();
		return *this;
	}


	File& File::operator=(File &Source) {
		close();
		newlineMode = Source.newlineMode;
		mainPath = Source.mainPath;
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
	File::operator bool() const {
		return !(mainFile.eof() || mainFile.fail() || mainFile.bad() || ExternalError);
	}
	bool File::operator!() const {
		return mainFile.eof() || mainFile.fail() || mainFile.bad() || ExternalError;
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

#endif
