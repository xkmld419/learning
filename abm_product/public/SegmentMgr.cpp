/*VER: 1*/ 
#include "SegmentMgr.h"
#include "FormatMethod.h"
#include "Process.h"
#include "Date.h"
#include "MBC.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

#undef DEBUG

bool SegmentMgr::bInited = false;
SegmentData *SegmentMgr::m_poSegmentData = 0;
vector<int> *SegmentMgr::m_poVetContent = 0;
HashList<SegmentMgr::Node> *SegmentMgr::m_poNodeHashList = 0;
int SegmentMgr::m_iCount = 0;

SegmentMgr::SegmentMgr()
{
}

#ifdef PRIVATE_MEMORY
void SegmentMgr::init()
{
    bInited = true;

    TOCIQuery qry(Environment::getDBConn());
	qry.setSQL("select count(1) from b_general_segment");
	qry.open();

	if(qry.next()){
		m_iCount = qry.field(0).asInteger();
	}
	qry.close();
    
    if(m_iCount<=0) return;
    
    m_poSegmentData = new SegmentData[m_iCount];
	m_poVetContent = new vector<int>();
	m_poNodeHashList = new HashList<Node>(50); 
    
    if(!m_poSegmentData){
		 THROW(MBC_SegmentMgr+1);
	}
	memset(m_poSegmentData, 0, sizeof(SegmentData)*m_iCount );
	
	/* Different SegmType belong to different group, 
	 * use SegmType as the Level_1 contents.
	 * In each group, each SegmValue sorted by eff_date in descding order,
	 * use different SegmValue as the Level_2 contents in the group.
	 * 
	 *       Level_1            Level_2
	 *      				   
	 *      0|-SegmType1-------0|-SegmValue1->SegmValue1->SegmValue1->...
	 *       |                  |
	 *       |                 1|-SegmValue2->SegmValue2->...
	 *       |                  |
	 *       |                 :|...
	 *       |                  |
	 *       |                 n|-SegmValueN->...
	 *       |               
	 *  (n+1)|-SegmType2---(n+1)|-SegmValue(n+1)->SegmValue(n+1)->...
	 *       |                  |
	 *       |             (n+2)|-SegmValue(n+2)->...
	 *       |                  |
	 *       |                 :|...
	 *       |                  |
	 *       |             (n+m)|-SegmValue(n+m)->...
	 *       :
	 *       :  
	 */
	qry.setSQL("select segment_id, segment_type, begin_segm, end_segm, "
			"nvl(eff_date, sysdate) eff_date, "
			"nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date "
			"from b_general_segment "
			"order by segment_type, begin_segm, eff_date desc");
	qry.open();

    int i=0, k=0;
	int lastType=-1, lastPos=0, currCnt;
	char sTemp[8];
	Node node;
	m_poVetContent->clear();
	while(qry.next()){
		if (i==m_iCount) THROW (MBC_SegmentMgr+2);
	     
		m_poSegmentData[i].m_iSegmID = qry.field(0).asInteger();
		strcpy(sTemp, qry.field(1).asString());
		m_poSegmentData[i].m_iSegmType = sTemp[2] | 0x20; 

		strcpy(m_poSegmentData[i].m_sBeginSegm, qry.field(2).asString());
		strcpy(m_poSegmentData[i].m_sEndSegm, qry.field(3).asString());
		strcpy(m_poSegmentData[i].m_sEffDate, qry.field(4).asString());
		strcpy(m_poSegmentData[i].m_sExpDate, qry.field(5).asString());
		
		m_poSegmentData[i].m_iNext = -1;
		
		if (lastType == m_poSegmentData[i].m_iSegmType) {
			if (strcmp(m_poSegmentData[i-1].m_sBeginSegm, m_poSegmentData[i].m_sBeginSegm)==0){
				m_poSegmentData[i-1].m_iNext = i;
			}else {
				/* Use vector to hold the Level_2 contents */
				m_poVetContent->push_back(i);
			}
		}else if (lastType!=-1){
			node.m_iPos = lastPos;
			node.m_iCnt = m_poVetContent->size() - lastPos;
			/* Use HashList to hold the Level_1 contents */
			dprintf("lastType:%d, iPos:%d, iCnt:%d\n", lastType, node.m_iPos, node.m_iCnt);
			m_poNodeHashList->add(lastType, node);
			lastType = m_poSegmentData[i].m_iSegmType;
			lastPos  = m_poVetContent->size();
			m_poVetContent->push_back(i);
		}else {
			lastPos = 0;
			lastType = m_poSegmentData[i].m_iSegmType;
			m_poVetContent->push_back(i);
		}
	    i++;
	}
	node.m_iPos = lastPos;
	node.m_iCnt = m_poVetContent->size() - lastPos;
	dprintf("lastType:%d, iPos:%d, iCnt:%d\n", lastType, node.m_iPos, node.m_iCnt);
	m_poNodeHashList->add(lastType, node);

	qry.close();
	
	return;
}
#else
void SegmentMgr::init()
{
    bInited = true;
	m_iCount = G_PPARAMINFO->m_poSegmentInfoData->getCount();
	//
	//if(G_PPARAMINFO->m_poSegmentInfoList[m_iCount].bInited == true)
    if(m_iCount<=0) return;
    m_poSegmentData = new SegmentData[m_iCount];
	m_poVetContent = new vector<int>();
	m_poNodeHashList = new HashList<Node>(50); 
    
    if(!m_poSegmentData){
		 THROW(MBC_ParamInfoMgr+91);
	}
	memset(m_poSegmentData, 0, sizeof(SegmentData)*m_iCount );
	
    int i=0, k=0;
	int lastType=-1, lastPos=0, currCnt;
	char sTemp[8];
	Node node;
	m_poVetContent->clear();
	int iTmp = 1;
	while(iTmp){
		if (i==m_iCount) THROW (MBC_SegmentMgr+2);
	    SegmentInfo *pSegmentInfo = &(G_PPARAMINFO->m_poSegmentInfoList[iTmp]); 
		m_poSegmentData[i].m_iSegmID = pSegmentInfo->m_iSegmID;
		m_poSegmentData[i].m_iSegmType = pSegmentInfo->m_iSegmType; 

		strcpy(m_poSegmentData[i].m_sBeginSegm, pSegmentInfo->m_sBeginSegm);
		strcpy(m_poSegmentData[i].m_sEndSegm, pSegmentInfo->m_sEndSegm);
		strcpy(m_poSegmentData[i].m_sEffDate, pSegmentInfo->m_sEffDate);
		strcpy(m_poSegmentData[i].m_sExpDate, pSegmentInfo->m_sExpDate);
		
		m_poSegmentData[i].m_iNext = -1;
		// 
		if (lastType == m_poSegmentData[i].m_iSegmType) {
			if (strcmp(m_poSegmentData[i-1].m_sBeginSegm, m_poSegmentData[i].m_sBeginSegm)==0){
				m_poSegmentData[i-1].m_iNext = i;
			}else {
				/* Use vector to hold the Level_2 contents */
				m_poVetContent->push_back(i);
			}
		}else if (lastType!=-1){
			node.m_iPos = lastPos;
			node.m_iCnt = m_poVetContent->size() - lastPos;
			/* Use HashList to hold the Level_1 contents */
			dprintf("lastType:%d, iPos:%d, iCnt:%d\n", lastType, node.m_iPos, node.m_iCnt);
			m_poNodeHashList->add(lastType, node);
			lastType = m_poSegmentData[i].m_iSegmType;
			lastPos  = m_poVetContent->size();
			m_poVetContent->push_back(i);
		}else {
			lastPos = 0;
			lastType = m_poSegmentData[i].m_iSegmType;
			m_poVetContent->push_back(i);
		}
	    i++;
		if(pSegmentInfo->m_iNextInfo) iTmp = pSegmentInfo->m_iNext;
		else iTmp = 0;
	}
	node.m_iPos = lastPos;
	node.m_iCnt = m_poVetContent->size() - lastPos;
	dprintf("lastType:%d, iPos:%d, iCnt:%d\n", lastType, node.m_iPos, node.m_iCnt);
	m_poNodeHashList->add(lastType, node);
	
	return;
}
#endif


// 判断IMSI是否在配置的段内
bool SegmentMgr::atSegment(const char *sSegmValue, int iSegmType, const char *sDate)
{
#ifndef PRIVATE_MEMORY       
if(m_iCount){
	if(G_PPARAMINFO->m_poSegmentInfoList[m_iCount].bInited == false)
		bInited = false;
}else{
		//bInited = false;	
		printf("The SegmentInfo load num is zero , please check this...\n");
}
#endif
    if(!bInited){
        init();
	}

	int i, m;
	int b, e;
	Date d;

	Node node;
	iSegmType |= 0x20;
	if (!m_poNodeHashList->get(iSegmType, &node)){
		dprintf("Not found this iSegmType: %d !\n", iSegmType);
		return false;
	}
	if (!sDate) sDate = d.toString();
	else {
		d = Date(sDate);
		sDate = d.toString();
	}
	dprintf("sSegmValue:%s iSegmType:%d, sDate:%s, iPos:%d, iCnt:%d\n", 
			sSegmValue, iSegmType, sDate, node.m_iPos, node.m_iCnt);

	e = node.m_iPos + node.m_iCnt;
	b = node.m_iPos-1;

	/* using binary search */
	while (b<e-1){
		m = (b+e)/2;
		i = (*m_poVetContent)[m];
		if (strcmp(sSegmValue, m_poSegmentData[i].m_sBeginSegm)<0){
			e = m;
		}else if (strcmp(sSegmValue, m_poSegmentData[i].m_sEndSegm)>0){
			b = m;
		}else {
			while (i>=0){
				if (strcmp(sDate, m_poSegmentData[i].m_sExpDate)>=0){
					return false;
				}else if (strcmp(sDate, m_poSegmentData[i].m_sEffDate)>=0){
					dprintf("SegmID:%d, BeginSegm:%s, EndSegm:%s, EffDate:%s ExpDate:%s\n", 
							m_poSegmentData[i].m_iSegmID, m_poSegmentData[i].m_sBeginSegm, 
							m_poSegmentData[i].m_sEndSegm, m_poSegmentData[i].m_sEffDate, 
							m_poSegmentData[i].m_sExpDate);
					return true;
				}else{
					i = m_poSegmentData[i].m_iNext;
					continue;
				}
			}
			return false;
		}
	}
    return false;
}
 
void SegmentMgr::showAll()
{
#ifndef PRIVATE_MEMORY       
if(m_iCount){
	if(G_PPARAMINFO->m_poSegmentInfoList[m_iCount].bInited == false)
		bInited = false;
}else{
//		bInited = false;	
		printf("The SegmentInfo load num is zero , please check this...\n");
}
#endif   
    if(!bInited){
        init();
	}

	int i, m, cnt;
	printf("All:\n");
	printf("%-15s %-15s %-15s %-15s %-15s %-15s %-8s\n",
			"SegmID", "SegmType","BeginSegm", "EndSegm", "EffDate", "EndDate", "iNext");
	for (i=0; i<m_iCount; i++){
		printf("%-15d %-15c %-15s %-15s %-15s %-15s %-8d\n",
				m_poSegmentData[i].m_iSegmID,
				m_poSegmentData[i].m_iSegmType,
				m_poSegmentData[i].m_sBeginSegm,
				m_poSegmentData[i].m_sEndSegm,
				m_poSegmentData[i].m_sEffDate,
				m_poSegmentData[i].m_sExpDate,
				m_poSegmentData[i].m_iNext
				);
	}
	printf("The level_2 contents:\n");
	printf("%-15s %-15s %-15s %-15s %-15s %-15s %-8s\n",
			"SegmID", "SegmType","BeginSegm", "EndSegm", "EffDate", "EndDate", "iNext");
	cnt=m_poVetContent->size();
	for (m=0; m<cnt; m++){
		i = (*m_poVetContent)[m];
		printf("%-15d %-15c %-15s %-15s %-15s %-15s %-8d\n",
				m_poSegmentData[i].m_iSegmID,
				m_poSegmentData[i].m_iSegmType,
				m_poSegmentData[i].m_sBeginSegm,
				m_poSegmentData[i].m_sEndSegm,
				m_poSegmentData[i].m_sEffDate,
				m_poSegmentData[i].m_sExpDate,
				m_poSegmentData[i].m_iNext
				);
	}
}

void SegmentMgr::dprintf(const char *sfmt, ...)
{
#ifdef DEBUG
	va_list ap;
	va_start(ap, sfmt);
	vprintf(sfmt, ap);
	va_end(ap);
#endif
}

