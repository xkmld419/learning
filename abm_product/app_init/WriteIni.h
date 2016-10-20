/*VER: 1*/ 
#ifndef _WRITEINI_H
#define _WRITEINI_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef LINEMAXCHARS
#define LINEMAXCHARS	150
#endif

#define FILEMAXSIZE  102400

class WriteIni{
private:

public:
	WriteIni();
	~WriteIni();
	char *trim( char *str );
	char *TruncStr( char *sp , char *sDst , int len);
	int SectionAndIdentCheck(char  const *IniFile,char const *Section,char const *Ident,
		int &length1,int &length2,int &length3,int &length4);
	int WriteIniString(char const *IniFile,char const *Section, char const *Ident, const char *sItem);
	int DelIniBySection(char const *IniFile,char const *Section);
	int DelIniByIdent(char const*IniFile,char const *Section,char const *Ident);
};

#endif


