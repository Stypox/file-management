//TODO clear() after a function runs without errors
//TODO remove C-style conversion
//TODO implement unix-windows conversion
//TODO add pointToEnd(index);
//TODO pointTo(inBounds line, inBounds word, outOfBounds char) shall not point to end, if not needed by functions IMPORTANT !
//TODO remove main file: now there is only one type of file
//TODO implement error system
//TODO newline mode auto-detection in constructor (add into Newline enum an Autodetect option)
//TODO (maybe) remove open references: the class should handle all of that automatically
//   * change functions that requires the file to be open

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



	/*
	Converts the parameter to an std::string object and returns it
	*/
	template<typename T>
	Tstr toString(T toConvert) {
		return static_cast<Tstr>(toConvert);
	}
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


	
	class File {
	public:
		class Char {
		private:
			File * file;
			const uint32 position;
		public:
			/*
			Constructor that takes the file containing the char (first parameter)
			and its position (second parameter).
			*/
			Char(File * file, const uint32 Position);
			
			/*
			Returns the corresponding char. See File::getChar(uint32).
			*/
			operator char() const;
			/*
			Replaces the corresponding char with the parameter. See
			File::replaceChar(uint32, char).
			*/
			void operator=(char Replacement) const;
		};
		class Chars {
		public:
			class Iterator {
			private:
				File * file;
				uint32 position;
			public:
				/*
				Constructor that takes a file (first parameter) and the starting
				position of this iterator (second parameter).
				*/
				Iterator(File * file, const uint32 Position);

				/*
				Returns true if the position of this iterator and the position of
				the parameter are different, otherwise returns false.
				*/
				bool operator!=(const Iterator& ToCompare) const;
				/*
				Returns a File::Char object pointing to the position of this
				iterator that can be used to read or replace the corresponding
				char.
				*/
				Char operator*() const;
				/*
				Increases by one the position of this iterator.
				*/
				void operator++();
			};
		private:
			File * file;
		public:
			/*
			Constructor that takes the file containing the chars.
			*/
			Chars(File * file);

			/*
			Returns a File::Char object pointing to the position specified by the
			parameter that can be used to read or replace the corresponding char.
			*/
			Char operator[](uint32 Position) const;
			/*
			Returns a File::Chars::Iterator object that points to the first char.
			*/
			Iterator begin() const;
			/*
			Returns a File::Chars::Iterator object that points to the char after
			the last one.
			*/
			Iterator end() const;
		};

		class Word {
		private:
			File * file;
			const uint32 position;
		public:
			/*
			Constructor that takes the file containing the word (first parameter)
			and its position (second parameter).
			*/
			Word(File * file, const uint32 Position);
			
			/*
			Returns the corresponding word. See File::getWord(uint32).
			*/
			operator Tstr() const;
			/*
			Replaces the corresponding word with the parameter. See
			File::replaceWord(uint32, T).
			*/
			template<typename T>
			void operator=(T Replacement) const;
		};
		class Words {
		public:
			class Iterator {
			private:
				File * file;
				uint32 position;
			public:
				/*
				Constructor that takes a file (first parameter) and the starting
				position of this iterator (second parameter).
				*/
				Iterator(File * file, const uint32 Position);

				/*
				Returns true if the position of this iterator and the position of
				the parameter are different, otherwise returns false.
				*/
				bool operator!=(const Iterator& ToCompare) const;
				/*
				Returns a File::Word object pointing to the position of this
				iterator that can be used to read or replace the corresponding
				word.
				*/
				Word operator*() const;
				/*
				Increases by one the position of this iterator.
				*/
				void operator++();
			};
		private:
			File * file;
		public:
			/*
			Constructor that takes the file containing the words.
			*/
			Words(File * file);

			/*
			Returns a File::Word object pointing to the position specified by the
			parameter that can be used to read or replace the corresponding word.
			*/
			Word operator[](uint32 Position) const;
			/*
			Returns a File::Words::Iterator object that points to the first word.
			*/
			Iterator begin() const;
			/*
			Returns a File::Words::Iterator object that points to the word after
			the last one.
			*/
			Iterator end() const;
		};

		class Line {
		private:
			File * file;
			const uint32 position;
		public:
			/*
			Constructor that takes the file containing the line (first parameter)
			and its position (second parameter).
			*/
			Line(File * file, const uint32 Position);
			
			/*
			Returns the corresponding line. See File::getLine(uint32).
			*/
			operator Tstr() const;
			/*
			Replaces the corresponding line with the parameter. See
			File::replaceLine(uint32, T).
			*/
			template<typename T>
			void operator=(T Replacement) const;
		};
		class Lines {
		public:
			class Iterator {
			private:
				File * file;
				uint32 position;
			public:
				/*
				Constructor that takes a file (first parameter) and the starting
				position of this iterator (second parameter).
				*/
				Iterator(File * file, const uint32 Position);

				/*
				Returns true if the position of this iterator and the position of
				the parameter are different, otherwise returns false.
				*/
				bool operator!=(const Iterator& ToCompare) const;
				/*
				Returns a File::Line object pointing to the position of this
				iterator that can be used to read or replace the corresponding
				line.
				*/
				Line operator*() const;
				/*
				Increases by one the position of this iterator.
				*/
				void operator++();
			};
		private:
			File * file;
		public:
			/*
			Constructor that takes the file containing the lines.
			*/
			Lines(File * file);

			/*
			Returns a File::Line object pointing to the position specified by the
			parameter that can be used to read or replace the corresponding line.
			*/
			Line operator[](uint32 Position) const;
			/*
			Returns a File::Lines::Iterator object that points to the first line.
			*/
			Iterator begin() const;
			/*
			Returns a File::Lines::Iterator object that points to the line after
			the last one.
			*/
			Iterator end() const;
		};

		struct State {
		public:
			bool open, eofError, failError, badError, externalError;

			/*
			Constructor that takes, in order: whether the file is open or not,
			end-of-file error, fail error, bad error, external error.
			*/
			State(bool Open, bool EofError, bool FailError, bool BadError, bool ExternalError);

			/*
			Returns true if there are no errors, otherwise returns false.
			*/
			operator bool() const;
			/*
			Saves on a std::string all the errors and returns it.
			*/
			std::string str() const;
			/*
			Saves on a std::string all the errors and returns it.
			*/
			operator Tstr() const;
			/*
			Saves on the parameter all the errors.
			*/
			void save(File &file) const;
		};

	public:
		Tfstm mainFile;
		Tstr mainPath;
		bool externalError;

		Newline newlineMode;


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
		File(File &Source);
		/*
		Assignment operator.
		Returns *this.
		*/
		File& operator= (File &Source);
		/*
		Move constructor.
		*/
		File(File &&Source);
		/*
		Move assignment operator.
		Returns *this.
		*/
		File& operator= (File &&Source);
		/*
		Destructor. Closes this file.
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
		starts from zero, that is the first char of this file is at position 0.
		This file is opened in binary input-output mode, if it wasn't already.
		Returns false if this file couldn't be opened or if the specified position
		is out of bounds, otherwise returns true.
		*/
		bool pointTo(Tspos Position);
		/*
		Moves the pointer to a char (third parameter) in a word (second parameter)
		in a line (first parameter). The indices start from 0, that is
		pointTo(0, 0, 0) points to the first char of the first word of the first
		line (not necessarily to the beginning of this file). While 0 means "move
		to the first", -1 means "don't move". So only pointTo(-1, -1, -1) will
		surely point to the beginning. The constexpr sp::dontMove can be used as
		-1. This file is opened in binary-input-output mode, if it wasn't already.
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
		Returns the number of lines (visible in a text editor: if this file is
		empty 1 is returned, if there is one newline 2 is returned...). The pointer
		is not moved. This file is opened in binary-input-output mode, if it wasn't
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
		path. Overwrites this file specified by the parameter if it already exists.
		This file is opened in binary-input-output mode.
		Returns false if this file couldn't be moved, otherwise returns true.
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
		Returns false if this file couldn't be renamed, otherwise returns true.
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
		of this file is discarded. If the new size is bigger than the current size
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
		State state() const;


		/*
		Returns the path used to open this file
		*/
		Tstr getPath() const;
		/*
		Sets the path used to open this file to the parameter. Closes this file if
		it was open. Clears all errors.
		*/
		void setPath(const Tstr Path);


		/*
		Reads all the chars after the pointer until a space or the end of this
		file are reached, formats them based on the parameter's type and saves them
		on the parameter. This file is opened in binary-input-output mode, if it
		wasn't already. If this file couldn't be opened, the end of this file was
		already reached or the read characters are not formattable based on the
		parameter's type the parameter is set to 0, if possible.
		Returns *this.
		*/
		template<typename T>
		File& operator>> (T &Out);
		/*
		Reads all the chars after the pointer until a space or the end of this
		file are reached, formats them in an 8-bit signed int and saves them on the
		parameter. This file is opened in binary-input-output mode, if it wasn't
		already. If this file couldn't be opened, the end of this file was
		already reached or the read characters are not formattable in an 8-bit
		signed int the parameter is set to 0.
		Returns *this.
		*/
		File& operator>> (int8 &Out);
		/*
		Reads all the chars after the pointer until a space or the end of this
		file are reached, formats them in an 8-bit unsigned int and saves them on
		the parameter. This file is opened in binary-input-output mode, if it
		wasn't already. If this file couldn't be opened, the end of this file was
		already reached or the read characters are not formattable in an 8-bit
		unsigned int the parameter is set to 0.
		Returns *this.
		*/
		File& operator>> (uint8 &Out);
		/*
		Inserts the content of this file at the beginning of the parameter. This
		file and the parameter are opened in binary-input-output mode, if they
		weren't already. The operation fails if this file or the parameter couldn't
		be opened.
		Returns *this.
		*/
		File& operator>> (File &Out);


		/*
		Casts the parameter to a std::string and writes it to this file at the
		pointer position, replacing existing chars and adding new chars at the end
		of this file if the pointer is/moves out of bounds. This file is opened in
		binary-input-output mode, if it wasn't already. The operation fails if this
		file couldn't be opened.
		Returns *this.
		*/
		template<typename T>
		File& operator<< (T In);
		/*
		Appends the content of the parameter to the end of this file. This file and
		the parameter are opened in binary-input-output mode, if they weren't
		already. The operation fails if this file or the parameter couldn't be
		opened.
		Returns *this.
		*/
		File& operator<< (File &In);


		/*
		Replaces all the content of this file with the content of the parameter.
		Creates this file if it doesn't exist. This file is opened in
		binary-input-output mode, if it wasn't already. The operation fails if this
		file couldn't be created or opened.
		Returns *this.
		*/
		template<typename T>
		File& operator= (T NewContent);
		/*
		Returns the content of this file concatenated with the content of the
		parameter. This file is opened in binary-input-output mode, if it wasn't
		already.
		If this file couldn't be opened returns only the content of the parameter.
		*/
		template<typename T>
		Tstr operator+ (T ToAdd);
		/*
		Appends the content of the parameter to the end of this file. This file and
		the parameter are opened in binary-input-output mode, if they weren't
		already. The operation fails if this file or the parameter couldn't be
		opened.
		Returns *this.
		*/
		File& operator+= (File &toAppend);
		/*
		Appends the content of the parameter to the end of this file. This file is
		opened in binary-input-output mode, if it wasn't already. The operation
		fails if this file couldn't be opened.
		Returns *this.
		*/
		template<typename T>
		File& operator+= (T toAppend);


		/*
		Returns true if the content of this file is identic to the content of the
		parameter, otherwise returns false. This file and the parameter are opened
		in binary-input-output mode, if they weren't already.
		Returns false if this file or the parameter couldn't be opened.
		*/
		bool operator== (File &ToCompare);
		/*
		Returns true if the content of this file is identic to the content of the
		parameter. This file is opened in binary-input-output mode, if it wasn't
		already.
		Returns false if this file couldn't be opened.
		*/
		template<typename T>
		bool operator== (T ToCompare);
		/*
		Returns true if the content of this file is different from the content of
		the parameter, otherwise returns false. This file and the parameter are
		opened in binary-input-output mode, if they weren't already.
		Returns true if this file or the parameter couldn't be opened.
		*/
		bool operator!= (File &ToCompare);
		/*
		Returns true if the content of this file is different from the content of
		the parameter, otherwise returns false. This file is opened in
		binary-input-output mode, if it wasn't already.
		Returns true if this file couldn't be opened.
		*/
		template<typename T>
		bool operator!= (T ToCompare);


		/*
		Saves the content of this file in an std::string and returns it. This file
		is opened in binary-input-output mode, if it wasn't already.
		Returns an empty string if this file couldn't be opened.
		*/
		operator Tstr();
		/*
		Saves the content of this file in an std::string and returns it. This file
		is opened in binary-input-output mode, if it wasn't already.
		Returns an empty string if this file couldn't be opened.
		*/
		Tstr str();
		/*
		Returns true if this file is ready for input and output operations,
		otherwise returns false.
		*/
		operator bool() const;
		/*
		Returns true if this file has errors, otherwise returns false.
		*/
		bool operator!() const;

		
		/*
		Returns a File::Chars object that can be used to read or replace chars in
		range-for-loops or with operator[].
		*/
		Chars chars();
		/*
		Returns a File::Words object that can be used to read or replace words in
		range-for-loops or with operator[].
		*/
		Words words();
		/*
		Returns a File::Lines object that can be used to read or replace lines in
		range-for-loops or with operator[].
		*/
		Lines lines();
		/*
		Returns a File::Line object that can can be used to read or replace the
		line specified by the parameter.
		*/
		Line operator[](uint32 Position);
		/*
		Returns a File::Lines::Iterator object that points to the first line.
		*/
		Lines::Iterator begin();
		/*
		Returns a File::Lines::Iterator object that points to the line after the
		last one.
		*/
		Lines::Iterator end();
	};
	

	template<typename T>
	inline void File::Word::operator=(T Replacement) const {
		file->replaceWord(position, Replacement);
	}
	template<typename T>
	inline void File::Line::operator=(T Replacement) const {
		file->replaceLine(position, Replacement);
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
			return append(strToAppend + toString(Replacement));
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
	inline File& File::operator>>(T &Out) {
		if (!open()) return *this;
		mainFile >> Out;
		return *this;
	}
	template<typename T>
	inline File& File::operator<<(T In) {
		if (!open()) return *this;
		mainFile << toString(In);
		mainFile.flush();
		return *this;
	}
	
	template<typename T>
	inline File& File::operator=(T NewContent) {
		sp::Tstr newContent = toString(NewContent);
		if (exists()) {
			if (!pointToBeg() || !resize(newContent.length())) return *this;
		}
		else {
			if (!create()) return *this;
		}

		mainFile << newContent;

		mainFile.flush();
		return *this;
	}
	template<typename T>
	inline Tstr File::operator+(T ToAdd) {
		return str() + toString(ToAdd);
	}
	template<typename T>
	inline File& File::operator+=(T toAppend) {
		append(toAppend);
		return *this;
	}


	template<typename T>
	inline bool File::operator==(T ToCompare) {
		Tstr toCompare = toString(ToCompare);
		if (toCompare.length() != getNrChars()) return false;
		if (!pointToBeg()) return false;
		
		char tempChar;
		uint32 currentPosition = 0;
		while (1) {
			tempChar = mainFile.get();
			if (mainFile.eof()) break;
			if (tempChar != toCompare[currentPosition]) return false;
			++currentPosition;
		}
		return true;
	}
	template<typename T>
	inline bool File::operator!=(T ToCompare) {
		return !operator==(ToCompare);
	}


	/*
	Returns the content of the first parameter concatenated with the content of the
	second parameter. The second parameter is opened in binary-input-output mode,
	if it wasn't already.
	If the second parameter file couldn't be opened returns only the content of the first parameter.
	*/
	template<typename T, typename = typename std::enable_if<!std::is_same<T, File>::value>::type>
	Tstr operator+(T First, File &Second) {
		return toString(First) + Second.str();
	}
}

#endif
