//FARE possibilita' di rimuovere (e se fattibile rimpiazzare) intervalli di caratteri/parole/linee, per diminuire il tempo necessario in caso che si debbano rimpiazzare piu' caratteri/parole/linee consecutivi.
//FARE deleteCurrentChar() (il carattere a cui si sta puntando adesso)
//FARE nelle funzioni get rimuovo automaticamente il '\r' alla fine della linea, quindi non farlo nelle replace/add (se uno volesse mettere apposta i '\r' cosi' lo puo' fare)
//FARE forse si puo' ottimizzare la scrittura su file scrivendo una stringa sola invece che due, es: "file << (Tstr + '\n');" invece che "file << Tstr << '\n';"
//FARE for (char letter : file)
//FARE specificare nella descrizione delle funzioni "Leaves the file open in binary input-output mode", "The file is opened, if it wasn't already", "Might set ... error/bit"
//FARE non so se serva ma forse bisogna fare file.clear() di tutto invece che solo l'eofbit perche' se c'e' il failbit impostato e il file e' gia' aperto non verra' mai chiuso ma sara impossibile da leggere o scrivere

#pragma once

#include <iostream>
#include <string>
#include <fstream>

class File;

struct FileState {
public:
	FileState(bool Open, bool Eof, bool Fail, bool Bad, bool TempErr, bool ExtErr);
	bool open, eof, fail, bad, tempErr, extErr;

	/*
	*/
	operator bool();
	std::string str();
	void save(File &file);
};

class File {
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

private:
	//togliere public FARE
public:
	//eliminare FARE
	void printSpaces(char Char) {
		switch (Char) {
		case ' ':
			std::cout << "S";
			break;
		case '\t':
			std::cout << "T";
			break;
		case '\n':
			std::cout << "N";
			break;
		case '\v':
			std::cout << "V";
			break;
		case '\f':
			std::cout << "F";
			break;
		case '\r':
			std::cout << "R";
			break;
		case -1:
			std::cout << "-1";
			break;
		default:
			std::cout << Char;
		}
	}
	void printSpaces(Tstr str) {
		for (char letter : str) {
			printSpaces(letter);
		}
	}
	Tfstm file;
	Tstr path, tempPath;
	//FARE usarli dappertutto
	bool TempError, ExternalError;
	

	
	/*
	The two pointers inside the file are moved to the new position
	The file is opened, if it wasn't already
	The indices start from 0: pointTo(0, 0, 0) points to the 1st char of the
		1st word of the 1st line (not necessarily to the beginning of the file)
	While 0 means "move to the first", -1 means "don't move"
		So only pointTo(-1, -1, -1) will surely point to the beginning
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
	std::streampos getPosition(uint32 Line, uint32 Word, uint32 Char);

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
	Returns false if one of the files couldn't be opened, otherwise true
	*/
	bool openTempToModifyFile(Tfstm &TempFile);
	/*
	Gets all the content from the 1st file and copies it to the 2nd
	Files must be already open
	*/
	void moveFileContent(Tfstm &From, Tfstm &To);

public:
	/*
	default constructor
	initializes path to an empty Tstr
		and tempPath to defaultTempPath
	*/
	File();
	/*
	constructor with path
	initializes path to the corresponding parameter
		and tempPath to path + defaultTempExtension
	*/
	File(Tstr Path);
	/*
	constructor with path and tempPath
	initializes path and tempPath to the corresponding parameters
	*/
	File(Tstr Path, Tstr TempPath);

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
	Returns 0 if the file couldn't be opened
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
	Deletes a word and a space using a temp file
	The space is deleted after if it's not \r, otherwise before
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
	Closes the file and clears all errors if it wasn't already open
	*/
	void close();
	/*
	Returns true if the file is open, otherwise false
	*/
	bool isOpen() const;


	/*
	Returns true if the file exists, otherwise false
	*/
	bool exists() const;
	/*
	Returns a stat object containing various infos about the file
	*/
	struct stat info();
	//FARE aggiungere altro: stati del file calcolati con stat


	/*
	Returns true if no errors are set
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
	Returns an object that contains all the infos about the file's error state flags
	*/
	FileState state() const;



	/*
	Returns the currently set path
	*/
	Tstr getPath() const;
	/*
	Modifies the path used to open the file
	Closes the file if it is open
	*/
	void setPath(Tstr Path);


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
	Affixes the content of the file to the parameter using a temp file
	The parameter must be already open in binary input-output mode
	Leaves both files open in binary input-output mode
	The operation fails if the file or the temp file couldn't be opened
		and if the parameter isn't already open
	If the parameter isn't already open ExternalError is set to 1
	Returns *this
	*/
	File& operator>> (Tfstm &Out);
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
	File& operator>> (char * &Out);
	/*
	Like ofstream::operator>>
	The file must be already open
	Returns *this
	*/
	File& operator>> (char &Out);
	/*
	Like ofstream::operator>>
	The file must be already open
	Returns *this
	*/
	File& operator>> (int64 &Out);
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
	Affixes the content of the file to the parameter using a temp file
	The parameter must be already open in binary input-output mode
	Leaves both files open in binary input-output mode
	The operation fails if the file or the temp file couldn't be opened
		and if the parameter isn't already open
	If the parameter isn't already open ExternalError is set to 1
	Returns *this
	*/
	File& operator<< (Tfstm &In);
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
	Like ofstream::operator<<
	The file must be already open
	Returns *this
	*/
	File& operator<< (int64 In);
	/*
	Like ofstream::operator<<
	The file must be already open
	Returns *this
	*/
	File& operator<< (double In);


	File& operator= (File);				//sostituisce il contenuto di questo file con quello del parametro
	File& operator= (Tfstm);
	File& operator+ (File);				//ritorna un file con all'inizio il contenuto di questo file e alla fine quello del parametro
	File& operator+ (Tfstm);
	File& operator+= (File);			//appende in fondo
	File& operator+= (Tfstm);
	File& operator+= (Tstr In);
	File& operator+= (const char * In);
	File& operator+= (char In);
	File& operator+= (int64 In);
	File& operator+= (double In);
	File& operator^ (File);				//scambia il contenuto dei due files FARE vedere se metterli
	File& operator^ (Tfstm);
	

	char operator[] (std::streampos Pos);				//ritorna il carattere in posizione specificata dal parametro
	bool operator== (File Path);				//se i due file hanno lo stesso path ritorna 1
	bool operator== (Tstr Path);
	bool operator!= (File Path);				//se i due file non hanno lo stesso path ritorna 1
	bool operator!= (Tstr Path);	


	operator char*();							//scrive tutto il file in un array di caratteri
	char * cString();
	/*
	Saves all the file in a Tstr and returns it
	Returns "" if the file couldn't be opened
	*/
	operator Tstr();
	/*
	Saves all the file in a Tstr and returns it
	Returns "" if the file couldn't be opened
	*/
	Tstr string();
	operator Tfstm();
	Tfstm fstream();
	operator bool();							//ritorna 1 se il file non ha problemi ed e' aperto
	bool operator! ();							//ritorna 1 se il file ha problemi o e' chiuso
};