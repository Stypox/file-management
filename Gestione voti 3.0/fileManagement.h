//FARE possibilita' di rimuovere (e se fattibile rimpiazzare) intervalli di caratteri/parole/linee, per diminuire il tempo necessario in caso che si debbano rimpiazzare piu' caratteri/parole/linee consecutivi.
//FARE deleteCurrentChar() (il carattere a cui si sta puntando adesso)
//FARE nelle funzioni get rimuovo automaticamente il '\r' alla fine della linea, quindi non farlo nelle replace/add (se uno volesse mettere apposta i '\r' cosi' lo puo' fare)
//FARE forse si puo' ottimizzare la scrittura su file scrivendo una stringa sola invece che due, es: "file << (Tstr + '\n');" invece che "file << Tstr << '\n';"
//FARE for (char letter : file)
//FARE specificare nella descrizione delle funzioni "Leaves the file open in binary input-output mode", "The file is opened, if it wasn't already", "Might set ... error/bit", "Moves/not the pointer"
//FARE non so se serva ma forse bisogna fare file.clear() di tutto invece che solo l'eofbit perche' se c'e' il failbit impostato e il file e' gia' aperto non verra' mai chiuso ma sara impossibile da leggere o scrivere
//FARE una lista di cose da scrivere in ogni descrizione, come hanno fatto su glfw con @

#pragma once

#include <iostream>
#include <string>
#include <fstream>

class File;

struct FileState {
public:
	bool open, eof, fail, bad, tempErr, extErr;

	FileState(bool Open, bool Eof, bool Fail, bool Bad, bool TempErr, bool ExtErr);

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


private:
	template<typename T>
	bool add(Tspos Pos, T ToAdd);

	template<typename T>
	bool append(T ToAppend);

	template<typename T>
	File& operator>> (T Out);

	template<typename T>
	File& operator<< (T In);

	//FARE togliere public
public:
	Tfstm file;
	Tstr path, tempPath;
	bool TempError, ExternalError;
	

	/*
	Counts the number of words in a Tstr,
		based on the spaces between them.
	*/
	uint32 countWords(Tstr String);
	/*
	Deletes all '\r' (Carriage Return) at the end of the Tstr
	*/
	void truncEndCR(Tstr &String);
	/*
	Opens file in read-mode and TempFile in read/write-mode
		To be used when modifying file using a temporany file
	If TempFile couldn't be opened file gets closed
		since it was opened in a not-default way
	TempFile should be closed
	Returns false if one of the files couldn't be opened, otherwise true
	*/
	bool openTempToModifyFile(Tfstm &TempFile);
	/*
	Gets all the content from the 1st file and copies it to the 2nd
	Files must be already open
	*/
	void moveFileContent(Tfstm &From, Tfstm &To);
	/*
	Retrieves the (start) position of the requested line/word/char
	Moves the pointer to the new position
	Returns -1 if the file couldn't be opened and
	-2 if the specified position is out of bounds
	*/
	Tspos getPositionMove(uint32 Line, uint32 Word, uint32 Char);


	/*
	Replaces all the characters between From and To with Replacement
	Uses a temp file if the size of replacement is smaller than To-From
	*/
	bool replaceSection(Tspos From, Tspos To, Tstr Replacement);
	/*
	Deletes all the chars between From and To, both included
	Uses a temp file
	The file is opened, if it wasn't already
	Leaves the file open in binary input-output mode
	Returns false if the files couldn't be opened, otherwise true
	*/
	bool deleteSection(Tspos From, Tspos To);


public:
	/*
	Default constructor
	Initializes path to an empty Tstr
		and tempPath to defaultTempPath
	*/
	File();
	/*
	Constructor with path
	Initializes path to the corresponding parameter
		and tempPath to path + defaultTempExtension
	*/
	File(Tstr Path);
	/*
	Constructor with path and tempPath
	Initializes path and tempPath to the corresponding parameters
	*/
	File(Tstr Path, Tstr TempPath);
	/*
	Destructor
	Closes the file
	*/
	~File();

	/*
	Moves the pointer to a new position
	pointTo(1) means that the next character is the second
	The file is opened, if it wasn't already
	Leaves the file open in binary input-output mode
	Returns false if the file couldn't be opened, otherwise true
	*/
	bool pointTo(Tspos Position);
	/*
	Moves the pointer to a char in a word in a line
	The indices start from 0: pointTo(0, 0, 0) points to the 1st char of the
		1st word of the 1st line (not necessarily to the beginning of the file)
	While 0 means "move to the first", -1 means "don't move"
		So only pointTo(-1, -1, -1) will surely point to the beginning
	The file is opened, if it wasn't already
	Returns false if the file couldn't be opened or	if the
		specified position is out of bounds, otherwise true
	*/
	bool pointTo(uint32 Line, uint32 Word, uint32 Char);
	/*
	The file is opened, if it wasn't already
	The two pointers inside the file are moved to the beginning
	The file's eofbit is cleared
	Returns true if the file opened correctly, otherwise false
	*/
	bool pointToBeg();
	/*
	The file is opened, if it wasn't already
	The two pointers inside the file are moved to the end
	The file's eofbit is cleared FARE se va bene
	Returns true if the file opened correctly, otherwise false
	*/
	bool pointToEnd();
	/*
	Retrieves the (start) position of the requested line/word/char
	Maintains the original file's pointers' position
	Returns -1 if the file couldn't be opened and
	-2 if the specified position is out of bounds
	*/
	Tspos getPosition(uint32 Line, uint32 Word, uint32 Char);


	/*
	Returns the number of '\n'
	Returns 0 if the file couldn't be opened
	*/
	uint32 getNrLines();
	/*
	Returns the number of words divided by spaces
	Returns 0 if the file couldn't be opened
	*/
	uint32 getNrWords();
	/*
	Returns the number of words of a line divided by spaces
	Returns 0 if the file couldn't be opened
	*/
	uint32 getNrWords(uint32 Line);
	/*
	Returns the number of chars (bytes)
	The file is neither opened nor closed
	Returns 0 if the file couldn't be axcessed
	*/
	uint32 getNrChars();
	/*
	Returns the number of chars of a line
	Doesn't count '\r' at the end of the line
	Returns 0 if the file couldn't be opened
	*/
	uint32 getNrChars(uint32 Line);
	/*
	Returns the number of chars of a word in a line
	Returns 0 if the file couldn't be opened
	*/
	uint32 getNrChars(uint32 Line, uint32 Word);

	
	/*
	Replaces the char at the pointer position
	The file must be already open
	Returns *this
	*/
	File& put(char ToPut);
	/*
	Returns all the characters after the pointer
		until '\r\n' or the end of file are reached
	The file must be already open
	Returns "" if the file is not open
	*/
	Tstr getLine();
	/*
	Returns all the characters after the pointer
		until a space or the end of file are reached
	The file must be already open
	Returns "" if the file is not open
	*/
	Tstr getWord();
	/*
	Returns the char at the pointer position
	The file must be already open
	Returns -1 if the file is not open or the end was reached
	*/
	char get();
	/*
	Saves on the parameter all the characters after the
	pointer until '\r\n' or the end of file are reached 
	The file must be already open
	Returns false if the file is not open or if the
		end was already reached, otherwise true
	*/
	bool getLine(Tstr &Line);
	/*
	Saves on the parameter all the characters after the
	pointer until a	space or the end of file are reached
	The file must be already open
	Returns false if the file is not open or if the
		end was already reached, otherwise true
	*/
	bool getWord(Tstr &Word);
	/*
	Saves the char at the pointer position on the parameter
	The file must be already open
	Returns false if the file is not open or
		the end was reached, otherwise true
	*/
	bool get(char &Char);


	/*
	Returns a line removing all '\r' at the end of it
	If the line is out of bounds "" is returned
	*/
	Tstr getLine(uint32 Line);
	/*
	Returns a word
	If the word is out of bounds "" is returned
	*/
	Tstr getWord(uint32 Word);
	/*
	Returns a word in a line
	If the word is out of bounds "" is returned
	*/
	Tstr getWord(uint32 Line, uint32 Word);
	/*
	Returns a char
	If the char is out of bounds 0 is returned
	*/
	char getChar(uint32 Char);
	/*
	Returns a char in a line
	If the char is out of bounds 0 is returned
	*/
	char getChar(uint32 Line, uint32 Char);
	/*
	Returns a char in  a word in a line
	If the char is out of bounds 0 is returned
	*/
	char getChar(uint32 Line, uint32 Word, uint32 Char);


	/*
	Returns the interval of lines between From and To, both included
	If some (or all) lines are out of bounds they just get ignored
	If To > From the lines are returned in top-to-bottom order,
		otherwise the order is inverted (bottom-to-top)
	Removes all '\r' at the end of lines and adds '\r\n' between them
	*/
	Tstr getLines(uint32 From, uint32 To);
	/*
	Returns the interval of words between From and To, both included
	If some (or all) words are out of bounds they just get ignored
	If To > From the words are returned in top-to-bottom order,
		otherwise the order is inverted (bottom-to-top)
	Separates words with ' ', replacing any other type of space
	*/
	Tstr getWords(uint32 From, uint32 To);
	/*
	Returns the interval of words in a line between From and To, both included
	If some (or all) words are out of bounds they just get ignored
	If To > From the words are returned in top-to-bottom order,
		otherwise the order is inverted (bottom-to-top)
	Separates words with ' ', replacing any other type of space
	*/
	Tstr getWords(uint32 Line, uint32 From, uint32 To);
	/*
	Returns the interval of chars between From and To, both included
	If some (or all) chars are out of bounds they just get ignored
	If To > From the chars are returned in top-to-bottom order,
		otherwise the order is inverted (bottom-to-top)
	*/
	Tstr getChars(uint32 From, uint32 To);
	/*
	Returns the interval of chars in a line between From and To, both included
	If some (or all) chars are out of bounds they just get ignored
	If To > From the chars are returned in top-to-bottom order,
		otherwise the order is inverted (bottom-to-top)
	*/
	Tstr getChars(uint32 Line, uint32 From, uint32 To);
	/*
	Returns the interval of chars in a word in a line between From and To, both included
	If some (or all) chars are out of bounds they just get ignored
	If To > From the chars are returned in top-to-bottom order,
		otherwise the order is inverted (bottom-to-top)
	*/
	Tstr getChars(uint32 Line, uint32 Word, uint32 From, uint32 To);


	bool add(Tspos Pos, Tstr ToAdd);
	bool add(Tspos Pos, const char * ToAdd);
	bool add(Tspos Pos, char ToAdd);
	bool add(Tspos Pos, int8 ToAdd);
	bool add(Tspos Pos, int16 ToAdd);
	bool add(Tspos Pos, int32 ToAdd);
	bool add(Tspos Pos, int64 ToAdd);
	bool add(Tspos Pos, uint8 ToAdd);
	bool add(Tspos Pos, uint16 ToAdd);
	bool add(Tspos Pos, uint32 ToAdd);
	bool add(Tspos Pos, uint64 ToAdd);
	bool add(Tspos Pos, float ToAdd);
	bool add(Tspos Pos, double ToAdd);

	/*
	Adds a line using a temp file
	If the specified line is out of bounds some newlines get created
	Closes files before returning, since they were opened in a not-default way
	Returns false if the files couldn't be opened, otherwise true
	*/
	bool addLine(uint32 Line, Tstr ToAdd);
	/*
	Adds a word using a temp file
	Closes files before returning, since they were opened in a not-default way
	Returns false if the files couldn't be opened or if the
		specified word is out of bounds, otherwise true
	*/
	bool addWord(uint32 Word, Tstr ToAdd);
	/*
	Adds a word in a line using a temp file
	Closes files before returning, since they were opened in a not-default way
	Returns false if the files couldn't be opened or if the
		specified word is out of bounds, otherwise true
	*/
	bool addWord(uint32 Line, uint32 Word, Tstr ToAdd);
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
	Replaces a line using a temp file
	Deletes all '\r' at the end of the line FARE
	If the specified line is out of bounds some newlines get created
	Closes files before returning, since they were opened in a not-default way
	Returns false if the files couldn't be opened, otherwise true
	*/
	bool replaceLine(uint32 Line, Tstr Replacement);
	/*
	Replaces a word using a temp file
	Closes files before returning, since they were opened in a not-default way
	Returns false if the files couldn't be opened or if the
		specified word is out of bounds, otherwise true
	*/
	bool replaceWord(uint32 Word, Tstr Replacement);
	/*
	Replaces a word in a line using a temp file
	Closes files before returning, since they were opened in a not-default way
	Returns false if the files couldn't be opened or if the
		specified word is out of bounds, otherwise true
	*/
	bool replaceWord(uint32 Line, uint32 Word, Tstr Replacement);
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


	bool append(Tstr ToAppend);
	bool append(char * ToAppend);
	bool append(char ToAppend);
	bool append(int8 ToAppend);
	bool append(int16 ToAppend);
	bool append(int32 ToAppend);
	bool append(int64 ToAppend);
	bool append(uint8 ToAppend);
	bool append(uint16 ToAppend);
	bool append(uint32 ToAppend);
	bool append(uint64 ToAppend);
	bool append(float ToAppend);
	bool append(double ToAppend);
	/*
	Adds a line (with endline before it) at the end of the file
	Returns false if the file couldn't be opened, otherwise true
	*/
	bool appendLine(Tstr ToAppend);
	/*
	Adds a word (with a space before it) at the end of the file
	Returns false if the file couldn't be opened, otherwise true
	*/
	bool appendWord(Tstr ToAppend);
	/*
	Adds a word (with a space before it) after the specified word
	Returns false if the file couldn't be opened, otherwise true
	*/
	bool appendWord(uint32 Line, Tstr ToAppend);
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
	bool move(File &toOverwrite);
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
	Closes and reopens the file to force it to save all the changes
	Returns false if the file isn't already open or couldn't be opened, otherwise true
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
	Returns true if there were errors while using the temp file
	*/
	bool tempErr() const;
	/*
	Sets the errors caused by the temp file state to a value
	*/
	void tempErr(bool Value);
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
	/*
	Returns the path used to open the temp file
	*/
	Tstr getTempPath() const;
	/*
	Modifies the path used to open the temp file
	*/
	void setTempPath(Tstr TempPath);


	/*
	Affixes the content of the file to the parameter using a temp file
	Leaves both files open in binary input-output mode
	The operation fails if the file, the temp file or the parameter file couldn't be opened
	If the temp file couldn't be opened TempError is set to 1
	If the parameter file couldn't be opened ExternalError is set to 1
	Returns *this
	*/
	File& operator>> (File &Out);
	/*
	Like ofstream::operator>>
	The file must be already open
	Returns *this
	*/
	File& operator>> (Tstr &Out);
	/*
	Like ofstream::operator>>
	The file must be already open
	Returns *this
	*/
	File& operator>> (char * Out);
	/*
	Like ofstream::operator>>
	The file must be already open
	Returns *this
	*/
	File& operator>> (char &Out);
	/*
	Like ofstream::operator>> but gets the parameter as a number
	The file must be already open
	Returns *this
	*/
	File& operator>> (int8 &Out);
	/*
	Like ofstream::operator>>
	The file must be already open
	Returns *this
	*/
	File& operator>> (int16 &Out);
	/*
	Like ofstream::operator>>
	The file must be already open
	Returns *this
	*/
	File& operator>> (int32 &Out);
	/*
	Like ofstream::operator>>
	The file must be already open
	Returns *this
	*/
	File& operator>> (int64 &Out);
	/*
	Like ofstream::operator>> but gets the parameter as a number
	The file must be already open
	Returns *this
	*/
	File& operator>> (uint8 &Out);
	/*
	Like ofstream::operator>>
	The file must be already open
	Returns *this
	*/
	File& operator>> (uint16 &Out);
	/*
	Like ofstream::operator>>
	The file must be already open
	Returns *this
	*/
	File& operator>> (uint32 &Out);
	/*
	Like ofstream::operator>>
	The file must be already open
	Returns *this
	*/
	File& operator>> (uint64 &Out);
	/*
	Like ofstream::operator>>
	The file must be already open
	Returns *this
	*/
	File& operator>> (float &Out);
	/*
	Like ofstream::operator>>
	The file must be already open
	Returns *this
	*/
	File& operator>> (double &Out);


	/*
	Appends the content of the parameter to the file
	Leaves both files open in binary input-output mode
	The operation fails if the file or the parameter file couldn't be opened
	If the parameter file couldn't be opened ExternalError is set to 1
	Returns *this
	*/
	File& operator<< (File &In);
	/*
	Like ofstream::operator<<
	The file must be already open
	Returns *this
	*/
	File& operator<< (Tstr In);
	/*
	Like ofstream::operator<<
	The file must be already open
	Returns *this
	*/
	File& operator<< (const char * In);
	/*
	Like ofstream::operator<<
	The file must be already open
	Returns *this
	*/
	File& operator<< (char In);
	/*
	Like ofstream::operator<< but inserts the parameter as a number
	The file must be already open
	Returns *this
	*/
	File& operator<< (int8 In);
	/*
	Like ofstream::operator<<
	The file must be already open
	Returns *this
	*/
	File& operator<< (int16 In);
	/*
	Like ofstream::operator<<
	The file must be already open
	Returns *this
	*/
	File& operator<< (int32 In);
	/*
	Like ofstream::operator<<
	The file must be already open
	Returns *this
	*/
	File& operator<< (int64 In);
	/*
	Like ofstream::operator<< but inserts the parameter as a number
	The file must be already open
	Returns *this
	*/
	File& operator<< (uint8 In);
	/*
	Like ofstream::operator<<
	The file must be already open
	Returns *this
	*/
	File& operator<< (uint16 In);
	/*
	Like ofstream::operator<<
	The file must be already open
	Returns *this
	*/
	File& operator<< (uint32 In);
	/*
	Like ofstream::operator<<
	The file must be already open
	Returns *this
	*/
	File& operator<< (uint64 In);
	/*
	Like ofstream::operator<<
	The file must be already open
	Returns *this
	*/
	File& operator<< (float In);
	/*
	Like ofstream::operator<<
	The file must be already open
	Returns *this
	*/
	File& operator<< (double In);


	//FARE vedere se mettere funzioni per supportare fstream ma sembra inutile. Eventualmente togliere anche sopra ^^^
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
	bool operator= (Tstr NewText);
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
	Tstr operator+ (Tstr ToAdd);
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
	File& operator+= (Tstr toAppend);
	

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
	Returns a pointer to an array of chars containing all the file
	The file is opened, if it wasn't already
	Leaves the file open in binary input-output mode
	Returns nullptr if the file couldn't be opened
	*/
	const char * cStr();
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
	operator bool();
	/*
	Returns false if there aren't errors, otherwise true
	*/
	bool operator!();


	/*
	
	*/
	FileIterator begin();
	/*
	
	*/
	FileIterator end();
};

std::string operator+ (std::string First, File &Second);