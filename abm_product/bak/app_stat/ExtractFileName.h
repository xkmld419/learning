#ifndef APP_STAT_EXTRA_FILE_NAME_H
#define APP_STAT_EXTRA_FILE_NAME_H

class ExtractFileName
{
	
public:
	ExtractFileName();
	virtual ~ExtractFileName();
	
	static char * GetSepData(char * pData_,int Num=1,char pSep_='_');
	static char * GetFileName(char * pData,char pSep='/');

private:
	static char FILENAME[1024];
	static char FILESEP[1024];
		
};

#endif
