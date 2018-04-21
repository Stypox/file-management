//TODO nelle funzioni get rimuovo automaticamente il '\r' alla fine della linea, quindi non farlo nelle replace/add (se uno volesse mettere apposta i '\r' cosi' lo puo' fare)
//TODO forse si puo' ottimizzare la scrittura su file scrivendo una stringa sola invece che due, es: "file << (Tstr + '\n');" invece che "file << Tstr << '\n';"
//TODO specificare nella descrizione delle funzioni "Leaves the file open in binary input-output mode", "The file is opened, if it wasn't already", "Might set ... error/bit", "Moves/not the pointer"
//TODO non so se serva ma forse bisogna fare file.clear() di tutto invece che solo l'eofbit perche' se c'e' il failbit impostato e il file e' gia' aperto non verra' mai chiuso ma sara' impossibile da leggere o scrivere
//TODO remove C-style conversions

//UTILI:
/*
max 75 chars x linea

main file - temp file

The main file is opened in binary input-output mode, if it wasn't already.
Returns false if the main file or the temp file couldn't be opened, otherwise returns true. // The main file must be open in binary-output mode.
Uses the temp file.
The pointer is not moved. // The pointer is moved to an untraceable position.
*/

#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <experimental\filesystem>

namespace sp {
	using Tstr = std::string;
	using Tfstm = std::fstream;
	using Tspos = std::streampos;
	using int8 = int8_t;
	using int16 = int16_t;
	using int32 = int32_t;
	using int64 = int64_t;
	using uint8 = uint8_t;
	using uint16 = uint16_t;
	using uint32 = uint32_t;
	using uint64 = uint64_t;


	enum class NLMode {
		win,
		unix
	};
#ifdef _WIN32
	constexpr NLMode defaultNewlineMode = NLMode::win;
#else
	constexpr NLMode defaultNewlineMode = NLMode::unix;
#endif

	constexpr uint32 dontMove = UINT32_MAX;
	 
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
		uint32_t position;
	public:
		FilePosition(File * file, uint32_t Position);

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
		uint32_t position;
	public:
		FileIterator(File * file, uint32_t Position);

		bool operator!= (const FileIterator& ToCompare);
		FilePosition operator* () const;
		void operator++();
	};

	class File {
	public:
		Tfstm mainFile;
		Tstr mainPath;
		bool ExternalError;

		NLMode newlineMode;


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
		position. The main file is opened in binary input-output
		mode, if it wasn't already.
		Returns -1 if the main file couldn't be opened and -2 if the specified
		position is out of bounds. The constexpr sp::fileNotOpen (= -1) and
		sp::outOfBounds (= -2) can be used to check.
		*/
		Tspos getPositionMove(uint32 Line, uint32 Word, uint32 Char);


		/*
		Replaces all the chars of an interval (both ends included) with a string.
		Uses a temp file only if the replacement is smaller than the interval to
		replace. The main file is opened in binary input-output mode, if it wasn't
		already.
		Returns false if the main file or the temp file (when used) couldn't be
		opened, otherwise returns true.
		*/
		bool replaceSection(Tspos From, Tspos To, Tstr Replacement);
		/*
		Deletes all the chars between From and To, both included. Uses the temp
		file. The main file is opened in binary input-output mode, if it wasn't
		already.
		Returns false if the main file or the temp file couldn't be opened,
		otherwise returns true.
		*/
		bool deleteSection(Tspos From, Tspos To);




	public:
		/*
		Default constructor. Initializes the main path to an empty string and the
		temp path to "temp.tmp". The newline mode is initialized to the first
		parameter, if provided, otherwise it is initialized based on the operating
		system.
		*/
		File(NLMode Mode = defaultNewlineMode);
		/*
		Constructor with a string. Initializes the main path to the first parameter
		and tempPath to (parameter + ".tmp"). The newline mode is initialized to
		the second parameter, if provided, otherwise it is initialized based on the
		operating system.
		*/
		File(Tstr MainPath, NLMode Mode = defaultNewlineMode);
		/*
		Copy constructor
		*/
		File(File & Source);
		/*
		Destructor. Closes the file.
		*/
		~File();

		/*
		Moves the pointer relative to the current position. The main file must be
		open in binary-input-output mode.
		Returns false if the main file is not open or the end was already reached,
		otherwise returns true.
		*/
		bool pointMove(Tspos Offset);
		/*
		Moves the pointer to the position specified by the parameter. The position
		starts from zero, that is the first char of the file is at position 0. The
		main file is opened in binary input-output mode, if it wasn't already.
		Returns false if the main file couldn't be opened or if the specified
		position is out of bounds, otherwise returns true.
		*/
		bool pointTo(Tspos Position);
		/*
		Moves the pointer to a char (third parameter) in a word (second parameter)
		in a line (first parameter). The indices start from 0, that is
		pointTo(0, 0, 0) points to the first char of the first word of the first
		line (not necessarily to the beginning of the file). While 0 means "move to
		the first", -1 means "don't move". So only pointTo(-1, -1, -1) will surely
		point to the beginning. The constexpr sp::dontMove can be used as -1. The 
		main file is opened in binary input-output mode, if it wasn't already.
		Returns false if the main file couldn't be opened or if the specified
		position is out of bounds, otherwise returns true.
		*/
		bool pointTo(uint32 Line, uint32 Word, uint32 Char);
		/*
		Moves the pointer to the beginning of the main file and clears its eofbit.
		The main file is opened in binary input-output mode, if it wasn't already.
		Returns false if the main file couldn't be opened, otherwise returns true.
		*/
		bool pointToBeg();
		/*
		Moves the pointer to the end of the main file and clears its eofbit.
		The main file is opened in binary input-output mode, if it wasn't already.
		Returns false if the main file couldn't be opened, otherwise returns true.
		*/
		bool pointToEnd();
		/*
		Returns the position of a char (third parameter) in a word (second
		parameter) in a line (first parameter). The indices start from 0, that is
		getPosition(0, 0, 0) returns the position of the first char of the first
		word of the first line (not necessarily 0). While 0 means "look for the
		first", -1 means "don't do anything". So only getPosition(-1, -1, -1) will
		surely return 0. The pointer is not moved. The constexpr sp::dontMove can
		be used as -1.The main file is opened inbinary input-output mode, if it
		wasn't already.
		Returns -1 if the main file couldn't be opened and -2 if the specified
		position is out of bounds. The constexpr sp::fileNotOpen (= -1) and
		sp::outOfBounds (= -2) can be used to check.
		*/
		Tspos getPosition(uint32 Line, uint32 Word, uint32 Char);


		/*
		Returns the number of lines (visible in a text editor: if the file is empty
		1 is returned, if there is one newline 2 is returned...). The pointer is
		not moved. The main file is opened in binary input-output mode, if it
		wasn't already.
		Returns 0 if the main file couldn't be opened.
		*/
		uint32 getNrLines();
		/*
		Returns the number of words divided by spaces. The pointer is not moved.
		The main file is opened in binary input-output mode, if it wasn't already.
		Returns 0 if the main file couldn't be opened.
		*/
		uint32 getNrWords();
		/*
		Returns the number of words in the line specified by the parameter divided
		by spaces. The pointer is not moved. The main file is opened in binary 
		input-output mode, if it wasn't already.
		Returns 0 if the main file couldn't be opened.
		*/
		uint32 getNrWords(uint32 Line);
		/*
		Returns the number of chars (or bytes). The pointer is not moved.
		Returns 0 if the main file couldn't be accessed via "stat".
		*/
		uint32 getNrChars();
		/*
		Returns the number of chars in the line specified by the parameter. The
		pointer is not moved. The main file is opened in binary input-output mode,
		if it wasn't already.
		Returns 0 if the main file couldn't be opened.
		*/
		uint32 getNrChars(uint32 Line);
		/*
		Returns the number of chars in a word (second parameter) in a line (first
		parameter). The pointer is not moved. The main file is opened in binary
		input-output mode, if it wasn't already.
		Returns 0 if the main file couldn't be opened.
		*/
		uint32 getNrChars(uint32 Line, uint32 Word);



		/*
		Returns all the chars after the pointer until the end of the line or the
		end of the main file are reached. The main file must be open in
		binary-input or binary-input-output mode.
		Returns an empty string if the main file is not open or the end was already
		reached.
		*/
		Tstr getLine();
		/*
		Saves on the parameter all the chars after the pointer until the end of the
		line or the end of the main file are reached. The main file must be open in
		binary-input or binary-input-output mode.
		Returns false if the main file is not open or the end was already reached,
		otherwise returns true.
		*/
		bool getLine(Tstr &Line);
		/*
		Returns the line specified by the parameter. The main file is opened in
		binary-input-output mode, if it wasn't already.
		Returns an empty string if the main file couldn't be opened or if the
		specified line is out of bounds.
		*/
		Tstr getLine(uint32 Line);
		/*
		Returns all the chars after the pointer until a space or the end of the
		main file are reached. The main file must be open in binary-input mode or
		binary-input-output mode.
		Returns an empty string if the main file is not open or the end was already
		reached.
		*/
		Tstr getWord();
		/*
		Saves on the parameter all the characters after the pointer until a space
		or the end of the main file are reached. The main file must be open in
		binary-input or binary-input-output mode.
		Returns false if the main file is not open or the end was already reached,
		otherwise returns true.
		*/
		bool getWord(Tstr &Word);
		/*
		Returns the word specified by the parameter. The main file is opened in
		binary-input-output mode, if it wasn't already.
		Returns an empty string if the main file couldn't be opened or if the
		specified word is out of bounds.
		*/
		Tstr getWord(uint32 Word);
		/*
		Returns a word (second parameter) in a line (first parameter). The main
		file is opened in binary-input-output mode, if it wasn't already.
		Returns an empty string if the main file couldn't be opened or if the
		specified word is out of bounds.
		*/
		Tstr getWord(uint32 Line, uint32 Word);
		/*
		Returns the char at the pointer position. The main file must be open in
		binary-input or binary-input-output mode.
		Returns -1 if the file is not open or the end was already reached.
		*/
		char getChar();
		/*
		Saves the char at the pointer position on the parameter. The main file must
		be open in binary-input or binary-input-output mode.
		Returns false if the main file is not open or the end was already reached,
		otherwise returns true.
		*/
		bool getChar(char &Char);
		/*
		Returns the char specified by the parameter. The main file is opened in
		binary-input-output mode, if it wasn't already.
		Returns -1 if the main file couldn't be opened or if the specified line is
		out of bounds.
		*/
		char getChar(uint32 Char);
		/*
		Returns a char (second parameter) in a line (first parameter). The main
		file is opened in binary-input-output mode, if it wasn't already.
		Returns -1 if the main file couldn't be opened or if the specified char is
		out of bounds.
		*/
		char getChar(uint32 Line, uint32 Char);
		/*
		Returns a char (third parameter) in a word (second parameter) in a line
		(first parameter). The main file is opened in binary-input-output mode, if
		it wasn't already.
		Returns -1 if the main file couldn't be opened or if the specified char is
		out of bounds.
		*/
		char getChar(uint32 Line, uint32 Word, uint32 Char);


		/*
		Returns the interval of lines between the first and the second parameter,
		both included. If the first parameter is bigger than the second the lines
		are returned in reverse order. If some (or all) lines are out of bounds
		they get ignored. The lines are always separeted by '\n'. The main file is
		opened in binary-input-output mode, if it wasn't already.
		Returns an empty string if the main file couldn't be opened.
		*/
		Tstr getLines(uint32 From, uint32 To);
		/*
		Returns the interval of words between the first and the second parameter,
		both included. If the first parameter is bigger than the second the words
		are returned in reverse order. If some (or all) words are out of bounds
		they get ignored. The words are separeted by spaces ' '. The main file is
		opened in binary-input-output mode, if it wasn't already.
		Returns an empty string if the main file couldn't be opened.
		*/
		Tstr getWords(uint32 From, uint32 To);
		/*
		Returns the interval of words in a line (first parameter) between the second and the third
		parameter, both included. If the second parameter is bigger than the third
		the words are returned in reverse order. If some (or all) words are out of
		bounds they get ignored. The words are separeted by spaces ' '. The main
		file is opened in binary-input-output mode, if it wasn't already.
		Returns an empty string if the main file couldn't be opened.
		*/
		Tstr getWords(uint32 Line, uint32 From, uint32 To);
		/*
		Returns the interval of chars between the first and the second parameter,
		both included. If the first parameter is bigger than the second the chars
		are returned in reverse order. If some (or all) chars are out of bounds
		they get ignored. The main file is opened in binary-input-output mode, if
		it wasn't already.
		Returns an empty string if the main file couldn't be opened.
		*/
		Tstr getChars(uint32 From, uint32 To);
		/*
		Returns the interval of chars in a line (first parameter) between the
		second and the third parameter, both included. If the second parameter is
		bigger than the third the chars are returned in reverse order. If some (or
		all) chars are out of bounds they get ignored. The main file is opened in
		binary-input-output mode, if it wasn't already.
		Returns an empty string if the main file couldn't be opened.
		*/
		Tstr getChars(uint32 Line, uint32 From, uint32 To);
		/*
		Returns the interval of chars in a word (second parameter) in a line (first
		parameter) between the third and the fourth parameter, both included. If the
		third parameter is bigger than the fourth the chars are returned in reverse
		order. If some (or all) chars are out of bounds they get ignored. The main
		file is opened in binary-input-output mode, if it wasn't already.
		Returns an empty string if the main file couldn't be opened.
		*/
		Tstr getChars(uint32 Line, uint32 Word, uint32 From, uint32 To);


		/*
		Inserts the second parameter in the position specified by the first. The
		main file is opened in binary-input-output mode, if it wasn't already.
		Returns false if the main file couldn't be opened or if the specified
		position is out of bounds, otherwise returns true.
		*/
		template<typename T>
		bool add(Tspos Pos, T ToAdd);
		/*
		Adds a line using a temp file
		If the specified line is out of bounds some newlines get created FARE
		Closes files before returning, since they were opened in a not-default way
		Returns false if the files couldn't be opened, otherwise true
		*/
		template<typename T>
		bool addLine(uint32 Line, T ToAdd);
		/*
		Adds a word using a temp file
		Closes files before returning, since they were opened in a not-default way
		Returns false if the files couldn't be opened or if the
			specified word is out of bounds, otherwise true
		*/
		template<typename T>
		bool addWord(uint32 Word, T ToAdd);
		/*
		Adds a word in a line using a temp file
		Closes files before returning, since they were opened in a not-default way
		Returns false if the files couldn't be opened or if the
			specified word is out of bounds, otherwise true
		*/
		template<typename T>
		bool addWord(uint32 Line, uint32 Word, T ToAdd);
		/*
		Adds a char
		Returns false if the file couldn't be opened or if the
			specified char is out of bounds, otherwise true
		*/
		bool addChar(uint32 Char, char ToAdd);
		/*
		Adds a char in a line
		Returns false if the file couldn't be opened or if the
		specified char is out of bounds, otherwise true
		*/
		bool addChar(uint32 Line, uint32 Char, char ToAdd);
		/*
		Adds a char in a word in a line
		Returns false if the file couldn't be opened or if the
		specified char is out of bounds, otherwise true
		*/
		bool addChar(uint32 Line, uint32 Word, uint32 Char, char ToAdd);



		/*
		Replaces the char at the pointer position with another char. The main file
		must be open in binary-output mode.
		Returns *this.
		*/
		File& replace(char ToPut);
		/*
		Replaces a line using a temp file
		Deletes all '\r' at the end of the line FARE
		If the specified line is out of bounds some newlines get created
		Closes files before returning, since they were opened in a not-default way
		Returns false if the files couldn't be opened, otherwise true
		*/
		template<typename T>
		bool replaceLine(uint32 Line, T Replacement);
		/*
		Replaces a word using a temp file
		Closes files before returning, since they were opened in a not-default way
		Returns false if the files couldn't be opened or if the
			specified word is out of bounds, otherwise true
		*/
		template<typename T>
		bool replaceWord(uint32 Word, T Replacement);
		/*
		Replaces a word in a line using a temp file
		Closes files before returning, since they were opened in a not-default way
		Returns false if the files couldn't be opened or if the
			specified word is out of bounds, otherwise true
		*/
		template<typename T>
		bool replaceWord(uint32 Line, uint32 Word, T Replacement);
		/*
		Replaces a char
		Leaves the file open after returning
		Returns false if the file couldn't be opened or if the
			specified char is out of bounds, otherwise true
		*/
		bool replaceChar(uint32 Char, char Replacement);
		/*
		Replaces a char in a line
		Returns false if the file couldn't be opened or if the
			specified char is out of bounds, otherwise true
		*/
		bool replaceChar(uint32 Line, uint32 Char, char Replacement);
		/*
		Replaces a char in a word in a line
		Returns false if the file couldn't be opened or if the
			specified char is out of bounds, otherwise true
		*/
		bool replaceChar(uint32 Line, uint32 Word, uint32 Char, char Replacement);


		/*
		Deletes a line using a temp file
		If the specified line is out of bounds nothing happens, returning true
		Closes files before returning, since they were opened in a not-default way
		Returns false if the files couldn't be opened, otherwise true
		*/
		bool deleteLine(uint32 Line);
		/*
		Deletes a word and all the spaces after it using a temp file
		The spaces are deleted if it's not \r
		If the specified word is out of bounds nothing happens, returning true
		Closes files before returning, since they were opened in a not-default way
		Returns false if the files couldn't be opened, otherwise true
		*/
		bool deleteWord(uint32 Word);
		/*
		Deletes a word and a space using a temp file
		The space is deleted after if it's not \r, otherwise before
		If the specified word is out of bounds nothing happens, returning true
		Closes files before returning, since they were opened in a not-default way
		Returns false if the files couldn't be opened, otherwise true
		*/
		bool deleteWord(uint32 Line, uint32 Word);
		/*
		Deletes the char at the pointer position. Uses the temp file. The main file
		must be open.
		Returns false if the main file or the temp file couldn't be opened or if
		the main file is not open, otherwise returns true.
		TODO test
		*/
		bool deleteChar();
		/*
		Deletes a char using a temp file
		If the specified char is out of bounds nothing happens, returning true
		Closes files before returning, since they were opened in a not-default way
		Returns false if the files couldn't be opened, otherwise true
		*/
		bool deleteChar(uint32 Char);
		/*
		Deletes a char in a line using a temp file
		If the specified char is out of bounds nothing happens, returning true
		Closes files before returning, since they were opened in a not-default way
		Returns false if the files couldn't be opened, otherwise true
		*/
		bool deleteChar(uint32 Line, uint32 Char);
		/*
		Deletes a char in a word in a line using a temp file
		If the specified char is out of bounds nothing happens, returning true
		Closes files before returning, since they were opened in a not-default way
		Returns false if the files couldn't be opened, otherwise true
		*/
		bool deleteChar(uint32 Line, uint32 Word, uint32 Char);


		bool deleteLines(uint32 From, uint32 To);
		bool deleteWords(uint32 From, uint32 To);
		bool deleteWords(uint32 Line, uint32 From, uint32 To);
		bool deleteChars(uint32 From, uint32 To);
		bool deleteChars(uint32 Line, uint32 From, uint32 To);
		bool deleteChars(uint32 Line, uint32 Word, uint32 From, uint32 To);


		template<typename T>
		bool append(T ToAppend);
		/*
		Adds a line (with endline before it) at the end of the file
		Returns false if the file couldn't be opened, otherwise true
		*/
		template<typename T>
		bool appendLine(T ToAppend);
		/*
		Adds a word (with a space before it) at the end of the file
		Returns false if the file couldn't be opened, otherwise true
		*/
		template<typename T>
		bool appendWord(T ToAppend);
		/*
		Adds a word (with a space before it) after the specified word
		Returns false if the file couldn't be opened, otherwise true
		*/
		template<typename T>
		bool appendWord(uint32 Line, T ToAppend);
		/*
		Adds a char at the end of the file
		Returns false if the file couldn't be opened, otherwise true
		*/
		bool appendChar(char ToAppend);
		/*
		Adds a char at the end of the specified line
		Returns false if the file couldn't be opened, otherwise true
		*/
		bool appendChar(uint32 Line, char ToAppend);
		/*
		Adds a char at the end of the specified word
		Returns false if the file couldn't be opened, otherwise true
		*/
		bool appendChar(uint32 Line, uint32 Word, char ToAppend);


		/*
		Removes all newlines ('\r' and '\n') at the end using a temp file
		Closes files before returning, since they were opened in a not-default way
		Returns false if the files couldn't be opened, otherwise true
		*/
		bool deleteLastEmptyLines();


		/*
		Creates the file using the current path
		Leaves the file open in binary input-output mode
		Returns false if it couldn't be created/opened, otherwise true
		*/
		bool create();
		/*
		Moves all the file's content to the new location
		Leaves the file open in binary input-output mode
		Returns false if either the old path file or the new path file
			couldn't be opened, otherwise true
		*/
		bool move(Tstr newPath);
		/*
		Overwrites newFile with this file's content
		Leaves all files open in binary input-output mode
		Can set ExternalError
		Returns false if either the parameter or the file
			couldn't be opened, otherwise true
		*/
		bool moveContent(File &toOverwrite);
		/*
		Swaps the content of this file with toSwap using a temp file
		Leaves all files open in binary input-output mode
		Can set ExternalError and TempError
		Returns false if either the parameter, the file or the
			temp file couldn't be opened, otherwise true
		*/
		bool swap(File &Other);
		/*
		Replaces the filename, extension included, with the new name
		Leaves the file open in binary input-output mode
		Returns false if either the old path file or the new path file
			couldn't be opened, otherwise true
		*/
		bool rename(Tstr newName);
		/*
		Deletes all the file's content
		Leaves the file open in binary input-output mode
		Returns false if the file couldn't be opened, otherwise true
		*/
		bool truncate();
		/*
		Removes the file
		Automatically closes the file if it's open
		*/
		void remove();

		
		uint32 size();
		bool resize(uint32 newSize);


		/*
		Opens the file in binary input-output mode
		If the file doesn't exist it won't be created
		Returns false if the file couldn't be opened, otherwise true
		*/
		bool open();
		/*
		Closes the file and clears all errors if it wasn't already closed
		*/
		void close();
		/*
		Returns true if the file is open, otherwise false
		*/
		bool isOpen() const;
		/*
		Flushes the file
		*/
		void update();


		/*
		Returns true if the file exists, otherwise false
		*/
		bool exists() const;
		/*
		Returns a stat object containing various infos about the file
		*/
		struct stat info();


		/*
		Returns true if there aren't errors
		*/
		bool good() const;
		/*
		Clears all the errors: eofbit, failbit, badbit, TempError and ExternalError
		*/
		void clear();
		/*
		Returns the file's eofbit, that is true if the end of file was reached
		*/
		bool eofErr() const;
		/*
		Sets the file's end-of-file error state flag (eofbit) to a value
		*/
		void eofErr(bool Value);
		/*
		Returns true if either failbit or badbit are set
		Failbit is true if there were logical errors
			Badbit is true if there were reading or writing errors
		*/
		bool failErr() const;
		/*
		Sets the file's logical error state flag (failbit) to a value
		*/
		void failErr(bool Value);
		/*
		Returns the file's badbit, that is true if there were reading/writing errors
		*/
		bool badErr() const;
		/*
		Sets the file's reading/writing error state flag (badbit) to a value
		*/
		void badErr(bool Value);
		/*
		Returns true if there were errors while using an external file
		*/
		bool extErr() const;
		/*
		Sets the errors caused by external files state to a value
		*/
		void extErr(bool Value);
		/*
		Returns an object that contains all the infos about the file's errors
		*/
		FileState state() const;


		/*
		Returns the path used to open the file
		*/
		Tstr getPath() const;
		/*
		Modifies the path used to open the file
		Closes the file if it is open
		*/
		void setPath(Tstr Path);


		template<typename T>
		File& operator>> (T Out);
		/*
		Affixes the content of the file to the parameter using a temp file
		Leaves both files open in binary input-output mode
		The operation fails if the file, the temp file or the parameter file couldn't be opened
		If the temp file couldn't be opened TempError is set to 1
		If the parameter file couldn't be opened ExternalError is set to 1
		Returns *this
		*/
		File& operator>> (File &Out);
		File& operator>> (int8 &Out);
		File& operator>> (uint8 &Out);


		template<typename T>
		File& operator<< (T In);
		/*
		Appends the content of the parameter to the file
		Leaves both files open in binary input-output mode
		The operation fails if the file or the parameter file couldn't be opened
		If the parameter file couldn't be opened ExternalError is set to 1
		Returns *this
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
		bool operator== (File &toCompare);
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
		if (!pointTo(Pos)) return false;
		std::string toAdd = toString(ToAdd);
		uint32_t fileLength = getNrChars();

		for (Tspos pointerPosition = Pos; pointerPosition < fileLength; pointerPosition += 1) {
			mainFile.seekg(pointerPosition);
			toAdd.push_back(mainFile.get());

			mainFile.seekg(pointerPosition);
			mainFile.put(toAdd[0]);
			toAdd.erase(0, 1);
		}

		mainFile << toAdd;
		mainFile.flush();
		return true;
	}
	template<typename T>
	inline bool File::addLine(uint32 Line, T ToAdd) {
		Tspos position = getPositionMove(Line, dontMove, dontMove);
		if (position == fileNotOpen) return false;
		Tstr newLine = (newlineMode == NLMode::win ? "\r\n" : "\n");
		if (position == outOfBounds) {
			uint32 nrLines = getNrLines();
			Tstr strToAppend = "";
			for (uint32 currentLine = 0; currentLine < Line - nrLines; ++currentLine) {
				strToAppend += newLine;
			}
			return append(strToAppend + ToAdd);
		}
		return add(position, toString(ToAdd) + newLine);
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
		if (position == fileNotOpen) return false;
		if (position == outOfBounds) {
			Tstr newLine = (newlineMode == NLMode::win ? "\r\n" : "\n");
			uint32 nrLines = getNrLines();
			Tstr strToAppend = "";

			for (uint32 currentLine = 0; currentLine < Line - nrLines; ++currentLine) {
				strToAppend += newLine;
			}
			return append(strToAppend + Replacement);
		}
		to = getPositionMove(Line + 1, dontMove, dontMove);

		if (to == outOfBounds) {
			return replaceSection(from, getNrChars() - 1, toString(Replacement));
		}
		return replaceSection(from, to - (Tspos)3, toString(Replacement));
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
		return append((newlineMode == NLMode::win ? "\r\n" : "\n") + toString(ToAppend));
	}
	template<typename T>
	inline bool File::appendWord(T ToAppend) {
		return append(" " + toString(ToAppend));
	}
	template<typename T>
	inline bool File::appendWord(uint32 Line, T ToAppend) {
		if (!pointTo(Line, dontMove, dontMove)) return false;

		char tempChar;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) return append(" " + toString(ToAppend));
			if (tempChar == '\r') break;
		}

		return add(mainFile.tellg() - (Tspos)1, " " + toString(ToAppend));
	}

	template<typename T>
	inline File & File::operator>>(T Out) {
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
		if (!truncate()) return false;

		mainFile << toString(NewText);

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