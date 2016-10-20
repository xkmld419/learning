/*VER: 2*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "Directory.h"
#include "File.h"
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>

#define FILE_LIST_BUFFER_LEN	1000

//##ModelId=41DCAC7E0028
Directory::Directory(char *sPath)
{
	int iLen;

	strcpy (m_sPathName, sPath);

	iLen = strlen (sPath);
	if (iLen > 0) {
		if (sPath[iLen-1] != '/')
			strcat (m_sPathName, "/");
	}
	m_poFileList = NULL;
	m_pDir = NULL;
	m_iFileNum = 0;
	m_iMaxFileNum=1000;
	m_tmpCurrGetFileNum=0;
}

//##ModelId=41DCAC7E0032
Directory::~Directory()
{
	if (m_poFileList != NULL)
		delete[] m_poFileList;
}

//##ModelId=41DCAC7E0033
bool Directory::makeDir(char *subDir)
{
	int iLen;
	char sTmpDir[200];
	char sTmpNew[200];
	char *pt = NULL;
	DIR *dirp;

	/* 空串 */
	iLen = strlen (subDir);
	if (iLen == 0)
		return false;

	strcpy (sTmpDir, subDir);

	if (sTmpDir[0] == '/')
		strcpy (sTmpNew, "/");
	else
		strcpy (sTmpNew, this->m_sPathName);

	pt = strtok (sTmpDir, "/");
	while (pt != NULL) {
		if (strlen (pt) == 0)
			continue;

		strcat (sTmpNew, pt);
		mkdir (sTmpNew, 0777);

		chmod (sTmpNew, 0777);
		strcat (sTmpNew, "/");

		pt = strtok (NULL, "/");
	}

	/* 测试目录是否可打开, 作为最终是否创建成功的标志 */
	dirp = opendir (sTmpNew);
	if (dirp == NULL)
		return false;
	else {
		closedir (dirp);
		return true;
	}
}

//##ModelId=41DCAC7E003B
int Directory::getFileList()
{
	DIR *dirp;
	struct dirent *dp;
	int iCount = 0;

	dirp = opendir (m_sPathName);
	if (dirp == NULL)
		return -1;

	while ((dp = readdir (dirp)) != NULL) {
		if (dp->d_name[0] == '.')
			continue;

		/* 有文件, 才申请空间 */
		if (iCount == 0) {
			m_poFileList = (File *) new File [FILE_LIST_BUFFER_LEN];
		}

		/* 缓冲满退出 */
		if (iCount >= FILE_LIST_BUFFER_LEN)
			break;
		
		m_poFileList[iCount] = File (m_sPathName, dp->d_name);

		iCount++;
	}	
	closedir (dirp);

	m_iFileNum = iCount;
	return m_iFileNum;
}


//##ModelId=41DCAC7E003C
int Directory::freeFileList()
{
	int iNum;

	if (m_poFileList != NULL)
		delete[] m_poFileList;

	iNum = m_iFileNum;

	m_iFileNum = 0;
	m_poFileList = (File *) NULL;
	
	return iNum;
}

//##ModelId=41DCAC7E003D
int Directory::getFileNum()
{
	return m_iFileNum;
}

//##ModelId=41DCAC7E0031
Directory::Directory()
{
	int iLen;

	getcwd (m_sPathName, 100);

	iLen = strlen (m_sPathName);
	if (iLen > 0) {
		if (m_sPathName[iLen-1] != '/')
			strcat (m_sPathName, "/");
	}
	m_poFileList = NULL;
	m_iFileNum = 0;
}

//##ModelId=41DCAC7E003E
File *Directory::getFile(int index)
{
	return (File *)&m_poFileList[index];
}

//##ModelId=41DCAC7E0046
char *Directory::getPathName()
{
	return m_sPathName;
}

//##ModelId=41DCAC7E0047
File *Directory::nextFile()
{
	struct dirent *dp;
	int iCount = 0;
        char sName[500];
        struct stat statbuf; 
  
	if (m_pDir == NULL)
		m_pDir = opendir (m_sPathName);
	if (m_pDir == NULL)
		return NULL;

	while ((dp=readdir(m_pDir)) != NULL) {
		
		if (dp->d_name[0] == '.')
			continue;
    snprintf(sName, 500, "%s%s", m_sPathName, dp->d_name);        
    if(lstat(sName, &statbuf)<0
         || S_ISDIR(statbuf.st_mode)!=0  )
         continue; 
		m_oCurFile = File (m_sPathName, dp->d_name);
		break;
	}	

	if (dp == NULL) {
		closedir (m_pDir);
		m_pDir = NULL;
		return NULL;
	}

	return (File *)&m_oCurFile;
}


File *Directory::nextFileRecursive( )
{
    static list<File> sv_AllFiles;
    static bool bFirst=true;
    
    if( sv_AllFiles.empty() )
        {
        if(bFirst==true)
            {
            m_tmpCurrGetFileNum=0;
            getFileRecursive(m_sPathName, sv_AllFiles);
            bFirst=false;
            }
            
        //////文件夹可能为空,或者都不能匹配文件名    
        if( sv_AllFiles.empty() )
            {
            bFirst=true;
            return NULL;
            }
        }


     m_oCurFile = sv_AllFiles.front();
     sv_AllFiles.pop_front();
     return (File *)&m_oCurFile;
     
}


int Directory::getFileRecursive(char * i_sPath , list<File> &i_sv_AllFiles)
{
  struct dirent *dp;
  char sName[500];
  struct stat statbuf;
  
  DIR * pDir=NULL;

  pDir = opendir(i_sPath);
  if (pDir == NULL)
    return 0;

  while ((dp=readdir(pDir)) != NULL) {
    
    if (dp->d_name[0] == '.')
      continue;
    snprintf(sName, 500, "%s%s", i_sPath, dp->d_name);        
    if(lstat(sName, &statbuf)<0 )
      continue;
    if( S_ISDIR(statbuf.st_mode)==0  )
      { ////是文件
      File thefile(i_sPath, dp->d_name);
      i_sv_AllFiles.push_back(thefile);
      m_tmpCurrGetFileNum++;
      if(m_tmpCurrGetFileNum>=m_iMaxFileNum) break;
      continue; 
      }
    strcat(sName,"/");  
    this->getFileRecursive(sName, i_sv_AllFiles);
  } 

  closedir (pDir);
  pDir = NULL;
  return 1;
  
}

void Directory::setMaxFileNum(int maxFileNum)
{
	m_iMaxFileNum=maxFileNum;
}

