#include "FileV2.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

FileV2::FileV2()
{
	m_poFp = NULL;
}

FileV2::~FileV2()
{
	if (m_poFp != NULL) 
		closeFile();
}

int FileV2::openFile(char const* sFile, int iMode)
{
	if (sFile == NULL)
		return -1;
	if (m_poFp != NULL) {
		if (closeFile() != 0)
			return -1;
	}
	
	char sType[8] = "a+";
	if (iMode == OPEN_BIN)
		strcat(sType, "b");
		
	while ((m_poFp=fopen(sFile, sType)) == NULL) {
		if (errno != EINTR) {
			perror("fopen");
			return -1;
		}			
	}  
	
	return 0;
}

int FileV2::write(void *pAddr, int iSize)
{
	while (fwrite(pAddr, iSize, 1, m_poFp) != 1) {
		if (errno != EINTR) {
			perror("fwrite");
			return -1;	
		}	
	}	
	return iSize;
}	

int FileV2::read(void *pAddr, int iSize)
{
	if (feof(m_poFp)) {
		if (closeFile() != 0)
			return -1;
		else
			return 0;		
	}	
	
	while (fread(pAddr, iSize, 1, m_poFp) != 1) {
		if (errno != EINTR) {
			perror("fread");
			return -1;	
		}	
	}
	
	return iSize;
}	

int FileV2::closeFile()
{
	if (m_poFp == NULL)
		return 0;
	while (fclose(m_poFp) != 0) {
		if (errno != EINTR) {
			perror("fclose");
			return -1;	
		}	
	}		
	m_poFp = NULL;
	return 0;
}	
