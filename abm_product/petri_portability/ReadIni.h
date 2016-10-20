/*VER: 1*/ 
#ifndef _READINI_H
#define	_READINI_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define LINEMAXCHARS	150

class ReadIni {
private:

public:
	ReadIni ();
	~ReadIni ();
	char *trim( char *str );
	char *readIniString( char const *IniFile, char const *Section, char const *Ident, char *sItem, char *defstr );
	int readIniInteger( char const *IniFile, char const *Section, char const *Ident, int defint );
	unsigned int readIniUInteger( char const *IniFile, char const *Section, char const *Ident, int defint );
	char *readIniStr( FILE *fd, char *Section, char *Ident, char *sItem, char *defstr );
	int readIniInt( FILE *fd, char *Section, char *Ident, int defint );
	int readIniUInt( FILE *fd, char *Section, char *Ident, int defint );
	char *TruncStr( char *sp , char *sDst , int len);
	int SectionCheck(char *IniFile,char *Section);
	char *SubStr (char *, char *, int);
};

#endif

