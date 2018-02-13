/*
Forma relativa e non. = sia relativamente all'inizio del file che relativamente ad una linea.
*/

//FARE possibilita' di rimuovere (e se fattibile rimpiazzare) intervalli di caratteri/parole/linee,
//per diminuire il tempo necessario in caso che si debbano rimpiazzare piu' caratteri/parole/linee consecutivi.

#pragma once

#include <iostream>
#include <string>
#include <fstream>


enum class EfileStatus{
	OK,
	Not_Open,
	Not_Openable,
	Unexisting_Line,
	Unexisting_Word,
	Unexisting_Char,
	Unknown
};

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
	fstm file;
	str path, tempPath;
	

	/*
	Returns false if the file couldn't be opened, otherwise true
	*/
	bool checkOpen();
	
	/*
	The two pointers inside the file are moved to the new position
	The file is opened, if it wasn't already
	Line and word are used as offsets, if set to 0 it starts from the beginning
	Eg: pointTo(3, 2, 4) points to the 5th char of the 3rd word of the 4th line
		pointTo(3, 0, 0) points to the 4th line
		pointTo(3, 0, 4) points to the 5th char of the 4th line
		pointTo(0, 0, 7) points to the 8th char from the beginning
		pointTo(0, 2, 0) points to the 3rd word from the beginning
	Returns false if the file couldn't be opened or	if the
		specified position is out of bounds, otherwise true
	*/
	bool pointTo(uint16 Line, uint16 Word, uint16 Char);
	/*
	The file is opened, if it wasn't already
	The two pointers inside the file are moved to the beginning
	The file's eofbit is cleared
	Returns true if the file opened correctly, otherwise false
	*/
	bool pointToBeg();
	/*
	Retrieves the (start) position of the requested line/word/char
	Maintains the original file's pointers' position
	Returns -1 if the file couldn't be opened or if
		the specified position is out of bounds
	*/
	std::streampos getPosition(uint16 Line, uint16 Word, uint16 Char);

	/*
	Counts the number of words in a string,
		based on the spaces between them.
	*/
	uint16 countWords(str String);
	/*
	Deletes all '\r' (Carriage Return) at the end of the string
	*/
	void truncEndCR(str & String);
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


	uint16 getNrLines();
	uint16 getNrWords();
	uint16 getNrChars();
	uint16 getNrWordsLine(uint16 Line);
	uint16 getNrCharsLine(uint16 Line);
	uint16 getNrCharsWord(uint16 Word);					//parola con posizione specificata rispetto all'inizio del file
	uint16 getNrCharsWord(uint16 Line, uint16 Word);			//parola con posizione specificata della linea specificata

	//ottiene linea/parola/carattere singoli
	str getLine(uint16 Line);
	str getWord(uint16 Word);
	str getWord(uint16 Line, uint16 Word);
	char getChar(uint16 Char);
	char getChar(uint16 Line, uint16 Char);
	char getChar(uint16 Line, uint16 Word, uint16 Char);

	//ottiene intervalli di linee/parole/caratteri. Se from < to vengono scritti dall'ultimo al primo
	str getLines(uint16 From, uint16 To);
	str getWords(uint16 From, uint16 To);
	str getWords(uint16 Line, uint16 From, uint16 To);
	str getChars(uint16 From, uint16 To);
	str getChars(uint16 Line, uint16 From, uint16 To);
	str getChars(uint16 Line, uint16 Word, uint16 From, uint16 To);

	//aggiunge linee/parole/caratteri. Forma relativa e non.
	bool addLine(uint16, str = "");		//aggiunge una linea (vuota) nella posizione specificata
	bool addWord(uint16, str);			//aggiunge una parola nella posizione specificata rispetto all'inizio del file
	bool addWord(uint16, uint16, str);		//aggiunge una parola nella posizione specificata della linea specificata
	bool addChar(uint16, char);			//aggiunge un carattere nella posizione specificata rispetto all'inizio del file
	bool addChar(uint16, uint16, char);		//aggiunge un carattere nella posizione specificata della linea specificata
	bool addChar(uint16, uint16, uint16, char);


	//FARE vedere se metterli o no
	//aggiunge linee/parole/caratteri in cima a file/linee/parole. Forma relativa e non.
	bool affixLine(str = "");			//aggiunge una linea all'inizio del file
	bool affixWord(str);				//aggiunge una parola all'inizio del file
	bool affixWordLine(uint16, str);		//aggiunge una parola all'inizio della linea
	bool affixChar(char);				//aggiunge un carattere all'inizio del file
	bool affixCharWord(uint16, char);		//aggiunge un carattere prima della parola con posizione specificata rispetto all'inizio del file
	bool affixCharWord(uint16, uint16, char);//aggiunge un carattere prima della parola con posizione specificata della linea specificata
	bool affixCharLine(uint16, char);		//aggiunge un carattere all'inizio della linea specificata


	//aggiunge linee/parole/caratteri in fondo a file/linee/parole. Forma relativa e non.
	bool appendLine(str = "");			//aggiunge una linea alla fine del file
	bool appendWord(str);				//aggiunge una parola alla fine del file
	bool appendWordLine(uint16, str);		//aggiunge una parola alla fine della linea
	bool appendChar(char);				//aggiunge un carattere alla fine del file
	bool appendCharLine(uint16, char);		//aggiunge un carattere alla fine della linea specificata
	bool appendCharWord(uint16, char);		//aggiunge un carattere dopo la parola con posizione specificata rispetto all'inizio del file
	bool appendCharWord(uint16, uint16, char);//aggiunge un carattere dopo la parola con posizione specificata della linea specificata

	/*
	Replaces a line using a temp file
	If the specified line is out of bounds some newlines get created
	Closes the file before returning, since it has been opened in a different way
	Returns false if the files couldn't be opened, otherwise true
	*/
	bool replaceLine(uint16 Line, str Replacement);
	/*
	Replaces a word using a temp file
	Closes the file before returning, since it has been opened in a different way
	Returns false if the files couldn't be opened or if the
		specified word is out of bounds, otherwise true
	*/
	bool replaceWord(uint16 Word, str Replacement);
	/*
	Replaces a word in a line using a temp file
	Closes the file before returning, since it has been opened in a different way
	Returns false if the files couldn't be opened or if the
		specified word is out of bounds, otherwise true
	*/
	bool replaceWord(uint16 Line, uint16 Word, str Replacement);
	/*
	Replaces a char using a temp file
	Closes the file before returning, since it has been opened in a different way
	Returns false if the files couldn't be opened or if the
		specified char is out of bounds, otherwise true
	*/
	bool replaceChar(uint16 Char, char Replacement);
	/*
	Replaces a char in a line using a temp file
	Closes the file before returning, since it has been opened in a different way
	Returns false if the files couldn't be opened or if the
		specified char is out of bounds, otherwise true
	*/
	bool replaceChar(uint16 Line, uint16 Char, char Replacement);
	/*
	Replaces a char in a word in a line using a temp file
	Closes the file before returning, since it has been opened in a different way
	Returns false if the files couldn't be opened or if the
		specified char is out of bounds, otherwise true
	*/
	bool replaceChar(uint16 Line, uint16 Word, uint16 Char, char Replacement);

	//rimuove linee/parole/caratteri. Per le parole rimuove anche uno spazio. Forma relativa e non. 
	bool deleteLine(uint16 Line);
	bool deleteWord(uint16 Word);
	bool deleteWord(uint16 Line, uint16 Word);
	bool deleteChar(uint16 Char);
	bool deleteChar(uint16 Line, uint16 Char);
	bool deleteChar(uint16 Line, uint16 Word, uint16 Char);


	//rimuove gli a capo alla fine del file
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
	

	char operator[] (uint16 Pos);				//ritorna il carattere in posizione specificata dal parametro
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
	operator std::string(); //FARE non va
	/*
	Saves all the file in a string and returns it
	Returns "" if the file couldn't be opened
	*/
	str string();
	operator bool();							//ritorna 1 se il file e' aperto
	bool operator! ();							//ritorna 1 se il file e' chiuso
};