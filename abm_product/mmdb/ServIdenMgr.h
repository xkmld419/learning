/*VER: 1*/ 
#ifndef SERVIDENMGR_H_HEADER_INCLUDED
#define SERVIDENMGR_H_HEADER_INCLUDED

#include "SHMData.h"
#include "SHMIntHashIndex.h"
#include "SHMStringTreeIndex.h"

#define CENTER_SERVIDEN_COUNT	(ParamDefineMgr::getLongParam("MEMORY_DB", "CENTER_SERVIDEN_COUNT"))

#define CAL_COUNT(count) \
	(\
	(int ) \
		( \
			(count)>1000000 ? \
			((count)*1.05) : \
			( \
				(count)>100000? \
				((count)*1.1): \
				(\
					(count)>10000? \
					((count)*1.15) : \
					( \
						(count)>1000? \
						10100 : \
						2000 \
					) \
				) \
			)\
		) \
	 ) 

struct ServIdenCenterData{					//¼ÇÂ¼½á¹¹
  char m_sAccNbr[32];
  long m_lServID;
  char m_sEffDate[16];
  char m_sExpDate[16];
  int m_iType;
	unsigned int iNextServOffset;
};

class UserInfoLogMgr
{
  public:
    UserInfoLogMgr();
    ~UserInfoLogMgr();

    void updateOPLog(long lOpSeq);
    void commit();
    
  private:
    int m_iCursor;
    int const m_iMax;
    long * m_plBuf;
};

class ServIdenMgr
{
  public:
	ServIdenMgr();

	~ServIdenMgr();

	void load();
	void unload();
	static int getServIdenData(char *sAccNbr,char sTime[16]);
  int UpdateCServIdenMemory();
  private:
	static SHMData<ServIdenCenterData> * m_pocServIdenData;
	static SHMStringTreeIndex * m_pocServIdenIndex;

  private:
	static bool m_bUploaded;
	UserInfoLogMgr * m_poUserLogMgr;
	
#ifdef USERINFO_OFFSET_ALLOWED
   public:
    static int m_offsetSecs  ;

    static int m_beginOffsetSecs;

    static void loadOffsetSecs();
#endif
    

};

#endif

