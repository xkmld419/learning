#include "SHMInfoCtl.h"
#include "ThreeLogGroup.h"
#include "Log.h"
#include <vector>


int SHMInfoCtl::create()
{   
    //创建共享内存
    ABMException oExp;
    if (SHM(openSHM(oExp, true)) != 0) {
    		ThreeLogGroup oLogGroup;
    		oLogGroup.log(MBC_MEM_CREATE_FAIL,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"创建共享内存失败!");
        Log::log(0, "SHMInfoCtl::create create SHM failed!");
        return -1;
    }
    return 0;
}

int SHMInfoCtl::attach()
{
    //链接共享内存
    ABMException oExp;
    if (SHM(openSHM(oExp, false)) != 0) {
    		ThreeLogGroup oLogGroup;
    		oLogGroup.log(MBC_MEM_LINK_FAIL,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"共享内存连接失败!");
        Log::log(0, "SHMInfoCtl::attach link SHM failed!");
        return -1;
    }
    return 0;
}

void SHMInfoCtl::getAllMin(const char *sBegin, const char *sEnd, std::vector<MinNbrStruct> &vec)
{
    if (!sBegin || !sEnd || !strlen(sBegin))
        return;
    MinNbrStruct sMin;    
    if (!strcmp(sBegin, sEnd)) {
        snprintf(sMin.m_sMinNbr, sizeof(MinNbrStruct), "%s", sBegin);
        vec.push_back(sMin);
        return;        
    }
        
    char *p = (char *)sBegin;
    char sHead[16] = {0};
    for (int i=0; *p ;++i, ++p) {
        if (*p == '0')
            sHead[i] = '0';
    }    
    int iBegin = atoi(sBegin);
    int iEnd = atoi(sEnd);
    for (int i=iBegin; i<=iEnd; ++i) {
        snprintf(sMin.m_sMinNbr, sizeof(MinNbrStruct), "%s%d", sHead, i);
        vec.push_back(sMin);        
    }
    return;       
}

//表加载到共享内存
int SHMInfoCtl::loadHeadCodeIn(TimesTenCMD *pTmp,bool bShow)
{
    unsigned int i, uiTmp;
    int iTotalCnt = 0;
    int iUpdCnt = 0;
    HCodeInfoStruct oData;
    HCodeInfoStruct* pData;
    HCodeInfoStruct* pBegin = (HCodeInfoStruct *)(*(SHM(m_poHCodeData)));    
    TIMESTAMP_STRUCT oEffTm, oExpTm;
//    try {
        pTmp->Execute();
        while (!pTmp->FetchNext()) {
            pTmp->getColumn(1, oData.m_sHead);
            pTmp->getColumn(2, oData.m_sTspID);
            pTmp->getColumn(3, oData.m_sZoneCode);
            pTmp->getColumn(4, &oEffTm);
            pTmp->getColumn(5, &oExpTm);
            pTmp->getColumn(6, (int *)&oData.m_uiHeadID);
            snprintf(oData.m_sEffDate, sizeof(oData.m_sEffDate), "%04d%02d%02d", oEffTm.year, oEffTm.month, oEffTm.day);
            snprintf(oData.m_sExpDate, sizeof(oData.m_sExpDate), "%04d%02d%02d", oExpTm.year, oExpTm.month, oExpTm.day);
            oData.m_uiNextOffset = 0;
            
            iTotalCnt++;
            //检查数据是否存在
            if (SHM(m_poHCodeIndex)->get(oData.m_sHead, &oData.m_uiNextOffset)) {
                uiTmp = oData.m_uiNextOffset;
                do {
                    pData = pBegin + uiTmp;
                    if (pData->m_uiHeadID == oData.m_uiHeadID) {
                        pData->assign(oData);
                        iUpdCnt++;
                        break;
                    }    
                    uiTmp = pData->m_uiNextOffset;   
                } while (uiTmp);
                
                if (uiTmp) 
                    continue;  
            }
            i = SHM(m_poHCodeData)->malloc();  //申请数据空间
            memcpy((void *)(pBegin+i), (const void *)&oData, sizeof(oData));
            SHM(m_poHCodeIndex)->add(oData.m_sHead, i);  //增加索引
            
            if(bShow)
            	showHCodeInfo(oData);
        }
        pTmp->Close();
    		Log::log(0, "共导入 %d 条数据:更新 %d 条，新增 %d 条!", iTotalCnt, iUpdCnt, iTotalCnt - iUpdCnt);
        return 0;
//    }
/*    catch(TTStatus st) {
        pTmp->Close();
        Log::log(0, "loadHeadCode failed! err_msg=%s", st.err_msg);
    }*/
    return -1;    
}

int SHMInfoCtl::loadMinInfoIn(TimesTenCMD *pTmp,bool bShow)
{
    unsigned int uiIdx, uiTmp;
    int iTotalCnt = 0;
    int iUpdCnt = 0;
    MinInfoStruct oMin;
    MinInfoStruct *pData;
    MinInfoStruct *pBegin = (MinInfoStruct *)(*(SHM(m_poMinData)));
    TIMESTAMP_STRUCT oEffTm, oExpTm;
    std::vector<MinNbrStruct> vMin;
//    try {    
        pTmp->Execute();
        while (!pTmp->FetchNext()) {
            pTmp->getColumn(1, oMin.m_sBeginMin);
            pTmp->getColumn(2, oMin.m_sEndMin);
            pTmp->getColumn(3, &oMin.m_iUserFlag);
            pTmp->getColumn(4, &oMin.m_iCountryID);
            pTmp->getColumn(5, oMin.m_sHomeCarrCode);
            pTmp->getColumn(6, oMin.m_sZoneCode);
            pTmp->getColumn(7, &oMin.m_iUserType);
            pTmp->getColumn(8, &oEffTm);
            pTmp->getColumn(9, &oExpTm);
            pTmp->getColumn(10, (int *)&oMin.m_uiMinInfoID);
            snprintf(oMin.m_sEffDate, sizeof(oMin.m_sEffDate), "%04d%02d%02d", oEffTm.year, oEffTm.month, oEffTm.day);
            snprintf(oMin.m_sExpDate, sizeof(oMin.m_sExpDate), "%04d%02d%02d", oExpTm.year, oExpTm.month, oExpTm.day);
            
            //iTotalCnt++;
            vMin.clear();
            getAllMin(oMin.m_sBeginMin, oMin.m_sEndMin, vMin);
            for (int i=0; i<vMin.size(); ++i) {
            		iTotalCnt++;
                if (SHM(m_poMinIndex)->get(vMin[i].m_sMinNbr, &oMin.m_uiNextOffset)) {
                    uiTmp = oMin.m_uiNextOffset;
                    do {
                        pData = pBegin + uiTmp;
                        if (pData->m_uiMinInfoID == oMin.m_uiMinInfoID) {
                            pData->assign(oMin);
                            iUpdCnt++;
                            break;    
                        }
                        uiTmp = pData->m_uiNextOffset;        
                    } while (uiTmp);
                    if (uiTmp)
                        continue;               
                }
                uiIdx = SHM(m_poMinData)->malloc();
                memcpy((void *)(pBegin + uiIdx), (const void *)&oMin, sizeof(oMin));
                SHM(m_poMinIndex)->add(vMin[i].m_sMinNbr, uiIdx);    
            }
            
            if(bShow)
            {
            	for(int k=0;k<iTotalCnt;k++)
            		showMinInfo(oMin);
            }
        }
        pTmp->Close();
        Log::log(0, "共导入 %d 条数据:更新 %d 条，新增 %d 条!", iTotalCnt, iUpdCnt, iTotalCnt - iUpdCnt);
        return 0;    
//    }
/*    catch(TTStatus st) {
        pTmp->Close();
        Log::log(0, "loadMinInfo failed! err_msg=%s", st.err_msg);
    }*/
    return -1;     
}

int SHMInfoCtl::loadLocalHeadIn(TimesTenCMD *pTmp,bool bShow)
{
    unsigned int i, uiTmp;
    int iTotalCnt = 0;
    int iUpdCnt = 0;
    int iParterID, iExchangeID;
    HCodeInfoStruct oData;
    HCodeInfoStruct* pData;
    HCodeInfoStruct* pBegin = (HCodeInfoStruct *)(*(SHM(m_poHCodeData)));    
    TIMESTAMP_STRUCT oEffTm, oExpTm;
//    try {
        pTmp->Execute();
        while (!pTmp->FetchNext()) {
            pTmp->getColumn(1, oData.m_sHead);
            pTmp->getColumn(2, &iParterID);
            pTmp->getColumn(3, &iExchangeID);
            pTmp->getColumn(4, &oEffTm);
            pTmp->getColumn(5, &oExpTm);
            pTmp->getColumn(6, (int *)&oData.m_uiHeadID);
            snprintf(oData.m_sTspID, sizeof(oData.m_sTspID), "%d", iParterID);
            snprintf(oData.m_sZoneCode, sizeof(oData.m_sZoneCode), "%d", iExchangeID);
            snprintf(oData.m_sEffDate, sizeof(oData.m_sEffDate), "%04d%02d%02d", oEffTm.year, oEffTm.month, oEffTm.day);
            snprintf(oData.m_sExpDate, sizeof(oData.m_sExpDate), "%04d%02d%02d", oExpTm.year, oExpTm.month, oExpTm.day);
            oData.m_uiNextOffset = 0;
            
            iTotalCnt++;
            //检查数据是否存在
            if (SHM(m_poHCodeIndex)->get(oData.m_sHead, &oData.m_uiNextOffset)) {
                uiTmp = oData.m_uiNextOffset;
                do {
                    pData = pBegin + uiTmp;
                    if (pData->m_uiHeadID == oData.m_uiHeadID) {
                        pData->assign(oData);
                        iUpdCnt++;
                        break;
                    }    
                    uiTmp = pData->m_uiNextOffset;   
                } while (uiTmp);
                
                if (uiTmp) 
                    continue;  
            }
            //申请数据空间
            i = SHM(m_poHCodeData)->malloc();
            memcpy((void *)(pBegin+i), (const void *)&oData, sizeof(oData));
            //增加索引
            SHM(m_poHCodeIndex)->add(oData.m_sHead, i); 
            
            if(bShow)
            	showHCodeInfo(oData);
        }
        pTmp->Close();
        Log::log(0, "共导入 %d 条数据:更新 %d 条，新增 %d 条!", iTotalCnt, iUpdCnt, iTotalCnt - iUpdCnt);
        return 0;
//    }
/*    catch(TTStatus st) {
        pTmp->Close();
        Log::log(0, "loadLocalHead failed! err_msg=%s", st.err_msg);
    }*/
    return -1;    
}

int SHMInfoCtl::loadImsiInfoIn(TimesTenCMD *pTmp,bool bShow)
{
    unsigned int uiIdx, uiTmp;
    int iTotalCnt = 0;
    int iUpdCnt = 0;
    IMSIInfoStruct oIMSI;
    IMSIInfoStruct *pData;
    IMSIInfoStruct *pBegin = (IMSIInfoStruct *)(*(SHM(m_poIMSIData)));
    TIMESTAMP_STRUCT oEffTm, oExpTm;
    std::vector<MinNbrStruct> vIMSI;
//    try {    
        pTmp->Execute();
        while (!pTmp->FetchNext()) {
            pTmp->getColumn(1, oIMSI.m_sBeginIMSI);
            pTmp->getColumn(2, oIMSI.m_sEndIMSI);
            pTmp->getColumn(3, &oIMSI.m_iIMSIType);
            pTmp->getColumn(4, oIMSI.m_sSponsorCode);
            pTmp->getColumn(5, oIMSI.m_sZoneCode);
            pTmp->getColumn(6, &oEffTm);
            pTmp->getColumn(7, &oExpTm);
            pTmp->getColumn(8, (int *)&oIMSI.m_uiIMSIID);
            snprintf(oIMSI.m_sEffDate, sizeof(oIMSI.m_sEffDate), "%04d%02d%02d", oEffTm.year, oEffTm.month, oEffTm.day);
            snprintf(oIMSI.m_sExpDate, sizeof(oIMSI.m_sExpDate), "%04d%02d%02d", oExpTm.year, oExpTm.month, oExpTm.day);
            
            //iTotalCnt++;
            vIMSI.clear();
            getAllMin(oIMSI.m_sBeginIMSI, oIMSI.m_sEndIMSI, vIMSI);
            for (int i=0; i<vIMSI.size(); ++i) {
            		iTotalCnt++;
                if (SHM(m_poIMSIIndex)->get(vIMSI[i].m_sMinNbr, &oIMSI.m_uiNextOffset)) {
                    uiTmp = oIMSI.m_uiNextOffset;
                    do {
                        pData = pBegin + uiTmp;
                        if (pData->m_uiIMSIID == oIMSI.m_uiIMSIID) {
                            pData->assign(oIMSI);
                            iUpdCnt++;
                            break;    
                        }
                        uiTmp = pData->m_uiNextOffset;        
                    } while (uiTmp);
                    if (uiTmp)
                        continue;               
                }
                uiIdx = SHM(m_poIMSIData)->malloc();
                memcpy((void *)(pBegin + uiIdx), (const void *)&oIMSI, sizeof(oIMSI));
                SHM(m_poIMSIIndex)->add(vIMSI[i].m_sMinNbr, uiIdx);    
            }
            
            if(bShow)
            {
            	for(int k=0;k<iTotalCnt;k++)
            		showImsiInfo(oIMSI);
            }
        }
        pTmp->Close();
        Log::log(0, "共导入 %d 条数据:更新 %d 条，新增 %d 条!", iTotalCnt, iUpdCnt, iTotalCnt - iUpdCnt);
        return 0;    
//    }
/*    catch(TTStatus st) {
        pTmp->Close();
        Log::log(0, "loadImsiInfo failed! err_msg=%s", st.err_msg);
    }*/
    return -1;     
}

//共享内存导出到表
int SHMInfoCtl::outHeadToTable(TimesTenCMD *pTmp, unsigned int count, HCodeInfoStruct *pStart)
{
		TIMESTAMP_STRUCT oEffTm, oExpTm;
//    try
//    {
    		while(count)
    		{
    			pTmp->setParam(1, pStart->m_sHead);
    			pTmp->setParam(2, pStart->m_sTspID);
    			pTmp->setParam(3, pStart->m_sZoneCode);
    			
    			long time=atol(pStart->m_sEffDate);
    			oEffTm.day=time%100;
    			oEffTm.month=(time/100)%100;
    			oEffTm.year=(time/100)/100;
    			pTmp->setParam(4, oEffTm);
    			time=atol(pStart->m_sExpDate);
    			oExpTm.day=time%100;
    			oExpTm.month=(time/100)%100;
    			oExpTm.year=(time/100)/100;
    			pTmp->setParam(5, oExpTm);
    			
    			pTmp->setParam(6, (int)(pStart->m_uiHeadID));
					
    			pTmp->Execute();
					
					count -= 1;
    		  pStart += 1;
  			}
//		}
/*    catch(TTStatus st)
    {
        Log::log(0, "SHMInfoCtl::outHeadToTable st.err_msg = %s", st.err_msg);
        return -1;
    }
*/		
		return 0;
}

int SHMInfoCtl::outMinToTable(TimesTenCMD *pTmp, unsigned int count, MinInfoStruct *pStart)
{
		TIMESTAMP_STRUCT oEffTm, oExpTm;
//		try
//		{
    		while(count)
    		{
    			pTmp->setParam(1, pStart->m_sBeginMin);
    			pTmp->setParam(2, pStart->m_sEndMin);
    			pTmp->setParam(3, pStart->m_iUserFlag);
    			pTmp->setParam(4, pStart->m_iCountryID);
    			pTmp->setParam(5, pStart->m_sHomeCarrCode);
    			pTmp->setParam(6, pStart->m_sZoneCode);
    			pTmp->setParam(7, pStart->m_iUserType);
					
    			long time=atol(pStart->m_sEffDate);
    			oEffTm.day=time%100;
    			oEffTm.month=(time/100)%100;
    			oEffTm.year=(time/100)/100;
    			pTmp->setParam(8, oEffTm);
    			time=atol(pStart->m_sExpDate);
    			oExpTm.day=time%100;
    			oExpTm.month=(time/100)%100;
    			oExpTm.year=(time/100)/100;
    			pTmp->setParam(9, oExpTm);
    			
    			pTmp->setParam(10, (int)(pStart->m_uiMinInfoID));
					
    			pTmp->Execute();
					
					count -= 1;
    		  pStart += 1;
  			}
/*		}
    catch(TTStatus st)
    {
        Log::log(0, "SHMInfoCtl::outMinToTable st.err_msg = %s", st.err_msg);
        return -1;
    }
*/		
		return 0;
}

int SHMInfoCtl::outImsiToTable(TimesTenCMD *pTmp, unsigned int count, IMSIInfoStruct *pStart)
{
		TIMESTAMP_STRUCT oEffTm, oExpTm;
//		try
//		{
    		while(count)
    		{
    			pTmp->setParam(1, pStart->m_sBeginIMSI);
    			pTmp->setParam(2, pStart->m_sEndIMSI);
    			pTmp->setParam(3, pStart->m_iIMSIType);
    			pTmp->setParam(4, pStart->m_sSponsorCode);
    			pTmp->setParam(5, pStart->m_sZoneCode);
					
    			long time=atol(pStart->m_sEffDate);
    			oEffTm.day=time%100;
    			oEffTm.month=(time/100)%100;
    			oEffTm.year=(time/100)/100;
    			pTmp->setParam(6, oEffTm);
    			time=atol(pStart->m_sExpDate);
    			oExpTm.day=time%100;
    			oExpTm.month=(time/100)%100;
    			oExpTm.year=(time/100)/100;
    			pTmp->setParam(7, oExpTm);
    			
    			pTmp->setParam(8, (int)(pStart->m_uiIMSIID));
					
    			pTmp->Execute();
					
					count -= 1;
    		  pStart += 1;
  			}
/*		}
    catch(TTStatus st)
    {
        Log::log(0, "SHMInfoCtl::outImsiToTable st.err_msg = %s", st.err_msg);
        return -1;
    }
*/		
		return 0;
}

//文件加载到共享内存(单条更新接口)
bool SHMInfoCtl::updateHCodeInfo(HCodeInfoStruct *pData,bool &bNew,bool bReplace)
{
	unsigned int i, uiTmp;
	HCodeInfoStruct* p;
	HCodeInfoStruct* pBegin = (HCodeInfoStruct *)(*(SHM(m_poHCodeData)));
	pData->m_uiNextOffset = 0;
	
  //检查数据是否存在
  if (SHM(m_poHCodeIndex)->get(pData->m_sHead, &(pData->m_uiNextOffset))) {
      uiTmp = pData->m_uiNextOffset;
      do {
          p = pBegin + uiTmp;
          if (p->m_uiHeadID == pData->m_uiHeadID) {
              p->assign(*pData);
              bNew=false;
              break;
          }
          uiTmp = p->m_uiNextOffset;   
      } while (uiTmp);
      
      if (uiTmp) 
        return true;
  }
  i = SHM(m_poHCodeData)->malloc();  //申请数据空间
  if(!i)
  	return false;
  memcpy((pBegin+i), pData, sizeof(HCodeInfoStruct));
  SHM(m_poHCodeIndex)->add(pData->m_sHead, i);  //增加索引
  
  return true;
}

bool SHMInfoCtl::updateMinInfo(MinInfoStruct *pData,bool &bNew,bool bReplace)
{
	unsigned int uiIdx, uiTmp;
	MinInfoStruct *p;
  MinInfoStruct *pBegin = (MinInfoStruct *)(*(SHM(m_poMinData)));
  pData->m_uiNextOffset = 0;
  std::vector<MinNbrStruct> vMin;
  
  vMin.clear();
  getAllMin(pData->m_sBeginMin, pData->m_sEndMin, vMin);
  for (int i=0; i<vMin.size(); ++i) {
      if (SHM(m_poMinIndex)->get(vMin[i].m_sMinNbr, &(pData->m_uiNextOffset))) {
          uiTmp = pData->m_uiNextOffset;
          do {
              p = pBegin + uiTmp;
              if (p->m_uiMinInfoID == pData->m_uiMinInfoID) {
                  p->assign(*pData);
                  bNew=false;
                  break;    
              }
              uiTmp = p->m_uiNextOffset;        
          } while (uiTmp);
          
          if (uiTmp)
            return true;
      }
      uiIdx = SHM(m_poMinData)->malloc();
      if(!uiIdx)
  			return false;
      memcpy((pBegin + uiIdx), pData, sizeof(MinInfoStruct));
      SHM(m_poMinIndex)->add(vMin[i].m_sMinNbr, uiIdx);
  }
  
  return true;
}

bool SHMInfoCtl::updateImsiInfo(IMSIInfoStruct *pData,bool &bNew,bool bReplace)
{
	unsigned int uiIdx, uiTmp;
	IMSIInfoStruct *p;
  IMSIInfoStruct *pBegin = (IMSIInfoStruct *)(*(SHM(m_poIMSIData)));
  pData->m_uiNextOffset = 0;
  std::vector<MinNbrStruct> vIMSI;
  
  vIMSI.clear();
  getAllMin(pData->m_sBeginIMSI, pData->m_sEndIMSI, vIMSI);
  for (int i=0; i<vIMSI.size(); ++i) {
      if (SHM(m_poIMSIIndex)->get(vIMSI[i].m_sMinNbr, &(pData->m_uiNextOffset))) {
          uiTmp = pData->m_uiNextOffset;
          do {
              p = pBegin + uiTmp;
              if (p->m_uiIMSIID == pData->m_uiIMSIID) {
                  p->assign(*pData);
                  bNew=false;
                  break;    
              }
              uiTmp = p->m_uiNextOffset;        
          } while (uiTmp);
          
          if (uiTmp)
            return true;
      }
      uiIdx = SHM(m_poIMSIData)->malloc();
      if(!uiIdx)
  			return false;
      memcpy((pBegin + uiIdx), pData, sizeof(IMSIInfoStruct));
      SHM(m_poIMSIIndex)->add(vIMSI[i].m_sMinNbr, uiIdx);
  }
	
  return true;
}

void SHMInfoCtl::showHCodeInfo(HCodeInfoStruct &oData)
{
    static bool bFirstHead = true;
    if ( bFirstHead ) {
        printf("HEAD    TSP_ID    ZONE_CODE    EFF_DATE    EXP_DATE    HEAD_ID\n");
        bFirstHead = false;
    }
    printf("%s %s %s %s %s %d\n",
           oData.m_sHead,
           oData.m_sTspID,
           oData.m_sZoneCode,
           oData.m_sEffDate,
           oData.m_sExpDate,
           oData.m_uiHeadID);
}

void SHMInfoCtl::showMinInfo(MinInfoStruct &oData)
{
    static bool bFirstMin = true;
    if ( bFirstMin ) {
        printf("BEGIN_MIN    END_MIN    USER_FLAG    COUNTRY_ID    HOME_CARRIER_CODE    ZONE_CODE    USER_TYPE    EFF_DATE    EXP_DATE    MIN_INFO_ID\n");
        bFirstMin = false;
    }
    printf("%s %s %d %d %s %s %d %s %s %d\n",
           oData.m_sBeginMin,
           oData.m_sEndMin,
           oData.m_iUserFlag,
           oData.m_iCountryID,
           oData.m_sHomeCarrCode,
           oData.m_sZoneCode,
           oData.m_iUserType,
           oData.m_sEffDate,
           oData.m_sExpDate,
           oData.m_uiMinInfoID);
}

void SHMInfoCtl::showImsiInfo(IMSIInfoStruct &oData)
{
    static bool bFirstImsi = true;
    if ( bFirstImsi ) {
        printf("IMSI_BEGIN    IMSI_END    IMSI_TYPE    SPONSOR_CODE    ZONE_CODE    EFF_DATE    EXP_DATE    ISMI_ID\n");
        bFirstImsi = false;
    }
    printf("%s %s %d %s %s %s %s %d\n",
           oData.m_sBeginIMSI,
           oData.m_sEndIMSI,
           oData.m_iIMSIType,
           oData.m_sSponsorCode,
           oData.m_sZoneCode,
           oData.m_sEffDate,
           oData.m_sExpDate,
           oData.m_uiIMSIID);
}
