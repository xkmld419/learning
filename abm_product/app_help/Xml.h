#ifndef XML_H_INCLUDED_C4353D06
#define XML_H_INCLUDED_C4353D06

#include <iostream>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>
#include <clocale>
#include "help.h"
#include "Markup.h"
#include "hbcfg.h"
#include "Log.h"
#include "MBC_28.h"
#include "CommandCom.h"
#ifdef	getch
#undef	getch
#endif
#define PARAM_STEP 1
/*
template <class Type>
class DeleteElem
{
public:

	void operator()(Type elem)
	{
		if(elem)
		{
			delete elem;
			elem=NULL;
		}
	}
};
*/
template <class Type>
void DeleteElem(Type elem)
{
	if(elem)
	{
		delete elem;
		elem=NULL;
	}
}

class Xml
{
public :
	Xml();
	~Xml();
	bool load(const char* fileName);
	bool loadFile(const char* file);
	bool loadVersion();
	bool loadCommand();
	bool loadParam();
	void parse(int argc,char argv[ARG_NUM][ARG_VAL_LEN]);
  void displayVersion();
  void displayCommand(char *cmd=NULL,char *subcmd=NULL);
  void displayParam(char *paramName=NULL);
  
public :
	help *m_xmlHelp;
	CMarkup m_xml;
	CommandCom *m_pCommandComm;
	bool bAlarmLog;
private :
	char getch(void);
};
#endif /* XML_H_INCLUDED_C4353D06*/
