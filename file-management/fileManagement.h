//TODO non so se serva ma forse bisogna fare file.clear() di tutto invece che solo l'eofbit perche' se c'e' il failbit impostato e il file e' gia' aperto non verra' mai chiuso ma sara' impossibile da leggere o scrivere
//TODO remove C-style conversion
//TODO add for(word : file.words()), for(line : file.lines())
//TODO implement unix-windows conversion
//TODO add pointToEnd(index);
//TODO pointTo(inBounds line, inBounds word, outOfBounds char) shall not point to end, if not needed by functions
//TODO remove main file: now there is only one type of file
//TODO implement error system
//TODO newline mode auto-detection in constructor
//TODO #ifndef SP_FILE_MANAGEMENT
//TODO move constructor
//TODO (maybe) remove open references: the class should handle all of that automatically

//UTILI:
/*
max 75 chars x line

this file

This file is opened in binary input-output mode, if it wasn't already.
Returns false if this file couldn't be opened, otherwise returns true. // This file must be open in binary-output mode.
The pointer is not moved. // The pointer is moved to an untraceable position.
*/

#ifndef SP_FILE_MANAGEMENT_H
#define SP_FILE_MANAGEMENT_H

#include <iostream>
#include <string>
#include <fstream>
#include <experimental/filesystem>
#include <sys/stat.h>

namespace sp {
	using Tstr = std::string;
	using Tfstm = std::fstream;
	using Tspos = std::streampos;
#ifdef _WIN32
	using Tstat = struct _stat64;
#else
	using Tstat = struct stat64;
#endif
	using int8 = int8_t;
	using int16 = int16_t;
	using int32 = int32_t;
	using int64 = int64_t;
	using uint8 = uint8_t;
	using uint16 = uint16_t;
	using uint32 = uint32_t;
	using uint64 = uint64_t;


	enum class Newline {
		CRLF,
		LF
	};
#ifdef _WIN32
	constexpr Newline defaultNewlineMode = Newline::CRLF;
#else
	constexpr Newline defaultNewlineMode = Newline::LF;
#endif

	constexpr uint32 dontMove = UINT32_MAX;
	constexpr uint32 maxFileSize = UINT32_MAX - 1;
	 
	constexpr std::streamoff fileNotOpen = -1;
	constexpr std::streamoff outOfBounds = -2;


	class File;

	struct FileState {
	public:
		bool open, eofError, failError, badError, externalError;

		FileState(bool Open, bool Eof, bool Fail, bool Bad, bool ExtErr);

		/*
		Returns true if the file is open and there are no errors
		*/
		operator bool();
		/*
		Saves the state of the file on a string
		*/
		std::string str();
		/*
		Saves the state of the file on another file
		*/
		void save(File &file);
	};

	class FilePosition {
		File * file;
		uint32 position;
	public:
		FilePosition(File * file, uint32 Position);

		/*
		Returns the char in the set position
		*/
		operator char();
		/*
		The char in the set position is replaced by the parameter
		Leaves the file open after returning
		Returns false if the file couldn't be opened or if the
			set position is out of bounds, otherwise true
		*/
		bool operator=(char newChar);
	};

	class FileIterator {
		File * file;
		uint32 position;
	public:
		FileIterator(File * file, uint32 Position);

		bool operator!= (const FileIterator& ToCompare);
		FilePosition operator* () const;
		void operator++();
	};

	class File {
	public:
		Tfstm mainFile;
		Tstr mainPath;
		bool ExternalError;

		Newline newlineMode;


		/*
		Converts the parameter to an std::string object and returns it
		*/
		template <typename T>
		Tstr toString(T toConvert);
		/*
		Converts the parameter to an std::string object and returns it
		*/
		Tstr toString(bool toConvert);
		/*
		Converts the parameter to an std::string object and returns it
		*/
		Tstr toString(char toConvert);
		/*
		Converts the parameter to an std::string object and returns it
		*/
		Tstr toString(int8 toConvert);
		/*
		Converts the parameter to an std::string object and returns it
		*/
		Tstr toString(int16 toConvert);
		/*
		Converts the parameter to an std::string object and returns it
		*/
		Tstr toString(int32 toConvert);
		/*
		Converts the parameter to an std::string object and returns it
		*/
		Tstr toString(int64 toConvert);
		/*
		Converts the parameter to an std::string object and returns it
		*/
		Tstr toString(uint8 toConvert);
		/*
		Converts the parameter to an std::string object and returns it
		*/
		Tstr toString(uint16 toConvert);
		/*
		Converts the parameter to an std::string object and returns it
		*/
		Tstr toString(uint32 toConvert);
		/*
		Converts the parameter to an std::string object and returns it
		*/
		Tstr toString(uint64 toConvert);
		/*
		Converts the parameter to an std::string object and returns it
		*/
		Tstr toString(float toConvert);
		/*
		Converts the parameter to an std::string object and returns it
		*/
		Tstr toString(double toConvert);
		/*
		Converts the parameter to an std::string object and returns it
		*/
		Tstr toString(long double toConvert);
		/*
		Returns the parameter itself
		*/
		Tstr toString(Tstr &toConvert);


		/*
		Reads all the content of the first file and writes it to the second
		starting from where the pointer currently is. The first file should be open
		in binary-input mode and the second one in binary-output mode.
		*/
		void moveFileContent(Tfstm &From, Tfstm &To);
		/*
		Returns the position of a char (third parameter) in a word (second
		parameter) in a line (first parameter). The indices start from 0, that is
		getPositionMove(0, 0, 0) returns the position of the first char of the
		first word of the first line(not necessarily 0). While 0 means "look for
		the first", -1 means "don't do anything". So only 
		getPositionMove(-1, -1, -1) will surely return 0. The constexpr
		sp::dontMove can be used as -1. The pointer is moved to an untraceable
		position. This file is opened in binary-input-output mode, if it wasn't
		already.
		Returns -1 if this file couldn't be opened and -2 if the specified position
		is out of bounds. The constexpr sp::fileNotOpen (= -1) and sp::outOfBounds
		(= -2) can be used to check.
		*/
		Tspos getPositionMove(uint32 Line, uint32 Word, uint32 Char);


		/*
		Replaces all the chars of an interval (both ends included) with a string.
		This file is opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool replaceSection(Tspos From, Tspos To, Tstr Replacement);
		/*
		Deletes all the chars between the first and the second parameter, both
		included. This file is opened in binary-input-output mode, if it wasn't
		already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool deleteSection(Tspos From, Tspos To);
		

	public:
		/*
		Default constructor. Initializes the path to an empty string. The newline
		mode is initialized to the first parameter, if provided, otherwise it is
		initialized based on the operating system.
		*/
		File(Newline Mode = defaultNewlineMode);
		/*
		Constructor with a string. Initializes the path to the first parameter. The
		newline mode is initialized to the second parameter, if provided, otherwise
		it is initialized based on the operating system.
		*/
		File(Tstr MainPath, Newline Mode = defaultNewlineMode);
		/*
		Copy constructor.
		*/
		File(File & Source);
		/*
		Destructor. Closes the file.
		*/
		~File();


		/*
		Moves the pointer relative to the current position. This file must be open
		in binary-input-output mode.
		Returns false if this file is not open or the end was already reached,
		otherwise returns true.
		*/
		bool pointMove(Tspos Offset);
		/*
		Moves the pointer to the position specified by the parameter. The position
		starts from zero, that is the first char of the file is at position 0. This
		file is opened in binary input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened or if the specified position
		is out of bounds, otherwise returns true.
		*/
		bool pointTo(Tspos Position);
		/*
		Moves the pointer to a char (third parameter) in a word (second parameter)
		in a line (first parameter). The indices start from 0, that is
		pointTo(0, 0, 0) points to the first char of the first word of the first
		line (not necessarily to the beginning of the file). While 0 means "move to
		the first", -1 means "don't move". So only pointTo(-1, -1, -1) will surely
		point to the beginning. The constexpr sp::dontMove can be used as -1. This
		file is opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened or if the specified position
		is out of bounds, otherwise returns true.
		*/
		bool pointTo(uint32 Line, uint32 Word, uint32 Char);
		/*
		Moves the pointer to the beginning of this file and clears its eofbit. This
		file is opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool pointToBeg();
		/*
		Moves the pointer to the end of this file and clears its eofbit. This file
		is opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool pointToEnd();
		/*
		Returns the position of a char (third parameter) in a word (second
		parameter) in a line (first parameter). The indices start from 0, that is
		getPosition(0, 0, 0) returns the position of the first char of the first
		word of the first line (not necessarily 0). While 0 means "look for the
		first", -1 means "don't do anything". So only getPosition(-1, -1, -1) will
		surely return 0. The pointer is not moved. The constexpr sp::dontMove can
		be used as -1. This file is opened in binary-input-output mode, if it
		wasn't already.
		Returns -1 if this file couldn't be opened and -2 if the specified position
		is out of bounds. The constexpr sp::fileNotOpen (= -1) and sp::outOfBounds
		(= -2) can be used to check.
		*/
		Tspos getPosition(uint32 Line, uint32 Word, uint32 Char);


		/*
		Returns the number of lines (visible in a text editor: if the file is empty
		1 is returned, if there is one newline 2 is returned...). The pointer is
		not moved. This file is opened in binary-input-output mode, if it wasn't
		already.
		Returns 0 if this file couldn't be opened.
		*/
		uint32 getNrLines();
		/*
		Returns the number of words divided by spaces. The pointer is not moved.
		This file is opened in binary-input-output mode, if it wasn't already.
		Returns 0 if this file couldn't be opened.
		*/
		uint32 getNrWords();
		/*
		Returns the number of words in the line specified by the parameter divided
		by spaces. The pointer is not moved. This file is opened in
		binary-input-output mode, if it wasn't already.
		Returns 0 if this file couldn't be opened.
		*/
		uint32 getNrWords(uint32 Line);
		/*
		Returns the number of chars (or bytes) in this file. The pointer is not moved.
		Returns 0 if this file couldn't be accessed via "stat".
		*/
		uint32 getNrChars();
		/*
		Returns the number of chars in the line specified by the parameter. The
		pointer is not moved. This file is opened in binary-input-output mode, if
		it wasn't already.
		Returns 0 if this file couldn't be opened.
		*/
		uint32 getNrChars(uint32 Line);
		/*
		Returns the number of chars in a word (second parameter) in a line (first
		parameter). The pointer is not moved. This file is opened in
		binary-input-output mode, if it wasn't already.
		Returns 0 if this file couldn't be opened.
		*/
		uint32 getNrChars(uint32 Line, uint32 Word);



		/*
		Returns all the chars after the pointer until the end of the line or the
		end of this file are reached. This file must be open in binary-input or
		binary-input-output mode.
		Returns an empty string if this file is not open or the end was already
		reached.
		*/
		Tstr getLine();
		/*
		Saves on the parameter all the chars after the pointer until the end of the
		line or the end of this file are reached. This file must be open in
		binary-input or binary-input-output mode.
		Returns false if this file is not open or the end was already reached,
		otherwise returns true.
		*/
		bool getLine(Tstr &Line);
		/*
		Returns the line specified by the parameter. This file is opened in
		binary-input-output mode, if it wasn't already.
		Returns an empty string if this file couldn't be opened or if the specified
		line is out of bounds.
		*/
		Tstr getLine(uint32 Line);
		/*
		Returns all the chars after the pointer until a space or the end of this
		file are reached. This file must be open in binary-input mode or
		binary-input-output mode.
		Returns an empty string if this file is not open or the end was already
		reached.
		*/
		Tstr getWord();
		/*
		Saves on the parameter all the characters after the pointer until a space
		or the end of this file are reached. This file must be open in binary-input
		or binary-input-output mode.
		Returns false if this file is not open or the end was already reached,
		otherwise returns true.
		*/
		bool getWord(Tstr &Word);
		/*
		Returns the word specified by the parameter. This file is opened in
		binary-input-output mode, if it wasn't already.
		Returns an empty string if this file couldn't be opened or if the specified
		word is out of bounds.
		*/
		Tstr getWord(uint32 Word);
		/*
		Returns a word (second parameter) in a line (first parameter). This file is
		opened in binary-input-output mode, if it wasn't already.
		Returns an empty string if this file couldn't be opened or if the specified
		word is out of bounds.
		*/
		Tstr getWord(uint32 Line, uint32 Word);
		/*
		Returns the char at the pointer position. This file must be open in
		binary-input or binary-input-output mode.
		Returns -1 if this file is not open or the end was already reached.
		*/
		char getChar();
		/*
		Saves the char at the pointer position on the parameter. This file must be
		open in binary-input or binary-input-output mode.
		Returns false if this file is not open or the end was already reached,
		otherwise returns true.
		*/
		bool getChar(char &Char);
		/*
		Returns the char specified by the parameter. This file is opened in
		binary-input-output mode, if it wasn't already.
		Returns -1 if this file couldn't be opened or if the specified line is out
		of bounds.
		*/
		char getChar(uint32 Char);
		/*
		Returns a char (second parameter) in a line (first parameter). This file is
		opened in binary-input-output mode, if it wasn't already.
		Returns -1 if this file couldn't be opened or if the specified char is out
		of bounds.
		*/
		char getChar(uint32 Line, uint32 Char);
		/*
		Returns a char (third parameter) in a word (second parameter) in a line
		(first parameter). This file is opened in binary-input-output mode, if it
		wasn't already.
		Returns -1 if this file couldn't be opened or if the specified char is out
		of bounds.
		*/
		char getChar(uint32 Line, uint32 Word, uint32 Char);


		/*
		Returns the interval of lines between the first and the second parameter,
		both included. If the first parameter is bigger than the second the lines
		are returned in reverse order. If some (or all) lines are out of bounds
		they are ignored. This file is opened in binary-input-output mode, if it
		wasn't already.
		Returns an empty string if this file couldn't be opened.
		*/
		Tstr getLines(uint32 From, uint32 To);
		/*
		Returns the interval of words between the first and the second parameter,
		both included. If the first parameter is bigger than the second the words
		are returned in reverse order. If some (or all) words are out of bounds
		they are ignored. Returned words are separeted by spaces ' '. This file is
		opened in binary-input-output mode, if it wasn't already.
		Returns an empty string if this file couldn't be opened.
		*/
		Tstr getWords(uint32 From, uint32 To);
		/*
		Returns the interval of words in a line (first parameter) between the
		second and the third parameter, both included. If the second parameter is
		bigger than the third the words are returned in reverse order. If some (or
		all) words are out of bounds they are ignored. Returned words are separeted
		by spaces ' '. This file is opened in binary-input-output mode, if it
		wasn't already.
		Returns an empty string if this file couldn't be opened.
		*/
		Tstr getWords(uint32 Line, uint32 From, uint32 To);
		/*
		Returns the interval of chars between the first and the second parameter,
		both included. If the first parameter is bigger than the second the chars
		are returned in reverse order. If some (or all) chars are out of bounds
		they are ignored. This file is opened in binary-input-output mode, if it
		wasn't already.
		Returns an empty string if this file couldn't be opened.
		*/
		Tstr getChars(uint32 From, uint32 To);
		/*
		Returns the interval of chars in a line (first parameter) between the
		second and the third parameter, both included. If the second parameter is
		bigger than the third the chars are returned in reverse order. If some (or
		all) chars are out of bounds they are ignored. This file is opened in
		binary-input-output mode, if it wasn't already.
		Returns an empty string if this file couldn't be opened.
		*/
		Tstr getChars(uint32 Line, uint32 From, uint32 To);
		/*
		Returns the interval of chars in a word (second parameter) in a line (first
		parameter) between the third and the fourth parameter, both included. If
		the third parameter is bigger than the fourth the chars are returned in
		reverse order. If some (or all) chars are out of bounds they are ignored.
		This file is opened in binary-input-output mode, if it wasn't already.
		Returns an empty string if this file couldn't be opened.
		*/
		Tstr getChars(uint32 Line, uint32 Word, uint32 From, uint32 To);


		/*
		Inserts the second parameter in the position specified by the first. If the
		specified position is out of bounds some '\0' are added. This file is
		opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		template<typename T>
		bool add(Tspos Pos, T ToAdd);
		/*
		Inserts a line containing the second parameter before the line specified by
		the first. If the specified line is out of bounds some newlines are
		created. This file is opened in binary-input-output mode, if it wasn't
		already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		template<typename T>
		bool addLine(uint32 Line, T ToAdd);
		/*
		Inserts the content of the second parameter before the word specified by
		the first, with a space ' ' between them. This file is opened in
		binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened or if the specified position
		is out of bounds, otherwise returns true.
		*/
		template<typename T>
		bool addWord(uint32 Word, T ToAdd);
		/*
		Inserts the content of the third parameter before a word (second parameter)
		in a line (first parameter), with a space ' ' between them. This file is
		opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened or if the specified position
		is out of bounds, otherwise returns true.
		*/
		template<typename T>
		bool addWord(uint32 Line, uint32 Word, T ToAdd);
		/*
		Inserts the parameter at the pointer position. If the pointer position is
		out of bounds some '\0' are added. This file must be open in
		binary-input-output mode.
		Returns false if this file is not open or the end was already reached,
		otherwise returns true.
		*/
		bool addChar(char ToAdd);
		/*
		Inserts the second parameter in the position specified by the first. If the
		specified position is out of bounds some '\0' are added. This file is
		opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool addChar(uint32 Char, char ToAdd);
		/*
		Inserts the third parameter before a char (second parameter) in a line
		(first parameter). This file is opened in binary-input-output mode, if it
		wasn't already.
		Returns false if this file couldn't be opened or if the specified position
		is out of bounds, otherwise returns true.
		*/
		bool addChar(uint32 Line, uint32 Char, char ToAdd);
		/*
		Inserts the fourth parameter before a char (third parameter) in a word
		(second parameter) in a line (first parameter). This file is opened in
		binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened or if the specified position
		is out of bounds, otherwise returns true.
		*/
		bool addChar(uint32 Line, uint32 Word, uint32 Char, char ToAdd);



		/*
		Replaces the content of the line specified by the first parameter with the
		content of the second parameter. If the specified line is out of bounds
		some newlines are created. This file is opened in binary-input-output mode,
		if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		template<typename T>
		bool replaceLine(uint32 Line, T Replacement);
		/*
		Replaces the word specified by the first parameter with the content of the
		second parameter. This file is opened in binary-input-output mode, if it
		wasn't already.
		Returns false if this file couldn't be opened or if the specified position
		is out of bounds, otherwise returns true.
		*/
		template<typename T>
		bool replaceWord(uint32 Word, T Replacement);
		/*
		Replaces a word (second parameter) in a line (first parameter) with the
		content of the third parameter. This file is opened in binary-input-output
		mode, if it wasn't already.
		Returns false if this file couldn't be opened or if the specified position
		is out of bounds, otherwise returns true.
		*/
		template<typename T>
		bool replaceWord(uint32 Line, uint32 Word, T Replacement);
		/*
		Replaces the char at the pointer position with the parameter. If the
		pointer position is out of bounds some '\0' are added. This file must be
		open in binary-output or binary-input-output mode.
		Returns false if this file is not open, otherwise returns true.
		*/
		bool replaceChar(char Replacement);
		/*
		Replaces the char specified by the first parameter with the second
		parameter. If the specified position is out of bounds some '\0' are added.
		This file is opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool replaceChar(uint32 Char, char Replacement);
		/*
		Replaces a char (second parameter) in a line (first parameter) with the
		third parameter. This file is opened in binary-input-output mode, if it
		wasn't already.
		Returns false if this file couldn't be opened or if the specified position
		is out of bounds, otherwise returns true.
		*/
		bool replaceChar(uint32 Line, uint32 Char, char Replacement);
		/*
		Replaces a char (third parameter) in a word (second parameter) in a line
		(first parameter) with the fourth parameter. This file is opened in
		binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened or if the specified position
		is out of bounds, otherwise returns true.
		*/
		bool replaceChar(uint32 Line, uint32 Word, uint32 Char, char Replacement);


		/*
		Deletes the line specified by the parameter. If the specified line is out
		of bounds the function does nothing and returns true. This file is opened
		in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool deleteLine(uint32 Line);
		/*
		Deletes the word specified by the parameter and all the spaces around it,
		unless they are required as word dividers. If the specified word is out
		of bounds the function does nothing and returns true. This file is opened
		in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool deleteWord(uint32 Word);
		/*
		Deletes a word (second parameter) in a line (first parameter) and all the
		spaces around it, unless they are required as word dividers. If the
		specified word is out of bounds the function does nothing and returns true.
		This file is opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool deleteWord(uint32 Line, uint32 Word);
		/*
		Deletes the char at the pointer position. If the pointer position is out of
		bounds the function does nothing and returns true. This file must be open
		in binary-input-output mode.
		Returns false if this file is not open, otherwise returns true.
		*/
		bool deleteChar();
		/*
		Deletes the char specified by the parameter. If the specified char is out
		of bounds the function does nothing and returns true. This file is opened
		in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool deleteChar(uint32 Char);
		/*
		Deletes a char (second parameter) in a line (first parameter). If the
		specified char is out of bounds the function does nothing and returns true.
		This file is opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool deleteChar(uint32 Line, uint32 Char);
		/*
		Deletes a char (third parameter) in a word (second parameter) in a line
		(first parameter). If the specified char is out of bounds the function does
		nothing and returns true. This file is opened in binary-input-output mode,
		if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool deleteChar(uint32 Line, uint32 Word, uint32 Char);


		/*
		Deletes the interval of lines between the first and the second parameter,
		both included. If some (or all) lines are out of bounds they are ignored.
		This file is opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool deleteLines(uint32 From, uint32 To);
		/*
		Deletes the interval of words between the first and the second parameter,
		both included. Deletes all the spaces around the interval, unless they are
		required as word dividers. If some (or all) words are out of bounds they
		are ignored. This file is opened in binary-input-output mode, if it wasn't
		already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool deleteWords(uint32 From, uint32 To);
		/*
		Deletes the interval of words in a line (first parameter) between the
		second and the third parameter, both included. Deletes all the spaces
		around the interval, unless they are required as word dividers. If some (or
		all) words are out of bounds they are ignored. This file is opened in
		binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool deleteWords(uint32 Line, uint32 From, uint32 To);
		/*
		Deletes the interval of chars between the first and the second parameter,
		both included. If some (or all) chars are out of bounds they are ignored.
		This file is opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool deleteChars(uint32 From, uint32 To);
		/*
		Deletes the interval of chars in a line (first parameter) between the
		second and the third parameter, both included. If some (or all) chars are
		out of bounds they are ignored. This file is opened in binary-input-output
		mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool deleteChars(uint32 Line, uint32 From, uint32 To);
		/*
		Deletes the interval of chars in a word (second parameter) in a line (first
		parameter) between the third and the fourth parameter, both included. If
		some (or all) chars are out of bounds they are ignored. This file is opened
		in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool deleteChars(uint32 Line, uint32 Word, uint32 From, uint32 To);


		/*
		Appends the content of the parameter to the end of this file. This file is
		opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		template<typename T>
		bool append(T ToAppend);
		/*
		Appends a line containing the parameter to the end of this file. This file
		is opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		template<typename T>
		bool appendLine(T ToAppend);
		/*
		Appends the content of the parameter to the end of this file. Also adds a
		space ' ' before it, but only if this file didn't end with a space. This
		file is opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		template<typename T>
		bool appendWord(T ToAppend);
		/*
		Appends the content of the second parameter to the end of a line (first
		parameter). Also adds a space ' ' before it, but only if the line didn't
		end with a space. This file is opened in binary-input-output mode, if it
		wasn't already.
		Returns false if this file couldn't be opened or if the specified position
		is out of bounds, otherwise returns true.
		*/
		template<typename T>
		bool appendWord(uint32 Line, T ToAppend);
		/*
		Appends the parameter to the end of this file. This file is opened in
		binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool appendChar(char ToAppend);
		/*
		Appends the second parameter to the end of a line (first parameter). This
		file is opened in binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened or if the specified position
		is out of bounds, otherwise returns true.
		*/
		bool appendChar(uint32 Line, char ToAppend);
		/*
		Appends the third parameter to the end of a word (second parameter) in a
		line (first parameter). This file is opened in binary-input-output mode, if
		it wasn't already.
		Returns false if this file couldn't be opened or if the specified position
		is out of bounds, otherwise returns true.
		*/
		bool appendChar(uint32 Line, uint32 Word, char ToAppend);


		/*
		Removes all the empty lines at the end of this file. This file is opened in
		binary-input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened, otherwise returns true.		
		*/
		bool deleteLastEmptyLines();


		/*
		Creates this file using the currently set path, if it doesn't already
		exists. This file is opened in binary-input-output mode.
		Returs false if this file couldn't be created or opened, otherwise returns
		true.
		*/
		bool create();
		/*
		Moves this file to the path specified by the parameter, and changes the
		path. Overwrites the file specified by the parameter if it already exists.
		This file is opened in binary-input-output mode.
		Returns false if the file couldn't be moved, otherwise returns true.
		*/
		bool move(Tstr newPath);
		/*
		Copies this file to the path specified by the parameter. If the file
		specified by the parameter doesn't exist it is created. This file is opened
		in binary-input-output mode, if it wasn't already.
		Returns false if this file or the file specified by the parameter couldn't
		be opened, otherwise returns true.
		*/
		bool copy(Tstr copyPath);
		/*
		Overwrites the parameter with the content of this file. Creates the
		parameter if it doesn't exist. The newline mode is also copied to the
		parameter. This file and the parameter are opened in binary-input-output
		mode, if they weren't already.
		Returns false if this file or the parameter couldn't be opened, otherwise
		returns true.
		*/
		bool copy(File &toOverwrite);
		/*
		Swaps the content of this file with the content of the file specified by
		the parameter. Also sets the newline mode based on the operating system.
		This file and the parameter are opened in binary-input-output mode, if they
		weren't already.
		Returns false if this file or the file specified by the parameter couldn't
		be opened, otherwise returns true.
		*/
		bool swap(Tstr swapPath);
		/*
		Swaps the content of this file with the content of the parameter. Also
		swaps the newline modes. This file and the parameter are opened in
		binary-input-output mode, if they weren't already.
		Returns false if this file or the parameter couldn't be opened, otherwise
		returns true.
		*/
		bool swap(File &Other);
		/*
		Renames this file to the name specified by the parameter parameter, and
		changes the path. Overwrites the file specified by the parameter if it
		already exists. This file is opened in binary-input-output mode
		Returns false if the file couldn't be renamed, otherwise returns true.
		*/
		bool rename(Tstr newName);
		/*
		Deletes all the content of this file. If this file is open its pointer is
		moved to the beginning and its eofbit is cleared.
		Returns false if this file couldn't be truncated, otherwise returns true.
		*/
		bool truncate();
		/*
		Removes this file after closing it, if it was open.
		*/
		void remove();


		/*
		Returns the number of chars (or bytes) in this file. The pointer is not
		moved.
		Returns 0 if this file couldn't be accessed via "stat".
		*/
		uint32 size();
		/*
		Changes the size of this file to the size (in bytes) specified by the
		parameter. If the new size is smaller than the current size, the remainder
		of the file is discarded. If the new size is bigger than the current size
		the new area is filled with '\0'.
		Returns false if this file couldn't be resized, otherwise returns true.
		*/
		bool resize(uint32 newSize);


		/*
		Opens this file in binary-input-output mode using the currently set path.
		Clears all errors. If this file was already open clears all errors and
		returns true.
		Returns false if this file couldn't be opened, otherwise returns true.
		*/
		bool open();
		/*
		Closes this file if it was open. Clears all errors.
		*/
		void close();
		/*
		Returns true if this file is open, otherwise returns false
		*/
		bool isOpen() const;
		/*
		Syncronizes this file with its content on the disk
		*/
		void update();


		/*
		Returns true if this file exists, otherwise returns false
		Returns false if the existence of this file couldn't be verified.
		*/
		bool exists() const;
		/*
		Returns a struct stat object containing various filesystem infos about this
		file.
		*/
		Tstat info();


		/*
		Returns true if there are no errors, otherwise returns false
		*/
		bool good() const;
		/*
		Sets all the value of all errors to false: eofErr, failErr, badErr, extErr.
		*/
		void clear();
		/*
		Returns the value of the end-of-file (eof) error, that is true if the end
		of this file was reached.
		*/
		bool eofErr() const;
		/*
		Returns the value of the fail error, that is true if there were logical
		errors on input/output operations.
		*/
		bool failErr() const;
		/*
		Returns the value of the bad error, that is true if there were reading or
		writing errors on input/output operations.
		*/
		bool badErr() const;
		/*
		Returns the value of the external error, that is true if there were
		problems while using an external file.
		*/
		bool extErr() const;
		/*
		Returns an object that contains all the infos about this file's errors
		*/
		FileState state() const;


		/*
		Returns the path used to open this file
		*/
		Tstr getPath() const;
		/*
		Sets the path used to open this file to the parameter. Closes this file if
		it was open. Clears all errors.
		*/
		void setPath(Tstr Path);

		/*
		Reads all the chars after the pointer until a space or the end of this
		file are reached, formats them based on the parameter's type and saves them
		on the parameter. This file must be open in binary-input mode or
		binary-input-output mode. If this file is not open, the end was already
		reached or the read characters are not formattable based on the parameter's
		type the parameter is not modified.
		Returns *this.
		*/
		template<typename T>
		File& operator>> (T &Out);
		/*
		Inserts the content of this file at the beginning of the parameter. This
		file and the parameter are opened in binary-input-output mode, if they
		weren't already. The operation fails if this file or the parameter couldn't
		be opened.
		Returns *this.
		*/
		File& operator>> (File &Out);
		/*
		Reads all the chars after the pointer until a space or the end of this
		file are reached, formats them in an 8-bit signed int and saves them on the
		parameter. This file must be open in binary-input mode or
		binary-input-output mode. If this file is not open, the end was already
		reached or the read characters are not formattable based on the parameter's
		type the parameter is not modified.
		Returns *this.
		*/
		File& operator>> (int8 &Out);
		/*
		Reads all the chars after the pointer until a space or the end of this
		file are reached, formats them in an 8-bit unsigned int and saves them on
		the parameter. This file must be open in binary-input mode or
		binary-input-output mode. If this file is not open, the end was already
		reached or the read characters are not formattable based on the parameter's
		type the parameter is not modified.
		Returns *this.
		*/
		File& operator>> (uint8 &Out);


		/*
		Casts the parameter to a std::string and writes it to this file at the
		pointer position, replacing existing chars and adding new chars at the end
		of the file if the pointer is/goes out of bounds. This file must be open in
		binary-output mode or binary-input-output mode.
		Returns *this.
		*/
		template<typename T>
		File& operator<< (T In);
		/*
		Appends the content of the parameter to this file. This file and the
		parameter are opened in binary-input-output mode, if they weren't already.
		The operation fails if this file or the parameter couldn't be opened.
		Returns *this.
		*/
		File& operator<< (File &In);


		/*
		Copies the settings from the parameter into this file:
			path, tempPath, errors, but not the file content
		If the parameter is open the file will be opened too in binary input-output mode
		Returns *this
		*/
		File& operator= (File &Source);
		/*
		Replaces all the content of this file with the parameter
		The file is opened, if it wasn't already
		Leaves the file open in binary input-output mode
		Returns false if the file couldn't be opened, otherwise true
		*/
		template<typename T>
		bool operator= (T NewText);
		/*
		Returns a string with at the start the content of this
			file and then the content of the parameter
		The file is opened, if it wasn't already
		Leaves the file open in binary input-output mode
		If this file or the parameter file couldn't be opened
			the returned string won't include them
		*/
		Tstr operator+ (File &ToAdd);
		/*
		Returns a string with at the start the content
			of this file and then the parameter
		The file is opened, if it wasn't already
		Leaves the file open in binary input-output mode
		If this file couldn't be opened toAdd will be returned
		*/
		template<typename T>
		Tstr operator+ (T ToAdd);
		/*
		Appends the content of the parameter to this file
		The file is opened, if it wasn't already
		Leaves the file open in binary input-output mode
		If this file couldn't be opened nothing happens
		Returns *this
		*/
		File& operator+= (File &toAppend);
		/*
		Appends the parameter to this file
		The file is opened, if it wasn't already
		Leaves the file open in binary input-output mode
		If this file couldn't be opened nothing happens
		Returns *this
		*/
		template<typename T>
		File& operator+= (T toAppend);


		/*
		Returns a FilePosition object to get/modify the char
			using operator char and operator=
		*/
		const FilePosition operator[] (uint32 Position);
		/*
		Returns true if the content of the files
			is identic, otherwise false
		Returns false if a file couldn't be opened
		*/
		bool operator== (File &toCompare); //TODO add == (T toCompare)
		/*
		Returns false if the content of the files is identic, otherwise true
		Returns true if either this file or the parameter file couldn't be opened
		*/
		bool operator!= (File &toCompare);


		/*
		Returns a string containing all the file
		The file is opened, if it wasn't already
		Leaves the file open in binary input-output mode
		Returns "" if the file couldn't be opened
		*/
		operator Tstr();
		/*
		Returns a string containing all the file
		The file is opened, if it wasn't already
		Leaves the file open in binary input-output mode
		Returns "" if the file couldn't be opened
		*/
		Tstr str();
		/*
		Returns true if there aren't errors, otherwise false
		*/
		operator bool() const;
		/*
		Returns false if there aren't errors, otherwise true
		*/
		bool operator!() const;


		/*

		*/
		FileIterator begin();
		/*

		*/
		FileIterator end();
	};
	
	
	template<typename T>
	inline Tstr File::toString(T toConvert) {
		return static_cast<Tstr>(toConvert);
	}

	template<typename T>
	inline bool File::add(Tspos Pos, T ToAdd) {
		if (Pos < 0 || !open()) return false;
		Tstr toAdd = toString(ToAdd);
		uint32 fileSize = getNrChars();

		if (Pos < fileSize) {
			char tempChar;
			for (Tspos getPosition = fileSize - 1, putPosition = fileSize + toAdd.length() - 1; getPosition >= Pos; getPosition -= static_cast<Tspos>(1), putPosition -= static_cast<Tspos>(1)) {
				mainFile.seekg(getPosition);
				tempChar = mainFile.get();
				mainFile.seekg(putPosition);
				mainFile.put(tempChar);
			}
		}

		mainFile.seekg(Pos);
		mainFile << toAdd;
		mainFile.flush();
		return true;
	}
	template<typename T>
	inline bool File::addLine(uint32 Line, T ToAdd) {
		Tspos position = getPositionMove(Line, dontMove, dontMove);
		if (position == fileNotOpen) return false;
		Tstr newline = (newlineMode == Newline::CRLF ? "\r\n" : "\n");
		if (position == outOfBounds) {
			uint32 nrLines = getNrLines();
			Tstr strToAppend = "";
			for (uint32 currentLine = 0; currentLine <= Line - nrLines; ++currentLine) {
				strToAppend += newline;
			}
			return append(strToAppend + toString(ToAdd));
		}
		return add(position, toString(ToAdd) + newline);
	}
	template<typename T>
	inline bool File::addWord(uint32 Word, T ToAdd) {
		return addWord(dontMove, Word, ToAdd);
	}
	template<typename T>
	inline bool File::addWord(uint32 Line, uint32 Word, T ToAdd) {
		Tspos position = getPositionMove(Line, Word, dontMove);
		if (position < 0) return false;
		return add(position, toString(ToAdd) + " ");
	}

	template<typename T>
	inline bool File::replaceLine(uint32 Line, T Replacement) {
		Tspos from, to;
		from = getPositionMove(Line, dontMove, dontMove);
		if (from == fileNotOpen) return false;
		if (from == outOfBounds) {
			Tstr newline = (newlineMode == Newline::CRLF ? "\r\n" : "\n");
			uint32 nrLines = getNrLines();
			Tstr strToAppend = "";

			for (uint32 currentLine = 0; currentLine < Line - nrLines + 1; ++currentLine) {
				strToAppend += newline;
			}
			return append(strToAppend + Replacement);
		}
		to = getPositionMove(Line + 1, dontMove, dontMove);

		if (to == outOfBounds) {
			return replaceSection(from, getNrChars() - 1, toString(Replacement));
		}
		if (newlineMode == Newline::CRLF) {
			return replaceSection(from, to - static_cast<Tspos>(3), toString(Replacement));
		}
		else {
			return replaceSection(from, to - static_cast<Tspos>(2), toString(Replacement));
		}
	}
	template<typename T>
	inline bool File::replaceWord(uint32 Word, T Replacement) {
		return replaceWord(dontMove, Word, Replacement);
	}
	template<typename T>
	inline bool File::replaceWord(uint32 Line, uint32 Word, T Replacement) {
		Tspos from, to;
		from = getPositionMove(Line, Word, dontMove);
		if (from < 0) return false;

		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) return replaceSection(from, getNrChars() - 1, toString(Replacement));
			if (isspace(tempChar)) break;
		}
		return replaceSection(from, mainFile.tellg() - (Tspos)2, toString(Replacement));
	}

	template<typename T>
	inline bool File::append(T ToAppend) {
		if (!pointToEnd()) return false;

		mainFile << toString(ToAppend);

		mainFile.flush();
		return true;
	}
	template<typename T>
	inline bool File::appendLine(T ToAppend) {
		return append((newlineMode == Newline::CRLF ? "\r\n" : "\n") + toString(ToAppend));
	}
	template<typename T>
	inline bool File::appendWord(T ToAppend) {
		uint32 fileSize = getNrChars();
		if (fileSize != 0) {
			if (!pointTo(fileSize - 1)) return false;
			if (isspace(mainFile.get())) return append(toString(ToAppend));
			return append(" " + toString(ToAppend));
		}
		return append(toString(ToAppend));		
	}
	template<typename T>
	inline bool File::appendWord(uint32 Line, T ToAppend) {
		Tspos pos = getPositionMove(Line + 1, dontMove, dontMove);
		if (pos == fileNotOpen) return false;
		if (pos == outOfBounds) {
			if (Line >= getNrLines()) return false;
			return appendWord(ToAppend);
		}
		pos -= static_cast<Tspos>(newlineMode == Newline::CRLF ? 2 : 1);

		mainFile.seekg(pos - static_cast<Tspos>(1));
		if (isspace(mainFile.get())) return add(pos, ToAppend);
		return add(pos, " " + toString(ToAppend));
	}

	template<typename T>
	inline File & File::operator>>(T &Out) {
		mainFile >> Out;
		return *this;
	}
	template<typename T>
	inline File & File::operator<<(T In) {
		mainFile << toString(In);
		mainFile.flush();
		return *this;
	}
	
	template<typename T>
	inline bool File::operator=(T NewText) {
		sp::Tstr newText = toString(NewText);
		if (exists()) {
			if (!pointToBeg() || !resize(newText.length())) return false;
		}
		else {
			if (!create()) return false;
		}

		mainFile << newText;

		mainFile.flush();
		return true;
	}
	template<typename T>
	inline Tstr File::operator+(T ToAdd) {
		return str() + toString(ToAdd);
	}
	template<typename T>
	inline File & File::operator+=(T toAppend) {
		if (!pointToEnd()) return *this;
		mainFile << toString(toAppend);

		mainFile.flush();
		return *this;
	}

	Tstr operator+ (Tstr First, File &Second);
}

#endif
