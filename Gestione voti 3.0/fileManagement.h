/*
Forma relativa e non. = sia relativamente all'inizio del file che relativamente ad una linea.
*/

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

public:
	fstm file;
	str path, tempPath;
	

	bool checkOpen();							//ritorna 1 se il file si è aperto senza problemi

	bool pointTo(uint16 Line, uint16 Word, uint16 Char);				//i valori partono da 0 (0 = 1a riga/parola/carattere, 1 = 2a...); line e word vengono usati come offsets
	bool pointToBeg();

	uint16 countWords(str String);


public:
	File();
	File(str Path);
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

	//rimpiazza linee/parole/caratteri. Forma relativa e non.
	bool replaceLine(uint16 Line, str Replacement);
	bool replaceWord(uint16 Word, str Replacement);
	bool replaceWord(uint16 Line, uint16 Word, str Replacement);
	bool replaceChar(uint16 Char, char Replacement);
	bool replaceChar(uint16 Line, uint16 Char, char Replacement);
	bool replaceChar(uint16 Line, uint16 Word, uint16 Char, char Replacement);

	//rimuove linee/parole/caratteri. Forma relativa e non. Per le parole rimuove anche uno spazio
	bool deleteLine(uint16);
	bool deleteWord(uint16);
	bool deleteWord(uint16, uint16);
	bool deleteChar(uint16);
	bool deleteChar(uint16, uint16);
	bool deleteChar(uint16, uint16, uint16);

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

	//aggiunge linee/parole/caratteri. Forma relativa e non.
	bool addLine(uint16, str = "");		//aggiunge una linea (vuota) nella posizione specificata
	bool addWord(uint16, str);			//aggiunge una parola nella posizione specificata rispetto all'inizio del file
	bool addWord(uint16, uint16, str);		//aggiunge una parola nella posizione specificata della linea specificata
	bool addChar(uint16, char);			//aggiunge un carattere nella posizione specificata rispetto all'inizio del file
	bool addChar(uint16, uint16, char);		//aggiunge un carattere nella posizione specificata della linea specificata
	bool addChar(uint16, uint16, uint16, char);

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
	File operator+ (File);					//ritorna un file con all'inizio il contenuto di questo file e alla fine quello del parametro
	File operator+ (fstm);
	bool operator+= (File);				//come operator<<
	bool operator+= (fstm);
	bool operator^ (File);				//scambia il contenuto dei due files
	bool operator^ (fstm);
	

	char operator[] (uint16 Pos);				//ritorna il carattere in posizione specificata dal parametro
	bool operator== (File File);				//se i due file hanno lo stesso path ritorna 1
	bool operator== (str Path);
	bool hasSamePath(File File);
	bool hasSamePath(str Path);
	bool operator!= (File File);				//se i due file non hanno lo stesso path ritorna 1
	bool operator!= (str Path);
	bool hasntSamePath(File File);
	bool hasntSamePath(str Path);
	operator char*();							//scrive tutto il file in un array di caratteri
	char * toArray();
	operator str();								//scrive tutto il file in una stringa
	str toString();
	operator bool();							//ritorna 1 se il file e' aperto
	bool operator! ();							//ritorna 1 se il file e' chiuso
};