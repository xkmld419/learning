/*VER: 1*/ 
#ifndef PUBLIC__SEGMENT_MGR_H 
#define PUBLIC__SEGMENT_MGR_H  

#include <HashList.h>
#include <vector>
using namespace std;

class SegmentInfo
{
    public:
            int	 m_iSegmID;
	        int  m_iSegmType;
            char m_sBeginSegm[32];
            char m_sEndSegm[32];
            char m_sEffDate[16];
            char m_sExpDate[16];
            unsigned int m_iNext;
            unsigned int m_iNextInfo; 
			bool bInited;
};

class SegmentData 
{
  public:
 
	int	 m_iSegmID;
	int  m_iSegmType;
    char m_sBeginSegm[32];
    char m_sEndSegm[32];
    char m_sEffDate[16];
    char m_sExpDate[16];

	int m_iNext;
};

class SegmentMgr
{
  private:		
	struct Node{
		int m_iPos;	
		int m_iCnt;
	};
  public:
	SegmentMgr();
  public:
    static void init();
    static bool atSegment(const char *sSegmValue, int iSegmType, const char *sDate=0);
	static void showAll();
        
	/* Use HashList to hold Level_1 contents */
	static HashList<Node> *m_poNodeHashList;
	/* Use vector to hold Level_2 contents */
	static vector<int> *m_poVetContent;	

    static SegmentData *m_poSegmentData;
	static int m_iCount;
    static bool bInited;
	
	static void dprintf(const char *sfmt, ...);
};

#endif /* PUBLIC__SEGMENT_MGR_H */
