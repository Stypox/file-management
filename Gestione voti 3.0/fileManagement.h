//FARE possibilita' di rimuovere (e se fattibile rimpiazzare) intervalli di caratteri/parole/linee, per diminuire il tempo necessario in caso che si debbano rimpiazzare piu' caratteri/parole/linee consecutivi.
//FARE deleteCurrentChar() (il carattere a cui si sta puntando adesso)
//FARE nelle funzioni get rimuovo automaticamente il '\r' alla fine della linea, quindi non farlo nelle replace/add (se uno volesse mettere apposta i '\r' cosi' lo puo' fare)
//FARE forse si puo' ottimizzare la scrittura su file scrivendo una stringa sola invece che due, es: "file << (str + '\n');" invece che "file << str << '\n';"
//FARE for (char letter : file)

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
	
	const str defaultTempPath = "temp.tmp";
	const str defaultTempExtension = ".tmp";

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
	

	/*
	Opens the file in binary input-output mode
		If the file doesn't exist it won't be created
	Returns false if the file couldn't be opened, otherwise true
	*/
	bool checkOpen();
	
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
	void truncEndCR(str & String);
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
	void moveFileContent(fstm & From, fstm & To);

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
	Deletes all '\r' at the end of the line FARE
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


	//FARE vedere se metterli o no
	bool affixLine(str ToAffix);
	bool affixWord(str ToAffix);
	bool affixWord(uint32 Line, str ToAffix);
	bool affixChar(char ToAffix);
	bool affixChar(uint32 Line, char ToAffix);
	bool affixChar(uint32 Line, uint32 Word, char ToAffix);


	bool appendLine(str ToAppend);
	bool appendWord(str ToAppend);
	bool appendWord(uint32 Line, str ToAppend);
	bool appendChar(char ToAppend);
	bool appendChar(uint32 Line, char ToAppend);
	bool appendChar(uint32 Line, uint32 Word, char ToAppend);


	/*
	Removes all newlines ('\r' and '\n') at the end using a temp file
	Closes files before returning, since they were opened in a not-default way
	Returns false if the files couldn't be opened, otherwise true
	*/
	bool deleteLastEmptyLines();


	bool create();
	bool rename(str newName);
	bool remove();


	bool open();  
	void close();
	bool exists();
	bool isOpen() const;
	bool isClosed() const;

	bool good() const;
	void clear();
	bool eof() const;
	void clearEof();
	bool fail() const;
	void clearFail();
	bool bad() const;
	void clearBad();
	bool rdstate() const;
	void clearRdstate();
	//FARE aggiungere altro


	str getPath() const;
	bool setPath();


	bool operator>> (File);				//mette il contenuto di questo file all'inizio del parametro
	bool operator>> (fstm);
	bool operator>> (str);				//
	bool operator>> (char *);			//funziona come std::fstream>>TYPE
	bool operator>> (char);				//il file deve essere aperto
	bool operator>> (int64);			//


	File operator<< (File);				//mette il contenuto del parametro alla fine di questo file 
	File operator<< (fstm);
	File operator<< (str);				//
	File operator<< (char *);			//appende alla fine del file
	File operator<< (char);				//
	File operator<< (int64);			//


	bool operator= (File);				//sostituisce il contenuto di questo file con quello del parametro
	bool operator= (fstm);
	File operator+ (File);				//ritorna un file con all'inizio il contenuto di questo file e alla fine quello del parametro
	File operator+ (fstm);
	bool operator+= (File);				//come operator<<
	bool operator+= (fstm);
	bool operator^ (File);				//scambia il contenuto dei due files
	bool operator^ (fstm);
	

	char operator[] (uint32 Pos);				//ritorna il carattere in posizione specificata dal parametro
	bool operator== (File File);				//se i due file hanno lo stesso path ritorna 1
	bool operator== (str Path);
	bool samePath(File File);
	bool samePath(str Path);
	bool operator!= (File File);				//se i due file non hanno lo stesso path ritorna 1
	bool operator!= (str Path);
	operator char*();							//scrive tutto il file in un array di caratteri
	char * cString();
	/*
	Saves all the file in a string and returns it
	Returns "" if the file couldn't be opened
	*/
	operator str(); //FARE non va
	/*
	Saves all the file in a string and returns it
	Returns "" if the file couldn't be opened
	*/
	str string();
	operator bool();							//ritorna 1 se il file e' aperto
	bool operator! ();							//ritorna 1 se il file e' chiuso
};