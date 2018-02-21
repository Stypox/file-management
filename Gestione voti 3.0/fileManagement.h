//FARE possibilita' di rimuovere (e se fattibile rimpiazzare) intervalli di caratteri/parole/linee, per diminuire il tempo necessario in caso che si debbano rimpiazzare piu' caratteri/parole/linee consecutivi.
//FARE deleteCurrentChar() (il carattere a cui si sta puntando adesso)
//FARE nelle funzioni get rimuovo automaticamente il '\r' alla fine della linea, quindi non farlo nelle replace/add (se uno volesse mettere apposta i '\r' cosi' lo puo' fare)
//FARE forse si puo' ottimizzare la scrittura su file scrivendo una stringa sola invece che due, es: "file << (str + '\n');" invece che "file << str << '\n';"
//FARE for (char letter : file)
//FARE specificare nella descrizione delle funzioni "Leaves the file open in binary input-output mode", "The file is opened, if it wasn't already", "Might set ... error/bit"
//FARE non so se serva ma forse bisogna fare file.clear() di tutto invece che solo l'eofbit perche' se c'e' il failbit impostato e il file e' gia' aperto non verra' mai chiuso ma sara impossibile da leggere o scrivere

#pragma once

#include <iostream>
#include <string>
#include <fstream>

class File {
private:
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
	void printSpaces(str string) {
		for (char letter : string) {
			printSpaces(letter);
		}
	}
	fstm file;
	str path, tempPath;
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
	Counts the number of words in a string,
		based on the spaces between them.
	*/
	uint32 countWords(str String);
	/*
	Deletes all '\r' (Carriage Return) at the end of the string
	*/
	void truncEndCR(str &String);
	/*
	Opens file in read-mode and TempFile in read/write-mode
		To be used when modifying file using a temporany file
	If TempFile couldn't be opened file gets closed
		since it was opened in a not-default way
	Returns false if one of the files couldn't be opened, otherwise true
	*/
	bool openTempToModifyFile(fstm &TempFile);
	/*
	Gets all the content from the 1st file and copies it to the 2nd
	Files must be already open
	*/
	void moveFileContent(fstm &From, fstm &To);

public:
	/*
	default constructor
	initializes path to an empty string
		and tempPath to defaultTempPath
	*/
	File();
	/*
	constructor with path
	initializes path to the corresponding parameter
		and tempPath to path + defaultTempExtension
	*/
	File(str Path);
	/*
	constructor with path and tempPath
	initializes path and tempPath to the corresponding parameters
	*/
	File(str Path, str TempPath);

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
	str getLine(uint32 Line);
	/*
	Returns a word
	If the word is out of bounds "" is returned
	*/
	str getWord(uint32 Word);
	/*
	Returns a word in a line
	If the word is out of bounds "" is returned
	*/
	str getWord(uint32 Line, uint32 Word);
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
	str getLines(uint32 From, uint32 To);
	/*
	Returns the interval of words between From and To, both included
	If some (or all) words are out of bounds they just get ignored
	If To > From the words are returned in top-to-bottom order,
		otherwise the order is inverted (bottom-to-top)
	Separates words with ' ', replacing any other type of space
	*/
	str getWords(uint32 From, uint32 To);
	/*
	Returns the interval of words in a line between From and To, both included
	If some (or all) words are out of bounds they just get ignored
	If To > From the words are returned in top-to-bottom order,
		otherwise the order is inverted (bottom-to-top)
	Separates words with ' ', replacing any other type of space
	*/
	str getWords(uint32 Line, uint32 From, uint32 To);
	/*
	Returns the interval of chars between From and To, both included
	If some (or all) chars are out of bounds they just get ignored
	If To > From the chars are returned in top-to-bottom order,
		otherwise the order is inverted (bottom-to-top)
	*/
	str getChars(uint32 From, uint32 To);
	/*
	Returns the interval of chars in a line between From and To, both included
	If some (or all) chars are out of bounds they just get ignored
	If To > From the chars are returned in top-to-bottom order,
		otherwise the order is inverted (bottom-to-top)
	*/
	str getChars(uint32 Line, uint32 From, uint32 To);
	/*
	Returns the interval of chars in a word in a line between From and To, both included
	If some (or all) chars are out of bounds they just get ignored
	If To > From the chars are returned in top-to-bottom order,
		otherwise the order is inverted (bottom-to-top)
	*/
	str getChars(uint32 Line, uint32 Word, uint32 From, uint32 To);

	
	/*
	Adds a line using a temp file
	If the specified line is out of bounds some newlines get created
	Closes files before returning, since they were opened in a not-default way
	Returns false if the files couldn't be opened, otherwise true
	*/
	bool addLine(uint32 Line, str ToAdd);
	/*
	Adds a word using a temp file
	Closes files before returning, since they were opened in a not-default way
	Returns false if the files couldn't be opened or if the
		specified word is out of bounds, otherwise true
	*/
	bool addWord(uint32 Word, str ToAdd);
	/*
	Adds a word in a line using a temp file
	Closes files before returning, since they were opened in a not-default way
	Returns false if the files couldn't be opened or if the
		specified word is out of bounds, otherwise true
	*/
	bool addWord(uint32 Line, uint32 Word, str ToAdd);
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
	bool replaceLine(uint32 Line, str Replacement);
	/*
	Replaces a word using a temp file
	Closes files before returning, since they were opened in a not-default way
	Returns false if the files couldn't be opened or if the
		specified word is out of bounds, otherwise true
	*/
	bool replaceWord(uint32 Word, str Replacement);
	/*
	Replaces a word in a line using a temp file
	Closes files before returning, since they were opened in a not-default way
	Returns false if the files couldn't be opened or if the
		specified word is out of bounds, otherwise true
	*/
	bool replaceWord(uint32 Line, uint32 Word, str Replacement);
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
	bool appendLine(str ToAppend);
	/*
	Adds a word (with a space before it) at the end of the file
	Returns false if the file couldn't be opened, otherwise true
	*/
	bool appendWord(str ToAppend);
	/*
	Adds a word (with a space before it) after the specified word
	Returns false if the file couldn't be opened, otherwise true
	*/
	bool appendWord(uint32 Line, str ToAppend);
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
	bool move(str newPath);
	/*
	Replaces the filename, extension included, with the new name
	Leaves the file open in binary input-output mode
	Returns false if either the old path file or the new path file
		couldn't be opened, otherwise true
	*/
	bool rename(str newName);
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
	Closes the file if it wasn't already
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
	struct stat stat();
	//FARE aggiungere altro: stati del file calcolati con stat


	/*
	Returns the file's goodbit, that is true only if
		neither eofbit, failbit nor badbit are set
	Eofbit is true if the end of file was reached
		Failbit is true if there were logical errors
		Badbit is true if there were reading or writing errors
	*/
	bool good() const;
	/*
	Clears all the file's error state flags (eofbit, failbit, badbit)
	*/
	void clear();
	/*
	Returns the file's eofbit, that is true if the end of file was reached
	*/
	bool eof() const;
	/*
	Sets the file's end-of-file error state flag (eofbit) to a value
	*/
	void eof(bool Value);
	/*
	Returns true if either failbit or badbit are set
	Failbit is true if there were logical errors
		Badbit is true if there were reading or writing errors
	*/
	bool fail() const;
	/*
	Sets the file's logical error state flag (failbit) to a value
	*/
	void fail(bool Value);
	/*
	Returns the file's badbit, that is true if there were reading/writing errors
	*/
	bool bad() const;
	/*
	Sets the file's reading/writing error state flag (badbit) to a value
	*/
	void bad(bool Value);
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
	std::ios_base::iostate rdstate() const;


	/*
	Returns the currently set path
	*/
	str getPath() const;
	/*
	Modifies the path used to open the file
	Closes the file if it is open
	*/
	void setPath(str Path);


	/*
	Affixes the content of the file to the parameter using a temp file
	Leaves both files open in binary input-output mode
	The operation fails if the file, the temp file or the parameter file couldn't be opened
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
	File& operator>> (fstm &Out);
	/*
	Like ofstream::operator>>
	The file must be already open
	Returns *this
	*/
	File& operator>> (str &Out);
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
	File& operator<< (fstm &In);
	/*
	Like ofstream::operator<<
	The file must be already open
	Returns *this
	*/
	File& operator<< (str In);
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
	File& operator= (fstm);
	File& operator+ (File);				//ritorna un file con all'inizio il contenuto di questo file e alla fine quello del parametro
	File& operator+ (fstm);
	File& operator+= (File);				//come operator<<
	File& operator+= (fstm);
	File& operator^ (File);				//scambia il contenuto dei due files FARE vedere se metterli
	File& operator^ (fstm);
	

	char operator[] (std::streampos Pos);				//ritorna il carattere in posizione specificata dal parametro
	bool operator== (File Path);				//se i due file hanno lo stesso path ritorna 1
	bool operator== (str Path);
	bool operator!= (File Path);				//se i due file non hanno lo stesso path ritorna 1
	bool operator!= (str Path);	


	operator char*();							//scrive tutto il file in un array di caratteri
	char * cString();
	/*
	Saves all the file in a string and returns it
	Returns "" if the file couldn't be opened
	*/
	operator str();
	/*
	Saves all the file in a string and returns it
	Returns "" if the file couldn't be opened
	*/
	str string();
	operator fstm();
	fstm fstream();
	operator bool();							//ritorna 1 se il file non ha problemi ed e' aperto
	bool operator! ();							//ritorna 1 se il file ha problemi o e' chiuso
};