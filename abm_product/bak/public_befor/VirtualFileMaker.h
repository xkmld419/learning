/*VER: 1*/ 
#ifndef VIRTUAL_FILE_MAKER_H
#define VIRTUAL_FILE_MAKER_H

class VirtualFileMaker
{
  public:
	static int makeVirtualFile(char const *sFileName, int iProcessID);
	static void setFileState(int iFileID, char const *sState);
    static void setSourceType(int iFileID, int iSourceType);
};

#endif
