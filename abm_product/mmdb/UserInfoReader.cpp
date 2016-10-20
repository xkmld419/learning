/*VER: 13*/ 
#include "UserInfoReader.h"
#include "Date.h"
#include "StdEvent.h"
#include <vector>
#include "CommonMacro.h"
#include "Log.h"
#include <string>
#include "ParamDefineMgr.h"
#include "Value.h"
#include "Process.h"
#include "IDParser.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

using namespace std;

static HashList<int> m_poGenCycleFeeIndex(512);
static bool g_bLoadGenCycleFee = false;

void loadGenCycleFeeIndex()
{
	DEFINE_QUERY (qry);
  qry.setSQL("select product_id  from product where nvl(gen_cyclefee,'Y') != 'N'");
  qry.open();
  while (qry.next())
  {
		m_poGenCycleFeeIndex.add(qry.field(0).asInteger (), 1);
  }
  qry.close();
  
  g_bLoadGenCycleFee = true;
}

#ifdef SHM_FILE_USERINFO

char const *Serv::getAccNBR(int iType, char *stime)
{
    unsigned int l;
    
	if(!m_poServInfo)
	{
		return 0;
	}

	l = m_poServInfo->m_iServIdentOffset;

    while (l) {
        if (m_pSHMFileExternal->m_poServIdent[l].m_iType != iType ||
                strcmp (stime, m_pSHMFileExternal->m_poServIdent[l].m_sExpDate)>=0 ||
                strcmp (stime, m_pSHMFileExternal->m_poServIdent[l].m_sEffDate)<0 ) {
            l = m_pSHMFileExternal->m_poServIdent[l].m_iServNextOffset;
            continue;
        }

        return m_pSHMFileExternal->m_poServIdent[l].m_sAccNbr;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    l = m_poServInfo->m_iServIdentOffset;

    Date date_sDate(stime);

    while (l) {

        Date date_EffDate(m_pSHMFileExternal->m_poServIdent[l].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if (m_pSHMFileExternal->m_poServIdent[l].m_iType == iType 
            &&    p >= UserInfoReader::m_beginOffsetSecs
            &&    p <= UserInfoReader::m_offsetSecs ) {

                return m_pSHMFileExternal->m_poServIdent[l].m_sAccNbr;

    }
    else {
        Date date_ExpDate(m_pSHMFileExternal->m_poServIdent[l].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
        if (m_pSHMFileExternal->m_poServIdent[l].m_iType == iType 
            &&    p >= UserInfoReader::m_beginOffsetSecs
            &&    p <= UserInfoReader::m_offsetSecs ) {
                return m_pSHMFileExternal->m_poServIdent[l].m_sAccNbr;
        }
    }

        l = m_pSHMFileExternal->m_poServIdent[l].m_iServNextOffset;

    }
    #endif

    return 0;
}

char const *UserInfoInterface::convertAccNBR(char *accnbr, int iSrcType, int iDestType, char *stime)
{
    unsigned int k, l,k_bak,l_bak;

    char sTemp[MAX_BILLING_NBR_LEN];
    memset (sTemp, 0, sizeof(sTemp));

    char *p = (char *)sTemp;

    while (*accnbr) {
        if (*accnbr >= 'A' && *accnbr <= 'Z') {
            *p = (*accnbr | 0X20);
        } else {
            *p = *accnbr;
        }

        p++;
        accnbr++;
    }  

   
    if (!(m_pSHMFileExternal->m_poServIdentIndex->get(sTemp, &k))) {
        return false;
    }

	k_bak = k;
	l_bak = 0;

	while (k) {

    # ifdef STRICT_SERV_IDENTIFICATION

        if (strcmp(stime, m_pSHMFileExternal->m_poServIdent[k].m_sEffDate) < 0 ||
            strcmp(stime, m_pSHMFileExternal->m_poServIdent[k].m_sExpDate) >=0 ||
            m_pSHMFileExternal->m_poServIdent[k].m_iType!=iSrcType ) {
            k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
            continue;
        }

    # else
        if (strcmp (stime, m_pSHMFileExternal->m_poServIdent[k].m_sExpDate) >=0 ||
            m_pSHMFileExternal->m_poServIdent[k].m_iType!=iSrcType ) {
            k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
            continue;
        }

    # endif

        if ((l=m_pSHMFileExternal->m_poServIdent[k].m_iServOffset)==0) return 0;

        l = m_pSHMFileExternal->m_poServ[l].m_iServIdentOffset;
		l_bak = l;
        while (l) {
            if (m_pSHMFileExternal->m_poServIdent[l].m_iType != iDestType ||
                strcmp (stime, m_pSHMFileExternal->m_poServIdent[l].m_sExpDate)>=0 ||
                strcmp (stime, m_pSHMFileExternal->m_poServIdent[l].m_sEffDate)<0 ) {
                l = m_pSHMFileExternal->m_poServIdent[l].m_iServNextOffset;
                continue;
            }

            return m_pSHMFileExternal->m_poServIdent[l].m_sAccNbr;
		}
        //return 0;
		k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
	}

#ifdef USERINFO_OFFSET_ALLOWED
	k = k_bak;
	Date date_sDate(stime);
	if(l_bak>0)
	{
		l = l_bak;
		while(l)
		{
			Date date_EffDate(m_pSHMFileExternal->m_poServIdent[l].m_sEffDate);
			long p = date_EffDate.diffSec(date_sDate);
			if (p < UserInfoReader::m_beginOffsetSecs  ||
				p > UserInfoReader::m_offsetSecs ||
				m_pSHMFileExternal->m_poServIdent[l].m_iType!=iDestType)
			{
				Date date_ExpDate(m_pSHMFileExternal->m_poServIdent[l].m_sExpDate);
				p = date_sDate.diffSec(date_ExpDate);
				if (p < UserInfoReader::m_beginOffsetSecs  ||
					p > UserInfoReader::m_offsetSecs  ||
					m_pSHMFileExternal->m_poServIdent[l].m_iType!=iDestType)
				{
					l = m_pSHMFileExternal->m_poServIdent[l].m_iServNextOffset;
						continue;
				}
			}
            return m_pSHMFileExternal->m_poServIdent[l].m_sAccNbr;
		}
	}
	while(k)
	{
		Date date_EffDate(m_pSHMFileExternal->m_poServIdent[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if (p < UserInfoReader::m_beginOffsetSecs  ||
			p > UserInfoReader::m_offsetSecs ||
			m_pSHMFileExternal->m_poServIdent[k].m_iType!=iSrcType)
		{
			Date date_ExpDate(m_pSHMFileExternal->m_poServIdent[k].m_sExpDate);
			p = date_sDate.diffSec(date_ExpDate);
			if (p < UserInfoReader::m_beginOffsetSecs  ||
				p > UserInfoReader::m_offsetSecs  ||
				m_pSHMFileExternal->m_poServIdent[k].m_iType!=iSrcType)
			{
				k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
					continue;
			}
		}
		if ((l=m_pSHMFileExternal->m_poServIdent[k].m_iServOffset)==0) return 0;
		l = m_pSHMFileExternal->m_poServ[l].m_iServIdentOffset;
		while (l)
		{
			if(m_pSHMFileExternal->m_poServIdent[l].m_iType == iDestType &&
			   strcmp (stime, m_pSHMFileExternal->m_poServIdent[l].m_sExpDate)<0 &&
			   strcmp (stime, m_pSHMFileExternal->m_poServIdent[l].m_sEffDate)>=0 )
			{
				return m_pSHMFileExternal->m_poServIdent[l].m_sAccNbr;
		    }
			Date date_EffDate(m_pSHMFileExternal->m_poServIdent[l].m_sEffDate);
			p = date_EffDate.diffSec(date_sDate);
			if (p < UserInfoReader::m_beginOffsetSecs  ||
				p > UserInfoReader::m_offsetSecs ||
				m_pSHMFileExternal->m_poServIdent[l].m_iType!=iDestType)
			{
				Date date_ExpDate(m_pSHMFileExternal->m_poServIdent[l].m_sExpDate);
				p = date_sDate.diffSec(date_ExpDate);
				if (p < UserInfoReader::m_beginOffsetSecs  ||
					p > UserInfoReader::m_offsetSecs  ||
					m_pSHMFileExternal->m_poServIdent[l].m_iType!=iDestType)
				{
					l = m_pSHMFileExternal->m_poServIdent[l].m_iServNextOffset;
						continue;
				}
			}
            return m_pSHMFileExternal->m_poServIdent[l].m_sAccNbr;
		}
		k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
	}
#endif
    return 0;
}

////封装：先把号码和区号拼起来找，找不到的话直接使用号码找
bool UserInfoInterface::getServ_A( Serv &serv, char *area_code,char *accnbr, char stime[16], int pNetworkID, bool isPhy) 
{
    char sTemp[64];
    if(area_code && accnbr&&(0==strncmp(accnbr,area_code,strlen(area_code))))
    	{
    		if(!getServ(serv, accnbr, stime, pNetworkID, isPhy))
    			{
    				snprintf (sTemp, 64, "%s%s", area_code,  accnbr);  
        		if(getServ(serv, sTemp, stime, pNetworkID, isPhy) ){
            	return true;
    				}else
    					return false;    			
    			}else
    				return true;	
    }
    if( area_code && area_code[0] ){
        snprintf (sTemp, 64, "%s%s", area_code,  accnbr);  
        if(getServ(serv, sTemp, stime, pNetworkID, isPhy) ){
            return true;
        }
    }
    
    return getServ(serv, accnbr, stime, pNetworkID, isPhy);

}

////封装：对于对端号码不使用网络标识，先根据号码来找，然后把号码和区号拼起来找，以后的所有对端查找全部用这个方法
bool UserInfoInterface::getServ_Z( Serv &serv, char *area_code,char *accnbr, char stime[16], bool isPhy)
{
    char sTemp[64];
	if(!accnbr) return false;
	if(getServ(serv, accnbr, stime,0,isPhy) ){
		return true;
    }
    if(!area_code) return false;
    snprintf (sTemp,64,"%s%s",area_code,accnbr);
    return getServ(serv, sTemp, stime, 0, isPhy);
}


//##ModelId=4232A42F02B3
//##Documentation
//## 根据业务号码，找出主产品实例相关客户资料。
bool UserInfoInterface::getServ( Serv &serv, char *accnbr, char stime[16], int pNetworkID, bool isPhy) 
{
    unsigned int k, l;

    char sTemp[MAX_BILLING_NBR_LEN];
    memset (sTemp, 0, sizeof(sTemp));

    char *p = (char *)sTemp;

	if(!clearServ(serv))
	{
		 return false;
	}
    
    while (*accnbr) {
        if (*accnbr >= 'A' && *accnbr <= 'Z') {
            *p = (*accnbr | 0X20);
        } else {
            *p = *accnbr;
        }

        p++;
        accnbr++;
    }    

    
    if (!(m_pSHMFileExternal->m_poServIdentIndex->get(sTemp, &k))) {
        return false;
    }
    NetworkPriority * pPriChain = 0;
    unsigned int begin_k = k;
    if(pNetworkID)
    {
        pPriChain = m_poNetworkMgr->getPriority(pNetworkID);
    }
    if(!pPriChain)
    {
        pPriChain = &NetworkPriority::NETWORK_NONE;
    }
    NetworkPriority * pSavedChain = pPriChain;
    while(pPriChain) {
        while (k) {
    
        # ifdef STRICT_SERV_IDENTIFICATION
    
            if (strcmp(stime, m_pSHMFileExternal->m_poServIdent[k].m_sEffDate) < 0 ||
                strcmp(stime, m_pSHMFileExternal->m_poServIdent[k].m_sExpDate) >=0 ||
                (isPhy == (m_pSHMFileExternal->m_poServIdent[k].m_iType!=1)) ||
                 (pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_pSHMFileExternal->m_poServIdent[k].m_iNetworkID)) {
                k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
                continue;
            }
    
        # else
            if (strcmp (stime, m_pSHMFileExternal->m_poServIdent[k].m_sExpDate) >=0 ||
                (isPhy == (m_pSHMFileExternal->m_poServIdent[k].m_iType!=1)) ||
                 (pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_pSHMFileExternal->m_poServIdent[k].m_iNetworkID)) {
                k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
                continue;
            }
    
        # endif
    
            if ((l=m_pSHMFileExternal->m_poServIdent[k].m_iServOffset)!=0) {
                strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1);
                serv.m_poServInfo = m_pSHMFileExternal->m_poServ + l;
                serv.m_poServProductInfo = m_pSHMFileExternal->m_poServProduct+m_pSHMFileExternal->m_poServ[l].m_iServProductOffset;
                serv.m_poServAcctInfo = m_pSHMFileExternal->m_poServAcct + m_pSHMFileExternal->m_poServ[l].m_iServAcctOffset;
    
                unsigned int k;
                if (m_pSHMFileExternal->m_poCustIndex->get(m_pSHMFileExternal->m_poServ[l].m_lCustID, &k)) {
                    serv.m_poCustInfo = m_pSHMFileExternal->m_poCust + k;
                } else {
                    serv.m_poCustInfo = 0;
                }
                return true;
            } else {
                return false;
            }    
        }
        k = begin_k;
	#ifdef PRIVATE_MEMORY
	pPriChain = pPriChain->m_poNext;
	#else
	if(pPriChain->m_iNextOffset)
		pPriChain = m_poNetworkMgr->getHeadNetworkPriority() + pPriChain->m_iNextOffset ;
	else
		pPriChain = NULL;
	#endif
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = begin_k;
    pPriChain = pSavedChain;

    Date date_sDate(stime);

    while(pPriChain) {
        while (k) {
    
            Date date_EffDate(m_pSHMFileExternal->m_poServIdent[k].m_sEffDate);
            long p = date_EffDate.diffSec(date_sDate);
    
            if (p < UserInfoReader::m_beginOffsetSecs  ||
                    p > UserInfoReader::m_offsetSecs ||
                (isPhy == (m_pSHMFileExternal->m_poServIdent[k].m_iType!=1)) ||
                 (pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_pSHMFileExternal->m_poServIdent[k].m_iNetworkID)) {
          Date date_ExpDate(m_pSHMFileExternal->m_poServIdent[k].m_sExpDate);
          p = date_sDate.diffSec(date_ExpDate);
          if (p < UserInfoReader::m_beginOffsetSecs  ||
                    p > UserInfoReader::m_offsetSecs ||
                (isPhy == (m_pSHMFileExternal->m_poServIdent[k].m_iType!=1)) ||
                 (pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_pSHMFileExternal->m_poServIdent[k].m_iNetworkID))
          {
                    k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
                    continue;
                }
            }
    
            if ((l=m_pSHMFileExternal->m_poServIdent[k].m_iServOffset)!=0) {
                strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1);
                serv.m_poServInfo = m_pSHMFileExternal->m_poServ + l;
                serv.m_poServProductInfo = m_pSHMFileExternal->m_poServProduct+m_pSHMFileExternal->m_poServ[l].m_iServProductOffset;
                serv.m_poServAcctInfo = m_pSHMFileExternal->m_poServAcct + m_pSHMFileExternal->m_poServ[l].m_iServAcctOffset;
    
                unsigned int k;
                if (m_pSHMFileExternal->m_poCustIndex->get(m_pSHMFileExternal->m_poServ[l].m_lCustID, &k)) {
                    serv.m_poCustInfo = m_pSHMFileExternal->m_poCust + k;
                } else {
                    serv.m_poCustInfo = 0;
                }
                return true;
            } else {
                return false;
            }    
        }
    k = begin_k;
		#ifdef PRIVATE_MEMORY
		pPriChain = pPriChain->m_poNext;
		#else
		if(pPriChain->m_iNextOffset)
			pPriChain = m_poNetworkMgr->getHeadNetworkPriority() + pPriChain->m_iNextOffset ;
		else
			pPriChain = NULL;
		#endif
    }


    #endif

    return false;
}

//根据业务号码找出一机双号的另一个用户
bool UserInfoInterface::getServ( Serv &serv, char *accnbr, long lServID,char stime[16],bool isPhy) 
{
    unsigned int k, l;

    char sTemp[MAX_BILLING_NBR_LEN];
    memset (sTemp, 0, sizeof(sTemp));

    char *p = (char *)sTemp;
    
	if(!clearServ(serv))
	{
		 return false;
	}

    while (*accnbr) {
        if (*accnbr >= 'A' && *accnbr <= 'Z') {
            *p = (*accnbr | 0X20);
        } else {
            *p = *accnbr;
        }

        p++;
        accnbr++;
    }    

    
    if (!(m_pSHMFileExternal->m_poServIdentIndex->get(sTemp, &k))) {
        return false;
    }
    while (k) {
    # ifdef STRICT_SERV_IDENTIFICATION

        if (strcmp(stime, m_pSHMFileExternal->m_poServIdent[k].m_sEffDate) < 0 ||
            strcmp(stime, m_pSHMFileExternal->m_poServIdent[k].m_sExpDate) >=0 ||
            (isPhy == (m_pSHMFileExternal->m_poServIdent[k].m_iType!=1)) ||
			(lServID == m_pSHMFileExternal->m_poServIdent[k].m_lServID)
			){
            k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
            continue;
        }
    # else
        if (strcmp (stime, m_pSHMFileExternal->m_poServIdent[k].m_sExpDate) >=0 ||
            (isPhy == (m_pSHMFileExternal->m_poServIdent[k].m_iType!=1)) ||
			(lServID == m_pSHMFileExternal->m_poServIdent[k].m_lServID)
			){
            k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
            continue;
        }
    # endif

    if ((l=m_pSHMFileExternal->m_poServIdent[k].m_iServOffset)!=0)
	{
        strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1);
        serv.m_poServInfo = m_pSHMFileExternal->m_poServ + l;
        serv.m_poServProductInfo = m_pSHMFileExternal->m_poServProduct+m_pSHMFileExternal->m_poServ[l].m_iServProductOffset;
        serv.m_poServAcctInfo = m_pSHMFileExternal->m_poServAcct + m_pSHMFileExternal->m_poServ[l].m_iServAcctOffset;

        unsigned int k;
        if (m_pSHMFileExternal->m_poCustIndex->get(m_pSHMFileExternal->m_poServ[l].m_lCustID, &k)) {
            serv.m_poCustInfo = m_pSHMFileExternal->m_poCust + k;
        } else {
            serv.m_poCustInfo = 0;
        }
        return true;
	}else{
		return false;
	}
	}
	return false;
}

//##ModelId=423677970042
//##Documentation
//## 根据SERVID，找出主产品实例相关客户资料。
bool UserInfoInterface::getServ( Serv &serv, long servid, char stime[16]) 
{
    unsigned int l;
    
	if(!clearServ(serv))
	{
		 return false;
	}

    if (m_pSHMFileExternal->m_poServIndex->get(servid, &l)) {
        strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1 );
        serv.m_poServInfo = m_pSHMFileExternal->m_poServ + l;
        serv.m_poServProductInfo = m_pSHMFileExternal->m_poServProduct+m_pSHMFileExternal->m_poServ[l].m_iServProductOffset;
        serv.m_poServAcctInfo = m_pSHMFileExternal->m_poServAcct + m_pSHMFileExternal->m_poServ[l].m_iServAcctOffset;

        unsigned int k;
        if (m_pSHMFileExternal->m_poCustIndex->get(m_pSHMFileExternal->m_poServ[l].m_lCustID, &k)) {
            serv.m_poCustInfo = m_pSHMFileExternal->m_poCust + k;
        } else {
            serv.m_poCustInfo = 0;
        }
        return true;
    } else {
        return false;
    }    
}

//## 根据号码/号码类型 查找用户资料
bool UserInfoInterface::getServ (Serv &serv, char *accnbr, int iSrcType, char *stime)
{
    unsigned int k, l,k_bak;

    char sTemp[MAX_BILLING_NBR_LEN];
    memset (sTemp, 0, sizeof(sTemp));

    char *p = (char *)sTemp;   
	
	if(!clearServ(serv))
	{
		 return false;
	}

    while (*accnbr) {
        if (*accnbr >= 'A' && *accnbr <= 'Z') {
            *p = (*accnbr | 0X20);
        } else {
            *p = *accnbr;
        }
        
        p++;
        accnbr++;
    }
   
    if (!(m_pSHMFileExternal->m_poServIdentIndex->get(sTemp, &k))) {
        return false;
    }

	k_bak = k;

    while (k) {

    #ifdef STRICT_SERV_IDENTIFICATION

        if (strcmp(stime, m_pSHMFileExternal->m_poServIdent[k].m_sEffDate) < 0 ||
            strcmp(stime, m_pSHMFileExternal->m_poServIdent[k].m_sExpDate) >=0 ||
            m_pSHMFileExternal->m_poServIdent[k].m_iType!=iSrcType ) {
            k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
            continue;
        }

    #else
        if (strcmp (stime, m_pSHMFileExternal->m_poServIdent[k].m_sExpDate) >=0 ||
            m_pSHMFileExternal->m_poServIdent[k].m_iType!=iSrcType ) {
            k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
            continue;
        }
    #endif
    
        if ((l=m_pSHMFileExternal->m_poServIdent[k].m_iServOffset) != 0)
    	{
            strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1);
            serv.m_poServInfo = m_pSHMFileExternal->m_poServ + l;
            serv.m_poServProductInfo = m_pSHMFileExternal->m_poServProduct+m_pSHMFileExternal->m_poServ[l].m_iServProductOffset;
            serv.m_poServAcctInfo = m_pSHMFileExternal->m_poServAcct + m_pSHMFileExternal->m_poServ[l].m_iServAcctOffset;
    
            unsigned int k;
            if (m_pSHMFileExternal->m_poCustIndex->get(m_pSHMFileExternal->m_poServ[l].m_lCustID, &k)) {
                serv.m_poCustInfo = m_pSHMFileExternal->m_poCust + k;
            } else {
                serv.m_poCustInfo = 0;
            }
            return true;
    	} else {
    		return false;
    	}
    }
#ifdef USERINFO_OFFSET_ALLOWED
	k = k_bak;
	Date date_sDate(stime);
	while(k)
	{
		Date date_EffDate(m_pSHMFileExternal->m_poServIdent[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if (p < UserInfoReader::m_beginOffsetSecs  ||
			p > UserInfoReader::m_offsetSecs ||
			m_pSHMFileExternal->m_poServIdent[k].m_iType!=iSrcType)
		{
			Date date_ExpDate(m_pSHMFileExternal->m_poServIdent[k].m_sExpDate);
			p = date_sDate.diffSec(date_ExpDate);
			if (p < UserInfoReader::m_beginOffsetSecs  ||
				p > UserInfoReader::m_offsetSecs  ||
				m_pSHMFileExternal->m_poServIdent[k].m_iType!=iSrcType)
			{
				k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
					continue;
			}
		}
		if ((l=m_pSHMFileExternal->m_poServIdent[k].m_iServOffset)!=0)
		{
            //strcpy (serv.m_sTime, stime);
            strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1);
            serv.m_poServInfo = m_pSHMFileExternal->m_poServ + l;
            serv.m_poServProductInfo = m_pSHMFileExternal->m_poServProduct+m_pSHMFileExternal->m_poServ[l].m_iServProductOffset;
            serv.m_poServAcctInfo = m_pSHMFileExternal->m_poServAcct + m_pSHMFileExternal->m_poServ[l].m_iServAcctOffset;

            unsigned int k;
            if (m_pSHMFileExternal->m_poCustIndex->get(m_pSHMFileExternal->m_poServ[l].m_lCustID, &k)) {
                serv.m_poCustInfo = m_pSHMFileExternal->m_poCust + k;
            } else {
                serv.m_poCustInfo = 0;
            }
            return true;
        }
		else
		{
            return false;
        } 
	}
#endif
    return false;
}

//## 根据号码/号码类型、网络标识 查找用户资料
bool UserInfoInterface::getServN (Serv &serv, char *accnbr, int iSrcType, char *stime,int pNetworkID)
{
    unsigned int k, l,k_bak;

    char sTemp[MAX_BILLING_NBR_LEN];
    memset (sTemp, 0, sizeof(sTemp));

    char *p = (char *)sTemp;   
	
	if(!clearServ(serv))
	{
		 return false;
	}

    while (*accnbr) {
        if (*accnbr >= 'A' && *accnbr <= 'Z') {
            *p = (*accnbr | 0X20);
        } else {
            *p = *accnbr;
        }
        
        p++;
        accnbr++;
    }
   
    if (!(m_pSHMFileExternal->m_poServIdentIndex->get(sTemp, &k))) {
        return false;
    }

	k_bak = k;

	NetworkPriority * pPriChain = 0;
    if(pNetworkID)
    {
        pPriChain = m_poNetworkMgr->getPriority(pNetworkID);
    }
    if(!pPriChain)
    {
        pPriChain = &NetworkPriority::NETWORK_NONE;
    }
    NetworkPriority * pSavedChain = pPriChain;
    while(pPriChain)
	{
		while (k) {
		#ifdef STRICT_SERV_IDENTIFICATION
			if (strcmp(stime, m_pSHMFileExternal->m_poServIdent[k].m_sEffDate) < 0 ||
				strcmp(stime, m_pSHMFileExternal->m_poServIdent[k].m_sExpDate) >=0 ||
				m_pSHMFileExternal->m_poServIdent[k].m_iType!=iSrcType ||
				(pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_pSHMFileExternal->m_poServIdent[k].m_iNetworkID))
			{
				k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
				continue;
			}

		#else
			if (strcmp (stime, m_pSHMFileExternal->m_poServIdent[k].m_sExpDate) >=0 ||
				m_pSHMFileExternal->m_poServIdent[k].m_iType!=iSrcType ||
				(pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_pSHMFileExternal->m_poServIdent[k].m_iNetworkID))
			{
				k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
				continue;
			}
		#endif	    
			if ((l=m_pSHMFileExternal->m_poServIdent[k].m_iServOffset) != 0)
    		{
				//strcpy (serv.m_sTime, stime);
				strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1);
				serv.m_poServInfo = m_pSHMFileExternal->m_poServ + l;
				serv.m_poServProductInfo = m_pSHMFileExternal->m_poServProduct+m_pSHMFileExternal->m_poServ[l].m_iServProductOffset;
				serv.m_poServAcctInfo = m_pSHMFileExternal->m_poServAcct + m_pSHMFileExternal->m_poServ[l].m_iServAcctOffset;
	    
				unsigned int k;
				if (m_pSHMFileExternal->m_poCustIndex->get(m_pSHMFileExternal->m_poServ[l].m_lCustID, &k)) {
					serv.m_poCustInfo = m_pSHMFileExternal->m_poCust + k;
				} else {
					serv.m_poCustInfo = 0;
				}
				return true;
    		} else {
    			return false;
    		}
		}
		k = k_bak;
		#ifdef PRIVATE_MEMORY
		pPriChain = pPriChain->m_poNext;
		#else
		if(pPriChain->m_iNextOffset)
			pPriChain = m_poNetworkMgr->getHeadNetworkPriority() + pPriChain->m_iNextOffset ;
		else
			pPriChain = NULL;
		#endif
	}
#ifdef USERINFO_OFFSET_ALLOWED
	k = k_bak;
	Date date_sDate(stime);
	pPriChain = pSavedChain;
	while(pPriChain) {
		while(k)
		{
			Date date_EffDate(m_pSHMFileExternal->m_poServIdent[k].m_sEffDate);
			long p = date_EffDate.diffSec(date_sDate);
			if (p < UserInfoReader::m_beginOffsetSecs  ||
				p > UserInfoReader::m_offsetSecs ||
				m_pSHMFileExternal->m_poServIdent[k].m_iType!=iSrcType ||
				(pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_pSHMFileExternal->m_poServIdent[k].m_iNetworkID))
			{
				Date date_ExpDate(m_pSHMFileExternal->m_poServIdent[k].m_sExpDate);
				p = date_sDate.diffSec(date_ExpDate);
				if (p < UserInfoReader::m_beginOffsetSecs  ||
					p > UserInfoReader::m_offsetSecs ||
					m_pSHMFileExternal->m_poServIdent[k].m_iType!=iSrcType ||
					(pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_pSHMFileExternal->m_poServIdent[k].m_iNetworkID))
				{
					k = m_pSHMFileExternal->m_poServIdent[k].m_iNextOffset;
						continue;
				}
			}
			if ((l=m_pSHMFileExternal->m_poServIdent[k].m_iServOffset)!=0)
			{	
				//strcpy (serv.m_sTime, stime);
				strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1);
				serv.m_poServInfo = m_pSHMFileExternal->m_poServ + l;
				serv.m_poServProductInfo = m_pSHMFileExternal->m_poServProduct+m_pSHMFileExternal->m_poServ[l].m_iServProductOffset;
				serv.m_poServAcctInfo = m_pSHMFileExternal->m_poServAcct + m_pSHMFileExternal->m_poServ[l].m_iServAcctOffset;

				unsigned int k;
				if (m_pSHMFileExternal->m_poCustIndex->get(m_pSHMFileExternal->m_poServ[l].m_lCustID, &k)) {
					serv.m_poCustInfo = m_pSHMFileExternal->m_poCust + k;
				} else {
					serv.m_poCustInfo = 0;
				}
				return true;
			}
			else
			{
				return false;
			} 
		}
		k = k_bak;
        pPriChain = pPriChain->m_poNext;
	}
#endif
    return false;
}

int UserInfoInterface::getServOrg(long servid, char * sDate)
{
    ServLocationInfo *pLocation = 
            (ServLocationInfo *)(*m_pSHMFileExternal->m_poServLocationData);
    ServInfo * pServ = (ServInfo *)(*m_pSHMFileExternal->m_poServData);
    
    unsigned int k;
    if (!m_pSHMFileExternal->m_poServIndex->get (servid, &k)) {
            return 1;
    }
    
    unsigned int * pTemp;
    pTemp = &(pServ[k].m_iLocationOffset);

    while (*pTemp) {
        if (strcmp (sDate, pLocation[*pTemp].m_sEffDate) >= 0
                && strcmp (sDate, pLocation[*pTemp].m_sExpDate) < 0) {
            return pLocation[*pTemp].m_iOrgID;
        }
        pTemp = &(pLocation[*pTemp].m_iNextOffset);
    }

#ifdef USERINFO_OFFSET_ALLOWED

    unsigned int l;

    l = pServ[k].m_iLocationOffset;

    Date date_sDate(sDate);

    while(l) {
        Date date_EffDate(pLocation[l].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);

        if(p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs) {
            return pLocation[l].m_iOrgID;
        } else {
            Date date_ExpDate(m_pSHMFileExternal->m_poServLocation[l].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
            if (p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs) {
                return pLocation[l].m_iOrgID;
            }
        }
        l = pLocation[l].m_iNextOffset;
    }
#endif

    return 1;
}   

char const *Serv::getServTypeEffDate(char *sDate)
{
	 ServTypeInfo * pServType = *m_pSHMFileExternal->m_poServTypeData;
    unsigned int k;

    if (!sDate) sDate = m_sTime;

    k = m_poServInfo->m_iServTypeOffset;

    while (k) {
        if (strcmp (pServType[k].m_sEffDate, sDate)<=0 &&
                strcmp (pServType[k].m_sExpDate, sDate)>0 ) {
					return pServType[k].m_sEffDate;
        }
        k = pServType[k].m_iNextOffset;
    }
	return 0;
}

bool Serv::hasProduct(int nProductID,char* pBegin,char *pEndDate)
{
    if((0 == pBegin)||(pBegin[0] ==0))
        return false;
    ServTypeInfo * pServType = *m_pSHMFileExternal->m_poServTypeData;
    if(!pEndDate) pEndDate = m_sTime;
    unsigned int k;
    if((!m_poServInfo)||(!pServType))
        return false;
    k = m_poServInfo->m_iServTypeOffset;
    if(strncmp(pBegin,pEndDate,14)>0)
    {
        //保证pEndDate > pBegin
        char sTemp[20]={0};
        strcpy(sTemp,pEndDate);
        strcpy(pEndDate,pBegin);
        strcpy(pBegin,sTemp);
    }

    while(k)
    {
         if ((strcmp(pEndDate,pServType[k].m_sEffDate)>0&&
             strcmp(pBegin,pServType[k].m_sExpDate)<0))
         {
             if(pServType[k].m_iProductID == nProductID)
                 return true;
         }
         k = pServType[k].m_iNextOffset;
    }
    return false;
}
int Serv::getProductID(char *sDate)
{
        ServTypeInfo * pServType = *m_pSHMFileExternal->m_poServTypeData;
        unsigned int k;

        if (!sDate) sDate = m_sTime;

		if(!m_poServInfo){
			return -1;
		}

        k = m_poServInfo->m_iServTypeOffset;

        while (k) {
                if (strcmp (pServType[k].m_sEffDate, sDate)<=0 &&
                        strcmp (pServType[k].m_sExpDate, sDate)>0 ) {
                        return pServType[k].m_iProductID;
                }

                k = pServType[k].m_iNextOffset;
        }
    #ifdef USERINFO_OFFSET_ALLOWED
    
    unsigned int l;

    l = m_poServInfo->m_iServTypeOffset;

    Date date_sDate(sDate);

    while(l) {
        Date date_EffDate(pServType[l].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if(p>= UserInfoReader::m_beginOffsetSecs 
            && p <= UserInfoReader::m_offsetSecs) {
            return pServType[l].m_iProductID;
        }
        else {
            Date date_ExpDate(pServType[l].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
            if (p>= UserInfoReader::m_beginOffsetSecs
                && p <= UserInfoReader::m_offsetSecs) {
                    return pServType[l].m_iProductID;
            }
        }
        l = pServType[l].m_iNextOffset;
    }
    #endif

        return -1;    
}

//##ModelId=4232B430006F
int Serv::getOrgID(char *sDate)
{
	if(!m_poServInfo){
		return -1;
	}

    unsigned k = m_poServInfo->m_iLocationOffset;
    if (!sDate) sDate = m_sTime;

  /* 
    while (k && strcmp(sDate, m_poServLocation[k].m_sEffDate)<0) {
        k = m_poServLocation[k].m_iNextOffset;
    }

    if (k) {
        return m_poServLocation[k].m_iOrgID;
    }
  */

   while(k) {
      if(strcmp(m_pSHMFileExternal->m_poServLocation[k].m_sEffDate,sDate)<=0
        && strcmp(m_pSHMFileExternal->m_poServLocation[k].m_sExpDate,sDate)>0) {
         return  m_pSHMFileExternal->m_poServLocation[k].m_iOrgID;
       }
       
      k = m_pSHMFileExternal->m_poServLocation[k].m_iNextOffset;
    }

    
    #ifdef USERINFO_OFFSET_ALLOWED
    
    unsigned int l;

    l = m_poServInfo->m_iLocationOffset;

    Date date_sDate(sDate);

    while(l) {
        Date date_EffDate(m_pSHMFileExternal->m_poServLocation[l].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if(p>= UserInfoReader::m_beginOffsetSecs 
            && p <= UserInfoReader::m_offsetSecs) {
            return m_pSHMFileExternal->m_poServLocation[l].m_iOrgID;
        }
        else {
            Date date_ExpDate(m_pSHMFileExternal->m_poServLocation[l].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
            if (p>= UserInfoReader::m_beginOffsetSecs 
            && p <= UserInfoReader::m_offsetSecs) {
                return m_pSHMFileExternal->m_poServLocation[l].m_iOrgID;
            }
        }
        l = m_pSHMFileExternal->m_poServLocation[l].m_iNextOffset;
    }
    #endif

    return -1; 
}

//##ModelId=4232B6FC02F0
int Serv::getBillingCycleType()
{
	if(!m_poServInfo){
		return 1;
	}
    return m_poServInfo->m_iCycleType;
}

//##ModelId=4232B70B032E
BillingCycle const *Serv::getBillingCycle(char *sTime, int iCycleType)
{
    if (iCycleType == -1&&m_poServInfo) iCycleType = m_poServInfo->m_iCycleType;

    if (!sTime) sTime = m_sTime;

    return m_poBillingCycleMgr->getBillingCycle (iCycleType, sTime);
}

//##ModelId=42437460021F
char *Serv::getState(char *sDate)
{
    //##最新状态不一定在状态表中？
/*
    昆明这里SERV表里面的STATE_DATE不是最新的状态的时间,是资料变更时间
    2005-09-24 xueqt

    if (sDate) {
        if (strcmp(sDate, m_poServInfo->m_sStateDate) >= 0 )
            return m_poServInfo->m_sState;
    } else {
        if (strcmp(m_sTime, m_poServInfo->m_sStateDate) >= 0 )
            return m_poServInfo->m_sState;
    }    
*/
	if(!m_poServInfo){
		return 0;
	}

    unsigned int k = m_poServInfo->m_iStateOffset;

    if (!sDate) sDate = m_sTime;

  /*
    while (k && strcmp(sDate, m_poServState[k].m_sEffDate)<0) {
        k = m_poServState[k].m_iNextOffset;
    }

    if (k) {
        return m_poServState[k].m_sState;
    }
  */
  
    while(k) {
      if(strcmp(m_pSHMFileExternal->m_poServState[k].m_sEffDate,sDate)<=0
        && strcmp(m_pSHMFileExternal->m_poServState[k].m_sExpDate, sDate)>0) {
        return m_pSHMFileExternal->m_poServState[k].m_sState;
      }
     
      k = m_pSHMFileExternal->m_poServState[k].m_iNextOffset;
    }
      
    return 0; 
}

char *Serv::getPreState()
{
	if(!m_poServInfo){
		return 0;
	}	
    unsigned k = m_poServInfo->m_iStateOffset;

    while (k) {
        k = m_pSHMFileExternal->m_poServState[k].m_iNextOffset;
    }

    if (k) {
        return m_pSHMFileExternal->m_poServState[k].m_sState;
    }

    return 0; 
}

char *Serv::getLatestState()
{
	if(!m_poServInfo){
		return 0;
	}
    unsigned int k = m_poServInfo->m_iStateOffset;
    if (k) {
        return m_pSHMFileExternal->m_poServState[k].m_sState;
    }
    
    return 0; 
}

char *Serv::getLatestStateDate()
{
    unsigned int k = m_poServInfo->m_iStateOffset;
    if (k) {
		return m_pSHMFileExternal->m_poServState[k].m_sEffDate;
    }
    
    return 0; 
}

char *Serv::getBillingState(char *sDate)
{
    if (!sDate) sDate = m_sTime;

    char * sState = getState (sDate);
    if (!sState) return 0;

    int iProductID = getProductID (sDate);

        int offers[MAX_OFFERS];

    memset (offers, 0, sizeof (offers));

    getOffers (offers, sDate);
    
    sState = m_poStateConvertMgr->getBillingState (sState, iProductID, offers[0]);

    return sState;
}

int ServProduct::getUsingDay(char *sBeginDate, char *sEndDate, Serv * pServ)
{
    Serv oServ;

	if(!m_poServInfo){
		return 0;
		}
    if (pServ == 0) {
        UserInfoInterface interface;

        interface.getServ (oServ, m_poServInfo->m_lServID, 0);
        pServ = &oServ;
    }


    Date d1(sBeginDate), d2(sEndDate);
    d1.setHour (23); d1.setMin (59); d1.setSec (59);
    d2.setHour (23); d2.setMin (59); d2.setSec (59);

    int iRet (0);

    while (d1 <= d2) {
        if (pServ->getUsing (d1) && getEff (d1.toString ()))
            iRet++;

        d1.addDay (1);
    }

    return iRet;
}

int Serv::getUsingDay(char *sBeginDate, char * sEndDate)
{
    Date d1(sBeginDate), d2(sEndDate);
    d1.setHour (23); d1.setMin (59); d1.setSec (59);
    d2.setHour (23); d2.setMin (59); d2.setSec (59);

    char sDate[16];
    int iRet (0);

    for (; d1 <= d2; d1.addDay (1)) {
        strcpy (sDate, d1.toString ());

        char * sState = getState (sDate);
        if (!sState) continue;

        int iProductID = getProductID (sDate);

        int offers[MAX_OFFERS];

        memset (offers, 0, sizeof (offers));

        getOffers (offers, sDate);
        
        sState = m_poStateConvertMgr->getBillingState (sState, iProductID, offers[0]);

        if ((sState[2]|0x20) == 'a') iRet++;
    }
    
    return iRet;
}

bool Serv::getUsing(Date d)
{
/*
    d.setHour (23); d.setMin (59); d.setSec (59);
    char * sState = getState (d.toString ());
    if (!sState) return false;

    switch (sState[2] | 0x20) {
      case 'a':
          case 'd':
          case 'e':
          case 's':
                return true;
      default:
        return false;
    }
*/
    
    d.setHour (23); d.setMin (59); d.setSec (59);
    char sDate[16];
    strcpy (sDate, d.toString ());

    char * sState = getState (sDate);
    if (!sState) return false;

    int iProductID = getProductID (sDate);

        int offers[MAX_OFFERS];

    memset (offers, 0, sizeof (offers));

    getOffers (offers, sDate);
    
    sState = m_poStateConvertMgr->getBillingState (sState, iProductID, offers[0]);

    if ((sState[2]|0x20) == 'a') return true;

    return false;

}

//##ModelId=42414E7A0176
long ServProduct::getServProductID()
{
    return m_poServProductInfo->m_lServProductID;
}

//##ModelId=42414E81022B
long ServProduct::getServID()
{
    return m_poServProductInfo->m_lServID;
}

//##ModelId=42414E86026E
int ServProduct::getProductID()
{
    return m_poServProductInfo->m_iProductID;
}

bool ServProduct::getEff(char *sDate)
{
    return (strcmp (sDate, m_poServProductInfo->m_sEffDate)>=0 &&
                strcmp (sDate, m_poServProductInfo->m_sExpDate)<0);
}

//##ModelId=42414E8C0226
char *ServProduct::getEffDate()
{
    return m_poServProductInfo->m_sEffDate;
}

//##ModelId=42414E9202F7
char *ServProduct::getExpDate()
{
    return m_poServProductInfo->m_sExpDate;
}

//##ModelId=4234E8030031
UserInfoInterface::UserInfoInterface(): UserInfoReader ()
{
    if (!m_pSHMFileExternal->m_bAttached) THROW(MBC_UserInfoReader+1);
}

UserInfoInterface::UserInfoInterface(int iMode):UserInfoReader(iMode)
{
    if (!m_pSHMFileExternal->m_bAttached) THROW(MBC_UserInfoReader+1);
}

//##ModelId=42353FFC014D
UserInfoInterface::operator bool()
{
    return m_pSHMFileExternal->m_bAttached;
}

//##ModelId=4232A90601B0
long Serv::getServID()
{
	if(m_poServInfo){
		return m_poServInfo->m_lServID;
	}
	
	return -1;
}

//##ModelId=4232AE380131
bool Serv::belongOfferInstance(long instanceid, char *sdate)
{
	if(!m_poServInfo){
		return false;
	}
    unsigned int k = m_poServInfo->m_iOfferInstanceOffset;
    unsigned int i;

    char temp[16];

    if (sdate) strcpy (temp, sdate);
    else strcpy (temp, m_sTime);

    while (k ) {
        i = k;
        while (i) {
            if (strcmp(temp, m_pSHMFileExternal->m_poOfferInstance[k].m_sEffDate) >=0 &&
            strcmp(temp, m_pSHMFileExternal->m_poOfferInstance[k].m_sExpDate)<0 &&
            m_pSHMFileExternal->m_poOfferInstance[k].m_lOfferInstanceID == instanceid) {
                return true;
            }

            i = m_pSHMFileExternal->m_poOfferInstance[i].m_iNextOffset;
        }

        k = m_pSHMFileExternal->m_poOfferInstance[k].m_iParentOffset;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iOfferInstanceOffset;

    Date date_sDate(temp);

    while(k) {
        i = k;
        while(i) {
            Date date_EffDate(m_pSHMFileExternal->m_poOfferInstance[i].m_sEffDate);
            long p = date_EffDate.diffSec(date_sDate) ;
            if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs
                && m_pSHMFileExternal->m_poOfferInstance[k].m_lOfferInstanceID == instanceid) {
                return true;
            }
            else {
                Date date_ExpDate(m_pSHMFileExternal->m_poOfferInstance[i].m_sExpDate);
                p = date_sDate.diffSec(date_ExpDate) ;
                if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs
                && m_pSHMFileExternal->m_poOfferInstance[k].m_lOfferInstanceID == instanceid) {
                    return true;
                }
            }
            i = m_pSHMFileExternal->m_poOfferInstance[i].m_iNextOffset;
        }

        k = m_pSHMFileExternal->m_poOfferInstance[k].m_iParentOffset;
    }
    #endif

    return false;
}

//##ModelId=4232AE690131
bool Serv::belongOffer(int offerid, char *sdate)
{
	if(!m_poServInfo){
		return false;
	}
    unsigned int k = m_poServInfo->m_iOfferInstanceOffset;
    unsigned int i;

    if (!sdate) sdate = m_sTime;

    while (k ) {
        i = k;
        while (i) {
            if (strcmp(sdate, m_pSHMFileExternal->m_poOfferInstance[k].m_sEffDate) >=0 &&
            strcmp(sdate, m_pSHMFileExternal->m_poOfferInstance[k].m_sExpDate)<0 &&
            m_pSHMFileExternal->m_poOfferInstance[k].m_iOfferID == offerid) {
                return true;
            }

            i = m_pSHMFileExternal->m_poOfferInstance[i].m_iNextOffset;
        }

        k = m_pSHMFileExternal->m_poOfferInstance[k].m_iParentOffset;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iOfferInstanceOffset;

    Date date_sDate(sdate);

    while(k) {
        i = k;
        while(i) {
            Date date_EffDate(m_pSHMFileExternal->m_poOfferInstance[i].m_sEffDate);
            long p = date_EffDate.diffSec(date_sDate) ;
            if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs &&
            m_pSHMFileExternal->m_poOfferInstance[k].m_iOfferID == offerid) {
                return true;
            }
            else {
                Date date_ExpDate(m_pSHMFileExternal->m_poOfferInstance[i].m_sExpDate);
                p = date_sDate.diffSec(date_ExpDate) ;
                if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs &&
                m_pSHMFileExternal->m_poOfferInstance[k].m_iOfferID == offerid) {
                    return true;
                }
            }
            i = m_pSHMFileExternal->m_poOfferInstance[i].m_iNextOffset;
        }

        k = m_pSHMFileExternal->m_poOfferInstance[k].m_iParentOffset;
    }
    #endif

    return false;
}

bool Serv::hasServProduct(int nServProductID,char * pBeginDate,char* pEndDate )
{
    if((0== pBeginDate)||(pBeginDate[0] ==0))
        return false;
    if(!pEndDate) pEndDate = m_sTime;

    ServProductIteration spiter;
    ServProduct sp;

    bool ret;
    ret = getServProductIteration(spiter,pBeginDate,true);

    if(!ret)
        return false;
    char* pEffDate=0;
    char* pExpDate=0;
    while (spiter.next(sp)) {
        if (sp.getProductID() == nServProductID)
        {
            pEffDate = sp.getEffDate();
	    pExpDate = sp.getExpDate();
	    if((strcmp(pBeginDate,pExpDate)<=0)
                &&(strcmp(pEndDate,pEffDate)>0))
                return true;
        }
    }
    return false;
}

bool Serv::hasServProductEx(int nServProductID,char * pBeginDate,char* pEndDate )
{
    if((0== pBeginDate)||(pBeginDate[0] ==0))
        return false;
    if(!pEndDate) pEndDate = m_sTime;

    ServProductIteration spiter;
    ServProduct sp;

    bool ret;
    ret = getServProductIteration(spiter,pBeginDate,true);

    if(!ret)
        return false;
    char* pEffDate=0;
    while (spiter.next(sp)) 
	{
		if (sp.getProductID() == nServProductID)
		{
			pEffDate = sp.getEffDate();
			if((pEffDate) && (strcmp(pBeginDate,pEffDate)<=0) && (strcmp(pEndDate,pEffDate)>0))
                return true;
		}
    }
    return false;
}

//##ModelId=4232AEBF0370
bool Serv::hasServProduct(int productid, ServProduct & sp, char *sdate) 
{
    ServProductIteration spiter;
    bool ret;

    if (sdate)
        ret = getServProductIteration(spiter, sdate);
    else
        ret = getServProductIteration(spiter);

    if (!ret) return false;
    
    while (spiter.next(sp)) {
        if (sp.getProductID() == productid)
            return true;
    }

    return false;
}

//##ModelId=4232B01003D8
char const *Serv::getProductEffDate(int productid, char *sdate)
{
    ServProductIteration spiter;
    ServProduct sp;
    bool ret;

    if (sdate)
        ret = getServProductIteration(spiter, sdate);
    else
        ret = getServProductIteration(spiter);

    if (!ret) return false;
    
    while (spiter.next(sp)) {
        if (sp.getProductID() == productid)
            return sp.getEffDate ();
    }

    return 0;
}

char const *Serv::getServProductEffDate(long iServProductID, char *sdate)
{
    ServProductIteration spiter;
    ServProduct sp;
    bool ret;

    if (sdate)
        ret = getServProductIteration(spiter, sdate);
    else
        ret = getServProductIteration(spiter);

    if (!ret) return false;
    
    while (spiter.next(sp)) {
        if (sp.getServProductID() == iServProductID)
            return sp.getEffDate ();
    }

    return 0;
}

void Serv::getAcctID(vector<long> & vAcctID)
{
    if(!m_poServInfo){
        return ;
    }
    
    unsigned int k = m_poServInfo->m_iServAcctOffset;

    while (k) {
        if (m_pSHMFileExternal->m_poServAcct[k].m_iState) {
            vAcctID.push_back (m_pSHMFileExternal->m_poServAcct[k].m_lAcctID);
        }
        k = m_pSHMFileExternal->m_poServAcct[k].m_iNextOffset;
    }
}


void Serv::getAcctID(vector<long> & vAcctID,char *sDate)
{
    //严格匹配生效和失效时间判断

    if(!m_poServInfo){
        return ;
    }
    unsigned int k = m_poServInfo->m_iServAcctOffset;

    while (k) 
    {
        if (strcmp(sDate,m_pSHMFileExternal->m_poServAcct[k].m_sEffDate)>=0 &&
         strcmp(sDate,m_pSHMFileExternal->m_poServAcct[k].m_sExpDate)<0)
        {
              vAcctID.push_back (m_pSHMFileExternal->m_poServAcct[k].m_lAcctID);
        }
        k = m_pSHMFileExternal->m_poServAcct[k].m_iNextOffset;
    }
}


void Serv::getAcctID(vector<long> & vAcctID,StdEvent * poEvent)
{
    //只比较到月份
    if(!m_poServInfo){
        return ;
    }
    unsigned int k = m_poServInfo->m_iServAcctOffset;

    int iTemp = 0;
    char strBeginDate[16];
    char strEndDate[16];
    BillingCycle * cycle = m_poBillingCycleMgr->getBillingCycle(poEvent->m_iBillingCycleID);
    strcpy(strBeginDate,cycle->getStartDate());
    strcpy(strEndDate,cycle->getEndDate());
    while (k) 
    {
        if ( strcmp(m_pSHMFileExternal->m_poServAcct[k].m_sEffDate,strEndDate) < 0 &&
                strcmp(m_pSHMFileExternal->m_poServAcct[k].m_sExpDate,strBeginDate) > 0)
        {
            vAcctID.push_back (m_pSHMFileExternal->m_poServAcct[k].m_lAcctID);
            iTemp ++;
        }
        k = m_pSHMFileExternal->m_poServAcct[k].m_iNextOffset;
    }
    if(iTemp> 0)
    {
        return;
    }
#ifdef USERINFO_OFFSET_ALLOWED

    unsigned int m;

    m = m_poServInfo->m_iServAcctOffset;
    Date date_sBeginDate(strBeginDate);
    Date date_sEndDate(strEndDate);

    while(m)
    {
        Date date_EffDate(m_pSHMFileExternal->m_poServAcct[m].m_sEffDate);
        long p = date_EffDate.diffSec(date_sEndDate);

        if(p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs) 
        {
            vAcctID.push_back (m_pSHMFileExternal->m_poServAcct[m].m_lAcctID);
        }else
        {
            Date date_ExpDate(m_pSHMFileExternal->m_poServAcct[m].m_sExpDate);
            p = date_sBeginDate.diffSec(date_ExpDate);
            if (p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs)
            {
                  vAcctID.push_back (m_pSHMFileExternal->m_poServAcct[m].m_lAcctID);
            }
        }
        m = m_pSHMFileExternal->m_poServAcct[m].m_iNextOffset;
    }
    return;
#endif
}
    
//##ModelId=4232B69901F3
long Serv::getAcctID(int acctitemtype)
{
    if(!m_poServInfo){
        return 0L;
    }
    unsigned int k = m_poServInfo->m_iServAcctOffset;

    while (k) {
        if (m_pSHMFileExternal->m_poServAcct[k].m_iState && 
        m_poAcctItemMgr->getBelongGroup(acctitemtype, m_pSHMFileExternal->m_poServAcct[k].m_iAcctItemGroup)) {
            return m_pSHMFileExternal->m_poServAcct[k].m_lAcctID;
        }
        k = m_pSHMFileExternal->m_poServAcct[k].m_iNextOffset;
    }
    return 0;
}

long Serv::getAcctID(int acctitemtype,char *sDate)
{
    if(!m_poServInfo){
        return 0L;
    }
    unsigned int k = m_poServInfo->m_iServAcctOffset;
    
    while (k)
    {
        //严格匹配生效和失效时间判断
         if (strcmp(sDate,m_pSHMFileExternal->m_poServAcct[k].m_sEffDate)>=0 &&
         strcmp(sDate,m_pSHMFileExternal->m_poServAcct[k].m_sExpDate)<0)
        {
              if (m_poAcctItemMgr->getBelongGroup(acctitemtype, m_pSHMFileExternal->m_poServAcct[k].m_iAcctItemGroup)) 
             {
                return m_pSHMFileExternal->m_poServAcct[k].m_lAcctID;
            }
        }
        k = m_pSHMFileExternal->m_poServAcct[k].m_iNextOffset;
    }
    return 0;
}


long Serv::getAcctID(int acctitemtype,StdEvent * poEvent)
{
    if(!m_poServInfo){
        return 0L;
    }
    unsigned int k =  m_poServInfo->m_iServAcctOffset;

    char strBeginDate[16];
    char strEndDate[16];
    BillingCycle * cycle = m_poBillingCycleMgr->getBillingCycle(poEvent->m_iBillingCycleID);
    strcpy(strBeginDate,cycle->getStartDate());
    strcpy(strEndDate,cycle->getEndDate());
    while (k)
    {
        if ( strcmp(m_pSHMFileExternal->m_poServAcct[k].m_sEffDate,strEndDate) < 0 &&
                strcmp(m_pSHMFileExternal->m_poServAcct[k].m_sExpDate,strBeginDate) > 0)
        {
              if (m_poAcctItemMgr->getBelongGroup(acctitemtype, m_pSHMFileExternal->m_poServAcct[k].m_iAcctItemGroup)) 
             {
                return m_pSHMFileExternal->m_poServAcct[k].m_lAcctID;
            }
        }
        k = m_pSHMFileExternal->m_poServAcct[k].m_iNextOffset;
    }
#ifdef USERINFO_OFFSET_ALLOWED

    unsigned int m;

    m = m_poServInfo->m_iServAcctOffset;
    Date date_sBeginDate(strBeginDate);
    Date date_sEndDate(strEndDate);

    while(m)
    {
        Date date_EffDate(m_pSHMFileExternal->m_poServAcct[m].m_sEffDate);
        long p = date_EffDate.diffSec(date_sEndDate);

        if(p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs) 
        {
            if( m_poAcctItemMgr->getBelongGroup(acctitemtype, m_pSHMFileExternal->m_poServAcct[m].m_iAcctItemGroup)) 
            {
                  return m_pSHMFileExternal->m_poServAcct[m].m_lAcctID;
            }
        }else
        {
            Date date_ExpDate(m_pSHMFileExternal->m_poServAcct[m].m_sExpDate);
            p = date_sBeginDate.diffSec(date_ExpDate);
            if (p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs)
            {
                if( m_poAcctItemMgr->getBelongGroup(acctitemtype, m_pSHMFileExternal->m_poServAcct[m].m_iAcctItemGroup)) 
                {
                      return m_pSHMFileExternal->m_poServAcct[m].m_lAcctID;
                }
            }
        }
        m = m_pSHMFileExternal->m_poServAcct[m].m_iNextOffset;
    }
#endif
    return 0;
}

//##ModelId=423505310263
//--- xueqt 20060512 modify for family plan.
int Serv::getPricingPlans(ExPricingPlan ret[MAX_PRICINGPLANS], char *sdate)
{
    unsigned int k, l;
    int i = 0, j, m;
    int offers[MAX_OFFERS];
    char *temp;

    if (sdate) temp = sdate;
    else temp = m_sTime;
    
	if(!m_poServInfo){
		return 0;
	}
    OfferInfo * pOffer = (OfferInfo *)(*m_pSHMFileExternal->m_poOfferData);

    l = m_pSHMFileExternal->m_poCust[m_poServInfo->m_iCustOffset].m_iCustPlanOffset;
    while (l) {
            if ( m_pSHMFileExternal->m_poCustPlan[l].m_iObjectType == CUST_TYPE ) {
                    if ( strcmp(temp, m_pSHMFileExternal->m_poCustPlan[l].m_sEffDate) >= 0      &&
                            strcmp(temp, m_pSHMFileExternal->m_poCustPlan[l].m_sExpDate) < 0 ) {

                            ret[i].m_iPricingPlanID = m_pSHMFileExternal->m_poCustPlan[l].m_iPricingPlanID;
                            ret[i].m_iCustPlanOffset = l;
                            i++;

                    }

            } else if (m_pSHMFileExternal->m_poCustPlan[l].m_iObjectType == SERV_INSTANCE_TYPE) {
                    if (m_pSHMFileExternal->m_poCustPlan[l].m_lObjectID == m_poServInfo->m_lServID      &&
                            strcmp(temp, m_pSHMFileExternal->m_poCustPlan[l].m_sEffDate) >= 0           &&
                                    strcmp(temp, m_pSHMFileExternal->m_poCustPlan[l].m_sExpDate) < 0 ) {

                            ret[i].m_iPricingPlanID = m_pSHMFileExternal->m_poCustPlan[l].m_iPricingPlanID;
                            ret[i].m_iCustPlanOffset = l;
                            i++;
                    }

            }

            l = m_pSHMFileExternal->m_poCustPlan[l].m_iNextOffset;
    }

/* add by xueqt 20060512 for family plan */

    k = m_poServInfo->m_iGroupOffset;
    ServGroupBaseInfo * pGroupBase = (ServGroupBaseInfo *)(*m_pSHMFileExternal->m_poServGroupBaseData);

    while (k) {
        if (m_pSHMFileExternal->m_poServGroupBaseIndex->get (m_pSHMFileExternal->m_poServGroup[k].m_lGroupID, &l)
                && pGroupBase[l].m_iGroupType==2 
                && strcmp(temp, m_pSHMFileExternal->m_poServGroup[k].m_sEffDate)>=0 
                && strcmp(temp, m_pSHMFileExternal->m_poServGroup[k].m_sExpDate)<0) {

            if (pGroupBase[l].m_iCustOffset) {
                l = m_pSHMFileExternal->m_poCust[pGroupBase[l].m_iCustOffset].m_iCustPlanOffset;

                while (l) {
                    if (m_pSHMFileExternal->m_poCustPlan[l].m_iObjectType==SERV_GROUP_TYPE 
                            && m_pSHMFileExternal->m_poCustPlan[l].m_lObjectID==m_pSHMFileExternal->m_poServGroup[k].m_lGroupID
                            && strcmp(temp, m_pSHMFileExternal->m_poCustPlan[l].m_sEffDate) >= 0
                                && strcmp(temp, m_pSHMFileExternal->m_poCustPlan[l].m_sExpDate) < 0 ) {

                        ret[i].m_iPricingPlanID = m_pSHMFileExternal->m_poCustPlan[l].m_iPricingPlanID;
                        ret[i].m_iCustPlanOffset = l;
                        i++;
                    }


                    l = m_pSHMFileExternal->m_poCustPlan[l].m_iNextOffset;
                }
            }
        }

        k = m_pSHMFileExternal->m_poServGroup[k].m_iNextOffset;
    }
    
/* end xueqt 20060512 */

    j = getOffers(offers, temp);
    for (m=0; m<j; m++) {
            if (m_pSHMFileExternal->m_poOfferIndex->get (offers[m], &k)) {
                    ret[i].m_iPricingPlanID = pOffer[k].m_iPricingPlan;
                    ret[i].m_iCustPlanOffset = 0;
                    i++;
            }
    }


    return i;

}

//##ModelId=423506410214
int Serv::getBasePricingPlan(char *sDate)
{
    ServTypeInfo * pServType = *m_pSHMFileExternal->m_poServTypeData;
    unsigned int k;

    if (!sDate) sDate = m_sTime;
	if(!m_poServInfo){
		return 0;
	}
    k = m_poServInfo->m_iServTypeOffset;

    while (k) {
        if (strcmp (pServType[k].m_sEffDate, sDate)<=0 && 
            strcmp (pServType[k].m_sExpDate, sDate)>0 ) {
            return pServType[k].m_iPricingPlanID;

        }

        k = pServType[k].m_iNextOffset;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iServTypeOffset;

    Date date_sDate(sDate);

    while (k) {
        Date date_EffDate(pServType[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs  ) {
            return pServType[k].m_iPricingPlanID;

        }
        else {
            Date date_ExpDate(pServType[k].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
            if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs  ) {
                return pServType[k].m_iPricingPlanID;

            }
        }

        k = pServType[k].m_iNextOffset;
    }

    #endif

    return 0;
}

//##ModelId=423506930302
long Serv::getCustID()
{
	if(m_poCustInfo){
		return m_poCustInfo->m_lCustID;
	}
	
	return -1;
}

//##ModelId=42355F1103A7
char * Serv::getCustType()
{
	if(m_poCustInfo){
		return m_poCustInfo->m_sCustType;
	}
	
	return 0;
}

//##ModelId=423677D80135
int Serv::getCustServ(vector<long> &ret)
{
    int i = 0;
    unsigned int k;
	if(!m_poServInfo){
		return 0;
	}
    //##找到cust
    if (!(m_pSHMFileExternal->m_poCustIndex->get(m_poServInfo->m_lCustID, &k)))
        return 0;

    k = m_pSHMFileExternal->m_poCust[k].m_iServOffset;
    while (k) {
        ret.push_back (m_pSHMFileExternal->m_poServ[k].m_lServID);
        k = m_pSHMFileExternal->m_poServ[k].m_iCustNext;
        i++;
    }
    return i;
}

int Serv::getOfferIns(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER],char *sDate)
{
    if(!sDate)
    {
        sDate = m_sTime;
    }
	if(!m_poServInfo){
		return 0;
	}
    int iRet = 0;
    int iLastNum = 0;
    unsigned int k = m_poServInfo->m_iOfferDetailOffset;
    unsigned int j = 0;
    //1.先找一遍直属的商品实例
    while(k){
        if (strcmp(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate, sDate)<=0 && 
            strcmp(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate, sDate)>0 ) 
        {
            strcpy(ret[iRet].m_sTime,sDate);
            ret[iRet].m_bAccued = false;
            ret[iRet].m_poDetailIns = m_pSHMFileExternal->m_poOfferDetailIns + k;
            ret[iRet++].m_poOfferIns = m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[k].m_iParentOffset;            
            if(iRet == MAX_OFFER_INSTANCE_NUMBER)
            {
                Log::log(0, "[WARNING] Serv ID: %ld 's Offer Detail Instance Number Greater than %d, Please check it.", m_poServInfo->m_lServID, MAX_OFFER_INSTANCE_NUMBER);
                return iRet;
            }
        }
        k = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextProdOfferOffset;
    }
    //2.再找多层的实例关系TODO
    for(; iLastNum<iRet; iLastNum++)
    {
        if(m_pSHMFileExternal->m_poOfferDetailInsOfferInsIndex->get(ret[iLastNum].m_poOfferIns->m_lProdOfferInsID,&j))
        {
            while(j)
            {
                if (strcmp(m_pSHMFileExternal->m_poOfferDetailIns[j].m_sEffDate, sDate)<=0 && 
                    strcmp(m_pSHMFileExternal->m_poOfferDetailIns[j].m_sExpDate, sDate)>0 )
                {
                    strcpy(ret[iRet].m_sTime,sDate);
                    ret[iRet].m_bAccued = false;
                    ret[iRet].m_poDetailIns = m_pSHMFileExternal->m_poOfferDetailIns + j;
                    ret[iRet++].m_poOfferIns = m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[j].m_iParentOffset;
                    if(iRet == MAX_OFFER_INSTANCE_NUMBER)
                    {
                        return iRet;
                    }
                }
                j = m_pSHMFileExternal->m_poOfferDetailIns[j].m_iNextProdOfferOffset;
            }
        }
    }
    //增加基本商品
    if(getBaseOfferIns(ret+iRet,sDate))
    {
        iRet++;
    }
    /*if(iRet > 1)//需要排序
    {
        OfferInsQO tempG;
        for(int l=0; l<iRet; l++)
        {
        }
    }*/
    return iRet;
}

int Serv::getOfferInsBillingCycle(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER],char *sDate)
{
    if(!sDate)
        sDate = m_sTime;
    
	if(!m_poServInfo)
		return 0;

    BillingCycleMgr bcm;
    BillingCycle * pCycle = bcm.getOccurBillingCycle(1, sDate);
    if(!pCycle){
        Log::log (0, "\n\n[error!]根据指定时间的帐务周期在内存中不存在...");
        return 0;
    }
    
    int iRet = 0;
    int iLastNum = 0;
    unsigned int k = m_poServInfo->m_iOfferDetailOffset;
    unsigned int j = 0;
    bool bDateMark = false;
    char sTempDate[15] = {0};
    //1.先找一遍直属的商品实例
    while(k){
        if(strcmp(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate, pCycle->getStartDate()) <= 0 && 
            strcmp(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate, pCycle->getStartDate()) > 0){
            bDateMark = true;
            strncpy(sTempDate, pCycle->getStartDate(), 14);
        }else if(strcmp(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate, pCycle->getStartDate()) >= 0 && 
            strcmp(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate, pCycle->getEndDate()) < 0){
            bDateMark = true;
            strncpy(sTempDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate, 14);
        }
        
        if(bDateMark){
            strcpy(ret[iRet].m_sTime, sTempDate);
            ret[iRet].m_bAccued = false;
            ret[iRet].m_poDetailIns = m_pSHMFileExternal->m_poOfferDetailIns + k;
            ret[iRet++].m_poOfferIns = m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[k].m_iParentOffset;            
            if(iRet == MAX_OFFER_INSTANCE_NUMBER){
                Log::log(0, "[WARNING] Serv ID: %ld 's Offer Detail Instance Number Greater than %d, Please check it.", m_poServInfo->m_lServID, MAX_OFFER_INSTANCE_NUMBER);
                return iRet;
            }
        }
        k = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextProdOfferOffset;
        bDateMark = false;
    }
    
    //2.再找多层的实例关系TODO
    for(; iLastNum<iRet; iLastNum++){
        if(m_pSHMFileExternal->m_poOfferDetailInsOfferInsIndex->get(ret[iLastNum].m_poOfferIns->m_lProdOfferInsID,&j)){
            while(j){
                if(strcmp(m_pSHMFileExternal->m_poOfferDetailIns[j].m_sEffDate, pCycle->getStartDate()) <= 0 && 
                    strcmp(m_pSHMFileExternal->m_poOfferDetailIns[j].m_sExpDate, pCycle->getStartDate()) > 0){
                    bDateMark = true;
                    strncpy(sTempDate, pCycle->getStartDate(), 14);
                }else if(strcmp(m_pSHMFileExternal->m_poOfferDetailIns[j].m_sEffDate, pCycle->getStartDate()) >= 0 && 
                    strcmp(m_pSHMFileExternal->m_poOfferDetailIns[j].m_sEffDate, pCycle->getEndDate()) < 0){
                    bDateMark = true;
                    strncpy(sTempDate, m_pSHMFileExternal->m_poOfferDetailIns[j].m_sEffDate, 14);
                }

                if (bDateMark){
                    strcpy(ret[iRet].m_sTime, sTempDate);
                    ret[iRet].m_bAccued = false;
                    ret[iRet].m_poDetailIns = m_pSHMFileExternal->m_poOfferDetailIns + j;
                    ret[iRet++].m_poOfferIns = m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[j].m_iParentOffset;
                    
                    if(iRet == MAX_OFFER_INSTANCE_NUMBER)
                        return iRet;
                }
                j = m_pSHMFileExternal->m_poOfferDetailIns[j].m_iNextProdOfferOffset;
                bDateMark = false;
            }
        }
    }
    
    //增加基本商品
    if(getBaseOfferIns(ret+iRet, sDate))
        iRet++;
    
    return iRet;
}

int Serv::getBaseOfferIns(OfferInsQO * ret, char *sDate)
{
    if(!sDate)
    {
        sDate = m_sTime;
    }
	if(!m_poServInfo){
		return 0;
	}
    unsigned int k = m_poServInfo->m_iBaseOfferOffset;
    while(k){
        if (strcmp (m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate, sDate)<=0 && 
            strcmp (m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate, sDate)>0 )
        {
            strcpy(ret->m_sTime,sDate);
            ret->m_bAccued = false;
            ret->m_poDetailIns = m_pSHMFileExternal->m_poOfferDetailIns + k;
            ret->m_poOfferIns = m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[k].m_iParentOffset;
            return 1;
        }
        k = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextProdOfferOffset;
    }

#ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iBaseOfferOffset;

    Date date_sDate(sDate);

    while (k) {

        Date date_EffDate(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);

        if (p >= UserInfoReader::m_beginOffsetSecs  &&
                p <= UserInfoReader::m_offsetSecs)
        {
            strcpy(ret->m_sTime,sDate);
            ret->m_bAccued = false;
            ret->m_poDetailIns = m_pSHMFileExternal->m_poOfferDetailIns + k;
            ret->m_poOfferIns = m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[k].m_iParentOffset;
            return 1;
        }
        Date date_ExpDate(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate);
        p = date_sDate.diffSec(date_ExpDate);
        if (p >= UserInfoReader::m_beginOffsetSecs  &&
                p <= UserInfoReader::m_offsetSecs)
        {
            strcpy(ret->m_sTime,sDate);
            ret->m_bAccued = false;
            ret->m_poDetailIns = m_pSHMFileExternal->m_poOfferDetailIns + k;
            ret->m_poOfferIns = m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[k].m_iParentOffset;
            return 1;
        }
        k = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextProdOfferOffset;
    }
#endif
    return 0;
}

int Serv::getBaseOfferID(char *sDate)
{
    if(!sDate)
    {
        sDate = m_sTime;
    }
	if(!m_poServInfo){
		return -1;
	}
    unsigned int k = m_poServInfo->m_iBaseOfferOffset;
    while(k){
        if (strcmp (m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate, sDate)<=0 && 
            strcmp (m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate, sDate)>0 )
        {
            return m_pSHMFileExternal->m_poProdOfferIns[m_pSHMFileExternal->m_poOfferDetailIns[k].m_iParentOffset].m_iOfferID;
        }
        k = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextProdOfferOffset;
    }
    
#ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iBaseOfferOffset;

    Date date_sDate(sDate);

    while (k) {

        Date date_EffDate(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);

        if (p >= UserInfoReader::m_beginOffsetSecs  &&
                p <= UserInfoReader::m_offsetSecs)
        {
            return m_pSHMFileExternal->m_poProdOfferIns[m_pSHMFileExternal->m_poOfferDetailIns[k].m_iParentOffset].m_iOfferID;
        }
        Date date_ExpDate(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate);
        p = date_sDate.diffSec(date_ExpDate);
        if (p >= UserInfoReader::m_beginOffsetSecs  &&
                p <= UserInfoReader::m_offsetSecs)
        {
            return m_pSHMFileExternal->m_poProdOfferIns[m_pSHMFileExternal->m_poOfferDetailIns[k].m_iParentOffset].m_iOfferID;
        }
        k = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextProdOfferOffset;
    }
#endif
    return -1;
}

bool Serv::hasOffer (int iOfferID, char *sDate)
{
    OfferInsQO ret[MAX_OFFERS];
    int retNum = getOfferIns(ret, sDate);
    for(int i=0; i<retNum; i++)
    {
        if(ret[i].m_poOfferIns->m_iOfferID == iOfferID)
        {
            return true;
        }
    }
    return false;
}

CustPricingTariffInfo * UserInfoInterface::getCustTariff(unsigned int iCustPlanOffset, char *sCalledNBR)
{
    long lCustPlanID = m_pSHMFileExternal->m_poCustPlan[iCustPlanOffset].m_lCustPlanID;    

    unsigned int k;
    CustPricingTariffInfo *p = (CustPricingTariffInfo *)(*m_pSHMFileExternal->m_poCustTariffData);

    if (!m_pSHMFileExternal->m_poCustTariffIndex->getMax (sCalledNBR, &k)) return 0;

    while (k) {
        if (p[k].m_lCustPlanID == lCustPlanID) return p+k;

        if (p[k].m_lCustPlanID > lCustPlanID) 
            k = p[k].m_iLeft;
        else 
            k = p[k].m_iRight;
    }

    return 0;
}

long UserInfoInterface::getCustomObjectID(int iPlanObjectID, unsigned int iCustPlanOffset)
{
    static unsigned int lastOffset = 0;
    static int lastObjectID = 0;
    static unsigned int k = 0;

    CustPricingObjectInfo * pCustObject = (CustPricingObjectInfo *)(*m_pSHMFileExternal->m_poCustPricingObjectData);

    long lRet = 0;

    if (lastObjectID!=iPlanObjectID  || lastOffset!=iCustPlanOffset) {
        k = m_pSHMFileExternal->m_poCustPlan[iCustPlanOffset].m_iObjectOffset;
        lastObjectID = iPlanObjectID;
        lastOffset = iCustPlanOffset;
    }

    while (k) {
        if (pCustObject[k].m_iPricingObjectID == iPlanObjectID) {
            lRet = pCustObject[k].m_lObjectID;
            k = pCustObject[k].m_iNextOffset;

            return lRet;
        }

            k = pCustObject[k].m_iNextOffset;
    }

    lastOffset = 0;
    lastObjectID = 0;
    return 0;
}

bool UserInfoInterface::isBindServObject(long lServID, unsigned int iCustPlanOffset)
{
        CustPricingObjectInfo * pCustObject = (CustPricingObjectInfo *)(*m_pSHMFileExternal->m_poCustPricingObjectData);

        unsigned int k = m_pSHMFileExternal->m_poCustPlan[iCustPlanOffset].m_iObjectOffset;

        while (k) {
                if (lServID==pCustObject[k].m_lObjectID && pCustObject[k].m_iObjectType==SERV_INSTANCE_TYPE)
                        return true;

                k = pCustObject[k].m_iNextOffset;
        }

        return false;
}

CustPricingObjectInfo const * UserInfoInterface::getCustomObject(int iPlanObjectID, unsigned int iCustPlanOffset)
{
    static unsigned int lastOffset = 0;
    static int lastObjectID = 0;
    static unsigned int k = 0;

    CustPricingObjectInfo * pCustObject = (CustPricingObjectInfo *)(*m_pSHMFileExternal->m_poCustPricingObjectData);

    CustPricingObjectInfo const * pRet = 0;

    if (lastObjectID!=iPlanObjectID  || lastOffset!=iCustPlanOffset) {
        k = m_pSHMFileExternal->m_poCustPlan[iCustPlanOffset].m_iObjectOffset;
        lastObjectID = iPlanObjectID;
        lastOffset = iCustPlanOffset;
    }

        while (k) {
                if (pCustObject[k].m_iPricingObjectID == iPlanObjectID) {
            pRet = pCustObject + k;
            k = pCustObject[k].m_iNextOffset;

            return pRet;
                }

                k = pCustObject[k].m_iNextOffset;
        }

    lastOffset = 0;
    lastObjectID = 0;
        return 0;
}

char const * UserInfoInterface::getCustomPlanEffDate(unsigned int iCustPlanOffset)
{
    return m_pSHMFileExternal->m_poCustPlan[iCustPlanOffset].m_sEffDate;

}

int UserInfoInterface::getGroupServ(long lServGroupID, vector<long> &vServ, char *sDate)
{
    unsigned int k;
    int ret = 0;
    if (!m_pSHMFileExternal->m_poServGroupBaseIndex->get (lServGroupID, &k)) return 0;
    ServGroupBaseInfo * pGroupBase = (ServGroupBaseInfo *)(*m_pSHMFileExternal->m_poServGroupBaseData);
    ServGroupInfo * pGroup = (ServGroupInfo *)(*m_pSHMFileExternal->m_poServGroupData);

    k = pGroupBase[k].m_iServOffset;
    
    while (k) {
        if (strcmp (sDate, pGroup[k].m_sEffDate)>=0  && strcmp (sDate, pGroup[k].m_sExpDate)<0) {
            vServ.push_back (pGroup[k].m_lServID);
            ret++;
        }

        k = pGroup[k].m_iGroupNext;
    }

    return ret;

}

int UserInfoInterface::getCustServ(long lCustID, vector<long> &vServ)
{
    unsigned int k;
    int ret = 0;
    if (!m_pSHMFileExternal->m_poCustIndex->get (lCustID, &k)) return 0;
    k = m_pSHMFileExternal->m_poCust[k].m_iServOffset;
    
    while (k) {
        vServ.push_back (m_pSHMFileExternal->m_poServ[k].m_lServID);
        ret++;
        k = m_pSHMFileExternal->m_poServ[k].m_iCustNext;
    }

    return ret;
}

int UserInfoInterface::getCustAcct(long lCustID, vector<long> &vAcct)
{
    unsigned int k;
    int ret = 0;
    if (!m_pSHMFileExternal->m_poCustIndex->get (lCustID, &k)) return 0;
    k = m_pSHMFileExternal->m_poCust[k].m_iAcctOffset;
    AcctInfo * pAcct = *m_pSHMFileExternal->m_poAcctData;
    
    while (k) {
        if ((pAcct[k].m_sState[2]|0X20) == 'a') {
            vAcct.push_back (pAcct[k].m_lAcctID);
            ret++;
        }
        k = pAcct[k].m_iNextOffset;
    }

    return ret;
}

/*
**    获取某AcctID的支付关系
*/
int UserInfoInterface::getPayInfo(vector<ServAcctInfo> &ret, long lAcctID, char *sDate)
{
    unsigned int l;
    int iTemp = 0;

    unsigned int k;

    if (m_pSHMFileExternal->m_poServAcctIndex->get (lAcctID, &l)) 
    {
        while (l)
        {
            if (m_pSHMFileExternal->m_poServAcct[l].m_iState)
            {
                if (sDate)
                {
                    /*    进行用户有效性的校验    */
                    if (m_pSHMFileExternal->m_poServIndex->get (m_pSHMFileExternal->m_poServAcct[l].m_lServID, &k))
                    {
                        k = m_pSHMFileExternal->m_poServ[k].m_iStateOffset;
                        
                        /*
                        while (k && strcmp(sDate, m_poServState[k].m_sEffDate)<0)
                        {
                            k = m_poServState[k].m_iNextOffset;
                        }
                        */
                        
                        while(k) {
                          if(strcmp(m_pSHMFileExternal->m_poServState[k].m_sEffDate,sDate)<=0
                            && strcmp(m_pSHMFileExternal->m_poServState[k].m_sExpDate,sDate)>0) {
                             break;
                          }
                          
                          k = m_pSHMFileExternal->m_poServState[k].m_iNextOffset;
                        }

                        if (k && (m_pSHMFileExternal->m_poServState[k].m_sState[2]|0x20)!='x')
                        {
                            ret.push_back (m_pSHMFileExternal->m_poServAcct[l]);
                            iTemp++;
                        }                        
                    }                    
                } else
                {
                    ret.push_back (m_pSHMFileExternal->m_poServAcct[l]);
                    iTemp++;
                }
            }
            l = m_pSHMFileExternal->m_poServAcct[l].m_iAcctNextOffset;
        }
    }
    return iTemp;
}

int UserInfoInterface::getPayInfo(vector<ServAcctInfo> &ret, long lAcctID, StdEvent * poEvent)
{
    unsigned int l;
    int iTemp = 0,iFind = -1;

    unsigned int k;
    char strBeginDate[16];
    char strEndDate[16];
    BillingCycle * cycle = m_poBillingCycleMgr->getBillingCycle(poEvent->m_iBillingCycleID);;
    strcpy(strBeginDate,cycle->getStartDate());
    strcpy(strEndDate,cycle->getEndDate());

    if (m_pSHMFileExternal->m_poServAcctIndex->get (lAcctID, &l))
    {
        while (l)
        {
            if ( strcmp(m_pSHMFileExternal->m_poServAcct[l].m_sEffDate,strEndDate) < 0 &&
                strcmp(m_pSHMFileExternal->m_poServAcct[l].m_sExpDate,strEndDate) >= 0)
            {
                ret.push_back (m_pSHMFileExternal->m_poServAcct[l]);
                iTemp++;
            }
            l = m_pSHMFileExternal->m_poServAcct[l].m_iAcctNextOffset;
        }
    }
    if(iTemp)
    {
        return iTemp;
    }
#ifdef USERINFO_OFFSET_ALLOWED

    unsigned int m;
    Date date_sBeginDate(strBeginDate);
    Date date_sEndDate(strEndDate);

    if(m_pSHMFileExternal->m_poServAcctIndex->get (lAcctID, &m))
    {
        while(m)
        {
            Date date_EffDate(m_pSHMFileExternal->m_poServAcct[m].m_sEffDate);
            long p = date_EffDate.diffSec(date_sEndDate);
            if(p>= UserInfoReader::m_beginOffsetSecs
                        && p <= UserInfoReader::m_offsetSecs)
            {
                    ret.push_back (m_pSHMFileExternal->m_poServAcct[m]);
                    iTemp ++;
            }else
            {
                Date date_ExpDate(m_pSHMFileExternal->m_poServAcct[m].m_sExpDate);
                p = date_sBeginDate.diffSec(date_ExpDate);
                if(p>= UserInfoReader::m_beginOffsetSecs
                        && p <= UserInfoReader::m_offsetSecs)
                {
                    ret.push_back (m_pSHMFileExternal->m_poServAcct[m]);
                    iTemp ++;
                }
            }
            m = m_pSHMFileExternal->m_poServAcct[m].m_iAcctNextOffset;
        }
    }
#endif
    return iTemp;
}

//##客户个性话参数`
long UserInfoInterface::getCustomParamValue (int iParamID, unsigned int iCustPlanOffset)
{
    unsigned int k = m_pSHMFileExternal->m_poCustPlan[iCustPlanOffset].m_iParamOffset;
    CustPricingParamInfo * pCustParam = (CustPricingParamInfo *)(*m_pSHMFileExternal->m_poCustPricingParamData);

    while (k) {
        if (pCustParam[k].m_iParamID == iParamID) {
            return pCustParam[k].m_lValue;
        }
        k = pCustParam[k].m_iNextOffset;
    }

    return 0;
}

//找对象的个性化的定价信息
int UserInfoInterface::getCustomPricingPlans(ExPricingPlan ret[MAX_PRICINGPLANS],int iObjectType,long lObjectID,char *sDate)
{
    unsigned int k;
    unsigned long lCustID;
    int i = 0;

    switch (iObjectType) {
      case SERV_INSTANCE_TYPE:
        if (!m_pSHMFileExternal->m_poServIndex->get (lObjectID, &k)) return 0;
        k = m_pSHMFileExternal->m_poCust[m_pSHMFileExternal->m_poServ[k].m_iCustOffset].m_iCustPlanOffset;
        break;

      case CUST_TYPE:
        lCustID = lObjectID;
        if (!m_pSHMFileExternal->m_poCustIndex->get (lCustID, &k)) return 0;
        k = m_pSHMFileExternal->m_poCust[k].m_iCustPlanOffset;

        break;

      case ACCT_TYPE:
        if (!m_pSHMFileExternal->m_poAcctIndex->get (lObjectID, &k)) return 0;
        lCustID = (*m_pSHMFileExternal->m_poAcctData)[k].m_lCustID;

        if (!m_pSHMFileExternal->m_poCustIndex->get (lCustID, &k)) return 0;
        k = m_pSHMFileExternal->m_poCust[k].m_iCustPlanOffset;

        break;

      case SERV_GROUP_TYPE: {
        ServGroupBaseInfo * pGroupBase = (ServGroupBaseInfo *)(*m_pSHMFileExternal->m_poServGroupBaseData);
        if (!m_pSHMFileExternal->m_poServGroupBaseIndex->get (lObjectID, &k)) return 0;

        if (pGroupBase[k].m_iCustOffset) {
            k = m_pSHMFileExternal->m_poCust[pGroupBase[k].m_iCustOffset].m_iCustPlanOffset;
        } else {
            k = 0;
        }

        break;
      }

      default:
        return 0;
        
    }

    while (k) {
        if ( m_pSHMFileExternal->m_poCustPlan[k].m_iObjectType == CUST_TYPE                 &&
                    strcmp(sDate, m_pSHMFileExternal->m_poCustPlan[k].m_sEffDate) >= 0  &&
                    strcmp(sDate, m_pSHMFileExternal->m_poCustPlan[k].m_sExpDate) < 0 ) {

            ret[i].m_iPricingPlanID = m_pSHMFileExternal->m_poCustPlan[k].m_iPricingPlanID;
            ret[i].m_iCustPlanOffset = k;
            i++;

        } else if (m_pSHMFileExternal->m_poCustPlan[k].m_iObjectType == iObjectType) {
            if (m_pSHMFileExternal->m_poCustPlan[k].m_lObjectID == lObjectID             &&
                    strcmp(sDate, m_pSHMFileExternal->m_poCustPlan[k].m_sEffDate) >= 0   &&
                            strcmp(sDate, m_pSHMFileExternal->m_poCustPlan[k].m_sExpDate) < 0 ) {

                    ret[i].m_iPricingPlanID = m_pSHMFileExternal->m_poCustPlan[k].m_iPricingPlanID;
                    ret[i].m_iCustPlanOffset = k;
                    i++;
            }

        }

        k = m_pSHMFileExternal->m_poCustPlan[k].m_iNextOffset;
    }

    return i;

}

//cdk增加商品示例的查询
//获得帐户的商品实例
int UserInfoInterface::getAcctOfferIns(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER], long lAcctID, char *sDate)
{
    unsigned int p = 0;
    if (!m_pSHMFileExternal->m_poAcctIndex->get (lAcctID, &p)) return 0;

    int iRet = 0;
    int iLastNum = 0;
    unsigned int k = m_pSHMFileExternal->m_poAcct[p].m_iOfferDetailOffset;
    unsigned int j = 0;
    //1.先找一遍直属的商品实例
    while(k){
        if (!sDate||
            (strcmp(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate, sDate)<=0 && 
            strcmp(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate, sDate)>0 )) 
        {
            strcpy(ret[iRet].m_sTime,sDate);
            ret[iRet].m_bAccued = false;
            ret[iRet].m_poDetailIns = m_pSHMFileExternal->m_poOfferDetailIns + k;
            ret[iRet++].m_poOfferIns = m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[k].m_iParentOffset;
            if(iRet == MAX_OFFER_INSTANCE_NUMBER)
            {
                return iRet;
            }
        }
        k = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextProdOfferOffset;
    }
    //2.再找多层的实例关系TODO
    for(; iLastNum<iRet; iLastNum++)
    {
        if(m_pSHMFileExternal->m_poOfferDetailInsOfferInsIndex->get(ret[iLastNum].m_poOfferIns->m_lProdOfferInsID,&j))
        {
            while(j)
            {
                if (strcmp(m_pSHMFileExternal->m_poOfferDetailIns[j].m_sEffDate, sDate)<=0 && 
                    strcmp(m_pSHMFileExternal->m_poOfferDetailIns[j].m_sExpDate, sDate)>0 )
                {
                    strcpy(ret[iRet].m_sTime,sDate);
                    ret[iRet].m_bAccued = false;
                    ret[iRet].m_poDetailIns = m_pSHMFileExternal->m_poOfferDetailIns + j;
                    ret[iRet++].m_poOfferIns = m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[j].m_iParentOffset;
                    if(iRet == MAX_OFFER_INSTANCE_NUMBER)
                    {
                        return iRet;
                    }
                }
                j = m_pSHMFileExternal->m_poOfferDetailIns[j].m_iNextProdOfferOffset;
            }
        }
    }
    /*if(iRet > 1)//需要排序
    {
        OfferInsQO tempG;
        for(int l=0; l<iRet; l++)
        {
        }
    }*/
    return iRet;
}
int UserInfoInterface::getCustOfferIns(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER], long lCustID, char *sDate)
{
    unsigned int p = 0;
    if (!m_pSHMFileExternal->m_poCustIndex->get (lCustID, &p)) return 0;

    int iRet = 0;
    int iLastNum = 0;
    unsigned int k = m_pSHMFileExternal->m_poCust[p].m_iOfferDetailOffset;
    unsigned int j = 0;
    //1.先找一遍直属的商品实例
    while(k){
        if (!sDate||
            (strcmp(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate, sDate)<=0 && 
            strcmp(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate, sDate)>0 )) 
        {
            strcpy(ret[iRet].m_sTime,sDate);
            ret[iRet].m_bAccued = false;
            ret[iRet].m_poDetailIns = m_pSHMFileExternal->m_poOfferDetailIns + k;
            ret[iRet++].m_poOfferIns = m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[k].m_iParentOffset;
            if(iRet == MAX_OFFER_INSTANCE_NUMBER)
            {
                return iRet;
            }
        }
        k = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextProdOfferOffset;
    }
    //2.再找多层的实例关系TODO
    for(; iLastNum<iRet; iLastNum++)
    {
        if(m_pSHMFileExternal->m_poOfferDetailInsOfferInsIndex->get(ret[iLastNum].m_poOfferIns->m_lProdOfferInsID,&j))
        {
            while(j)
            {
                if (strcmp(m_pSHMFileExternal->m_poOfferDetailIns[j].m_sEffDate, sDate)<=0 && 
                    strcmp(m_pSHMFileExternal->m_poOfferDetailIns[j].m_sExpDate, sDate)>0 )
                {
                    strcpy(ret[iRet].m_sTime,sDate);
                    ret[iRet].m_bAccued = false;
                    ret[iRet].m_poDetailIns = m_pSHMFileExternal->m_poOfferDetailIns + j;
                    ret[iRet++].m_poOfferIns = m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[j].m_iParentOffset;
                    if(iRet == MAX_OFFER_INSTANCE_NUMBER)
                    {
                        return iRet;
                    }
                }
                j = m_pSHMFileExternal->m_poOfferDetailIns[j].m_iNextProdOfferOffset;
            }
        }
    }

    return iRet;
}

bool UserInfoInterface::isOfferInsBelongOrg(long lOfferInstID, int iOrgID, char * sDate)
{
    unsigned int j = 0;
    long ltempAcctID = 0;
    long ltempCustID = 0;
    long ltempOfferInsID = 0;
    unsigned int p;
    
    
    if (!m_pSHMFileExternal->m_poProdOfferInsIndex->get (lOfferInstID, &j)) return false;

    unsigned int k = m_pSHMFileExternal->m_poProdOfferIns[j].m_iOfferDetailInsOffset;  

    for (; k; k=m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextDetailOffset) {
        
        if (!sDate||
            (strcmp(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate, sDate)>0 || 
            strcmp(m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate, sDate)<=0 )) 
        {
            continue;
        }
        
        switch (m_pSHMFileExternal->m_poOfferDetailIns[k].m_iInsType)
        {
            case SERV_INSTANCE_TYPE:
                if (m_pSHMFileExternal->m_poServIndex->get (m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID, &p)) {
                        j = m_pSHMFileExternal->m_poServ[p].m_iLocationOffset;
                        while (j && strcmp(sDate, m_pSHMFileExternal->m_poServLocation[j].m_sEffDate)<0) {
                            j = m_pSHMFileExternal->m_poServLocation[j].m_iNextOffset;
                        }
                        if (j) {
                                return m_poOrgMgr->getBelong (m_pSHMFileExternal->m_poServLocation[j].m_iOrgID, iOrgID);
                        }
                }
                break;
            case ACCT_TYPE:
                if(!ltempAcctID) ltempAcctID = m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID;
                break;
            case CUST_TYPE:
                if(!ltempCustID) ltempCustID = m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID;
                break;
            case OFFER_INSTANCE_TYPE:
                if(!ltempOfferInsID) ltempOfferInsID = m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID;
                break;
        }     
    }

    if(ltempCustID) return getCustBelongOrg(ltempCustID, iOrgID);

    if(ltempAcctID) return getAcctBelongOrg(ltempAcctID, iOrgID);

    if(ltempOfferInsID) 
        return isOfferInsBelongOrg(ltempOfferInsID, iOrgID, sDate);

    return true;
}
int UserInfoInterface::getOfferInsByID(OfferInsQO& ret,long lProdOfferInstID,char* sDate)
{
    unsigned int k=0;
    if (!m_pSHMFileExternal->m_poProdOfferInsIndex->get (lProdOfferInstID, &k)) return 0;

    ret.m_poDetailIns = 0;
    ret.m_poOfferIns = m_pSHMFileExternal->m_poProdOfferIns + k;	
    strcpy(ret.m_sTime,sDate);	
    return 1; 	
}
int UserInfoInterface::getAllOfferInsByID(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER], long lProdOfferInsID, char *sDate)
{
    unsigned int k;
    int iRet = 0;
    unsigned int j = 0;
    
    if (!m_pSHMFileExternal->m_poProdOfferInsIndex->get (lProdOfferInsID, &k)) return 0;
    
    strcpy(ret[iRet].m_sTime,sDate);
    ret[iRet].m_poDetailIns = 0;
    ret[iRet++].m_poOfferIns = m_pSHMFileExternal->m_poProdOfferIns + k;
    if(iRet == MAX_OFFER_INSTANCE_NUMBER)
    {
        return iRet;
    }
    for(int iLastNum = 0; iLastNum<iRet; iLastNum++)
    {
        if(m_pSHMFileExternal->m_poOfferDetailInsOfferInsIndex->get(ret[iLastNum].m_poOfferIns->m_lProdOfferInsID,&j))
        {
            if (strcmp(m_pSHMFileExternal->m_poOfferDetailIns[j].m_sEffDate, sDate)<=0 && 
                strcmp(m_pSHMFileExternal->m_poOfferDetailIns[j].m_sExpDate, sDate)>0 )
            {
                strcpy(ret[iRet].m_sTime,sDate);
                ret[iRet].m_poDetailIns = m_pSHMFileExternal->m_poOfferDetailIns + j;
                ret[iRet++].m_poOfferIns = m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[j].m_iParentOffset;
                if(iRet == MAX_OFFER_INSTANCE_NUMBER)
                {
                    return iRet;
                }
            }
        }
    }
    return iRet;
}

long UserInfoInterface::getAcctID(long lServID, int acctitemtype)
{
    unsigned int l;
    
    if (!m_pSHMFileExternal->m_poServIndex->get(lServID, &l))
    {
        return 0;
    }
    unsigned int k = m_pSHMFileExternal->m_poServ[l].m_iServAcctOffset;
    while (k) {
        if (m_pSHMFileExternal->m_poServAcct[k].m_iState && 
        m_poAcctItemMgr->getBelongGroup(acctitemtype, m_pSHMFileExternal->m_poServAcct[k].m_iAcctItemGroup)) {
            return m_pSHMFileExternal->m_poServAcct[k].m_lAcctID;
        }
        k = m_pSHMFileExternal->m_poServAcct[k].m_iNextOffset;
    }
    return 0;
}

long UserInfoInterface::getAcctID(long lServID, int acctitemtype, char * sDate)
{
    unsigned int l;
    //严格配置生效和失效时间
    if (!m_pSHMFileExternal->m_poServIndex->get(lServID, &l))
    {
        return 0;
    }
    unsigned int k = m_pSHMFileExternal->m_poServ[l].m_iServAcctOffset;
    while (k)
    {
        if (strcmp(sDate,m_pSHMFileExternal->m_poServAcct[k].m_sEffDate)>=0 &&
             strcmp(sDate,m_pSHMFileExternal->m_poServAcct[k].m_sExpDate)<0)
        {
            if( m_poAcctItemMgr->getBelongGroup(acctitemtype, m_pSHMFileExternal->m_poServAcct[k].m_iAcctItemGroup)) 
            {
                  return m_pSHMFileExternal->m_poServAcct[k].m_lAcctID;
            }
        }
        k = m_pSHMFileExternal->m_poServAcct[k].m_iNextOffset;
    }
    return 0;
}

long UserInfoInterface::getAcctID(long lServID, int acctitemtype, StdEvent * poEvent)
{
    unsigned int l;
    //只比较到月份
    if (!m_pSHMFileExternal->m_poServIndex->get(lServID, &l))
    {
        return 0;
    }
    
    char strBeginDate[16];
    char strEndDate[16];
    BillingCycle * cycle = m_poBillingCycleMgr->getBillingCycle(poEvent->m_iBillingCycleID);
    strcpy(strBeginDate,cycle->getStartDate());
    strcpy(strEndDate,cycle->getEndDate());
    
    unsigned int k = m_pSHMFileExternal->m_poServ[l].m_iServAcctOffset;
    while (k) 
    {
        if ( strcmp(m_pSHMFileExternal->m_poServAcct[k].m_sEffDate,strEndDate) < 0 &&
                strcmp(m_pSHMFileExternal->m_poServAcct[k].m_sExpDate,strBeginDate) > 0)
        {
            if( m_poAcctItemMgr->getBelongGroup(acctitemtype, m_pSHMFileExternal->m_poServAcct[k].m_iAcctItemGroup)) 
            {
                  return m_pSHMFileExternal->m_poServAcct[k].m_lAcctID;
            }
        }
        k = m_pSHMFileExternal->m_poServAcct[k].m_iNextOffset;
    }
#ifdef USERINFO_OFFSET_ALLOWED

    unsigned int m;

    m = m_pSHMFileExternal->m_poServ[l].m_iServAcctOffset;
    Date date_sBeginDate(strBeginDate);
    Date date_sEndDate(strEndDate);

    while(m)
    {
        Date date_EffDate(m_pSHMFileExternal->m_poServAcct[m].m_sEffDate);
        long p = date_EffDate.diffSec(date_sEndDate);

        if(p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs) 
        {
            if( m_poAcctItemMgr->getBelongGroup(acctitemtype, m_pSHMFileExternal->m_poServAcct[m].m_iAcctItemGroup)) 
            {
                  return m_pSHMFileExternal->m_poServAcct[m].m_lAcctID;
            }
        }else
        {
            Date date_ExpDate(m_pSHMFileExternal->m_poServAcct[m].m_sExpDate);
            p = date_sBeginDate.diffSec(date_ExpDate);
            if (p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs)
            {
                if( m_poAcctItemMgr->getBelongGroup(acctitemtype, m_pSHMFileExternal->m_poServAcct[m].m_iAcctItemGroup)) 
                {
                      return m_pSHMFileExternal->m_poServAcct[m].m_lAcctID;
                }
            }
        }
        m = m_pSHMFileExternal->m_poServAcct[m].m_iNextOffset;
    }
#endif

    return 0;
}

//##ModelId=42414DD400F5
int Serv::getOffers(int  offers[MAX_OFFERS], char *sdate)
{
	if(!m_poServInfo){
		return 0;
	}
    unsigned int k = m_poServInfo->m_iOfferInstanceOffset;
    int temp[MAX_OFFERS];//##输出要求从高层次－》低层次

    int i = 0;

    if (!sdate) sdate = m_sTime;

    while (k && i<MAX_OFFERS) {
        while (k) {
            if (strcmp(sdate, m_pSHMFileExternal->m_poOfferInstance[k].m_sEffDate) >= 0 &&
                strcmp(sdate, m_pSHMFileExternal->m_poOfferInstance[k].m_sExpDate) < 0) {

                temp[i] = m_pSHMFileExternal->m_poOfferInstance[k].m_iOfferID;
                i++;
                break;
            } else {
                k = m_pSHMFileExternal->m_poOfferInstance[k].m_iNextOffset;
            }
        }


        if (!k) break;

        k = m_pSHMFileExternal->m_poOfferInstance[k].m_iParentOffset;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    if ( i == 0 ) {

        k = m_poServInfo->m_iOfferInstanceOffset;
        
        Date date_sDate(sdate);

        while (k && i<MAX_OFFERS) {
            while (k) {
                Date date_EffDate(m_pSHMFileExternal->m_poOfferInstance[k].m_sEffDate);
                long p = date_EffDate.diffSec(date_sDate);
                if (p >= UserInfoReader::m_beginOffsetSecs
                    &&p <= UserInfoReader::m_offsetSecs) {

                    temp[i] = m_pSHMFileExternal->m_poOfferInstance[k].m_iOfferID;
                    i++;
                    break;
                } else {
                    Date date_ExpDate(m_pSHMFileExternal->m_poOfferInstance[k].m_sExpDate);
                    p = date_sDate.diffSec(date_ExpDate);
                    if (p >= UserInfoReader::m_beginOffsetSecs
                    &&p <= UserInfoReader::m_offsetSecs) {

                        temp[i] = m_pSHMFileExternal->m_poOfferInstance[k].m_iOfferID;
                        i++;
                        break;
                    }
                    k = m_pSHMFileExternal->m_poOfferInstance[k].m_iNextOffset;
                }
            }


            if (!k) break;

        k = m_pSHMFileExternal->m_poOfferInstance[k].m_iParentOffset;
    }
    }

    #endif

    int j = 0;
    for (j=0; j<i; j++) {
        offers[j] = temp[i-1-j];
    }
    return i;
}

//##ModelId=42414DF202E3
int Serv::getOfferInstances(long offerinstances[MAX_OFFERINSTANCES], char *sdate)
{
	if(!m_poServInfo){
		return 0;
	}
    unsigned int k = m_poServInfo->m_iOfferInstanceOffset;
    long temp[MAX_OFFERINSTANCES];

    int i = 0;

    if (!sdate) sdate = m_sTime;

    while (k && i<MAX_OFFERINSTANCES) {
        while (k) {
            if (strcmp(sdate, m_pSHMFileExternal->m_poOfferInstance[k].m_sEffDate) >= 0 &&
                strcmp(sdate, m_pSHMFileExternal->m_poOfferInstance[k].m_sExpDate) < 0) {
                temp[i] = m_pSHMFileExternal->m_poOfferInstance[k].m_lOfferInstanceID;
                i++;
                break;
            } else {
                k = m_pSHMFileExternal->m_poOfferInstance[k].m_iNextOffset;
            }
        }
        
        if (!k) break;

        k = m_pSHMFileExternal->m_poOfferInstance[k].m_iParentOffset;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    if ( i == 0 ) {

        k = m_poServInfo->m_iOfferInstanceOffset;
        
        Date date_sDate(sdate);

        while (k && i<MAX_OFFERINSTANCES) {
            while (k) {
                Date date_EffDate(m_pSHMFileExternal->m_poOfferInstance[k].m_sEffDate);
                long p = date_EffDate.diffSec(date_sDate);
                if (p >= UserInfoReader::m_beginOffsetSecs
                    &&p <= UserInfoReader::m_offsetSecs) {

                    temp[i] = m_pSHMFileExternal->m_poOfferInstance[k].m_lOfferInstanceID;
                    i++;
                    break;
                } else {
                    Date date_ExpDate(m_pSHMFileExternal->m_poOfferInstance[k].m_sExpDate);
                    p = date_sDate.diffSec(date_ExpDate);
                    if (p >= UserInfoReader::m_beginOffsetSecs
                    &&p <= UserInfoReader::m_offsetSecs) {
                        temp[i] = m_pSHMFileExternal->m_poOfferInstance[k].m_lOfferInstanceID;
                        i++;
                        break;
                    }
                    k = m_pSHMFileExternal->m_poOfferInstance[k].m_iNextOffset;
                }
            }


            if (!k) break;

        k = m_pSHMFileExternal->m_poOfferInstance[k].m_iParentOffset;
    }
    }

    #endif

    int j = 0;
    for (j=0; j<i; j++) {
        offerinstances[j] = temp[i-1-j];
    }

    return i;

}

bool UserInfoInterface::getServProduct(long lServProductID, ServProduct & servProduct)
{
    unsigned int k;

    if (!m_pSHMFileExternal->m_poServProductIndex->get (lServProductID, &k)) return false;

    servProduct.m_poServProductInfo = m_pSHMFileExternal->m_poServProduct + k;
    servProduct.m_poServInfo = m_pSHMFileExternal->m_poServ + m_pSHMFileExternal->m_poServProduct[k].m_iServOffset;
    return true;
}

bool Serv::getServProduct(long lServProductID, ServProduct & servProduct)
{
	if(!m_poServInfo){
		return false;
	}
    unsigned int k = m_poServInfo->m_iServProductOffset;

    ServProductInfo * pServProduct = *m_pSHMFileExternal->m_poServProductData;

    while (k) {
        if (pServProduct[k].m_lServProductID == lServProductID) {
            servProduct.m_poServProductInfo = pServProduct + k;
            servProduct.m_poServInfo = m_pSHMFileExternal->m_poServ + pServProduct[k].m_iServOffset;
            return true;
        }

        k = pServProduct[k].m_iNextOffset;
    }

    return false;
}
//##ModelId=42414E330142
bool Serv::getServProductIteration(ServProductIteration &servProductIteration, char *sdate, bool isall)
{
	if(!m_poServInfo){
		return false;
	}
    servProductIteration.m_bAll = isall;
    if (sdate)
        strcpy (servProductIteration.m_sTime, sdate);
    else
        strcpy (servProductIteration.m_sTime, m_sTime);
    
    servProductIteration.m_iCurOffset = m_poServInfo->m_iServProductOffset;

    return servProductIteration.m_iCurOffset ? true : false;
}

//##ModelId=4242BA380063
bool Serv::belongOrg(int orgid)
{
    int org = getOrgID ();
    if (org == -1) return false;

    return m_poOrgMgr->getBelong (org, orgid);    
}

bool ServGroupIteration::next(long & lGroupID)
{
    ServGroupBaseInfo * pGroupBase = (ServGroupBaseInfo *)(*m_pSHMFileExternal->m_poServGroupBaseData);

    if (m_iCurOffset) {
        lGroupID = pGroupBase[m_iCurOffset].m_lGroupID;
        m_iCurOffset++;

        if (m_iCurOffset == m_pSHMFileExternal->m_poServGroupBaseData->getCount ()+1) {
            m_iCurOffset = 0;
        }
        return true;
    }

    return false;
}

bool CustIteration::next(long & lCustID)
{
    while (m_iCurOffset) {
        if ((m_pSHMFileExternal->m_poCust[m_iCurOffset].m_sState[2]|0x20) == 'a') {
            lCustID = m_pSHMFileExternal->m_poCust[m_iCurOffset].m_lCustID;
            break;
        } else {
            m_iCurOffset++;
            if (m_iCurOffset == (m_pSHMFileExternal->m_poCustData->getCount ()+1)) {
                m_iCurOffset = 0;
            }
        }

    }

    if (m_iCurOffset) {
        m_iCurOffset++;
        if (m_iCurOffset == (m_pSHMFileExternal->m_poCustData->getCount ()+1)) {
            m_iCurOffset = 0;
        }
        return true;
    }

    return false;
}

//##ModelId=42414F0F010C
bool ServIteration::next(Serv &serv)
{
    if (!m_iCurOffset)
        return false;

    strcpy (serv.m_sTime, m_sTime);
    serv.m_poServInfo = m_pSHMFileExternal->m_poServ + m_iCurOffset;
    serv.m_poServProductInfo = m_pSHMFileExternal->m_poServProduct + m_pSHMFileExternal->m_poServ[m_iCurOffset].m_iServProductOffset;
    serv.m_poServAcctInfo = m_pSHMFileExternal->m_poServAcct + m_pSHMFileExternal->m_poServ[m_iCurOffset].m_iServAcctOffset;

    unsigned int k;
    if (m_pSHMFileExternal->m_poCustIndex->get(m_pSHMFileExternal->m_poServ[m_iCurOffset].m_lCustID, &k)) {
        serv.m_poCustInfo = m_pSHMFileExternal->m_poCust + k;
    } else {
        serv.m_poCustInfo = 0;
    }

    m_iCurOffset++;

    if (m_iCurOffset == m_pSHMFileExternal->m_poServData->getCount()+1)
        m_iCurOffset = 0;

    return true;
}

bool ServIteration::nextByOrg(Serv &serv)
{
    if (!m_iCurOffset) return false;

    ServLocationInfo *p = (ServLocationInfo *)(*m_pSHMFileExternal->m_poServLocationData);

    unsigned int l = (unsigned int)(p[m_iCurOffset].m_lServID);

    strcpy (serv.m_sTime, m_sTime);
    serv.m_poServInfo = m_pSHMFileExternal->m_poServ + l;
    serv.m_poServProductInfo = m_pSHMFileExternal->m_poServProduct + m_pSHMFileExternal->m_poServ[l].m_iServProductOffset;
    serv.m_poServAcctInfo = m_pSHMFileExternal->m_poServAcct + m_pSHMFileExternal->m_poServ[l].m_iServAcctOffset;

    unsigned int k;
    if (m_pSHMFileExternal->m_poCustIndex->get(m_pSHMFileExternal->m_poServ[l].m_lCustID, &k)) {
        serv.m_poCustInfo = m_pSHMFileExternal->m_poCust + k;
    } else {
        serv.m_poCustInfo = 0;
    }

    m_iLastOffset = m_iCurOffset;

    m_iCurOffset = p[m_iCurOffset].m_iOrgNext;

    return true;
}

bool ServIteration::belongCurServLocation(char *sTime)
{
    ServLocationInfo *p = (ServLocationInfo *)(*m_pSHMFileExternal->m_poServLocationData);

    p += m_iLastOffset;

    return (strcmp (sTime, p->m_sEffDate) >=0 &&
                strcmp (sTime, p->m_sExpDate) < 0);
}

//##ModelId=42414F1C02D8
bool ServProductIteration::next(ServProduct &servProduct)
{
    if (!m_iCurOffset)
        return false;

    if (!m_bAll) {
        while (m_iCurOffset) {
            if (strcmp(m_pSHMFileExternal->m_poServProduct[m_iCurOffset].m_sEffDate, m_sTime) > 0 ||
            strcmp(m_sTime, m_pSHMFileExternal->m_poServProduct[m_iCurOffset].m_sExpDate) >= 0) {
                
                m_iCurOffset = m_pSHMFileExternal->m_poServProduct[m_iCurOffset].m_iNextOffset;
            } else {
                break;
            }
        }
        
        if (!m_iCurOffset)
            return false;
    }

    servProduct.m_poServProductInfo = m_pSHMFileExternal->m_poServProduct+m_iCurOffset;
    servProduct.m_poServInfo = m_pSHMFileExternal->m_poServ+m_pSHMFileExternal->m_poServProduct[m_iCurOffset].m_iServOffset;
    m_iCurOffset = m_pSHMFileExternal->m_poServProduct[m_iCurOffset].m_iNextOffset;
    return true;
}


//##ModelId=42414DB202A5
bool UserInfoInterface::getServIteration(ServIteration &servIteration)
{
    unsigned int k = m_pSHMFileExternal->m_poServData->getCount();
    if (!k) return false;
    
    Date d;
    strcpy (servIteration.m_sTime, d.toString());

    servIteration.m_iCurOffset = 1;
    
    return true;
}

bool UserInfoInterface::getServIterByOrg(ServIteration &servIter, int iOrgID)
{
    unsigned int k;

    if (!m_pSHMFileExternal->m_poServLocationIndex->get (iOrgID, &k)) return false;

    servIter.m_iCurOffset = k;

    return true;
}

bool UserInfoInterface::getServGroupIteration(ServGroupIteration &groupIteration)
{
    unsigned int k = m_pSHMFileExternal->m_poServGroupBaseData->getCount();
    if (!k) return false;
    
    groupIteration.m_iCurOffset = 1;
    
    return true;

}

bool UserInfoInterface::getCustIteration(CustIteration &custIteration)
{
    unsigned int k = m_pSHMFileExternal->m_poCustData->getCount();
    if (!k) return false;
    
    custIteration.m_iCurOffset = 1;
    
    return true;

}

int UserInfoInterface::getAcctCreditAmount(long lAcctID)
{
    unsigned int k;
    
    if (!m_pSHMFileExternal->m_poAcctIndex->get (lAcctID, &k)) return 0;

    AcctInfo * pAcctInfo = *(m_pSHMFileExternal->m_poAcctData);
    return pAcctInfo[k].m_iCreditAmount;
}

int UserInfoInterface::getAcctCreditGrade(long lAcctID)
{
    unsigned int k;
    
    if (!m_pSHMFileExternal->m_poAcctIndex->get (lAcctID, &k)) return 0;

    AcctInfo * pAcctInfo = *(m_pSHMFileExternal->m_poAcctData);
    return pAcctInfo[k].m_iCreditGrade;
}

bool UserInfoInterface::getAcctBelongOrg(long lAcctID, int iOrgID)
{
    unsigned int k, l;

    if (!m_pSHMFileExternal->m_poServAcctIndex->get (lAcctID, &l)) return false;

    while (l) {
        if (m_pSHMFileExternal->m_poServIndex->get (m_pSHMFileExternal->m_poServAcct[l].m_lServID, &k)) {
            k = m_pSHMFileExternal->m_poServ[k].m_iLocationOffset;

            if (k) {
                return m_poOrgMgr->getBelong (m_pSHMFileExternal->m_poServLocation[k].m_iOrgID, iOrgID);
            }
        }

        l = m_pSHMFileExternal->m_poServAcct[l].m_iNextOffset;

    } 

    return false;
}

bool UserInfoInterface::getCustBelongOrg(long lCustID, int iOrgID)
{
    unsigned int k, l;

    if (!(m_pSHMFileExternal->m_poCustIndex->get(lCustID, &k))) return false;

        k = m_pSHMFileExternal->m_poCust[k].m_iServOffset;
        while (k) {
        l = m_pSHMFileExternal->m_poServ[k].m_iLocationOffset;

        if (l) {
            return m_poOrgMgr->getBelong (m_pSHMFileExternal->m_poServLocation[l].m_iOrgID, iOrgID);
        }

        k = m_pSHMFileExternal->m_poServ[k].m_iCustNext; 
        }

    return false;
}

bool UserInfoInterface::getServGroupBelongOrg(int iServGroupID, int iOrgID)
{
    unsigned int k, l;

        if (!m_pSHMFileExternal->m_poServGroupBaseIndex->get (iServGroupID, &k)) return false;

        ServGroupBaseInfo * pGroupBase = (ServGroupBaseInfo *)(*m_pSHMFileExternal->m_poServGroupBaseData);
        ServGroupInfo * pGroup = (ServGroupInfo *)(*m_pSHMFileExternal->m_poServGroupData);

        l = pGroupBase[k].m_iServOffset;

        while (l) {
        if (m_pSHMFileExternal->m_poServIndex->get (pGroup[l].m_lServID, &k)) {
                        k = m_pSHMFileExternal->m_poServ[k].m_iLocationOffset;

                        if (k) {
                                return m_poOrgMgr->getBelong (m_pSHMFileExternal->m_poServLocation[k].m_iOrgID, iOrgID);
                        }
                }

                l = pGroup[l].m_iGroupNext;
        }

        return false;
}

bool UserInfoInterface::getOfferInsIteration(OfferInsIteration &offerinsiter)
{
    unsigned int k = m_pSHMFileExternal->m_poProdOfferInsData->getCount();
    if (!k) return false;
    

    offerinsiter.m_iCurOffset = 1;
    
    return true;
}

//##ModelId=4248BC6B013E
bool Serv::belongServGroup(long groupid, char *sdate)
{
	if(!m_poServInfo){
		return false;
	}
    unsigned int k = m_poServInfo->m_iGroupOffset;

    if (!sdate) sdate = m_sTime;

    while (k) {
        if (groupid == m_pSHMFileExternal->m_poServGroup[k].m_lGroupID) {
            if (strcmp(sdate, m_pSHMFileExternal->m_poServGroup[k].m_sEffDate) >=0 &&
                strcmp(sdate, m_pSHMFileExternal->m_poServGroup[k].m_sExpDate) < 0)
                return true;
        } 

        if (groupid < m_pSHMFileExternal->m_poServGroup[k].m_lGroupID)
            break; //##sorted by m_lGroupID

        k = m_pSHMFileExternal->m_poServGroup[k].m_iNextOffset;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iGroupOffset;

    Date date_sDate(sdate);

    while (k) {
        if (groupid == m_pSHMFileExternal->m_poServGroup[k].m_lGroupID) {
            Date date_EffDate(m_pSHMFileExternal->m_poServGroup[k].m_sEffDate);
            long p = date_EffDate.diffSec(date_sDate);
            if (p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs)
            {
                return true;
            }
            else {
                Date date_ExpDate(m_pSHMFileExternal->m_poServGroup[k].m_sExpDate);
                p = date_sDate.diffSec(date_ExpDate);
                if (p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs)
                {
                    return true;
                }
            }
        } 

        if (groupid < m_pSHMFileExternal->m_poServGroup[k].m_lGroupID)
            break; //##sorted by m_lGroupID

        k = m_pSHMFileExternal->m_poServGroup[k].m_iNextOffset;
    }

    #endif

    return false;
}

//##ModelId=4248BC7600EA
bool Serv::belongCustGroup(long groupid)
{
	if(!m_poCustInfo){
		return false;
	}
    unsigned int k = m_poCustInfo->m_iGroupOffset;
    while (k) {
        if (groupid == m_pSHMFileExternal->m_poCustGroup[k].m_lGroupID) {
            /*if (strcmp(m_sTime, m_poCustGroup[k].m_sEffDate) >=0 &&
                strcmp(m_sTime, m_poCustGroup[k].m_sExpDate) < 0)*/
                return true;
        } 

        if (groupid < m_pSHMFileExternal->m_poCustGroup[k].m_lGroupID)
            return false; //##sorted by m_lGroupID

        k = m_pSHMFileExternal->m_poCustGroup[k].m_iNextOffset;
    }

    return false;
}


//##ModelId=424A47DC01D0
BillingCycleMgr * UserInfoReader::m_poBillingCycleMgr = 0;

//##ModelId=424A47DC01EE
AcctItemMgr * UserInfoReader::m_poAcctItemMgr = 0;

//##ModelId=424A47DC0202
OrgMgr * UserInfoReader::m_poOrgMgr = 0;

StateConvertMgr * UserInfoReader::m_poStateConvertMgr = 0;

NetworkPriorityMgr * UserInfoReader::m_poNetworkMgr = 0;
    
OfferMgr * UserInfoReader::m_poOfferMgr = 0;

//##ModelId=42525763028B
UserInfoReader::UserInfoReader() : SHMFileInfo ()
{
    if (!m_poBillingCycleMgr) {
        m_poBillingCycleMgr = new BillingCycleMgr ();
        if (!m_poBillingCycleMgr) THROW(MBC_UserInfoReader+2);
    }

    if (!m_poAcctItemMgr) {
        m_poAcctItemMgr = new AcctItemMgr ();
        if (!m_poAcctItemMgr) THROW(MBC_UserInfoReader+2);
    }

    if (!m_poOrgMgr) {
        m_poOrgMgr = new OrgMgr ();
        if (!m_poOrgMgr) THROW(MBC_UserInfoReader+2);
    }

    if (!m_poStateConvertMgr) {
        m_poStateConvertMgr = new StateConvertMgr ();
        if (!m_poStateConvertMgr) THROW(MBC_UserInfoReader+2);
    }
    if(!m_poNetworkMgr) {
        m_poNetworkMgr = new NetworkPriorityMgr();
        if (!m_poNetworkMgr) THROW(MBC_UserInfoReader+2);
#ifdef PRIVATE_MEMORY
        m_poNetworkMgr->load();
#endif
    }
    if(!m_poOfferMgr){
		m_poOfferMgr = new OfferMgr;
		if(!m_poOfferMgr)
        {
            THROW(MBC_UserInfoReader+2);
        }
        m_poOfferMgr->load();
        m_poOfferMgr->loadSpecialAttr();
	}
	  	
    #ifdef USERINFO_OFFSET_ALLOWED
    if(m_offsetSecs==-1){
      loadOffsetSecs();
    }
    #endif
}

UserInfoReader::UserInfoReader(int iMode) : SHMFileInfo ()
{
	if(iMode == USER_INFO_BASE)
	{
		if(!m_poNetworkMgr) {
			m_poNetworkMgr = new NetworkPriorityMgr();
			if (!m_poNetworkMgr) THROW(MBC_UserInfoReader+2);
#ifdef PRIVATE_MEMORY
			m_poNetworkMgr->load();
#endif
		}
		if(!m_poOfferMgr){
			m_poOfferMgr = new OfferMgr;
			if(!m_poOfferMgr)
			{
				THROW(MBC_UserInfoReader+2);
			}
			m_poOfferMgr->load();
			m_poOfferMgr->loadSpecialAttr();
		}
	}
	if(iMode == USRE_INFO_NOITEMGROUP)
	{

    	if (!m_poBillingCycleMgr) {
 	       m_poBillingCycleMgr = new BillingCycleMgr ();
   	     if (!m_poBillingCycleMgr) THROW(MBC_UserInfoReader+2);
	    }

	    if (!m_poAcctItemMgr) {
	        m_poAcctItemMgr = new AcctItemMgr (USRE_INFO_NOITEMGROUP);
	        if (!m_poAcctItemMgr) THROW(MBC_UserInfoReader+2);
	    }

	    if (!m_poOrgMgr) {
 	       m_poOrgMgr = new OrgMgr ();
	        if (!m_poOrgMgr) THROW(MBC_UserInfoReader+2);
	    }

 	   if (!m_poStateConvertMgr) {
  	      m_poStateConvertMgr = new StateConvertMgr ();
 	       if (!m_poStateConvertMgr) THROW(MBC_UserInfoReader+2);
	    }
 	   if(!m_poNetworkMgr) {
 	       m_poNetworkMgr = new NetworkPriorityMgr();
 	       if (!m_poNetworkMgr) THROW(MBC_UserInfoReader+2);
 	       m_poNetworkMgr->load();
	    }
 	   if(!m_poOfferMgr){
 	   		m_poOfferMgr = new OfferMgr;
 	   		if(!m_poOfferMgr)
 	   		{
 	   		THROW(MBC_UserInfoReader+2);
 	   		}
				m_poOfferMgr->load();
 	   		m_poOfferMgr->loadSpecialAttr();
 	   }
	}
    #ifdef USERINFO_OFFSET_ALLOWED
    if(m_offsetSecs==-1){
      loadOffsetSecs();
    }
    #endif
}

#ifdef USERINFO_OFFSET_ALLOWED

int UserInfoReader::m_offsetSecs =  -1;

int UserInfoReader::m_beginOffsetSecs = 0;

void UserInfoReader::loadOffsetSecs()
{
    char *pt;

    if ((pt = getenv ("PROCESS_ID")) == NULL) {
        m_offsetSecs = 0;
        return;
    }
    int temp_proc = atoi (pt);
    DEFINE_QUERY(qry);
    char sql[128];
    sprintf(sql,"select offset_mins*60 from b_cfg_userinfo_offset a,wf_process b where b.process_id = %d and b.billflow_id = a.billflow_id", temp_proc);
    qry.setSQL (sql);
    qry.open ();

    if(qry.next ()) {
        m_offsetSecs= qry.field(0).asInteger () ;
    } else {
        m_offsetSecs = 0;

        }
    
    qry.close ();
}
#endif

long UserInfoReader::execOperation(int iOperation,StdEvent * poEvent,int iAttrID)
{
	Value value;
	G_CURATTRID = iAttrID;
	Process::m_poOperationMgr->execute (iOperation, poEvent);
	IDParser::getValue ((EventSection*) poEvent, "51900", &value);
	return value.m_lValue;
}


char * Serv::getAttr(int attrid, char *sDate)
{
	if(!m_poServInfo){
		return 0;
	}
    unsigned int k = m_poServInfo->m_iAttrOffset;

    ServAttrInfo *pServAttr = (ServAttrInfo *)(*m_pSHMFileExternal->m_poServAttrData);
    while (k && pServAttr[k].m_iAttrID < attrid ) {
        k = pServAttr[k].m_iNextOffset;
    }

    if (!k || !(pServAttr[k].m_iAttrID==attrid)) return 0;

    
    if (!sDate) sDate = m_sTime;

    while (k && (pServAttr[k].m_iAttrID==attrid)) {

        if ( strcmp(pServAttr[k].m_sEffDate, sDate) <=0 &&
            strcmp(pServAttr[k].m_sExpDate, sDate) > 0 ) {
            return pServAttr[k].m_sValue;    
        }

        k = pServAttr[k].m_iNextOffset;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iAttrOffset;

    while (k && pServAttr[k].m_iAttrID < attrid ) {
        k = pServAttr[k].m_iNextOffset;
    }

    Date date_sDate(sDate);

    while (k && (pServAttr[k].m_iAttrID==attrid)) {
        
        Date date_EffDate(pServAttr[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs  ) {
            return pServAttr[k].m_sValue;
        }
        else {
            Date date_ExpDate(pServAttr[k].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
            if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs  ) {
                return pServAttr[k].m_sValue;
            }
        }
        k = pServAttr[k].m_iNextOffset;
    }

    #endif


    return 0;
}

char * Serv::getAttrNew(int attrid, char * pDate, char * m_sValue, int iSize)
{
	if(!m_poServInfo){
		return 0;
	}
	char sDate[16]={0};
	unsigned int k = m_poServInfo->m_iAttrOffset;

	if((pDate)&&(!pDate[0]))
	{
		pDate =m_sTime;
	}

	ServAttrInfo *pServAttr = (ServAttrInfo *)(*m_pSHMFileExternal->m_poServAttrData);
	while (k && pServAttr[k].m_iAttrID < attrid ) {
		k = pServAttr[k].m_iNextOffset;
	}

	if (!pDate) {  // sDate为空表示获取最新的属性值
		while (k && (pServAttr[k].m_iAttrID==attrid)) {
			if(strcmp(pServAttr[k].m_sEffDate, sDate) >= 0) {
				strcpy(sDate, pServAttr[k].m_sEffDate);
				strncpy(m_sValue, pServAttr[k].m_sValue, iSize);
			}

			k = pServAttr[k].m_iNextOffset;
		}
	}
	else {  // sDate不为空表示获取系统当前时间/话单时间的属性值
		while (k && (pServAttr[k].m_iAttrID==attrid)) {
			if ( strcmp(pServAttr[k].m_sEffDate, pDate) <=0 &&
				strcmp(pServAttr[k].m_sExpDate, pDate) > 0 ) {
					strncpy(m_sValue, pServAttr[k].m_sValue, iSize);
					return m_sValue;
			}

			k = pServAttr[k].m_iNextOffset;
		}
	}

	return m_sValue;
}


//##ModelId=42414E99010D
char * ServProduct::getAttr(int attrid, char *sDate)
{
    unsigned int k = m_poServProductInfo->m_iAttrOffset;

    ServProductAttrInfo *pServProductAttr = (ServProductAttrInfo *)(*m_pSHMFileExternal->m_poServProductAttrData);
    while (k && pServProductAttr[k].m_iAttrID < attrid ) {
        k = pServProductAttr[k].m_iNextOffset;
    }

    if (!k || !(pServProductAttr[k].m_iAttrID==attrid)) return 0;

    while (k && (pServProductAttr[k].m_iAttrID==attrid) ) { 
        if (sDate) {
            if (strcmp(pServProductAttr[k].m_sEffDate, sDate) <=0 &&
                strcmp(pServProductAttr[k].m_sExpDate, sDate) > 0 ) {
                return pServProductAttr[k].m_sValue;
            }
        } else {
            return pServProductAttr[k].m_sValue;
        }

        k = pServProductAttr[k].m_iNextOffset;
    } 

    #ifdef USERINFO_OFFSET_ALLOWED

    if (sDate) return 0;

    k = m_poServProductInfo->m_iAttrOffset;

    while (k && pServProductAttr[k].m_iAttrID < attrid ) {
        k = pServProductAttr[k].m_iNextOffset;
    }

    Date date_sDate(sDate);
    while (k && (pServProductAttr[k].m_iAttrID==attrid) ) { 
        Date date_EffDate(pServProductAttr[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if (p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs ) {
            return pServProductAttr[k].m_sValue;
        }
        else{
            Date date_ExpDate(pServProductAttr[k].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
            if (p >= UserInfoReader::m_beginOffsetSecs
                    &&p <= UserInfoReader::m_offsetSecs ) {
                return pServProductAttr[k].m_sValue;
            }
        }

        k = pServProductAttr[k].m_iNextOffset;
    } 

    #endif

    return 0;
}

bool Serv::isAttrValue(int attrid, char *sValue, char *sDate)
{
	if(!m_poServInfo){
		return false;
	}
        unsigned int k = m_poServInfo->m_iAttrOffset;

        ServAttrInfo *pServAttr = (ServAttrInfo *)(*m_pSHMFileExternal->m_poServAttrData);
        while (k && pServAttr[k].m_iAttrID < attrid ) {
                k = pServAttr[k].m_iNextOffset;
        }

        if (!k || !(pServAttr[k].m_iAttrID==attrid)) return false;

        if (!sDate) sDate = m_sTime;

        while (k && (pServAttr[k].m_iAttrID==attrid)) {

                if ( strcmp(pServAttr[k].m_sEffDate, sDate) <=0 &&
                        strcmp(pServAttr[k].m_sExpDate, sDate) > 0 &&
                        !strcmp(pServAttr[k].m_sValue, sValue) ) {

                        return true;
                }

                k = pServAttr[k].m_iNextOffset;
        }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iAttrOffset;

    while (k && pServAttr[k].m_iAttrID < attrid ) {
                k = pServAttr[k].m_iNextOffset;
        }

    Date date_sDate(sDate);

    while (k && (pServAttr[k].m_iAttrID==attrid)) {

        Date date_EffDate(pServAttr[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
    if ( p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs &&
                      !strcmp(pServAttr[k].m_sValue, sValue) ) {

                      return true;
     }
     else{
         Date date_ExpDate(pServAttr[k].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
           if ( p >= UserInfoReader::m_beginOffsetSecs
                    &&p <= UserInfoReader::m_offsetSecs &&
                  !strcmp(pServAttr[k].m_sValue, sValue) ) {

                      return true;
         }
     }
             

              k = pServAttr[k].m_iNextOffset;
        }

    #endif

        return false;
}

bool Serv::isAttrValueMax(int attrid, char *sValue, char *sDate)
{
	if(!m_poServInfo){
		return false;
	}
        unsigned int k = m_poServInfo->m_iAttrOffset;

        ServAttrInfo *pServAttr = (ServAttrInfo *)(*m_pSHMFileExternal->m_poServAttrData);
        while (k && pServAttr[k].m_iAttrID < attrid ) {
                k = pServAttr[k].m_iNextOffset;
        }

        if (!k || !(pServAttr[k].m_iAttrID==attrid)) return false;

        if (!sDate) sDate = m_sTime;

        while (k && (pServAttr[k].m_iAttrID==attrid)) {

                if ( strcmp(pServAttr[k].m_sEffDate, sDate) <=0 &&
                        strcmp(pServAttr[k].m_sExpDate, sDate) > 0 ) {

                        char *p = pServAttr[k].m_sValue;
                        char *t = sValue;

                        while (*p) {
                            if (*t != *p) return false;
                            p++; t++;
                        }

                        return true;
                }

                k = pServAttr[k].m_iNextOffset;
        }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iAttrOffset;

    while (k && pServAttr[k].m_iAttrID < attrid ) {
                k = pServAttr[k].m_iNextOffset;
        }

    Date date_sDate(sDate);

    while (k && (pServAttr[k].m_iAttrID==attrid)) {

        Date date_EffDate(pServAttr[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
              if ( p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs ) {

            char *p = pServAttr[k].m_sValue;
                        char *t = sValue;

                        while (*p) {
                            if (*t != *p) return false;
                            p++; t++;
                        }

                     return true;
    }
    else{
        Date date_ExpDate(pServAttr[k].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
              if ( p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs ) {

            char *p = pServAttr[k].m_sValue;
                        char *t = sValue;

                        while (*p) {
                            if (*t != *p) return false;
                            p++; t++;
                        }

                     return true;
        }
      }

              k = pServAttr[k].m_iNextOffset;
        }

    #endif

        return false;
}

int Serv::getAttrNumber(int attrid, char *sDate)
{
	if(!m_poServInfo){
		return 0;
	}
        unsigned int k = m_poServInfo->m_iAttrOffset;

        ServAttrInfo *pServAttr = (ServAttrInfo *)(*m_pSHMFileExternal->m_poServAttrData);
        while (k && pServAttr[k].m_iAttrID < attrid ) {
                k = pServAttr[k].m_iNextOffset;
        }

        if (!k || !(pServAttr[k].m_iAttrID==attrid)) return 0;

        if (!sDate) sDate = m_sTime;

        int iRet (0);

        while (k && (pServAttr[k].m_iAttrID==attrid)) {

                if ( strcmp(pServAttr[k].m_sEffDate, sDate) <=0 &&
                        strcmp(pServAttr[k].m_sExpDate, sDate) > 0 ) {

                        iRet++;

                }

                k = pServAttr[k].m_iNextOffset;
        }

    #ifdef USERINFO_OFFSET_ALLOWED

    if ( iRet == 0 ) {
        k = m_poServInfo->m_iAttrOffset;
        while (k && pServAttr[k].m_iAttrID < attrid ) {
                    k = pServAttr[k].m_iNextOffset;
            }

        Date date_sDate(sDate);

        while (k && (pServAttr[k].m_iAttrID==attrid)) {

            Date date_EffDate(pServAttr[k].m_sEffDate);
            long p = date_EffDate.diffSec(date_sDate);
      if ( p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs ) {

                iRet++;
       }
       else {
            Date date_ExpDate(pServAttr[k].m_sExpDate);
               p = date_sDate.diffSec(date_ExpDate);
         if ( p >= UserInfoReader::m_beginOffsetSecs
                    &&p <= UserInfoReader::m_offsetSecs ) {
                    iRet++;
               }
       }

                  k = pServAttr[k].m_iNextOffset;
            }

    }
    #endif

        return iRet;
}

//##ModelId=42578E4F0129
int &UserInfoAttr::getAttrID()
{
    return m_iAttrID;
}

//##ModelId=42578E570383
char *UserInfoAttr::getAttrValue()
{
    return m_sAttrValue;
}

//##ModelId=42578E60015F
char *UserInfoAttr::getEffDate()
{
    return m_sEffDate;
}

//##ModelId=42578E6603B7
char *UserInfoAttr::getExpDate()
{
    return m_sExpDate;
}

int Serv::getAttrs(vector<UserInfoAttr> &ret)
{
	if(!m_poServInfo){
		return 0;
	}
        unsigned int k = m_poServInfo->m_iAttrOffset;
        int i = 0;

        UserInfoAttr temp;

        ServAttrInfo *pServAttr = (ServAttrInfo *)(*m_pSHMFileExternal->m_poServAttrData);

        while (k) {
                temp.getAttrID() = pServAttr[k].m_iAttrID;
                strcpy (temp.getAttrValue(), pServAttr[k].m_sValue);
                strcpy (temp.getEffDate(), pServAttr[k].m_sEffDate);
                strcpy (temp.getExpDate(), pServAttr[k].m_sExpDate);

                ret.push_back (temp);
                i++;

                k = pServAttr[k].m_iNextOffset;
        }

        return i;
}

//##ModelId=42578D9C03AC
int ServProduct::getAttrs(vector<UserInfoAttr> &ret)
{
    unsigned int k = m_poServProductInfo->m_iAttrOffset;
    int i = 0;

    UserInfoAttr temp;

    ServProductAttrInfo *pServProductAttr = (ServProductAttrInfo *)(*m_pSHMFileExternal->m_poServProductAttrData);

    while (k) {
        temp.getAttrID() = pServProductAttr[k].m_iAttrID;
        strcpy (temp.getAttrValue(), pServProductAttr[k].m_sValue);
        strcpy (temp.getEffDate(), pServProductAttr[k].m_sEffDate);
        strcpy (temp.getExpDate(), pServProductAttr[k].m_sExpDate);
                
        ret.push_back (temp);
        i++;

        k = pServProductAttr[k].m_iNextOffset;
    }

    return i;
}

//##ModelId=425A2BD10371
char *Serv::getRentDate()
{
	if(m_poServInfo){
		return m_poServInfo->m_sRentDate;
	}
	
	return 0;
}

//##ModelId=425A2BD902E6
char *Serv::getCreateDate()
{
	if(m_poServInfo){
		return m_poServInfo->m_sCreateDate;
	}
	
	return 0;
}

//##ModelId=425A2BEE0264
char *Serv::getNscDate()
{
	if(m_poServInfo){
		return m_poServInfo->m_sNscDate;
	}
	
	return 0;
}

//##ModelId=425A2BFD0374
char *Serv::getStateDate(char * sDate)
{
    char *sTemp;

    if (!sDate) {
        sTemp = m_sTime;
    } else {
        sTemp = sDate;
    }
	if(!m_poServInfo){
		return 0;
	}

/*
    昆明这里SERV表里面的STATE_DATE不是最后的状态变更时间，而是用户资料的变更时间
    2005-09-24 xueqt

    从模型上来看,应该从SERV_STATE_ATTR里面取就足够了

    if (strcmp(sTemp, m_poServInfo->m_sStateDate)>=0) {
        return m_poServInfo->m_sStateDate;
    }
*/

    unsigned k = m_poServInfo->m_iStateOffset;
    
  /*
    while (k && strcmp(sTemp, m_poServState[k].m_sEffDate)<0) {
        k = m_poServState[k].m_iNextOffset;
    }

    if (k) {
                return m_poServState[k].m_sEffDate;
        }
   */
   
    while(k) {
      if(strcmp(m_pSHMFileExternal->m_poServState[k].m_sEffDate,sTemp)<=0 && strcmp(m_pSHMFileExternal->m_poServState[k].m_sExpDate, sTemp)>0) {
        return m_pSHMFileExternal->m_poServState[k].m_sEffDate;
      }

      k = m_pSHMFileExternal->m_poServState[k].m_iNextOffset;
    }
       
    return 0; 
}

//##ModelId=426F002B014E
ServInfo *Serv::getServInfo()
{
    return m_poServInfo;
}

//##ModelId=4271EA030243
ServProductInfo *ServProduct::getServProductInfo()
{
    return m_poServProductInfo;
}

 //##ModelId=4271EA3D0051
CustInfo * Serv::getCustInfo()
{
    return m_poCustInfo;
}

void Serv::setTime(char *sDate)
{
    strcpy(m_sTime, sDate);
}

int Serv::getFreeType()
{
	if(m_poServInfo){
		return m_poServInfo->m_iFreeType;
	}
	
	return 0;
}


bool Serv::isServAttrChanged(const char *sBeginTime,const char *sEndTime)
{	
	unsigned int k = m_poServInfo->m_iAttrOffset;
	ServAttrInfo * pServAttr = (ServAttrInfo *)(*m_pSHMFileExternal->m_poServAttrData);
	
	while(k) {
		if( ( strcmp(pServAttr[k].m_sEffDate, sBeginTime) >= 0 
			   && strcmp(pServAttr[k].m_sEffDate, sEndTime) <= 0 )
			||(strcmp(pServAttr[k].m_sExpDate, sBeginTime) >= 0
			   && strcmp(pServAttr[k].m_sExpDate, sEndTime) <= 0)
			) {
		  	return true;
		 }
		
		k = pServAttr[k].m_iNextOffset;
	}
	
	return false;
}


bool Serv::isServStateAttrChanged(const char *sBeginTime,const char *sEndTime)
{
	unsigned int k = m_poServInfo->m_iStateOffset;

	while(k) {
		if( (strcmp(m_pSHMFileExternal->m_poServState[k].m_sEffDate, sBeginTime) >= 0 
			     && strcmp(m_pSHMFileExternal->m_poServState[k].m_sEffDate, sEndTime) <= 0 ) ||
			   (strcmp(m_pSHMFileExternal->m_poServState[k].m_sExpDate, sBeginTime) >= 0 
			    && strcmp(m_pSHMFileExternal->m_poServState[k].m_sExpDate, sEndTime) <= 0 )
		  ) {
			  return true;
     	}
		
		k = m_pSHMFileExternal->m_poServState[k].m_iNextOffset;	
   }
   
   return false;
}

bool Serv::isServTypeChanged(const char *sBeginTime,const char *sEndTime)
{
	int j;
	unsigned int k = m_poServInfo->m_iServTypeOffset;
	
	if(!g_bLoadGenCycleFee) loadGenCycleFeeIndex();
		
	ServTypeInfo * pServType = (ServTypeInfo *)(*m_pSHMFileExternal->m_poServTypeData);
	
	while(k) {
		if( m_poGenCycleFeeIndex.get(pServType[k].m_iProductID,&j) ) {
			
			if( ( strcmp(pServType[k].m_sEffDate, sBeginTime) >= 0
				 && strcmp(pServType[k].m_sEffDate, sEndTime) <= 0 ) || 
			   ( strcmp(pServType[k].m_sExpDate, sBeginTime) >= 0
				 && strcmp(pServType[k].m_sExpDate, sEndTime) <= 0 )
			   ) {
				return true;
			 }
		}
		
		k = pServType[k].m_iNextOffset;
	}

	return false;
}

bool Serv::isServProductChanged(const char *sBeginTime,const char *sEndTime)
{
	unsigned int k = m_poServInfo->m_iServProductOffset;
	
	ServProductInfo * pServProduct= (ServProductInfo *)(*m_pSHMFileExternal->m_poServProductData);

  int j;
	unsigned int i;
	  
	if(!g_bLoadGenCycleFee) loadGenCycleFeeIndex();
    	
	while(k) {
		if( m_poGenCycleFeeIndex.get(pServProduct[k].m_iProductID,&j) ) {		  
			if( ( strcmp(pServProduct[k].m_sEffDate, sBeginTime) >= 0
				   && strcmp(pServProduct[k].m_sEffDate, sEndTime) <= 0 ) || 
				  ( strcmp(pServProduct[k].m_sExpDate, sBeginTime) >= 0
				   && strcmp(pServProduct[k].m_sExpDate, sEndTime) <= 0 )
				  ) {
				      return true;
			   }
		}
	  
		k = pServProduct[k].m_iNextOffset;
	}

	return false;
}


bool Serv::isServProductAttrChanged(const char *sBeginTime,const char *sEndTime)
{
	int j;
	unsigned int l;
	
	unsigned int k = m_poServInfo->m_iServProductOffset;
	ServProductInfo * pServProduct= (ServProductInfo *)(*m_pSHMFileExternal->m_poServProductData);
	ServProductAttrInfo * pServProductAttr = (ServProductAttrInfo *)(*m_pSHMFileExternal->m_poServProductAttrData);
	
	if(!g_bLoadGenCycleFee) loadGenCycleFeeIndex();
	
	while(k) {
      if(m_poGenCycleFeeIndex.get(pServProduct[k].m_iProductID,&j)) {
		    l = pServProduct[k].m_iAttrOffset;
		    while(l) {
			    if( (strcmp(pServProductAttr[l].m_sEffDate,sBeginTime) >= 0 
				       && strcmp(pServProductAttr[l].m_sEffDate,sEndTime) <= 0)
				     ||(strcmp(pServProductAttr[l].m_sExpDate,sBeginTime) >= 0
				     && strcmp(pServProductAttr[l].m_sExpDate,sEndTime) <= 0)
				    ) {
				   return true;
			    } 
			    l = pServProductAttr[l].m_iNextOffset;
	    	}
		  }
		  
		k = pServProduct[k].m_iNextOffset;
	}
	
	return false;
}


bool Serv::isProdOffer_Ins_Attr_Detail_Changed(const char *sBeginTime,const char *sEndTime)
{
  vector<ProdOfferIns *> vOfferIns;
  vector<OfferDetailIns *> vDetailIns;
	
  int i;
  unsigned int k = m_poServInfo->m_iOfferDetailOffset;
	
  while(k) {
    vDetailIns.push_back(m_pSHMFileExternal->m_poOfferDetailIns + k);
    vOfferIns.push_back( m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[k].m_iParentOffset);

    k = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextProdOfferOffset;
  }
	
  for(i = 0; i< vOfferIns.size(); ++i) {
    if(m_pSHMFileExternal->m_poOfferDetailInsOfferInsIndex->get(vOfferIns[i]->m_lProdOfferInsID,&k)) {
      while (k) {
        vDetailIns.push_back(m_pSHMFileExternal->m_poOfferDetailIns + k);
        vOfferIns.push_back( m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[k].m_iParentOffset );
        k = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextProdOfferOffset;
      }
    }
  }
	
  k = m_poServInfo->m_iBaseOfferOffset;
  while(k){
    vDetailIns.push_back(m_pSHMFileExternal->m_poOfferDetailIns + k);
    vOfferIns.push_back(m_pSHMFileExternal->m_poProdOfferIns + m_pSHMFileExternal->m_poOfferDetailIns[k].m_iParentOffset);
    k = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextProdOfferOffset;
  }
	
  for(i = 0; i < vDetailIns.size(); ++i) {
 	  if( ( strcmp(vDetailIns[i]->m_sEffDate, sBeginTime) >= 0
         && strcmp(vDetailIns[i]->m_sEffDate, sEndTime) <= 0 ) || 
        ( strcmp(vDetailIns[i]->m_sExpDate, sBeginTime) >= 0
         && strcmp(vDetailIns[i]->m_sExpDate, sEndTime) <= 0 )
       ) {	
       return true;
     }
  }
		
	for(i = 0; i < vOfferIns.size(); ++i) {
		if( ( strcmp(vOfferIns[i]->m_sEffDate, sBeginTime) >= 0
			   && strcmp(vOfferIns[i]->m_sEffDate, sEndTime) <= 0 ) || 
			  ( strcmp(vOfferIns[i]->m_sExpDate, sBeginTime) >= 0
				 && strcmp(vOfferIns[i]->m_sExpDate, sEndTime) <= 0 )
			) {	
			  	return true;
		}
		
		k = vOfferIns[i]->m_iOfferAttrOffset;	
		while(k) {
			if( ( strcmp(m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sEffDate, sBeginTime) >= 0
				 && strcmp(m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sEffDate, sEndTime) <= 0 ) || 
			    ( strcmp(m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sExpDate, sBeginTime) >= 0
				 && strcmp(m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sExpDate, sEndTime) <= 0 )
			   ) {	
			  	return true;
			 }
			k =  m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iNextOfferInsAttrOffset;
		}
		
				//协议期判断
		k = vOfferIns[i]->m_iOfferAgreementOffset;
		while (k) {
		  if( ( strcmp(m_pSHMFileExternal->m_poProdOfferInsAgreement[k].m_sEffDate, sBeginTime) >= 0
		  	   && strcmp(m_pSHMFileExternal->m_poProdOfferInsAgreement[k].m_sEffDate, sEndTime) <= 0 ) || 
		  	  ( strcmp(m_pSHMFileExternal->m_poProdOfferInsAgreement[k].m_sExpDate, sBeginTime) >= 0
		  	  && strcmp(m_pSHMFileExternal->m_poProdOfferInsAgreement[k].m_sExpDate, sEndTime) <= 0 )
		   ) {	
		   	  return true;
		  }
		  k = m_pSHMFileExternal->m_poProdOfferInsAgreement[k].m_iNextOfferInsAgreementOffset;
		}
		
	}
	
	return false;	
}

bool Serv::isServChanged (const char *sBeginTime,const char *sEndTime)
{
	if(!sBeginTime || !sEndTime) return false;
		
	bool bRet = ( isServAttrChanged(sBeginTime, sEndTime) ||  
	             isServStateAttrChanged(sBeginTime, sEndTime) ||
	             isServTypeChanged(sBeginTime, sEndTime) || 
	             isServProductChanged(sBeginTime, sEndTime) ||
	             isServProductAttrChanged(sBeginTime, sEndTime) || 
	             isProdOffer_Ins_Attr_Detail_Changed(sBeginTime, sEndTime)
	            );
	return bRet;
}

int OfferInsQO::getOfferInsAttr(ProdOfferInsAttr ret[MAX_OFFER_ATTRS], char * sDate)
{
    if(!sDate) sDate = m_sTime;
    int iRet = 0;
    unsigned int k = m_poOfferIns->m_iOfferAttrOffset;
    
    for (; k; k=m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iNextOfferInsAttrOffset)
    {
        if(sDate &&
            (strcmp (sDate, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sEffDate)<0 ||
             strcmp (sDate, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sExpDate)>=0))
        {
            continue;
        }
        ret[iRet].m_iAttrID = m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iAttrID;
        strcpy(ret[iRet++].m_sAttrValue, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sAttrValue);
    }
    
    return iRet;
}

//add by zhaoziwei
bool OfferInsQO::getOfferInsAgreement(ProdOfferInsAgreement &ret,char * sDate)
{
    if(!sDate) sDate = m_sTime;
    
		unsigned int k = m_poOfferIns->m_iOfferAgreementOffset;
		
    for (; k; k=m_pSHMFileExternal->m_poProdOfferInsAgreement[k].m_iNextOfferInsAgreementOffset)
		{
			if(sDate &&
          (strcmp (sDate, m_pSHMFileExternal->m_poProdOfferInsAgreement[k].m_sEffDate)<0 ||
           strcmp (sDate, m_pSHMFileExternal->m_poProdOfferInsAgreement[k].m_sExpDate)>=0))
      {
          continue;
      }
			
			ret=m_pSHMFileExternal->m_poProdOfferInsAgreement[k];
			return true;
		}
    
    return false;
}
//end

int OfferInsQO::getOfferInsAttrAndDate(ProdOfferInsAttr ret[MAX_OFFER_ATTRS], char * sDate)
{
    if(!sDate) sDate = m_sTime;
    int iRet = 0;
    unsigned int k = m_poOfferIns->m_iOfferAttrOffset;
    
    for (; k; k=m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iNextOfferInsAttrOffset)
    {
        if(sDate &&
            (strcmp (sDate, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sEffDate)<0 ||
             strcmp (sDate, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sExpDate)>=0))
        {
            continue;
        }
        ret[iRet].m_iAttrID = m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iAttrID;
		strcpy(ret[iRet].m_sEffDate,m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sEffDate);
		strcpy(ret[iRet].m_sExpDate,m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sExpDate);
        strcpy(ret[iRet++].m_sAttrValue, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sAttrValue);
    }    
    return iRet;
}

int OfferInsQO::getOfferInsAttrCntByAttrID(int iAttrID,char* sDate,StdEvent * poEvent)
{
    if(!sDate) sDate = m_sTime;
    int iRet = 0;
	int iOperation;
	if(poEvent && m_poOfferMgr->checkSpecialAttr(iAttrID,iOperation))
	{
		return 1;
	}
	unsigned int k = m_poOfferIns->m_iOfferAttrOffset;
    
    for (; k; k=m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iNextOfferInsAttrOffset)
    {
        if (m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iAttrID < iAttrID)
            continue;
        
        if (m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iAttrID > iAttrID)
            break;
            
        if(sDate &&
            (strcmp (sDate, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sEffDate)<0 ||
             strcmp (sDate, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sExpDate)>=0))
        {
            continue;
        }
        iRet++;
    }
    
    return iRet;
}
int OfferInsQO::getOfferInsAttrByAttrID(ProdOfferInsAttr ret[MAX_OFFER_ATTRS], int iAttrID, char * sDate,StdEvent * poEvent)
{
    int iRet = 0;
	int iOperation;
	long lResult;
	if(poEvent && m_poOfferMgr->checkSpecialAttr(iAttrID,iOperation))
	{
		lResult = execOperation(iOperation,poEvent,iAttrID);
		ret[iRet].m_iAttrID = iAttrID;
		sprintf(ret[iRet].m_sAttrValue,"%ld",lResult);
		return 1;
	}
    if(!sDate) sDate = m_sTime;
	unsigned int k = m_poOfferIns->m_iOfferAttrOffset;

    for (; k; k=m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iNextOfferInsAttrOffset)
    {
        if (m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iAttrID < iAttrID)
            continue;
        
        if (m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iAttrID > iAttrID)
            break;
            
        if(sDate &&
            (strcmp (sDate, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sEffDate)<0 ||
             strcmp (sDate, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sExpDate)>=0))
        {
            continue;
        }
        ret[iRet].m_iAttrID = m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iAttrID;
        strcpy(ret[iRet++].m_sAttrValue, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sAttrValue);
    }
    
    return iRet;
}

char* OfferInsQO::getOfferAttr(int iAttrID, char *sDate,StdEvent * poEvent)
{
	int iOperation;
	long lResult;
	static char strVal[32];
	if(poEvent && m_poOfferMgr->checkSpecialAttr(iAttrID,iOperation))
	{
		lResult = execOperation(iOperation,poEvent,iAttrID);
		sprintf(strVal,"%ld",lResult);
		return strVal;
	}
	if(!sDate) sDate = m_sTime;
    unsigned int k = m_poOfferIns->m_iOfferAttrOffset;   
    for (; k; k=m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iNextOfferInsAttrOffset) {
        if (m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iAttrID < iAttrID)
            continue;
        
        if (m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iAttrID > iAttrID)
            break;

        if (strcmp (sDate, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sEffDate)<0)
            continue;

        if (strcmp (sDate, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sExpDate)>=0)
            continue;

        return m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sAttrValue;
    }
    
	  return m_poOfferMgr->getOfferAttrValue(m_poOfferIns->m_iOfferID,iAttrID,sDate);

    return 0;
}

bool OfferInsQO::isOfferAttrValue(int iAttrID, char* iAttrVal, char *sDate,StdEvent * poEvent)
{
	int iOperation;
	long lResult;
	char strVal[32];
	if(poEvent && m_poOfferMgr->checkSpecialAttr(iAttrID,iOperation))
	{
		lResult = execOperation(iOperation,poEvent,iAttrID);
		sprintf(strVal,"%ld",lResult);
		if(!strcmp(strVal,iAttrVal)){
			return true;
		}else{
			return false;
		}
	}
    if(!sDate) sDate = m_sTime;
    unsigned int k = m_poOfferIns->m_iOfferAttrOffset;   
    for (; k; k=m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iNextOfferInsAttrOffset) {
        if (m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iAttrID < iAttrID)
            continue;
        
        if (m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_iAttrID > iAttrID)
            break;

        if (strcmp (sDate, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sEffDate)<0)
            continue;

        if (strcmp (sDate, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sExpDate)>=0)
            continue;

        if (!strcmp (iAttrVal, m_pSHMFileExternal->m_poProdOfferInsAttr[k].m_sAttrValue))
        {
            return true;
        }
    }

    return false;
}

bool OfferInsQO::isOfferMember(int iMemberID, char *sDate)
{
    if(!sDate) sDate = m_sTime;
    unsigned int k = m_poOfferIns->m_iOfferDetailInsOffset;  
    for (; k; k=m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextDetailOffset) {
        if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate)<0)
            continue;

        if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate)>=0)
            continue;
            
        if(m_pSHMFileExternal->m_poOfferDetailIns[k].m_iMemberID == iMemberID)
        {
            return true;
        }
        if(m_pSHMFileExternal->m_poOfferDetailIns[k].m_iInsType == OFFER_INSTANCE_TYPE)
        {
            if(__isOfferMember(m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID, iMemberID, sDate))
            {
                return true;
            }
        }
    }
    return false;
}

int OfferInsQO::getOfferInsMember(OfferDetailIns * ret[MAX_OFFER_MEMBERS],
                            int iMemberID, char *sDate)
{
    if(!sDate) sDate = m_sTime;
    int iRet = 0;
    int iLastNum = 0;
    unsigned int p = 0;

    unsigned int k = m_poOfferIns->m_iOfferDetailInsOffset;  

    for (; k; k=m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextDetailOffset) {
        if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate)<0)
            continue;
    
        if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate)>=0)
            continue;

        if (iMemberID && m_pSHMFileExternal->m_poOfferDetailIns[k].m_iMemberID!=iMemberID)
            continue;

        ret[iRet++] = &(m_pSHMFileExternal->m_poOfferDetailIns[k]);

        if(iRet==MAX_OFFER_MEMBERS) {
            return iRet;
        }
    }
    
    for(; iLastNum<iRet; iLastNum++) {
        if(ret[iLastNum]->m_iInsType == OFFER_INSTANCE_TYPE) {
            if(m_pSHMFileExternal->m_poProdOfferInsIndex->get(ret[iLastNum]->m_lInsID, &p)) {
                k = m_pSHMFileExternal->m_poProdOfferIns[p].m_iOfferDetailInsOffset;
                for (; k; k=m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextDetailOffset) {
                    if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate)<0)
                        continue;
                
                    if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate)>=0)
                        continue;

                    ret[iRet++] = &(m_pSHMFileExternal->m_poOfferDetailIns[k]);

                    if(iRet==MAX_OFFER_MEMBERS) {
                        return iRet;
                    }
                }
            }
        }
    }

    return iRet;
}

OfferDetailIns * OfferInsQO::getOneMemberIns(int iMemberID, char *sDate)
{
    if(!sDate) sDate = m_sTime;
    int iRet = 0;
    int iLastNum = 0;
    unsigned int p = 0;

    unsigned int k = m_poOfferIns->m_iOfferDetailInsOffset;  
    for (; k; k=m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextDetailOffset) {
        if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate)<0)
            continue;
    
        if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate)>=0)
            continue;

        if (m_pSHMFileExternal->m_poOfferDetailIns[k].m_iMemberID != iMemberID) 
            continue;

        return &(m_pSHMFileExternal->m_poOfferDetailIns[k]);
    }
    
    return 0;
}

int OfferInsQO::getOfferInsMember(OfferDetailIns ret[MAX_OFFER_MEMBERS], 
char *sDate)
{
    if(!sDate) sDate = m_sTime;
    int iRet = 0;
    int iLastNum = 0;
    unsigned int p = 0;

    unsigned int k = m_poOfferIns->m_iOfferDetailInsOffset;  
    for (; k; k=m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextDetailOffset) {
        if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate)<0)
            continue;
    
        if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate)>=0)
            continue;
        ret[iRet].m_lInsID = m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID;
        ret[iRet].m_iInsType = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iInsType;
        ret[iRet++].m_iMemberID = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iMemberID;
        if(iRet==MAX_OFFER_MEMBERS)
        {
            return iRet;
        }
    }
    
    for(; iLastNum<iRet; iLastNum++)
    {
        if(ret[iLastNum].m_iInsType == OFFER_INSTANCE_TYPE)
        {
            if(m_pSHMFileExternal->m_poProdOfferInsIndex->get(ret[iLastNum].m_lInsID, &p))
            {
                k = m_pSHMFileExternal->m_poProdOfferIns[p].m_iOfferDetailInsOffset;
                for (; k; k=m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextDetailOffset) 
                {
                    if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate)<0)
                        continue;
                
                    if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate)>=0)
                        continue;
                    ret[iRet].m_lInsID = m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID;
                    ret[iRet].m_iInsType = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iInsType;
                    ret[iRet++].m_iMemberID = m_pSHMFileExternal->m_poOfferDetailIns[k].m_iMemberID;
                    if(iRet==MAX_OFFER_MEMBERS)
                    {
                        return iRet;
                    }
                }
            }
        }
    }
    return iRet;
}

bool OfferInsQO::isOfferInsMember(long lServID, long lAcctID, int iMemberID, char *sDate)
{
    unsigned int k = m_poOfferIns->m_iOfferDetailInsOffset;  
    for (; k; k=m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextDetailOffset) {
        if(m_pSHMFileExternal->m_poOfferDetailIns[k].m_iMemberID != iMemberID)
            continue;
        if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate)<0)
            continue;
        if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate)>=0)
            continue;
            
        if((m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID == lServID && 
            m_pSHMFileExternal->m_poOfferDetailIns[k].m_iInsType == SERV_INSTANCE_TYPE)||
            (m_pSHMFileExternal->m_poOfferDetailIns[k].m_iInsType == ACCT_TYPE && 
            m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID == lAcctID))
        {
            return true;
        }
        ////增加客户级的判断
        if( m_pSHMFileExternal->m_poOfferDetailIns[k].m_iInsType == CUST_TYPE ){
            Serv oServ;
            if(  G_PUSERINFO->getServ(oServ, lServID, sDate)
              && oServ.getCustID() == m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID){
                return true;
            }
         } 
////
        if(m_pSHMFileExternal->m_poOfferDetailIns[k].m_iInsType == OFFER_INSTANCE_TYPE)
        {
            if( __isOfferInsMember(m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID,
                         lServID, lAcctID, iMemberID, sDate))
            {
                return true;
            }
        }
    }
    return false;
}

bool OfferInsQO::__isOfferInsMember(long lProdOfferInsID, long lServID, long lAcctID, int iMemberID, char *sDate)
{
    unsigned int p = 0;
    
    if(!m_pSHMFileExternal->m_poProdOfferInsIndex->get(lProdOfferInsID, &p)) return false;
    unsigned int k = m_pSHMFileExternal->m_poProdOfferIns[p].m_iOfferDetailInsOffset;
    for (; k; k=m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextDetailOffset) {
        if(m_pSHMFileExternal->m_poOfferDetailIns[k].m_iMemberID != iMemberID)
            continue;
        if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate)<0)
            continue;
        if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate)>=0)
            continue;
            
        if((m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID == lServID && 
            m_pSHMFileExternal->m_poOfferDetailIns[k].m_iInsType == SERV_INSTANCE_TYPE)||
            (m_pSHMFileExternal->m_poOfferDetailIns[k].m_iInsType == ACCT_TYPE && 
            m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID == lAcctID))
        {
            return true;
        }
        if(m_pSHMFileExternal->m_poOfferDetailIns[k].m_iInsType == OFFER_INSTANCE_TYPE)
        {
            if( __isOfferInsMember(m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID,
                         lServID, lAcctID, iMemberID, sDate))
            {
                return true;
            }
        }
    }
    return false;
}

bool OfferInsQO::__isOfferMember(long lProdOfferInsID, int iMemberID, char *sDate)
{
    unsigned int p = 0;
    
    if(m_pSHMFileExternal->m_poProdOfferInsIndex->get(lProdOfferInsID, &p))
    {
        unsigned int k = m_pSHMFileExternal->m_poProdOfferIns[p].m_iOfferDetailInsOffset;  
        for (; k; k=m_pSHMFileExternal->m_poOfferDetailIns[k].m_iNextDetailOffset) 
        {
            if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sEffDate)<0)
                continue;
    
            if (strcmp (sDate, m_pSHMFileExternal->m_poOfferDetailIns[k].m_sExpDate)>=0)
                continue;
                
            if(m_pSHMFileExternal->m_poOfferDetailIns[k].m_iMemberID == iMemberID)
            {
                return true;
            }
            if(m_pSHMFileExternal->m_poOfferDetailIns[k].m_iInsType == OFFER_INSTANCE_TYPE)
            {
                if(__isOfferMember(m_pSHMFileExternal->m_poOfferDetailIns[k].m_lInsID, iMemberID, sDate))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

#ifdef GROUP_CONTROL
bool UserInfoInterface::getAccNbrOfferType(char* pAccNbr,char* pStartDate,AccNbrOfferType& oAccNbrOfferType)
{
	if((0==pAccNbr)||(0== pAccNbr[0]))
		return false;
	unsigned int k =0;

	AccNbrOfferType* pAccNbrOfferType = (AccNbrOfferType*)(*m_pSHMFileExternal->m_poAccNbrData);
	if(0 !=m_pSHMFileExternal->m_poAccNbrIndex)
	{
		if(!m_pSHMFileExternal->m_poAccNbrIndex->get(pAccNbr,&k))
		{
			return false;
		}else
		{
			while(k>0)
			{
				
				if((strcmp(pStartDate,pAccNbrOfferType[k].m_sEffDate)>=0)&&
					(strcmp(pStartDate,pAccNbrOfferType[k].m_sExpDate)<=0))
				{
					memcpy(&oAccNbrOfferType,&pAccNbrOfferType[k],sizeof(AccNbrOfferType));
					return true;
				}
				k = pAccNbrOfferType[k].m_iNextOffset;
			}
		}
	}
	return false;
}

bool UserInfoInterface::checkAccNbrOfferType(char * pAccNbr,char* pStartDate,int iOfferType)
{
	if((0==pAccNbr)||(0== pAccNbr[0]))
		return false;
	unsigned int k =0;

	AccNbrOfferType* pAccNbrOfferType = (AccNbrOfferType*)(*m_pSHMFileExternal->m_poAccNbrData);
	if(0 !=m_pSHMFileExternal->m_poAccNbrIndex)
	{
		if(!m_pSHMFileExternal->m_poAccNbrIndex->get(pAccNbr,&k))
		{
			return false;
		}else
		{
			while(k>0)
			{
				if((strcmp(pStartDate,pAccNbrOfferType[k].m_sEffDate)>=0)&&
					(strcmp(pStartDate,pAccNbrOfferType[k].m_sExpDate)<=0)&&
					(pAccNbrOfferType[k].m_iOfferType == iOfferType))
					return true;
				k = pAccNbrOfferType[k].m_iNextOffset;
			}
		}
	}
	return false;
}
#endif
ProdOfferIns * UserInfoInterface::getOfferIns(long lOfferInstID)
{
    unsigned int k;

    if (!m_pSHMFileExternal->m_poProdOfferInsIndex->get (lOfferInstID, &k)) return 0;

    return m_pSHMFileExternal->m_poProdOfferIns + k;
}

bool OfferInsIteration::next(ProdOfferIns &offerins)
{
    if(!m_iCurOffset) return false;
    
    offerins.m_lProdOfferInsID = m_pSHMFileExternal->m_poProdOfferIns[m_iCurOffset].m_lProdOfferInsID;
    offerins.m_iOfferDetailInsOffset = m_pSHMFileExternal->m_poProdOfferIns[m_iCurOffset].m_iOfferDetailInsOffset;
    offerins.m_iAccumuOffset = m_pSHMFileExternal->m_poProdOfferIns[m_iCurOffset].m_iAccumuOffset;
    offerins.m_iOfferAttrOffset = m_pSHMFileExternal->m_poProdOfferIns[m_iCurOffset].m_iOfferAttrOffset;
    offerins.m_iOfferID = m_pSHMFileExternal->m_poProdOfferIns[m_iCurOffset].m_iOfferID;
    strcpy(offerins.m_sEffDate, m_pSHMFileExternal->m_poProdOfferIns[m_iCurOffset].m_sEffDate);
    strcpy(offerins.m_sExpDate, m_pSHMFileExternal->m_poProdOfferIns[m_iCurOffset].m_sExpDate);
    
    m_iCurOffset++;
    if (m_iCurOffset == (m_pSHMFileExternal->m_poProdOfferInsData->getCount ()+1)) 
    {
        m_iCurOffset = 0;
    }
    return true;
}

bool UserInfoInterface::clearServ(Serv &cur)
{
	cur.m_poServInfo=0;
	cur.m_poCustInfo=0;
	cur.m_poServProductInfo=0;
	cur.m_poServAcctInfo=0;
    memset(cur.m_sTime, 0, sizeof(cur.m_sTime) );

	return true;
}

#ifdef GROUP_CONTROL
bool UserInfoInterface::getGroupInfoByID(GroupInfo &groupInfo,int iGroupID,char *sDate)
{
	bool bRet = false;
	unsigned int k;
	if(NULL == sDate)
	{
		Date d;
		sDate=d.toString();
	}
	if(m_pSHMFileExternal->m_poGroupInfoIndex->get(iGroupID,&k))
	{
		while(k)
		{
			if(strcmp(m_pSHMFileExternal->m_poGroupInfo[k].m_sEffDate,sDate)<=0
                && strcmp(m_pSHMFileExternal->m_poGroupInfo[k].m_sExpDate,sDate)>0)
			{
				memcpy(&groupInfo,&m_pSHMFileExternal->m_poGroupInfo[k],sizeof(GroupInfo));
				bRet = true;
				return bRet;
			}
			k = m_pSHMFileExternal->m_poGroupInfo[k].m_iNextOffset;
		}
	}
	return bRet;
}

int UserInfoInterface::getGroupMemberByAccNbr(GroupMember ret[GROUP_NUM],char * pAccNbr,char *sDate)
{
	int iRet = 0;
	unsigned int k;
	if(NULL == pAccNbr)
	{
		return iRet;
	}
	if(NULL == sDate)
	{
		Date d;
		sDate=d.toString();
	}
    
	if(m_pSHMFileExternal->m_poGroupMemberIndex->get(pAccNbr,&k))
	{
		while(k)
		{
			if(strcmp(m_pSHMFileExternal->m_poGroupMember[k].m_sEffDate,sDate)<=0
                && strcmp(m_pSHMFileExternal->m_poGroupMember[k].m_sExpDate,sDate)>0)
			{
				ret[iRet++]=m_pSHMFileExternal->m_poGroupMember[k];
				if(iRet == GROUP_NUM)
				{
					return iRet;
				}
			}
			k = m_pSHMFileExternal->m_poGroupMember[k].m_iNextOffset;
		}
	}
	return iRet;
}

int UserInfoInterface::getNumberGroupByAccNbr(NumberGroup ret[GROUP_NUM],char * pAccNbr,char *sDate)
{
	int iRet = 0;
	unsigned int k;
	if(NULL == pAccNbr)
	{
		return iRet;
	}
	if(NULL == sDate)
	{
		Date d;
		sDate=d.toString();
	}
	if(m_pSHMFileExternal->m_poNumberGroupIndex->get(pAccNbr,&k))
	{
		while(k)
		{
			if(strcmp(m_pSHMFileExternal->m_poNumberGroup[k].m_sEffDate,sDate)<=0
                && strcmp(m_pSHMFileExternal->m_poNumberGroup[k].m_sExpDate,sDate)>0)
			{
				ret[iRet++]=m_pSHMFileExternal->m_poNumberGroup[k];
				if(iRet == GROUP_NUM)
				{
					return iRet;
				}
			}
			k = m_pSHMFileExternal->m_poNumberGroup[k].m_iNextOffset;
		}
	}
	return iRet;
}

int UserInfoInterface::getGroupResult(GroupInfoResult ret[GROUP_NUM],char * pCAccNbr,char * pGAccNbr,char * sDate)
{
	int iRet = 0;
	int iNumber;
	int iMember;
	GroupInfo groupInfo;
	NumberGroup aNumberInfo[GROUP_NUM];
	GroupMember aMemberInfo[GROUP_NUM];

	if(NULL == pCAccNbr || NULL == pGAccNbr)
	{
		return iRet;
	}
	if(NULL == sDate)
	{
		Date d;
		sDate=d.toString();
	}
	iNumber = getNumberGroupByAccNbr(aNumberInfo,pCAccNbr,sDate);
	if(iNumber>0)
	{
		iMember = getGroupMemberByAccNbr(aMemberInfo,pGAccNbr,sDate);
		if(iMember>0)
		{
			for(int i = 0;i<iNumber;i++)
			{
				for (int j = 0;j<iMember;j++)
				{
					if(aNumberInfo[i].m_iGroupID == aMemberInfo[j].m_iGroupID)
					{
						if(getGroupInfoByID(groupInfo,aNumberInfo[i].m_iGroupID,sDate))
						{
							ret[iRet].m_iGroupID = groupInfo.m_iGroupID;
							ret[iRet].m_iConditionID = groupInfo.m_iConditionID;
							ret[iRet].m_iStrategyID = groupInfo.m_iStrategyID;
							ret[iRet].m_iPriority = groupInfo.m_iPriority;
							ret[iRet++].m_lOfferInstID = aNumberInfo[i].m_lOfferInstID;
						}
					}
				}
			}
		}
	}
	return iRet;
}
#endif

#ifdef	IMSI_MDN_RELATION
bool UserInfoInterface::getImsiMdnRelationByImsi(ImsiMdnRelation &oRelation, char *sImsi,long iType, const char *sDate)
{
	unsigned int k;
	Date d, d2;
	
	if (!sImsi){
		return false;
	}
	if (!sDate){
		sDate = d.toString();
	}else{
		d = Date(sDate);
		sDate = d.toString();
	}
	
	if (!m_pSHMFileExternal->m_poImsiMdnRelationIndex->get(sImsi, &k)){
		return false;
	}

#ifdef USERINFO_OFFSET_ALLOWED
	d2 = d;
	d.addSec(UserInfoReader::m_offsetSecs);
	d2.addSec(-UserInfoReader::m_offsetSecs);
#endif

	while (k){
		if (strcmp(d2.toString(), m_pSHMFileExternal->m_poImsiMdnRelation[k].m_sExpDate)<0){
			if (strcmp(d.toString(), m_pSHMFileExternal->m_poImsiMdnRelation[k].m_sEffDate)>=0){
				//if(m_poImsiMdnRelation[k].m_itype == -1||m_poImsiMdnRelation[k].m_itype==iType ){	
				if(iType == 0 ||m_pSHMFileExternal->m_poImsiMdnRelation[k].m_itype==iType ){	
				oRelation = m_pSHMFileExternal->m_poImsiMdnRelation[k];
				return true;
				}
			}
			k = m_pSHMFileExternal->m_poImsiMdnRelation[k].m_iNextOffset;
		}
		/*	because the exp_date had been sorted in descending order,
			when sDate is larger then exp_date, stop searching. */
		else{
			return false;
		}
	}
	return false;
}
void UserInfoInterface::showImsiMdnRelationByImsi(char *sImsi, const char *sDate)
{
	unsigned int k;
	int iCnt=0, iFind=0;
	Date d, d2;
	
	if (!sImsi){
		return;
	}
	if (!sDate){
		sDate = d.toString();
	}else{
		d = Date(sDate);
		sDate = d.toString();
	}

	if (!m_pSHMFileExternal->m_poImsiMdnRelationIndex->get(sImsi, &k)){
		printf("没有找到关于IMSI: %s 的任何信息!\n", sImsi);
		return;
	}

#ifdef USERINFO_OFFSET_ALLOWED
	d2 = d;
	d.addSec(UserInfoReader::m_offsetSecs);
	d2.addSec(-UserInfoReader::m_offsetSecs);

#endif

	printf("  %-15s %-15s %-15s %-15s %-15s %-15s %-15s\n", 
		"RELATION_ID", "IMSI", "ACC_NBR", "AREA_CODE", "EFF_DATE", "EXP_DATE","TYPE");

	while (k){
		if (strcmp(d2.toString(), m_pSHMFileExternal->m_poImsiMdnRelation[k].m_sExpDate)<0 &&
			strcmp(d.toString(), m_pSHMFileExternal->m_poImsiMdnRelation[k].m_sEffDate)>=0){
			printf("* %-15ld %-15s %-15s %-15s %-15s %-15s %-15d\n", 
				m_pSHMFileExternal->m_poImsiMdnRelation[k].m_lRelationID, 
				m_pSHMFileExternal->m_poImsiMdnRelation[k].m_sImsi,
				m_pSHMFileExternal->m_poImsiMdnRelation[k].m_sAccNbr,
				m_pSHMFileExternal->m_poImsiMdnRelation[k].m_sAreaCode,
				m_pSHMFileExternal->m_poImsiMdnRelation[k].m_sEffDate,
				m_pSHMFileExternal->m_poImsiMdnRelation[k].m_sExpDate,
				m_pSHMFileExternal->m_poImsiMdnRelation[k].m_itype
				);	
			++iFind;
			++iCnt;
			k = m_pSHMFileExternal->m_poImsiMdnRelation[k].m_iNextOffset;
			continue;
		}
		/*	because the exp_date had been sorted in descending order,
			when sDate is larger then exp_date, stop searching. */
		printf("  %-15ld %-15s %-15s %-15s %-15s %-15s %-15d\n", 
			m_pSHMFileExternal->m_poImsiMdnRelation[k].m_lRelationID, 
			m_pSHMFileExternal->m_poImsiMdnRelation[k].m_sImsi,
			m_pSHMFileExternal->m_poImsiMdnRelation[k].m_sAccNbr,
			m_pSHMFileExternal->m_poImsiMdnRelation[k].m_sAreaCode,
			m_pSHMFileExternal->m_poImsiMdnRelation[k].m_sEffDate,
			m_pSHMFileExternal->m_poImsiMdnRelation[k].m_sExpDate,
			m_pSHMFileExternal->m_poImsiMdnRelation[k].m_itype
			);	
		k = m_pSHMFileExternal->m_poImsiMdnRelation[k].m_iNextOffset;
		++iCnt;
	}
	printf("共找到%d条与IMSI: %s 相关的信息\n", iCnt, sImsi);
	if (iFind){
#ifdef USERINFO_OFFSET_ALLOWED
		printf("其中有%d条符合时间(向后偏移:%d分钟): %s 要求(*标出), 第一条*记录将起作用。\n", 
				iFind, UserInfoReader::m_offsetSecs/60, sDate);
#else
		printf("其中有%d条符合时间: %s 要求(*标出), 第一条*记录将起作用。\n", iFind, sDate);
#endif
	}
	else{
#ifdef USERINFO_OFFSET_ALLOWED
		printf("但这些记录都不满足时间(向后偏移:%d分钟): %s 要求!\n",
				UserInfoReader::m_offsetSecs/60, sDate);
#else
		printf("但这些记录都不满足时间: %s 要求!\n", sDate);
#endif
	}
	return;
}
void UserInfoInterface::showAllImsiMdnRelation()
{
	unsigned int iCount = m_pSHMFileExternal->m_poImsiMdnRelationData->getCount();
	printf("%-15s %-15s %-15s %-15s %-15s %-15s %-15s %-15s\n", 
		"RELATION_ID", "IMSI", "ACC_NBR", "AREA_CODE", "EFF_DATE", "EXP_DATE", "TYPE","NEXTOFFSET");
	for (int i=1; i<=iCount; i++){
		printf("%-15ld %-15s %-15s %-15s %-15s %-15s %-15d %-15d\n", 
			m_pSHMFileExternal->m_poImsiMdnRelation[i].m_lRelationID, 
			m_pSHMFileExternal->m_poImsiMdnRelation[i].m_sImsi,
			m_pSHMFileExternal->m_poImsiMdnRelation[i].m_sAccNbr,
			m_pSHMFileExternal->m_poImsiMdnRelation[i].m_sAreaCode,
			m_pSHMFileExternal->m_poImsiMdnRelation[i].m_sEffDate,
			m_pSHMFileExternal->m_poImsiMdnRelation[i].m_sExpDate,
			m_pSHMFileExternal->m_poImsiMdnRelation[i].m_itype,
			m_pSHMFileExternal->m_poImsiMdnRelation[i].m_iNextOffset
			);	
	}
}
#endif	/* end if IMSI_MDN_RELATION */

//返回NP表中信息
	//add by jx 2009-10-26
bool UserInfoInterface::getNpInfo(NpInfo &oNpInfo,char *sAccNbr,char *sDate)
{
	unsigned int k;
	
	bool rtn = false;
	
	NpInfo *pNp = (NpInfo *)(*m_pSHMFileExternal->m_poNpData);
	
	if(!m_pSHMFileExternal->m_poNpIndex->get(sAccNbr,&k))
			return false;
	
	Date d;

	if (!sDate){
		sDate = d.toString();
	}else{
		d = Date(sDate);
		sDate = d.toString();
	}

	while( k )
	{
		
		if (strcmp(sDate, pNp[k].m_sEffDate) >= 0 &&
			strcmp(sDate, pNp[k].m_sExpDate) < 0) 
		{
			memcpy((void *)(&oNpInfo),(const void*)(&(pNp[k])),sizeof(NpInfo) );
			
			rtn = true;
			break;
		}
		else
			k = pNp[k].m_iNext;
	}

	return rtn;
}

#ifdef	IMMEDIATE_VALID
int UserInfoInterface::getRollbackInstByServ(long lServID,OfferInsQO * pOfferBuf, int iOfferNum,
											 long ret[MAX_OFFER_INSTANCE_NUMBER],const char *sDate)
{
	int iRet = 0;
	if (!sDate){
		Date d;
		sDate = d.toString();
	}
	unsigned int i;
	if (m_pSHMFileExternal->m_poServInstIndex->get(lServID,&i)){
		ProdOfferIns *pOffer ;
		long lInstID = 0;
		while(i){
			lInstID = m_pSHMFileExternal->m_poServInstRelation[i].m_lNewInstID;
			for (int j=0; j<iOfferNum; j++){
				if ((lInstID ==  pOfferBuf[j].m_poOfferIns->m_lProdOfferInsID)
					&& (strcmp(sDate,m_pSHMFileExternal->m_poServInstRelation[i].m_sEffDate)>0)
					&& (m_pSHMFileExternal->m_poServInstRelation[i].m_iProcState == 0)){
					ret[iRet++] = m_pSHMFileExternal->m_poServInstRelation[i].m_lOldInstID;
					break;
				}
			}
			i = m_pSHMFileExternal->m_poServInstRelation[i].m_iNextOffset;
		}
	}
	return iRet;
}

bool UserInfoInterface::updateRollbackInstByServ(long lServID,long lInstID)
{
	bool bRet = false;
	unsigned int i;
	if (m_pSHMFileExternal->m_poServInstIndex->get(lServID,&i)){
		while(i){
			if (lInstID == m_pSHMFileExternal->m_poServInstRelation[i].m_lOldInstID){
				m_pSHMFileExternal->m_poServInstRelation[i].m_iProcState = 1;
				bRet = true;
			}
			i = m_pSHMFileExternal->m_poServInstRelation[i].m_iNextOffset;
		}
	}else{
		Log::log(0, "[ERROR] ServID=%ld，的优惠回退资料不存在！",lServID);
	}
	if (!bRet){
		Log::log(0, "[ERROR] ServID=%ld，ProductOfferInstID=%ld,的优惠回退资料不存在！",lServID,lInstID);
	}
	return bRet;
}
#endif

#else

char const *Serv::getAccNBR(int iType, char *stime)
{
    unsigned int l;
    
	if(!m_poServInfo)
	{
		return 0;
	}

	l = m_poServInfo->m_iServIdentOffset;

    while (l) {
        if (m_poServIdent[l].m_iType != iType ||
                strcmp (stime, m_poServIdent[l].m_sExpDate)>=0 ||
                strcmp (stime, m_poServIdent[l].m_sEffDate)<0 ) {
            l = m_poServIdent[l].m_iServNextOffset;
            continue;
        }

        return m_poServIdent[l].m_sAccNbr;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    l = m_poServInfo->m_iServIdentOffset;

    Date date_sDate(stime);

    while (l) {

        Date date_EffDate(m_poServIdent[l].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if (m_poServIdent[l].m_iType == iType 
            &&    p >= UserInfoReader::m_beginOffsetSecs
            &&    p <= UserInfoReader::m_offsetSecs ) {

                return m_poServIdent[l].m_sAccNbr;

    }
    else {
        Date date_ExpDate(m_poServIdent[l].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
        if (m_poServIdent[l].m_iType == iType 
            &&    p >= UserInfoReader::m_beginOffsetSecs
            &&    p <= UserInfoReader::m_offsetSecs ) {
                return m_poServIdent[l].m_sAccNbr;
        }
    }

        l = m_poServIdent[l].m_iServNextOffset;

    }
    #endif

    return 0;
}

char const *UserInfoInterface::convertAccNBR(char *accnbr, int iSrcType, int iDestType, char *stime)
{
    unsigned int k, l,k_bak,l_bak;

    char sTemp[MAX_BILLING_NBR_LEN];
    memset (sTemp, 0, sizeof(sTemp));

    char *p = (char *)sTemp;
   

    while (*accnbr) {
        if (*accnbr >= 'A' && *accnbr <= 'Z') {
            *p = (*accnbr | 0X20);
        } else {
            *p = *accnbr;
        }

        p++;
        accnbr++;
    }  

   
    if (!(m_poServIdentIndex->get(sTemp, &k))) {
        return false;
    }

	k_bak = k;
	l_bak = 0;

	while (k) {

    # ifdef STRICT_SERV_IDENTIFICATION

        if (strcmp(stime, m_poServIdent[k].m_sEffDate) < 0 ||
            strcmp(stime, m_poServIdent[k].m_sExpDate) >=0 ||
            m_poServIdent[k].m_iType!=iSrcType ) {
            k = m_poServIdent[k].m_iNextOffset;
            continue;
        }

    # else
        if (strcmp (stime, m_poServIdent[k].m_sExpDate) >=0 ||
            m_poServIdent[k].m_iType!=iSrcType ) {
            k = m_poServIdent[k].m_iNextOffset;
            continue;
        }

    # endif

        if ((l=m_poServIdent[k].m_iServOffset)==0) return 0;

        l = m_poServ[l].m_iServIdentOffset;
		l_bak = l;
        while (l) {
            if (m_poServIdent[l].m_iType != iDestType ||
                strcmp (stime, m_poServIdent[l].m_sExpDate)>=0 ||
                strcmp (stime, m_poServIdent[l].m_sEffDate)<0 ) {
                l = m_poServIdent[l].m_iServNextOffset;
                continue;
            }

            return m_poServIdent[l].m_sAccNbr;
		}
        //return 0;
		k = m_poServIdent[k].m_iNextOffset;
	}

#ifdef USERINFO_OFFSET_ALLOWED
	k = k_bak;
	Date date_sDate(stime);
	if(l_bak>0)
	{
		l = l_bak;
		while(l)
		{
			Date date_EffDate(m_poServIdent[l].m_sEffDate);
			long p = date_EffDate.diffSec(date_sDate);
			if (p < UserInfoReader::m_beginOffsetSecs  ||
				p > UserInfoReader::m_offsetSecs ||
				m_poServIdent[l].m_iType!=iDestType)
			{
				Date date_ExpDate(m_poServIdent[l].m_sExpDate);
				p = date_sDate.diffSec(date_ExpDate);
				if (p < UserInfoReader::m_beginOffsetSecs  ||
					p > UserInfoReader::m_offsetSecs  ||
					m_poServIdent[l].m_iType!=iDestType)
				{
					l = m_poServIdent[l].m_iServNextOffset;
						continue;
				}
			}
            return m_poServIdent[l].m_sAccNbr;
		}
	}
	while(k)
	{
		Date date_EffDate(m_poServIdent[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if (p < UserInfoReader::m_beginOffsetSecs  ||
			p > UserInfoReader::m_offsetSecs ||
			m_poServIdent[k].m_iType!=iSrcType)
		{
			Date date_ExpDate(m_poServIdent[k].m_sExpDate);
			p = date_sDate.diffSec(date_ExpDate);
			if (p < UserInfoReader::m_beginOffsetSecs  ||
				p > UserInfoReader::m_offsetSecs  ||
				m_poServIdent[k].m_iType!=iSrcType)
			{
				k = m_poServIdent[k].m_iNextOffset;
					continue;
			}
		}
		if ((l=m_poServIdent[k].m_iServOffset)==0) return 0;
		l = m_poServ[l].m_iServIdentOffset;
		while (l)
		{
			if(m_poServIdent[l].m_iType == iDestType &&
			   strcmp (stime, m_poServIdent[l].m_sExpDate)<0 &&
			   strcmp (stime, m_poServIdent[l].m_sEffDate)>=0 )
			{
				return m_poServIdent[l].m_sAccNbr;
		    }
			Date date_EffDate(m_poServIdent[l].m_sEffDate);
			p = date_EffDate.diffSec(date_sDate);
			if (p < UserInfoReader::m_beginOffsetSecs  ||
				p > UserInfoReader::m_offsetSecs ||
				m_poServIdent[l].m_iType!=iDestType)
			{
				Date date_ExpDate(m_poServIdent[l].m_sExpDate);
				p = date_sDate.diffSec(date_ExpDate);
				if (p < UserInfoReader::m_beginOffsetSecs  ||
					p > UserInfoReader::m_offsetSecs  ||
					m_poServIdent[l].m_iType!=iDestType)
				{
					l = m_poServIdent[l].m_iServNextOffset;
						continue;
				}
			}
            return m_poServIdent[l].m_sAccNbr;
		}
		k = m_poServIdent[k].m_iNextOffset;
	}
#endif
    return 0;
}

////封装：先把号码和区号拼起来找，找不到的话直接使用号码找
bool UserInfoInterface::getServ_A( Serv &serv, char *area_code,char *accnbr, char stime[16], int pNetworkID, bool isPhy) 
{
    char sTemp[64];
    if(area_code && accnbr&&(0==strncmp(accnbr,area_code,strlen(area_code))))
    	{
    		if(!getServ(serv, accnbr, stime, pNetworkID, isPhy))
    			{
    				snprintf (sTemp, 64, "%s%s", area_code,  accnbr);  
        		if(getServ(serv, sTemp, stime, pNetworkID, isPhy) ){
            	return true;
    				}else
    					return false;    			
    			}else
    				return true;	
    }
    if( area_code && area_code[0] ){
        snprintf (sTemp, 64, "%s%s", area_code,  accnbr);  
        if(getServ(serv, sTemp, stime, pNetworkID, isPhy) ){
            return true;
        }
    }
    
    return getServ(serv, accnbr, stime, pNetworkID, isPhy);

}

////封装：对于对端号码不使用网络标识，先根据号码来找，然后把号码和区号拼起来找，以后的所有对端查找全部用这个方法
bool UserInfoInterface::getServ_Z( Serv &serv, char *area_code,char *accnbr, char stime[16], bool isPhy)
{
    char sTemp[64];
	if(!accnbr) return false;
	if(getServ(serv, accnbr, stime,0,isPhy) ){
		return true;
    }
    if(!area_code) return false;
    snprintf (sTemp,64,"%s%s",area_code,accnbr);
    return getServ(serv, sTemp, stime, 0, isPhy);
}


//##ModelId=4232A42F02B3
//##Documentation
//## 根据业务号码，找出主产品实例相关客户资料。
bool UserInfoInterface::getServ( Serv &serv, char *accnbr, char stime[16], int pNetworkID, bool isPhy) 
{
    unsigned int k, l;

    char sTemp[MAX_BILLING_NBR_LEN];
    memset (sTemp, 0, sizeof(sTemp));

    char *p = (char *)sTemp;

	if(!clearServ(serv))
	{
		 return false;
	}
    
    while (*accnbr) {
        if (*accnbr >= 'A' && *accnbr <= 'Z') {
            *p = (*accnbr | 0X20);
        } else {
            *p = *accnbr;
        }

        p++;
        accnbr++;
    }    

    
    if (!(m_poServIdentIndex->get(sTemp, &k))) {
        return false;
    }
    NetworkPriority * pPriChain = 0;
    unsigned int begin_k = k;
    if(pNetworkID)
    {
        pPriChain = m_poNetworkMgr->getPriority(pNetworkID);
    }
    if(!pPriChain)
    {
        pPriChain = &NetworkPriority::NETWORK_NONE;
    }
    NetworkPriority * pSavedChain = pPriChain;
    while(pPriChain) {
        while (k) {
    
        # ifdef STRICT_SERV_IDENTIFICATION
    
            if (strcmp(stime, m_poServIdent[k].m_sEffDate) < 0 ||
                strcmp(stime, m_poServIdent[k].m_sExpDate) >=0 ||
                (isPhy == (m_poServIdent[k].m_iType!=1)) ||
                 (pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_poServIdent[k].m_iNetworkID)) {
                k = m_poServIdent[k].m_iNextOffset;
                continue;
            }
    
        # else
            if (strcmp (stime, m_poServIdent[k].m_sExpDate) >=0 ||
                (isPhy == (m_poServIdent[k].m_iType!=1)) ||
                 (pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_poServIdent[k].m_iNetworkID)) {
                k = m_poServIdent[k].m_iNextOffset;
                continue;
            }
    
        # endif
    
            if ((l=m_poServIdent[k].m_iServOffset)!=0) {
                strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1);
                serv.m_poServInfo = m_poServ + l;
                serv.m_poServProductInfo = m_poServProduct+m_poServ[l].m_iServProductOffset;
                serv.m_poServAcctInfo = m_poServAcct + m_poServ[l].m_iServAcctOffset;
    
                unsigned int k;
                if (m_poCustIndex->get(m_poServ[l].m_lCustID, &k)) {
                    serv.m_poCustInfo = m_poCust + k;
                } else {
                    serv.m_poCustInfo = 0;
                }
                return true;
            } else {
                return false;
            }    
        }
        k = begin_k;
	#ifdef PRIVATE_MEMORY
	pPriChain = pPriChain->m_poNext;
	#else
	if(pPriChain->m_iNextOffset)
		pPriChain = m_poNetworkMgr->getHeadNetworkPriority() + pPriChain->m_iNextOffset ;
	else
		pPriChain = NULL;
	#endif
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = begin_k;
    pPriChain = pSavedChain;

    Date date_sDate(stime);

    while(pPriChain) {
        while (k) {
    
            Date date_EffDate(m_poServIdent[k].m_sEffDate);
            long p = date_EffDate.diffSec(date_sDate);
    
            if (p < UserInfoReader::m_beginOffsetSecs  ||
                    p > UserInfoReader::m_offsetSecs ||
                (isPhy == (m_poServIdent[k].m_iType!=1)) ||
                 (pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_poServIdent[k].m_iNetworkID)) {
          Date date_ExpDate(m_poServIdent[k].m_sExpDate);
          p = date_sDate.diffSec(date_ExpDate);
          if (p < UserInfoReader::m_beginOffsetSecs  ||
                    p > UserInfoReader::m_offsetSecs ||
                (isPhy == (m_poServIdent[k].m_iType!=1)) ||
                 (pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_poServIdent[k].m_iNetworkID))
          {
                    k = m_poServIdent[k].m_iNextOffset;
                    continue;
                }
            }
    
            if ((l=m_poServIdent[k].m_iServOffset)!=0) {
                strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1);
                serv.m_poServInfo = m_poServ + l;
                serv.m_poServProductInfo = m_poServProduct+m_poServ[l].m_iServProductOffset;
                serv.m_poServAcctInfo = m_poServAcct + m_poServ[l].m_iServAcctOffset;
    
                unsigned int k;
                if (m_poCustIndex->get(m_poServ[l].m_lCustID, &k)) {
                    serv.m_poCustInfo = m_poCust + k;
                } else {
                    serv.m_poCustInfo = 0;
                }
                return true;
            } else {
                return false;
            }    
        }
    k = begin_k;
		#ifdef PRIVATE_MEMORY
		pPriChain = pPriChain->m_poNext;
		#else
		if(pPriChain->m_iNextOffset)
			pPriChain = m_poNetworkMgr->getHeadNetworkPriority() + pPriChain->m_iNextOffset ;
		else
			pPriChain = NULL;
		#endif
    }


    #endif

    return false;
}

//根据业务号码找出一机双号的另一个用户
bool UserInfoInterface::getServ( Serv &serv, char *accnbr, long lServID,char stime[16],bool isPhy) 
{
    unsigned int k, l;

    char sTemp[MAX_BILLING_NBR_LEN];
    memset (sTemp, 0, sizeof(sTemp));

    char *p = (char *)sTemp;
    
	if(!clearServ(serv))
	{
		 return false;
	}

    while (*accnbr) {
        if (*accnbr >= 'A' && *accnbr <= 'Z') {
            *p = (*accnbr | 0X20);
        } else {
            *p = *accnbr;
        }

        p++;
        accnbr++;
    }    

    
    if (!(m_poServIdentIndex->get(sTemp, &k))) {
        return false;
    }
    while (k) {
    # ifdef STRICT_SERV_IDENTIFICATION

        if (strcmp(stime, m_poServIdent[k].m_sEffDate) < 0 ||
            strcmp(stime, m_poServIdent[k].m_sExpDate) >=0 ||
            (isPhy == (m_poServIdent[k].m_iType!=1)) ||
			(lServID == m_poServIdent[k].m_lServID)
			){
            k = m_poServIdent[k].m_iNextOffset;
            continue;
        }
    # else
        if (strcmp (stime, m_poServIdent[k].m_sExpDate) >=0 ||
            (isPhy == (m_poServIdent[k].m_iType!=1)) ||
			(lServID == m_poServIdent[k].m_lServID)
			){
            k = m_poServIdent[k].m_iNextOffset;
            continue;
        }
    # endif

    if ((l=m_poServIdent[k].m_iServOffset)!=0)
	{
        strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1);
        serv.m_poServInfo = m_poServ + l;
        serv.m_poServProductInfo = m_poServProduct+m_poServ[l].m_iServProductOffset;
        serv.m_poServAcctInfo = m_poServAcct + m_poServ[l].m_iServAcctOffset;

        unsigned int k;
        if (m_poCustIndex->get(m_poServ[l].m_lCustID, &k)) {
            serv.m_poCustInfo = m_poCust + k;
        } else {
            serv.m_poCustInfo = 0;
        }
        return true;
	}else{
		return false;
	}
	}
	return false;
}

//##ModelId=423677970042
//##Documentation
//## 根据SERVID，找出主产品实例相关客户资料。
bool UserInfoInterface::getServ( Serv &serv, long servid, char stime[16]) 
{
    unsigned int l;
    
	if(!clearServ(serv))
	{
		 return false;
	}

    if (m_poServIndex->get(servid, &l)) {
        strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1 );
        serv.m_poServInfo = m_poServ + l;
        serv.m_poServProductInfo = m_poServProduct+m_poServ[l].m_iServProductOffset;
        serv.m_poServAcctInfo = m_poServAcct + m_poServ[l].m_iServAcctOffset;

        unsigned int k;
        if (m_poCustIndex->get(m_poServ[l].m_lCustID, &k)) {
            serv.m_poCustInfo = m_poCust + k;
        } else {
            serv.m_poCustInfo = 0;
        }
        return true;
    } else {
        return false;
    }    
}

//## 根据号码/号码类型 查找用户资料
bool UserInfoInterface::getServ (Serv &serv, char *accnbr, int iSrcType, char *stime)
{
    unsigned int k, l,k_bak;

    char sTemp[MAX_BILLING_NBR_LEN];
    memset (sTemp, 0, sizeof(sTemp));

    char *p = (char *)sTemp;   
	
	if(!clearServ(serv))
	{
		 return false;
	}

    while (*accnbr) {
        if (*accnbr >= 'A' && *accnbr <= 'Z') {
            *p = (*accnbr | 0X20);
        } else {
            *p = *accnbr;
        }
        
        p++;
        accnbr++;
    }
   
    if (!(m_poServIdentIndex->get(sTemp, &k))) {
        return false;
    }

	k_bak = k;

    while (k) {

    #ifdef STRICT_SERV_IDENTIFICATION

        if (strcmp(stime, m_poServIdent[k].m_sEffDate) < 0 ||
            strcmp(stime, m_poServIdent[k].m_sExpDate) >=0 ||
            m_poServIdent[k].m_iType!=iSrcType ) {
            k = m_poServIdent[k].m_iNextOffset;
            continue;
        }

    #else
        if (strcmp (stime, m_poServIdent[k].m_sExpDate) >=0 ||
            m_poServIdent[k].m_iType!=iSrcType ) {
            k = m_poServIdent[k].m_iNextOffset;
            continue;
        }
    #endif
    
        if ((l=m_poServIdent[k].m_iServOffset) != 0)
    	{
            strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1);
            serv.m_poServInfo = m_poServ + l;
            serv.m_poServProductInfo = m_poServProduct+m_poServ[l].m_iServProductOffset;
            serv.m_poServAcctInfo = m_poServAcct + m_poServ[l].m_iServAcctOffset;
    
            unsigned int k;
            if (m_poCustIndex->get(m_poServ[l].m_lCustID, &k)) {
                serv.m_poCustInfo = m_poCust + k;
            } else {
                serv.m_poCustInfo = 0;
            }
            return true;
    	} else {
    		return false;
    	}
    }
#ifdef USERINFO_OFFSET_ALLOWED
	k = k_bak;
	Date date_sDate(stime);
	while(k)
	{
		Date date_EffDate(m_poServIdent[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if (p < UserInfoReader::m_beginOffsetSecs  ||
			p > UserInfoReader::m_offsetSecs ||
			m_poServIdent[k].m_iType!=iSrcType)
		{
			Date date_ExpDate(m_poServIdent[k].m_sExpDate);
			p = date_sDate.diffSec(date_ExpDate);
			if (p < UserInfoReader::m_beginOffsetSecs  ||
				p > UserInfoReader::m_offsetSecs  ||
				m_poServIdent[k].m_iType!=iSrcType)
			{
				k = m_poServIdent[k].m_iNextOffset;
					continue;
			}
		}
		if ((l=m_poServIdent[k].m_iServOffset)!=0)
		{
            //strcpy (serv.m_sTime, stime);
            strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1);
            serv.m_poServInfo = m_poServ + l;
            serv.m_poServProductInfo = m_poServProduct+m_poServ[l].m_iServProductOffset;
            serv.m_poServAcctInfo = m_poServAcct + m_poServ[l].m_iServAcctOffset;

            unsigned int k;
            if (m_poCustIndex->get(m_poServ[l].m_lCustID, &k)) {
                serv.m_poCustInfo = m_poCust + k;
            } else {
                serv.m_poCustInfo = 0;
            }
            return true;
        }
		else
		{
            return false;
        } 
	}
#endif
    return false;
}

//## 根据号码/号码类型、网络标识 查找用户资料
bool UserInfoInterface::getServN (Serv &serv, char *accnbr, int iSrcType, char *stime,int pNetworkID)
{
    unsigned int k, l,k_bak;

    char sTemp[MAX_BILLING_NBR_LEN];
    memset (sTemp, 0, sizeof(sTemp));

    char *p = (char *)sTemp;   
	
	if(!clearServ(serv))
	{
		 return false;
	}

    while (*accnbr) {
        if (*accnbr >= 'A' && *accnbr <= 'Z') {
            *p = (*accnbr | 0X20);
        } else {
            *p = *accnbr;
        }
        
        p++;
        accnbr++;
    }
   
    if (!(m_poServIdentIndex->get(sTemp, &k))) {
        return false;
    }

	k_bak = k;

	NetworkPriority * pPriChain = 0;
    if(pNetworkID)
    {
        pPriChain = m_poNetworkMgr->getPriority(pNetworkID);
    }
    if(!pPriChain)
    {
        pPriChain = &NetworkPriority::NETWORK_NONE;
    }
    NetworkPriority * pSavedChain = pPriChain;
    while(pPriChain)
	{
		while (k) {
		#ifdef STRICT_SERV_IDENTIFICATION
			if (strcmp(stime, m_poServIdent[k].m_sEffDate) < 0 ||
				strcmp(stime, m_poServIdent[k].m_sExpDate) >=0 ||
				m_poServIdent[k].m_iType!=iSrcType ||
				(pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_poServIdent[k].m_iNetworkID))
			{
				k = m_poServIdent[k].m_iNextOffset;
				continue;
			}

		#else
			if (strcmp (stime, m_poServIdent[k].m_sExpDate) >=0 ||
				m_poServIdent[k].m_iType!=iSrcType ||
				(pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_poServIdent[k].m_iNetworkID))
			{
				k = m_poServIdent[k].m_iNextOffset;
				continue;
			}
		#endif	    
			if ((l=m_poServIdent[k].m_iServOffset) != 0)
    		{
				//strcpy (serv.m_sTime, stime);
				strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1);
				serv.m_poServInfo = m_poServ + l;
				serv.m_poServProductInfo = m_poServProduct+m_poServ[l].m_iServProductOffset;
				serv.m_poServAcctInfo = m_poServAcct + m_poServ[l].m_iServAcctOffset;
	    
				unsigned int k;
				if (m_poCustIndex->get(m_poServ[l].m_lCustID, &k)) {
					serv.m_poCustInfo = m_poCust + k;
				} else {
					serv.m_poCustInfo = 0;
				}
				return true;
    		} else {
    			return false;
    		}
		}
		k = k_bak;
		#ifdef PRIVATE_MEMORY
		pPriChain = pPriChain->m_poNext;
		#else
		if(pPriChain->m_iNextOffset)
			pPriChain = m_poNetworkMgr->getHeadNetworkPriority() + pPriChain->m_iNextOffset ;
		else
			pPriChain = NULL;
		#endif
	}
#ifdef USERINFO_OFFSET_ALLOWED
	k = k_bak;
	Date date_sDate(stime);
	pPriChain = pSavedChain;
	while(pPriChain) {
		while(k)
		{
			Date date_EffDate(m_poServIdent[k].m_sEffDate);
			long p = date_EffDate.diffSec(date_sDate);
			if (p < UserInfoReader::m_beginOffsetSecs  ||
				p > UserInfoReader::m_offsetSecs ||
				m_poServIdent[k].m_iType!=iSrcType ||
				(pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_poServIdent[k].m_iNetworkID))
			{
				Date date_ExpDate(m_poServIdent[k].m_sExpDate);
				p = date_sDate.diffSec(date_ExpDate);
				if (p < UserInfoReader::m_beginOffsetSecs  ||
					p > UserInfoReader::m_offsetSecs ||
					m_poServIdent[k].m_iType!=iSrcType ||
					(pPriChain->m_iNetworkID !=0 && pPriChain->m_iNetworkID != m_poServIdent[k].m_iNetworkID))
				{
					k = m_poServIdent[k].m_iNextOffset;
						continue;
				}
			}
			if ((l=m_poServIdent[k].m_iServOffset)!=0)
			{	
				//strcpy (serv.m_sTime, stime);
				strncpy (serv.m_sTime, stime, sizeof(serv.m_sTime)-1);
				serv.m_poServInfo = m_poServ + l;
				serv.m_poServProductInfo = m_poServProduct+m_poServ[l].m_iServProductOffset;
				serv.m_poServAcctInfo = m_poServAcct + m_poServ[l].m_iServAcctOffset;

				unsigned int k;
				if (m_poCustIndex->get(m_poServ[l].m_lCustID, &k)) {
					serv.m_poCustInfo = m_poCust + k;
				} else {
					serv.m_poCustInfo = 0;
				}
				return true;
			}
			else
			{
				return false;
			} 
		}
		k = k_bak;
        pPriChain = pPriChain->m_poNext;
	}
#endif
    return false;
}

int UserInfoInterface::getServOrg(long servid, char * sDate)
{
    ServLocationInfo *pLocation = 
            (ServLocationInfo *)(*m_poServLocationData);
    ServInfo * pServ = (ServInfo *)(*m_poServData);
    
    unsigned int k;
    if (!m_poServIndex->get (servid, &k)) {
            return 1;
    }
    
    unsigned int * pTemp;
    pTemp = &(pServ[k].m_iLocationOffset);

    while (*pTemp) {
        if (strcmp (sDate, pLocation[*pTemp].m_sEffDate) >= 0
                && strcmp (sDate, pLocation[*pTemp].m_sExpDate) < 0) {
            return pLocation[*pTemp].m_iOrgID;
        }
        pTemp = &(pLocation[*pTemp].m_iNextOffset);
    }

#ifdef USERINFO_OFFSET_ALLOWED

    unsigned int l;

    l = pServ[k].m_iLocationOffset;

    Date date_sDate(sDate);

    while(l) {
        Date date_EffDate(pLocation[l].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);

        if(p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs) {
            return pLocation[l].m_iOrgID;
        } else {
            Date date_ExpDate(m_poServLocation[l].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
            if (p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs) {
                return pLocation[l].m_iOrgID;
            }
        }
        l = pLocation[l].m_iNextOffset;
    }
#endif

    return 1;
}   

char const *Serv::getServTypeEffDate(char *sDate)
{
	 ServTypeInfo * pServType = *m_poServTypeData;
    unsigned int k;

    if (!sDate) sDate = m_sTime;

    k = m_poServInfo->m_iServTypeOffset;

    while (k) {
        if (strcmp (pServType[k].m_sEffDate, sDate)<=0 &&
                strcmp (pServType[k].m_sExpDate, sDate)>0 ) {
					return pServType[k].m_sEffDate;
        }
        k = pServType[k].m_iNextOffset;
    }
	return 0;
}

bool Serv::hasProduct(int nProductID,char* pBegin,char *pEndDate)
{
    if((0 == pBegin)||(pBegin[0] ==0))
        return false;
    ServTypeInfo * pServType = *m_poServTypeData;
    if(!pEndDate) pEndDate = m_sTime;
    unsigned int k;
    if((!m_poServInfo)||(!pServType))
        return false;
    k = m_poServInfo->m_iServTypeOffset;
    if(strncmp(pBegin,pEndDate,14)>0)
    {
        //保证pEndDate > pBegin
        char sTemp[20]={0};
        strcpy(sTemp,pEndDate);
        strcpy(pEndDate,pBegin);
        strcpy(pBegin,sTemp);
    }

    while(k)
    {
         if ((strcmp(pEndDate,pServType[k].m_sEffDate)>0&&
             strcmp(pBegin,pServType[k].m_sExpDate)<0))
         {
             if(pServType[k].m_iProductID == nProductID)
                 return true;
         }
         k = pServType[k].m_iNextOffset;
    }
    return false;
}
int Serv::getProductID(char *sDate)
{
        ServTypeInfo * pServType = *m_poServTypeData;
        unsigned int k;

        if (!sDate) sDate = m_sTime;

		if(!m_poServInfo){
			return -1;
		}

        k = m_poServInfo->m_iServTypeOffset;

        while (k) {
                if (strcmp (pServType[k].m_sEffDate, sDate)<=0 &&
                        strcmp (pServType[k].m_sExpDate, sDate)>0 ) {
                        return pServType[k].m_iProductID;
                }

                k = pServType[k].m_iNextOffset;
        }
    #ifdef USERINFO_OFFSET_ALLOWED
    
    unsigned int l;

    l = m_poServInfo->m_iServTypeOffset;

    Date date_sDate(sDate);

    while(l) {
        Date date_EffDate(pServType[l].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if(p>= UserInfoReader::m_beginOffsetSecs 
            && p <= UserInfoReader::m_offsetSecs) {
            return pServType[l].m_iProductID;
        }
        else {
            Date date_ExpDate(pServType[l].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
            if (p>= UserInfoReader::m_beginOffsetSecs
                && p <= UserInfoReader::m_offsetSecs) {
                    return pServType[l].m_iProductID;
            }
        }
        l = pServType[l].m_iNextOffset;
    }
    #endif

        return -1;    
}

//##ModelId=4232B430006F
int Serv::getOrgID(char *sDate)
{
	if(!m_poServInfo){
		return -1;
	}

    unsigned k = m_poServInfo->m_iLocationOffset;
    if (!sDate) sDate = m_sTime;

  /* 
    while (k && strcmp(sDate, m_poServLocation[k].m_sEffDate)<0) {
        k = m_poServLocation[k].m_iNextOffset;
    }

    if (k) {
        return m_poServLocation[k].m_iOrgID;
    }
  */

   while(k) {
      if(strcmp(m_poServLocation[k].m_sEffDate,sDate)<=0 && strcmp(m_poServLocation[k].m_sExpDate,sDate)>0) {
         return  m_poServLocation[k].m_iOrgID;
       }
       
      k = m_poServLocation[k].m_iNextOffset;
    }

    
    #ifdef USERINFO_OFFSET_ALLOWED
    
    unsigned int l;

    l = m_poServInfo->m_iLocationOffset;

    Date date_sDate(sDate);

    while(l) {
        Date date_EffDate(m_poServLocation[l].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if(p>= UserInfoReader::m_beginOffsetSecs 
            && p <= UserInfoReader::m_offsetSecs) {
            return m_poServLocation[l].m_iOrgID;
        }
        else {
            Date date_ExpDate(m_poServLocation[l].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
            if (p>= UserInfoReader::m_beginOffsetSecs 
            && p <= UserInfoReader::m_offsetSecs) {
                return m_poServLocation[l].m_iOrgID;
            }
        }
        l = m_poServLocation[l].m_iNextOffset;
    }
    #endif

    return -1; 
}

//##ModelId=4232B6FC02F0
int Serv::getBillingCycleType()
{
	if(!m_poServInfo){
		return 1;
	}
    return m_poServInfo->m_iCycleType;
}

//##ModelId=4232B70B032E
BillingCycle const *Serv::getBillingCycle(char *sTime, int iCycleType)
{
    if (iCycleType == -1&&m_poServInfo) iCycleType = m_poServInfo->m_iCycleType;

    if (!sTime) sTime = m_sTime;

    return m_poBillingCycleMgr->getBillingCycle (iCycleType, sTime);
}

//##ModelId=42437460021F
char *Serv::getState(char *sDate)
{
    //##最新状态不一定在状态表中？
/*
    昆明这里SERV表里面的STATE_DATE不是最新的状态的时间,是资料变更时间
    2005-09-24 xueqt

    if (sDate) {
        if (strcmp(sDate, m_poServInfo->m_sStateDate) >= 0 )
            return m_poServInfo->m_sState;
    } else {
        if (strcmp(m_sTime, m_poServInfo->m_sStateDate) >= 0 )
            return m_poServInfo->m_sState;
    }    
*/
	if(!m_poServInfo){
		return 0;
	}

    unsigned int k = m_poServInfo->m_iStateOffset;

    if (!sDate) sDate = m_sTime;

  /*
    while (k && strcmp(sDate, m_poServState[k].m_sEffDate)<0) {
        k = m_poServState[k].m_iNextOffset;
    }

    if (k) {
        return m_poServState[k].m_sState;
    }
  */
  
    while(k) {
      if(strcmp(m_poServState[k].m_sEffDate,sDate)<=0 && strcmp(m_poServState[k].m_sExpDate, sDate)>0) {
        return m_poServState[k].m_sState;
      }
     
      k = m_poServState[k].m_iNextOffset;
    }
      
    return 0; 
}

char *Serv::getPreState()
{
	if(!m_poServInfo){
		return 0;
	}	
    unsigned k = m_poServInfo->m_iStateOffset;

    while (k) {
        k = m_poServState[k].m_iNextOffset;
    }

    if (k) {
        return m_poServState[k].m_sState;
    }

    return 0; 
}

char *Serv::getLatestState()
{
	if(!m_poServInfo){
		return 0;
	}
    unsigned int k = m_poServInfo->m_iStateOffset;
    if (k) {
        return m_poServState[k].m_sState;
    }
    
    return 0; 
}

char *Serv::getLatestStateDate()
{
    unsigned int k = m_poServInfo->m_iStateOffset;
    if (k) {
		return m_poServState[k].m_sEffDate;
    }
    
    return 0; 
}

char *Serv::getBillingState(char *sDate)
{
    if (!sDate) sDate = m_sTime;

    char * sState = getState (sDate);
    if (!sState) return 0;

    int iProductID = getProductID (sDate);

        int offers[MAX_OFFERS];

    memset (offers, 0, sizeof (offers));

    getOffers (offers, sDate);
    
    sState = m_poStateConvertMgr->getBillingState (sState, iProductID, offers[0]);

    return sState;
}

int ServProduct::getUsingDay(char *sBeginDate, char *sEndDate, Serv * pServ)
{
    Serv oServ;

	if(!m_poServInfo){
		return 0;
		}
    if (pServ == 0) {
        UserInfoInterface interface;

        interface.getServ (oServ, m_poServInfo->m_lServID, 0);
        pServ = &oServ;
    }


    Date d1(sBeginDate), d2(sEndDate);
    d1.setHour (23); d1.setMin (59); d1.setSec (59);
    d2.setHour (23); d2.setMin (59); d2.setSec (59);

    int iRet (0);

    while (d1 <= d2) {
        if (pServ->getUsing (d1) && getEff (d1.toString ()))
            iRet++;

        d1.addDay (1);
    }

    return iRet;
}

int Serv::getUsingDay(char *sBeginDate, char * sEndDate)
{
    Date d1(sBeginDate), d2(sEndDate);
    d1.setHour (23); d1.setMin (59); d1.setSec (59);
    d2.setHour (23); d2.setMin (59); d2.setSec (59);

    char sDate[16];
    int iRet (0);

    for (; d1 <= d2; d1.addDay (1)) {
        strcpy (sDate, d1.toString ());

        char * sState = getState (sDate);
        if (!sState) continue;

        int iProductID = getProductID (sDate);

        int offers[MAX_OFFERS];

        memset (offers, 0, sizeof (offers));

        getOffers (offers, sDate);
        
        sState = m_poStateConvertMgr->getBillingState (sState, iProductID, offers[0]);

        if ((sState[2]|0x20) == 'a') iRet++;
    }
    
    return iRet;
}

bool Serv::getUsing(Date d)
{
/*
    d.setHour (23); d.setMin (59); d.setSec (59);
    char * sState = getState (d.toString ());
    if (!sState) return false;

    switch (sState[2] | 0x20) {
      case 'a':
          case 'd':
          case 'e':
          case 's':
                return true;
      default:
        return false;
    }
*/
    
    d.setHour (23); d.setMin (59); d.setSec (59);
    char sDate[16];
    strcpy (sDate, d.toString ());

    char * sState = getState (sDate);
    if (!sState) return false;

    int iProductID = getProductID (sDate);

        int offers[MAX_OFFERS];

    memset (offers, 0, sizeof (offers));

    getOffers (offers, sDate);
    
    sState = m_poStateConvertMgr->getBillingState (sState, iProductID, offers[0]);

    if ((sState[2]|0x20) == 'a') return true;

    return false;

}

//##ModelId=42414E7A0176
long ServProduct::getServProductID()
{
    return m_poServProductInfo->m_lServProductID;
}

//##ModelId=42414E81022B
long ServProduct::getServID()
{
    return m_poServProductInfo->m_lServID;
}

//##ModelId=42414E86026E
int ServProduct::getProductID()
{
    return m_poServProductInfo->m_iProductID;
}

bool ServProduct::getEff(char *sDate)
{
    return (strcmp (sDate, m_poServProductInfo->m_sEffDate)>=0 &&
                strcmp (sDate, m_poServProductInfo->m_sExpDate)<0);
}

//##ModelId=42414E8C0226
char *ServProduct::getEffDate()
{
    return m_poServProductInfo->m_sEffDate;
}

//##ModelId=42414E9202F7
char *ServProduct::getExpDate()
{
    return m_poServProductInfo->m_sExpDate;
}

//##ModelId=4234E8030031
UserInfoInterface::UserInfoInterface(): UserInfoReader ()
{
    if (!m_bAttached) THROW(MBC_UserInfoReader+1);
}

UserInfoInterface::UserInfoInterface(int iMode):UserInfoReader(iMode)
{
    if (!m_bAttached) THROW(MBC_UserInfoReader+1);
}

//##ModelId=42353FFC014D
UserInfoInterface::operator bool()
{
    return m_bAttached;
}

//##ModelId=4232A90601B0
long Serv::getServID()
{
	if(m_poServInfo){
		return m_poServInfo->m_lServID;
	}
	
	return -1;
}

//##ModelId=4232AE380131
bool Serv::belongOfferInstance(long instanceid, char *sdate)
{
	if(!m_poServInfo){
		return false;
	}
    unsigned int k = m_poServInfo->m_iOfferInstanceOffset;
    unsigned int i;

    char temp[16];

    if (sdate) strcpy (temp, sdate);
    else strcpy (temp, m_sTime);

    while (k ) {
        i = k;
        while (i) {
            if (strcmp(temp, m_poOfferInstance[k].m_sEffDate) >=0 &&
            strcmp(temp, m_poOfferInstance[k].m_sExpDate)<0 &&
            m_poOfferInstance[k].m_lOfferInstanceID == instanceid) {
                return true;
            }

            i = m_poOfferInstance[i].m_iNextOffset;
        }

        k = m_poOfferInstance[k].m_iParentOffset;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iOfferInstanceOffset;

    Date date_sDate(temp);

    while(k) {
        i = k;
        while(i) {
            Date date_EffDate(m_poOfferInstance[i].m_sEffDate);
            long p = date_EffDate.diffSec(date_sDate) ;
            if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs
                && m_poOfferInstance[k].m_lOfferInstanceID == instanceid) {
                return true;
            }
            else {
                Date date_ExpDate(m_poOfferInstance[i].m_sExpDate);
                p = date_sDate.diffSec(date_ExpDate) ;
                if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs
                && m_poOfferInstance[k].m_lOfferInstanceID == instanceid) {
                    return true;
                }
            }
            i = m_poOfferInstance[i].m_iNextOffset;
        }

        k = m_poOfferInstance[k].m_iParentOffset;
    }
    #endif

    return false;
}

//##ModelId=4232AE690131
bool Serv::belongOffer(int offerid, char *sdate)
{
	if(!m_poServInfo){
		return false;
	}
    unsigned int k = m_poServInfo->m_iOfferInstanceOffset;
    unsigned int i;

    if (!sdate) sdate = m_sTime;

    while (k ) {
        i = k;
        while (i) {
            if (strcmp(sdate, m_poOfferInstance[k].m_sEffDate) >=0 &&
            strcmp(sdate, m_poOfferInstance[k].m_sExpDate)<0 &&
            m_poOfferInstance[k].m_iOfferID == offerid) {
                return true;
            }

            i = m_poOfferInstance[i].m_iNextOffset;
        }

        k = m_poOfferInstance[k].m_iParentOffset;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iOfferInstanceOffset;

    Date date_sDate(sdate);

    while(k) {
        i = k;
        while(i) {
            Date date_EffDate(m_poOfferInstance[i].m_sEffDate);
            long p = date_EffDate.diffSec(date_sDate) ;
            if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs &&
            m_poOfferInstance[k].m_iOfferID == offerid) {
                return true;
            }
            else {
                Date date_ExpDate(m_poOfferInstance[i].m_sExpDate);
                p = date_sDate.diffSec(date_ExpDate) ;
                if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs &&
                m_poOfferInstance[k].m_iOfferID == offerid) {
                    return true;
                }
            }
            i = m_poOfferInstance[i].m_iNextOffset;
        }

        k = m_poOfferInstance[k].m_iParentOffset;
    }
    #endif

    return false;
}

bool Serv::hasServProduct(int nServProductID,char * pBeginDate,char* pEndDate )
{
    if((0== pBeginDate)||(pBeginDate[0] ==0))
        return false;
    if(!pEndDate) pEndDate = m_sTime;

    ServProductIteration spiter;
    ServProduct sp;

    bool ret;
    ret = getServProductIteration(spiter,pBeginDate,true);

    if(!ret)
        return false;
    char* pEffDate=0;
    char* pExpDate=0;
    while (spiter.next(sp)) {
        if (sp.getProductID() == nServProductID)
        {
            pEffDate = sp.getEffDate();
	    pExpDate = sp.getExpDate();
	    if((strcmp(pBeginDate,pExpDate)<=0)
                &&(strcmp(pEndDate,pEffDate)>0))
                return true;
        }
    }
    return false;
}

bool Serv::hasServProductEx(int nServProductID,char * pBeginDate,char* pEndDate )
{
    if((0== pBeginDate)||(pBeginDate[0] ==0))
        return false;
    if(!pEndDate) pEndDate = m_sTime;

    ServProductIteration spiter;
    ServProduct sp;

    bool ret;
    ret = getServProductIteration(spiter,pBeginDate,true);

    if(!ret)
        return false;
    char* pEffDate=0;
    while (spiter.next(sp)) 
	{
		if (sp.getProductID() == nServProductID)
		{
			pEffDate = sp.getEffDate();
			if((pEffDate) && (strcmp(pBeginDate,pEffDate)<=0) && (strcmp(pEndDate,pEffDate)>0))
                return true;
		}
    }
    return false;
}

//##ModelId=4232AEBF0370
bool Serv::hasServProduct(int productid, ServProduct & sp, char *sdate) 
{
    ServProductIteration spiter;
    bool ret;

    if (sdate)
        ret = getServProductIteration(spiter, sdate);
    else
        ret = getServProductIteration(spiter);

    if (!ret) return false;
    
    while (spiter.next(sp)) {
        if (sp.getProductID() == productid)
            return true;
    }

    return false;
}

//##ModelId=4232B01003D8
char const *Serv::getProductEffDate(int productid, char *sdate)
{
    ServProductIteration spiter;
    ServProduct sp;
    bool ret;

    if (sdate)
        ret = getServProductIteration(spiter, sdate);
    else
        ret = getServProductIteration(spiter);

    if (!ret) return false;
    
    while (spiter.next(sp)) {
        if (sp.getProductID() == productid)
            return sp.getEffDate ();
    }

    return 0;
}

char const *Serv::getServProductEffDate(long iServProductID, char *sdate)
{
    ServProductIteration spiter;
    ServProduct sp;
    bool ret;

    if (sdate)
        ret = getServProductIteration(spiter, sdate);
    else
        ret = getServProductIteration(spiter);

    if (!ret) return false;
    
    while (spiter.next(sp)) {
        if (sp.getServProductID() == iServProductID)
            return sp.getEffDate ();
    }

    return 0;
}

void Serv::getAcctID(vector<long> & vAcctID)
{
    if(!m_poServInfo){
        return ;
    }
    
    unsigned int k = m_poServInfo->m_iServAcctOffset;

    while (k) {
        if (m_poServAcct[k].m_iState) {
            vAcctID.push_back (m_poServAcct[k].m_lAcctID);
        }
        k = m_poServAcct[k].m_iNextOffset;
    }
}


void Serv::getAcctID(vector<long> & vAcctID,char *sDate)
{
    //严格匹配生效和失效时间判断

    if(!m_poServInfo){
        return ;
    }
    unsigned int k = m_poServInfo->m_iServAcctOffset;

    while (k) 
    {
        if (strcmp(sDate,m_poServAcct[k].m_sEffDate)>=0 &&
         strcmp(sDate,m_poServAcct[k].m_sExpDate)<0)
        {
              vAcctID.push_back (m_poServAcct[k].m_lAcctID);
        }
        k = m_poServAcct[k].m_iNextOffset;
    }
}


void Serv::getAcctID(vector<long> & vAcctID,StdEvent * poEvent)
{
    //只比较到月份
    if(!m_poServInfo){
        return ;
    }
    unsigned int k = m_poServInfo->m_iServAcctOffset;

    int iTemp = 0;
    char strBeginDate[16];
    char strEndDate[16];
    BillingCycle * cycle = m_poBillingCycleMgr->getBillingCycle(poEvent->m_iBillingCycleID);
    strcpy(strBeginDate,cycle->getStartDate());
    strcpy(strEndDate,cycle->getEndDate());
    while (k) 
    {
        if ( strcmp(m_poServAcct[k].m_sEffDate,strEndDate) < 0 &&
                strcmp(m_poServAcct[k].m_sExpDate,strBeginDate) > 0)
        {
            vAcctID.push_back (m_poServAcct[k].m_lAcctID);
            iTemp ++;
        }
        k = m_poServAcct[k].m_iNextOffset;
    }
    if(iTemp> 0)
    {
        return;
    }
#ifdef USERINFO_OFFSET_ALLOWED

    unsigned int m;

    m = m_poServInfo->m_iServAcctOffset;
    Date date_sBeginDate(strBeginDate);
    Date date_sEndDate(strEndDate);

    while(m)
    {
        Date date_EffDate(m_poServAcct[m].m_sEffDate);
        long p = date_EffDate.diffSec(date_sEndDate);

        if(p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs) 
        {
            vAcctID.push_back (m_poServAcct[m].m_lAcctID);
        }else
        {
            Date date_ExpDate(m_poServAcct[m].m_sExpDate);
            p = date_sBeginDate.diffSec(date_ExpDate);
            if (p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs)
            {
                  vAcctID.push_back (m_poServAcct[m].m_lAcctID);
            }
        }
        m = m_poServAcct[m].m_iNextOffset;
    }
    return;
#endif
}
    
//##ModelId=4232B69901F3
long Serv::getAcctID(int acctitemtype)
{
    if(!m_poServInfo){
        return 0L;
    }
    unsigned int k = m_poServInfo->m_iServAcctOffset;

    while (k) {
        if (m_poServAcct[k].m_iState && 
        m_poAcctItemMgr->getBelongGroup(acctitemtype, m_poServAcct[k].m_iAcctItemGroup)) {
            return m_poServAcct[k].m_lAcctID;
        }
        k = m_poServAcct[k].m_iNextOffset;
    }
    return 0;
}



long Serv::getAcctID(int acctitemtype,char *sDate)
{
    if(!m_poServInfo){
        return 0L;
    }
    unsigned int k = m_poServInfo->m_iServAcctOffset;
    
    while (k)
    {
        //严格匹配生效和失效时间判断
         if (strcmp(sDate,m_poServAcct[k].m_sEffDate)>=0 &&
         strcmp(sDate,m_poServAcct[k].m_sExpDate)<0)
        {
              if (m_poAcctItemMgr->getBelongGroup(acctitemtype, m_poServAcct[k].m_iAcctItemGroup)) 
             {
                return m_poServAcct[k].m_lAcctID;
            }
        }
        k = m_poServAcct[k].m_iNextOffset;
    }
    return 0;
}


long Serv::getAcctID(int acctitemtype,StdEvent * poEvent)
{
    if(!m_poServInfo){
        return 0L;
    }
    unsigned int k =  m_poServInfo->m_iServAcctOffset;

    char strBeginDate[16];
    char strEndDate[16];
    BillingCycle * cycle = m_poBillingCycleMgr->getBillingCycle(poEvent->m_iBillingCycleID);
    strcpy(strBeginDate,cycle->getStartDate());
    strcpy(strEndDate,cycle->getEndDate());
    while (k)
    {
        if ( strcmp(m_poServAcct[k].m_sEffDate,strEndDate) < 0 &&
                strcmp(m_poServAcct[k].m_sExpDate,strBeginDate) > 0)
        {
              if (m_poAcctItemMgr->getBelongGroup(acctitemtype, m_poServAcct[k].m_iAcctItemGroup)) 
             {
                return m_poServAcct[k].m_lAcctID;
            }
        }
        k = m_poServAcct[k].m_iNextOffset;
    }
#ifdef USERINFO_OFFSET_ALLOWED

    unsigned int m;

    m = m_poServInfo->m_iServAcctOffset;
    Date date_sBeginDate(strBeginDate);
    Date date_sEndDate(strEndDate);

    while(m)
    {
        Date date_EffDate(m_poServAcct[m].m_sEffDate);
        long p = date_EffDate.diffSec(date_sEndDate);

        if(p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs) 
        {
            if( m_poAcctItemMgr->getBelongGroup(acctitemtype, m_poServAcct[m].m_iAcctItemGroup)) 
            {
                  return m_poServAcct[m].m_lAcctID;
            }
        }else
        {
            Date date_ExpDate(m_poServAcct[m].m_sExpDate);
            p = date_sBeginDate.diffSec(date_ExpDate);
            if (p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs)
            {
                if( m_poAcctItemMgr->getBelongGroup(acctitemtype, m_poServAcct[m].m_iAcctItemGroup)) 
                {
                      return m_poServAcct[m].m_lAcctID;
                }
            }
        }
        m = m_poServAcct[m].m_iNextOffset;
    }
#endif
    return 0;
}

//##ModelId=423505310263
//--- xueqt 20060512 modify for family plan.
int Serv::getPricingPlans(ExPricingPlan ret[MAX_PRICINGPLANS], char *sdate)
{
    unsigned int k, l;
    int i = 0, j, m;
    int offers[MAX_OFFERS];
    char *temp;

    if (sdate) temp = sdate;
    else temp = m_sTime;
    
	if(!m_poServInfo){
		return 0;
	}
    OfferInfo * pOffer = (OfferInfo *)(*m_poOfferData);

    l = m_poCust[m_poServInfo->m_iCustOffset].m_iCustPlanOffset;
    while (l) {
            if ( m_poCustPlan[l].m_iObjectType == CUST_TYPE ) {
                    if ( strcmp(temp, m_poCustPlan[l].m_sEffDate) >= 0      &&
                            strcmp(temp, m_poCustPlan[l].m_sExpDate) < 0 ) {

                            ret[i].m_iPricingPlanID = m_poCustPlan[l].m_iPricingPlanID;
                            ret[i].m_iCustPlanOffset = l;
                            i++;

                    }

            } else if (m_poCustPlan[l].m_iObjectType == SERV_INSTANCE_TYPE) {
                    if (m_poCustPlan[l].m_lObjectID == m_poServInfo->m_lServID      &&
                            strcmp(temp, m_poCustPlan[l].m_sEffDate) >= 0           &&
                                    strcmp(temp, m_poCustPlan[l].m_sExpDate) < 0 ) {

                            ret[i].m_iPricingPlanID = m_poCustPlan[l].m_iPricingPlanID;
                            ret[i].m_iCustPlanOffset = l;
                            i++;
                    }

            }

            l = m_poCustPlan[l].m_iNextOffset;
    }

/* add by xueqt 20060512 for family plan */

    k = m_poServInfo->m_iGroupOffset;
    ServGroupBaseInfo * pGroupBase = (ServGroupBaseInfo *)(*m_poServGroupBaseData);

    while (k) {
        if (m_poServGroupBaseIndex->get (m_poServGroup[k].m_lGroupID, &l)
                && pGroupBase[l].m_iGroupType==2 
                && strcmp(temp, m_poServGroup[k].m_sEffDate)>=0 
                && strcmp(temp, m_poServGroup[k].m_sExpDate)<0) {

            if (pGroupBase[l].m_iCustOffset) {
                l = m_poCust[pGroupBase[l].m_iCustOffset].m_iCustPlanOffset;

                while (l) {
                    if (m_poCustPlan[l].m_iObjectType==SERV_GROUP_TYPE 
                            && m_poCustPlan[l].m_lObjectID==m_poServGroup[k].m_lGroupID
                            && strcmp(temp, m_poCustPlan[l].m_sEffDate) >= 0
                                && strcmp(temp, m_poCustPlan[l].m_sExpDate) < 0 ) {

                        ret[i].m_iPricingPlanID = m_poCustPlan[l].m_iPricingPlanID;
                        ret[i].m_iCustPlanOffset = l;
                        i++;
                    }


                    l = m_poCustPlan[l].m_iNextOffset;
                }
            }
        }

        k = m_poServGroup[k].m_iNextOffset;
    }
    
/* end xueqt 20060512 */

    j = getOffers(offers, temp);
    for (m=0; m<j; m++) {
            if (m_poOfferIndex->get (offers[m], &k)) {
                    ret[i].m_iPricingPlanID = pOffer[k].m_iPricingPlan;
                    ret[i].m_iCustPlanOffset = 0;
                    i++;
            }
    }


    return i;

}

//##ModelId=423506410214
int Serv::getBasePricingPlan(char *sDate)
{
    ServTypeInfo * pServType = *m_poServTypeData;
    unsigned int k;

    if (!sDate) sDate = m_sTime;
	if(!m_poServInfo){
		return 0;
	}
    k = m_poServInfo->m_iServTypeOffset;

    while (k) {
        if (strcmp (pServType[k].m_sEffDate, sDate)<=0 && 
            strcmp (pServType[k].m_sExpDate, sDate)>0 ) {
            return pServType[k].m_iPricingPlanID;

        }

        k = pServType[k].m_iNextOffset;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iServTypeOffset;

    Date date_sDate(sDate);

    while (k) {
        Date date_EffDate(pServType[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs  ) {
            return pServType[k].m_iPricingPlanID;

        }
        else {
            Date date_ExpDate(pServType[k].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
            if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs  ) {
                return pServType[k].m_iPricingPlanID;

            }
        }

        k = pServType[k].m_iNextOffset;
    }

    #endif

    return 0;
}

//##ModelId=423506930302
long Serv::getCustID()
{
	if(m_poCustInfo){
		return m_poCustInfo->m_lCustID;
	}
	
	return -1;
}

//##ModelId=42355F1103A7
char * Serv::getCustType()
{
	if(m_poCustInfo){
		return m_poCustInfo->m_sCustType;
	}
	
	return 0;
}

//##ModelId=423677D80135
int Serv::getCustServ(vector<long> &ret)
{
    int i = 0;
    unsigned int k;
	if(!m_poServInfo){
		return 0;
	}
    //##找到cust
    if (!(m_poCustIndex->get(m_poServInfo->m_lCustID, &k)))
        return 0;

    k = m_poCust[k].m_iServOffset;
    while (k) {
        ret.push_back (m_poServ[k].m_lServID);
        k = m_poServ[k].m_iCustNext;
        i++;
    }
    return i;
}

int Serv::getOfferIns(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER],char *sDate)
{
    if(!sDate)
    {
        sDate = m_sTime;
    }
	if(!m_poServInfo){
		return 0;
	}
    int iRet = 0;
    int iLastNum = 0;
    unsigned int k = m_poServInfo->m_iOfferDetailOffset;
    unsigned int j = 0;
    //1.先找一遍直属的商品实例
    while(k){
        if (strcmp(m_poOfferDetailIns[k].m_sEffDate, sDate)<=0 && 
            strcmp(m_poOfferDetailIns[k].m_sExpDate, sDate)>0 ) 
        {
            strcpy(ret[iRet].m_sTime,sDate);
            ret[iRet].m_bAccued = false;
            ret[iRet].m_poDetailIns = m_poOfferDetailIns + k;
            ret[iRet++].m_poOfferIns = m_poProdOfferIns + m_poOfferDetailIns[k].m_iParentOffset;            
            if(iRet == MAX_OFFER_INSTANCE_NUMBER)
            {
                Log::log(0, "[WARNING] Serv ID: %ld 's Offer Detail Instance Number Greater than %d, Please check it.", m_poServInfo->m_lServID, MAX_OFFER_INSTANCE_NUMBER);
                return iRet;
            }
        }
        k = m_poOfferDetailIns[k].m_iNextProdOfferOffset;
    }
    //2.再找多层的实例关系TODO
    for(; iLastNum<iRet; iLastNum++)
    {
        if(m_poOfferDetailInsOfferInsIndex->get(ret[iLastNum].m_poOfferIns->m_lProdOfferInsID,&j))
        {
            while(j)
            {
                if (strcmp(m_poOfferDetailIns[j].m_sEffDate, sDate)<=0 && 
                    strcmp(m_poOfferDetailIns[j].m_sExpDate, sDate)>0 )
                {
                    strcpy(ret[iRet].m_sTime,sDate);
                    ret[iRet].m_bAccued = false;
                    ret[iRet].m_poDetailIns = m_poOfferDetailIns + j;
                    ret[iRet++].m_poOfferIns = m_poProdOfferIns + m_poOfferDetailIns[j].m_iParentOffset;
                    if(iRet == MAX_OFFER_INSTANCE_NUMBER)
                    {
                        return iRet;
                    }
                }
                j = m_poOfferDetailIns[j].m_iNextProdOfferOffset;
            }
        }
    }
    //增加基本商品
    if(getBaseOfferIns(ret+iRet,sDate))
    {
        iRet++;
    }
    /*if(iRet > 1)//需要排序
    {
        OfferInsQO tempG;
        for(int l=0; l<iRet; l++)
        {
        }
    }*/
    return iRet;
}

int Serv::getOfferInsBillingCycle(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER],char *sDate)
{
    if(!sDate)
        sDate = m_sTime;
    
	if(!m_poServInfo)
		return 0;

    BillingCycleMgr bcm;
    BillingCycle * pCycle = bcm.getOccurBillingCycle(1, sDate);
    if(!pCycle){
        Log::log (0, "\n\n[error!]根据指定时间的帐务周期在内存中不存在...");
        return 0;
    }
    
    int iRet = 0;
    int iLastNum = 0;
    unsigned int k = m_poServInfo->m_iOfferDetailOffset;
    unsigned int j = 0;
    bool bDateMark = false;
    char sTempDate[15] = {0};
    //1.先找一遍直属的商品实例
    while(k){
        if(strcmp(m_poOfferDetailIns[k].m_sEffDate, pCycle->getStartDate()) <= 0 && 
            strcmp(m_poOfferDetailIns[k].m_sExpDate, pCycle->getStartDate()) > 0){
            bDateMark = true;
            strncpy(sTempDate, pCycle->getStartDate(), 14);
        }else if(strcmp(m_poOfferDetailIns[k].m_sEffDate, pCycle->getStartDate()) >= 0 && 
            strcmp(m_poOfferDetailIns[k].m_sEffDate, pCycle->getEndDate()) < 0){
            bDateMark = true;
            strncpy(sTempDate, m_poOfferDetailIns[k].m_sEffDate, 14);
        }
        
        if(bDateMark){
            strcpy(ret[iRet].m_sTime, sTempDate);
            ret[iRet].m_bAccued = false;
            ret[iRet].m_poDetailIns = m_poOfferDetailIns + k;
            ret[iRet++].m_poOfferIns = m_poProdOfferIns + m_poOfferDetailIns[k].m_iParentOffset;            
            if(iRet == MAX_OFFER_INSTANCE_NUMBER){
                Log::log(0, "[WARNING] Serv ID: %ld 's Offer Detail Instance Number Greater than %d, Please check it.", m_poServInfo->m_lServID, MAX_OFFER_INSTANCE_NUMBER);
                return iRet;
            }
        }
        k = m_poOfferDetailIns[k].m_iNextProdOfferOffset;
        bDateMark = false;
    }
    
    //2.再找多层的实例关系TODO
    for(; iLastNum<iRet; iLastNum++){
        if(m_poOfferDetailInsOfferInsIndex->get(ret[iLastNum].m_poOfferIns->m_lProdOfferInsID,&j)){
            while(j){
                if(strcmp(m_poOfferDetailIns[j].m_sEffDate, pCycle->getStartDate()) <= 0 && 
                    strcmp(m_poOfferDetailIns[j].m_sExpDate, pCycle->getStartDate()) > 0){
                    bDateMark = true;
                    strncpy(sTempDate, pCycle->getStartDate(), 14);
                }else if(strcmp(m_poOfferDetailIns[j].m_sEffDate, pCycle->getStartDate()) >= 0 && 
                    strcmp(m_poOfferDetailIns[j].m_sEffDate, pCycle->getEndDate()) < 0){
                    bDateMark = true;
                    strncpy(sTempDate, m_poOfferDetailIns[j].m_sEffDate, 14);
                }

                if (bDateMark){
                    strcpy(ret[iRet].m_sTime, sTempDate);
                    ret[iRet].m_bAccued = false;
                    ret[iRet].m_poDetailIns = m_poOfferDetailIns + j;
                    ret[iRet++].m_poOfferIns = m_poProdOfferIns + m_poOfferDetailIns[j].m_iParentOffset;
                    
                    if(iRet == MAX_OFFER_INSTANCE_NUMBER)
                        return iRet;
                }
                j = m_poOfferDetailIns[j].m_iNextProdOfferOffset;
                bDateMark = false;
            }
        }
    }
    
    //增加基本商品
    if(getBaseOfferIns(ret+iRet, sDate))
        iRet++;
    
    return iRet;
}

int Serv::getBaseOfferIns(OfferInsQO * ret, char *sDate)
{
    if(!sDate)
    {
        sDate = m_sTime;
    }
	if(!m_poServInfo){
		return 0;
	}
    unsigned int k = m_poServInfo->m_iBaseOfferOffset;
    while(k){
        if (strcmp (m_poOfferDetailIns[k].m_sEffDate, sDate)<=0 && 
            strcmp (m_poOfferDetailIns[k].m_sExpDate, sDate)>0 )
        {
            strcpy(ret->m_sTime,sDate);
            ret->m_bAccued = false;
            ret->m_poDetailIns = m_poOfferDetailIns + k;
            ret->m_poOfferIns = m_poProdOfferIns + m_poOfferDetailIns[k].m_iParentOffset;
            return 1;
        }
        k = m_poOfferDetailIns[k].m_iNextProdOfferOffset;
    }

#ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iBaseOfferOffset;

    Date date_sDate(sDate);

    while (k) {

        Date date_EffDate(m_poOfferDetailIns[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);

        if (p >= UserInfoReader::m_beginOffsetSecs  &&
                p <= UserInfoReader::m_offsetSecs)
        {
            strcpy(ret->m_sTime,sDate);
            ret->m_bAccued = false;
            ret->m_poDetailIns = m_poOfferDetailIns + k;
            ret->m_poOfferIns = m_poProdOfferIns + m_poOfferDetailIns[k].m_iParentOffset;
            return 1;
        }
        Date date_ExpDate(m_poOfferDetailIns[k].m_sExpDate);
        p = date_sDate.diffSec(date_ExpDate);
        if (p >= UserInfoReader::m_beginOffsetSecs  &&
                p <= UserInfoReader::m_offsetSecs)
        {
            strcpy(ret->m_sTime,sDate);
            ret->m_bAccued = false;
            ret->m_poDetailIns = m_poOfferDetailIns + k;
            ret->m_poOfferIns = m_poProdOfferIns + m_poOfferDetailIns[k].m_iParentOffset;
            return 1;
        }
        k = m_poOfferDetailIns[k].m_iNextProdOfferOffset;
    }
#endif
    return 0;
}

int Serv::getBaseOfferID(char *sDate)
{
    if(!sDate)
    {
        sDate = m_sTime;
    }
	if(!m_poServInfo){
		return -1;
	}
    unsigned int k = m_poServInfo->m_iBaseOfferOffset;
    while(k){
        if (strcmp (m_poOfferDetailIns[k].m_sEffDate, sDate)<=0 && 
            strcmp (m_poOfferDetailIns[k].m_sExpDate, sDate)>0 )
        {
            return m_poProdOfferIns[m_poOfferDetailIns[k].m_iParentOffset].m_iOfferID;
        }
        k = m_poOfferDetailIns[k].m_iNextProdOfferOffset;
    }
    
#ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iBaseOfferOffset;

    Date date_sDate(sDate);

    while (k) {

        Date date_EffDate(m_poOfferDetailIns[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);

        if (p >= UserInfoReader::m_beginOffsetSecs  &&
                p <= UserInfoReader::m_offsetSecs)
        {
            return m_poProdOfferIns[m_poOfferDetailIns[k].m_iParentOffset].m_iOfferID;
        }
        Date date_ExpDate(m_poOfferDetailIns[k].m_sExpDate);
        p = date_sDate.diffSec(date_ExpDate);
        if (p >= UserInfoReader::m_beginOffsetSecs  &&
                p <= UserInfoReader::m_offsetSecs)
        {
            return m_poProdOfferIns[m_poOfferDetailIns[k].m_iParentOffset].m_iOfferID;
        }
        k = m_poOfferDetailIns[k].m_iNextProdOfferOffset;
    }
#endif
    return -1;
}

bool Serv::hasOffer (int iOfferID, char *sDate)
{
    OfferInsQO ret[MAX_OFFERS];
    int retNum = getOfferIns(ret, sDate);
    for(int i=0; i<retNum; i++)
    {
        if(ret[i].m_poOfferIns->m_iOfferID == iOfferID)
        {
            return true;
        }
    }
    return false;
}

CustPricingTariffInfo * UserInfoInterface::getCustTariff(unsigned int iCustPlanOffset, char *sCalledNBR)
{
    long lCustPlanID = m_poCustPlan[iCustPlanOffset].m_lCustPlanID;    

    unsigned int k;
    CustPricingTariffInfo *p = (CustPricingTariffInfo *)(*m_poCustTariffData);

    if (!m_poCustTariffIndex->getMax (sCalledNBR, &k)) return 0;

    while (k) {
        if (p[k].m_lCustPlanID == lCustPlanID) return p+k;

        if (p[k].m_lCustPlanID > lCustPlanID) 
            k = p[k].m_iLeft;
        else 
            k = p[k].m_iRight;
    }

    return 0;
}

long UserInfoInterface::getCustomObjectID(int iPlanObjectID, unsigned int iCustPlanOffset)
{
    static unsigned int lastOffset = 0;
    static int lastObjectID = 0;
    static unsigned int k = 0;

    CustPricingObjectInfo * pCustObject = (CustPricingObjectInfo *)(*m_poCustPricingObjectData);

    long lRet = 0;

    if (lastObjectID!=iPlanObjectID  || lastOffset!=iCustPlanOffset) {
        k = m_poCustPlan[iCustPlanOffset].m_iObjectOffset;
        lastObjectID = iPlanObjectID;
        lastOffset = iCustPlanOffset;
    }

    while (k) {
        if (pCustObject[k].m_iPricingObjectID == iPlanObjectID) {
            lRet = pCustObject[k].m_lObjectID;
            k = pCustObject[k].m_iNextOffset;

            return lRet;
        }

            k = pCustObject[k].m_iNextOffset;
    }

    lastOffset = 0;
    lastObjectID = 0;
    return 0;
}

bool UserInfoInterface::isBindServObject(long lServID, unsigned int iCustPlanOffset)
{
        CustPricingObjectInfo * pCustObject = (CustPricingObjectInfo *)(*m_poCustPricingObjectData);

        unsigned int k = m_poCustPlan[iCustPlanOffset].m_iObjectOffset;

        while (k) {
                if (lServID==pCustObject[k].m_lObjectID && pCustObject[k].m_iObjectType==SERV_INSTANCE_TYPE)
                        return true;

                k = pCustObject[k].m_iNextOffset;
        }

        return false;
}

CustPricingObjectInfo const * UserInfoInterface::getCustomObject(int iPlanObjectID, unsigned int iCustPlanOffset)
{
    static unsigned int lastOffset = 0;
    static int lastObjectID = 0;
    static unsigned int k = 0;

    CustPricingObjectInfo * pCustObject = (CustPricingObjectInfo *)(*m_poCustPricingObjectData);

    CustPricingObjectInfo const * pRet = 0;

    if (lastObjectID!=iPlanObjectID  || lastOffset!=iCustPlanOffset) {
        k = m_poCustPlan[iCustPlanOffset].m_iObjectOffset;
        lastObjectID = iPlanObjectID;
        lastOffset = iCustPlanOffset;
    }

        while (k) {
                if (pCustObject[k].m_iPricingObjectID == iPlanObjectID) {
            pRet = pCustObject + k;
            k = pCustObject[k].m_iNextOffset;

            return pRet;
                }

                k = pCustObject[k].m_iNextOffset;
        }

    lastOffset = 0;
    lastObjectID = 0;
        return 0;
}

char const * UserInfoInterface::getCustomPlanEffDate(unsigned int iCustPlanOffset)
{
    return m_poCustPlan[iCustPlanOffset].m_sEffDate;

}

int UserInfoInterface::getGroupServ(long lServGroupID, vector<long> &vServ, char *sDate)
{
    unsigned int k;
    int ret = 0;
    if (!m_poServGroupBaseIndex->get (lServGroupID, &k)) return 0;
    ServGroupBaseInfo * pGroupBase = (ServGroupBaseInfo *)(*m_poServGroupBaseData);
    ServGroupInfo * pGroup = (ServGroupInfo *)(*m_poServGroupData);

    k = pGroupBase[k].m_iServOffset;
    
    while (k) {
        if (strcmp (sDate, pGroup[k].m_sEffDate)>=0  && strcmp (sDate, pGroup[k].m_sExpDate)<0) {
            vServ.push_back (pGroup[k].m_lServID);
            ret++;
        }

        k = pGroup[k].m_iGroupNext;
    }

    return ret;

}

int UserInfoInterface::getCustServ(long lCustID, vector<long> &vServ)
{
    unsigned int k;
    int ret = 0;
    if (!m_poCustIndex->get (lCustID, &k)) return 0;
    k = m_poCust[k].m_iServOffset;
    
    while (k) {
        vServ.push_back (m_poServ[k].m_lServID);
        ret++;
        k = m_poServ[k].m_iCustNext;
    }

    return ret;
}

int UserInfoInterface::getCustAcct(long lCustID, vector<long> &vAcct)
{
    unsigned int k;
    int ret = 0;
    if (!m_poCustIndex->get (lCustID, &k)) return 0;
    k = m_poCust[k].m_iAcctOffset;
    AcctInfo * pAcct = *m_poAcctData;
    
    while (k) {
        if ((pAcct[k].m_sState[2]|0X20) == 'a') {
            vAcct.push_back (pAcct[k].m_lAcctID);
            ret++;
        }
        k = pAcct[k].m_iNextOffset;
    }

    return ret;
}

/*
**    获取某AcctID的支付关系
*/
int UserInfoInterface::getPayInfo(vector<ServAcctInfo> &ret, long lAcctID, char *sDate)
{
    unsigned int l;
    int iTemp = 0;

    unsigned int k;

    if (m_poServAcctIndex->get (lAcctID, &l)) 
    {
        while (l)
        {
            if (m_poServAcct[l].m_iState)
            {
                if (sDate)
                {
                    /*    进行用户有效性的校验    */
                    if (m_poServIndex->get (m_poServAcct[l].m_lServID, &k))
                    {
                        k = m_poServ[k].m_iStateOffset;
                        
                        /*
                        while (k && strcmp(sDate, m_poServState[k].m_sEffDate)<0)
                        {
                            k = m_poServState[k].m_iNextOffset;
                        }
                        */
                        
                        while(k) {
                          if(strcmp(m_poServState[k].m_sEffDate,sDate)<=0 && strcmp(m_poServState[k].m_sExpDate,sDate)>0) {
                             break;
                          }
                          
                          k = m_poServState[k].m_iNextOffset;
                        }

                        if (k && (m_poServState[k].m_sState[2]|0x20)!='x')
                        {
                            ret.push_back (m_poServAcct[l]);
                            iTemp++;
                        }                        
                    }                    
                } else
                {
                    ret.push_back (m_poServAcct[l]);
                    iTemp++;
                }
            }
            l = m_poServAcct[l].m_iAcctNextOffset;
        }
    }
    return iTemp;
}

int UserInfoInterface::getPayInfo(vector<ServAcctInfo> &ret, long lAcctID, StdEvent * poEvent)
{
    unsigned int l;
    int iTemp = 0,iFind = -1;

    unsigned int k;
    char strBeginDate[16];
    char strEndDate[16];
    BillingCycle * cycle = m_poBillingCycleMgr->getBillingCycle(poEvent->m_iBillingCycleID);;
    strcpy(strBeginDate,cycle->getStartDate());
    strcpy(strEndDate,cycle->getEndDate());

    if (m_poServAcctIndex->get (lAcctID, &l))
    {
        while (l)
        {
            if ( strcmp(m_poServAcct[l].m_sEffDate,strEndDate) < 0 &&
                strcmp(m_poServAcct[l].m_sExpDate,strEndDate) >= 0)
            {
                ret.push_back (m_poServAcct[l]);
                iTemp++;
            }
            l = m_poServAcct[l].m_iAcctNextOffset;
        }
    }
    if(iTemp)
    {
        return iTemp;
    }
#ifdef USERINFO_OFFSET_ALLOWED

    unsigned int m;
    Date date_sBeginDate(strBeginDate);
    Date date_sEndDate(strEndDate);

    if(m_poServAcctIndex->get (lAcctID, &m))
    {
        while(m)
        {
            Date date_EffDate(m_poServAcct[m].m_sEffDate);
            long p = date_EffDate.diffSec(date_sEndDate);
            if(p>= UserInfoReader::m_beginOffsetSecs
                        && p <= UserInfoReader::m_offsetSecs)
            {
                    ret.push_back (m_poServAcct[m]);
                    iTemp ++;
            }else
            {
                Date date_ExpDate(m_poServAcct[m].m_sExpDate);
                p = date_sBeginDate.diffSec(date_ExpDate);
                if(p>= UserInfoReader::m_beginOffsetSecs
                        && p <= UserInfoReader::m_offsetSecs)
                {
                    ret.push_back (m_poServAcct[m]);
                    iTemp ++;
                }
            }
            m = m_poServAcct[m].m_iAcctNextOffset;
        }
    }
#endif
    return iTemp;
}

//##客户个性话参数`
long UserInfoInterface::getCustomParamValue (int iParamID, unsigned int iCustPlanOffset)
{
    unsigned int k = m_poCustPlan[iCustPlanOffset].m_iParamOffset;
    CustPricingParamInfo * pCustParam = (CustPricingParamInfo *)(*m_poCustPricingParamData);

    while (k) {
        if (pCustParam[k].m_iParamID == iParamID) {
            return pCustParam[k].m_lValue;
        }
        k = pCustParam[k].m_iNextOffset;
    }

    return 0;
}

//找对象的个性化的定价信息
int UserInfoInterface::getCustomPricingPlans(ExPricingPlan ret[MAX_PRICINGPLANS],int iObjectType,long lObjectID,char *sDate)
{
    unsigned int k;
    unsigned long lCustID;
    int i = 0;

    switch (iObjectType) {
      case SERV_INSTANCE_TYPE:
        if (!m_poServIndex->get (lObjectID, &k)) return 0;
        k = m_poCust[m_poServ[k].m_iCustOffset].m_iCustPlanOffset;
        break;

      case CUST_TYPE:
        lCustID = lObjectID;
        if (!m_poCustIndex->get (lCustID, &k)) return 0;
        k = m_poCust[k].m_iCustPlanOffset;

        break;

      case ACCT_TYPE:
        if (!m_poAcctIndex->get (lObjectID, &k)) return 0;
        lCustID = (*m_poAcctData)[k].m_lCustID;

        if (!m_poCustIndex->get (lCustID, &k)) return 0;
        k = m_poCust[k].m_iCustPlanOffset;

        break;

      case SERV_GROUP_TYPE: {
        ServGroupBaseInfo * pGroupBase = (ServGroupBaseInfo *)(*m_poServGroupBaseData);
        if (!m_poServGroupBaseIndex->get (lObjectID, &k)) return 0;

        if (pGroupBase[k].m_iCustOffset) {
            k = m_poCust[pGroupBase[k].m_iCustOffset].m_iCustPlanOffset;
        } else {
            k = 0;
        }

        break;
      }

      default:
        return 0;
        
    }

    while (k) {
        if ( m_poCustPlan[k].m_iObjectType == CUST_TYPE                 &&
                    strcmp(sDate, m_poCustPlan[k].m_sEffDate) >= 0  &&
                    strcmp(sDate, m_poCustPlan[k].m_sExpDate) < 0 ) {

            ret[i].m_iPricingPlanID = m_poCustPlan[k].m_iPricingPlanID;
            ret[i].m_iCustPlanOffset = k;
            i++;

        } else if (m_poCustPlan[k].m_iObjectType == iObjectType) {
            if (m_poCustPlan[k].m_lObjectID == lObjectID             &&
                    strcmp(sDate, m_poCustPlan[k].m_sEffDate) >= 0   &&
                            strcmp(sDate, m_poCustPlan[k].m_sExpDate) < 0 ) {

                    ret[i].m_iPricingPlanID = m_poCustPlan[k].m_iPricingPlanID;
                    ret[i].m_iCustPlanOffset = k;
                    i++;
            }

        }

        k = m_poCustPlan[k].m_iNextOffset;
    }

    return i;

}

//cdk增加商品示例的查询
//获得帐户的商品实例
int UserInfoInterface::getAcctOfferIns(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER], long lAcctID, char *sDate)
{
    unsigned int p = 0;
    if (!m_poAcctIndex->get (lAcctID, &p)) return 0;

    int iRet = 0;
    int iLastNum = 0;
    unsigned int k = m_poAcct[p].m_iOfferDetailOffset;
    unsigned int j = 0;
    //1.先找一遍直属的商品实例
    while(k){
        if (!sDate||
            (strcmp(m_poOfferDetailIns[k].m_sEffDate, sDate)<=0 && 
            strcmp(m_poOfferDetailIns[k].m_sExpDate, sDate)>0 )) 
        {
            strcpy(ret[iRet].m_sTime,sDate);
            ret[iRet].m_bAccued = false;
            ret[iRet].m_poDetailIns = m_poOfferDetailIns + k;
            ret[iRet++].m_poOfferIns = m_poProdOfferIns + m_poOfferDetailIns[k].m_iParentOffset;
            if(iRet == MAX_OFFER_INSTANCE_NUMBER)
            {
                return iRet;
            }
        }
        k = m_poOfferDetailIns[k].m_iNextProdOfferOffset;
    }
    //2.再找多层的实例关系TODO
    for(; iLastNum<iRet; iLastNum++)
    {
        if(m_poOfferDetailInsOfferInsIndex->get(ret[iLastNum].m_poOfferIns->m_lProdOfferInsID,&j))
        {
            while(j)
            {
                if (strcmp(m_poOfferDetailIns[j].m_sEffDate, sDate)<=0 && 
                    strcmp(m_poOfferDetailIns[j].m_sExpDate, sDate)>0 )
                {
                    strcpy(ret[iRet].m_sTime,sDate);
                    ret[iRet].m_bAccued = false;
                    ret[iRet].m_poDetailIns = m_poOfferDetailIns + j;
                    ret[iRet++].m_poOfferIns = m_poProdOfferIns + m_poOfferDetailIns[j].m_iParentOffset;
                    if(iRet == MAX_OFFER_INSTANCE_NUMBER)
                    {
                        return iRet;
                    }
                }
                j = m_poOfferDetailIns[j].m_iNextProdOfferOffset;
            }
        }
    }
    /*if(iRet > 1)//需要排序
    {
        OfferInsQO tempG;
        for(int l=0; l<iRet; l++)
        {
        }
    }*/
    return iRet;
}
int UserInfoInterface::getCustOfferIns(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER], long lCustID, char *sDate)
{
    unsigned int p = 0;
    if (!m_poCustIndex->get (lCustID, &p)) return 0;

    int iRet = 0;
    int iLastNum = 0;
    unsigned int k = m_poCust[p].m_iOfferDetailOffset;
    unsigned int j = 0;
    //1.先找一遍直属的商品实例
    while(k){
        if (!sDate||
            (strcmp(m_poOfferDetailIns[k].m_sEffDate, sDate)<=0 && 
            strcmp(m_poOfferDetailIns[k].m_sExpDate, sDate)>0 )) 
        {
            strcpy(ret[iRet].m_sTime,sDate);
            ret[iRet].m_bAccued = false;
            ret[iRet].m_poDetailIns = m_poOfferDetailIns + k;
            ret[iRet++].m_poOfferIns = m_poProdOfferIns + m_poOfferDetailIns[k].m_iParentOffset;
            if(iRet == MAX_OFFER_INSTANCE_NUMBER)
            {
                return iRet;
            }
        }
        k = m_poOfferDetailIns[k].m_iNextProdOfferOffset;
    }
    //2.再找多层的实例关系TODO
    for(; iLastNum<iRet; iLastNum++)
    {
        if(m_poOfferDetailInsOfferInsIndex->get(ret[iLastNum].m_poOfferIns->m_lProdOfferInsID,&j))
        {
            while(j)
            {
                if (strcmp(m_poOfferDetailIns[j].m_sEffDate, sDate)<=0 && 
                    strcmp(m_poOfferDetailIns[j].m_sExpDate, sDate)>0 )
                {
                    strcpy(ret[iRet].m_sTime,sDate);
                    ret[iRet].m_bAccued = false;
                    ret[iRet].m_poDetailIns = m_poOfferDetailIns + j;
                    ret[iRet++].m_poOfferIns = m_poProdOfferIns + m_poOfferDetailIns[j].m_iParentOffset;
                    if(iRet == MAX_OFFER_INSTANCE_NUMBER)
                    {
                        return iRet;
                    }
                }
                j = m_poOfferDetailIns[j].m_iNextProdOfferOffset;
            }
        }
    }

    return iRet;
}

bool UserInfoInterface::isOfferInsBelongOrg(long lOfferInstID, int iOrgID, char * sDate)
{
    unsigned int j = 0;
    long ltempAcctID = 0;
    long ltempCustID = 0;
    long ltempOfferInsID = 0;
    unsigned int p;
    
    
    if (!m_poProdOfferInsIndex->get (lOfferInstID, &j)) return false;

    unsigned int k = m_poProdOfferIns[j].m_iOfferDetailInsOffset;  

    for (; k; k=m_poOfferDetailIns[k].m_iNextDetailOffset) {
        
        if (!sDate||
            (strcmp(m_poOfferDetailIns[k].m_sEffDate, sDate)>0 || 
            strcmp(m_poOfferDetailIns[k].m_sExpDate, sDate)<=0 )) 
        {
            continue;
        }
        
        switch (m_poOfferDetailIns[k].m_iInsType)
        {
            case SERV_INSTANCE_TYPE:
                if (m_poServIndex->get (m_poOfferDetailIns[k].m_lInsID, &p)) {
                        j = m_poServ[p].m_iLocationOffset;
                        while (j && strcmp(sDate, m_poServLocation[j].m_sEffDate)<0) {
                            j = m_poServLocation[j].m_iNextOffset;
                        }
                        if (j) {
                                return m_poOrgMgr->getBelong (m_poServLocation[j].m_iOrgID, iOrgID);
                        }
                }
                break;
            case ACCT_TYPE:
                if(!ltempAcctID) ltempAcctID = m_poOfferDetailIns[k].m_lInsID;
                break;
            case CUST_TYPE:
                if(!ltempCustID) ltempCustID = m_poOfferDetailIns[k].m_lInsID;
                break;
            case OFFER_INSTANCE_TYPE:
                if(!ltempOfferInsID) ltempOfferInsID = m_poOfferDetailIns[k].m_lInsID;
                break;
        }     
    }

    if(ltempCustID) return getCustBelongOrg(ltempCustID, iOrgID);

    if(ltempAcctID) return getAcctBelongOrg(ltempAcctID, iOrgID);

    if(ltempOfferInsID) 
        return isOfferInsBelongOrg(ltempOfferInsID, iOrgID, sDate);

    return true;
}
int UserInfoInterface::getOfferInsByID(OfferInsQO& ret,long lProdOfferInstID,char* sDate)
{
    unsigned int k=0;
    if (!m_poProdOfferInsIndex->get (lProdOfferInstID, &k)) return 0;

    ret.m_poDetailIns = 0;
    ret.m_poOfferIns = m_poProdOfferIns + k;	
    strcpy(ret.m_sTime,sDate);	
    return 1; 	
}
int UserInfoInterface::getAllOfferInsByID(OfferInsQO ret[MAX_OFFER_INSTANCE_NUMBER], long lProdOfferInsID, char *sDate)
{
    unsigned int k;
    int iRet = 0;
    unsigned int j = 0;
    
    if (!m_poProdOfferInsIndex->get (lProdOfferInsID, &k)) return 0;
    
    strcpy(ret[iRet].m_sTime,sDate);
    ret[iRet].m_poDetailIns = 0;
    ret[iRet++].m_poOfferIns = m_poProdOfferIns + k;
    if(iRet == MAX_OFFER_INSTANCE_NUMBER)
    {
        return iRet;
    }
    for(int iLastNum = 0; iLastNum<iRet; iLastNum++)
    {
        if(m_poOfferDetailInsOfferInsIndex->get(ret[iLastNum].m_poOfferIns->m_lProdOfferInsID,&j))
        {
            if (strcmp(m_poOfferDetailIns[j].m_sEffDate, sDate)<=0 && 
                strcmp(m_poOfferDetailIns[j].m_sExpDate, sDate)>0 )
            {
                strcpy(ret[iRet].m_sTime,sDate);
                ret[iRet].m_poDetailIns = m_poOfferDetailIns + j;
                ret[iRet++].m_poOfferIns = m_poProdOfferIns + m_poOfferDetailIns[j].m_iParentOffset;
                if(iRet == MAX_OFFER_INSTANCE_NUMBER)
                {
                    return iRet;
                }
            }
        }
    }
    return iRet;
}

long UserInfoInterface::getAcctID(long lServID, int acctitemtype)
{
    unsigned int l;
    
    if (!m_poServIndex->get(lServID, &l))
    {
        return 0;
    }
    unsigned int k = m_poServ[l].m_iServAcctOffset;
    while (k) {
        if (m_poServAcct[k].m_iState && 
        m_poAcctItemMgr->getBelongGroup(acctitemtype, m_poServAcct[k].m_iAcctItemGroup)) {
            return m_poServAcct[k].m_lAcctID;
        }
        k = m_poServAcct[k].m_iNextOffset;
    }
    return 0;
}

long UserInfoInterface::getAcctID(long lServID, int acctitemtype, char * sDate)
{
    unsigned int l;
    //严格配置生效和失效时间
    if (!m_poServIndex->get(lServID, &l))
    {
        return 0;
    }
    unsigned int k = m_poServ[l].m_iServAcctOffset;
    while (k)
    {
        if (strcmp(sDate,m_poServAcct[k].m_sEffDate)>=0 &&
             strcmp(sDate,m_poServAcct[k].m_sExpDate)<0)
        {
            if( m_poAcctItemMgr->getBelongGroup(acctitemtype, m_poServAcct[k].m_iAcctItemGroup)) 
            {
                  return m_poServAcct[k].m_lAcctID;
            }
        }
        k = m_poServAcct[k].m_iNextOffset;
    }
    return 0;
}

long UserInfoInterface::getAcctID(long lServID, int acctitemtype, StdEvent * poEvent)
{
    unsigned int l;
    //只比较到月份
    if (!m_poServIndex->get(lServID, &l))
    {
        return 0;
    }
    
    char strBeginDate[16];
    char strEndDate[16];
    BillingCycle * cycle = m_poBillingCycleMgr->getBillingCycle(poEvent->m_iBillingCycleID);
    strcpy(strBeginDate,cycle->getStartDate());
    strcpy(strEndDate,cycle->getEndDate());
    
    unsigned int k = m_poServ[l].m_iServAcctOffset;
    while (k) 
    {
        if ( strcmp(m_poServAcct[k].m_sEffDate,strEndDate) < 0 &&
                strcmp(m_poServAcct[k].m_sExpDate,strBeginDate) > 0)
        {
            if( m_poAcctItemMgr->getBelongGroup(acctitemtype, m_poServAcct[k].m_iAcctItemGroup)) 
            {
                  return m_poServAcct[k].m_lAcctID;
            }
        }
        k = m_poServAcct[k].m_iNextOffset;
    }
#ifdef USERINFO_OFFSET_ALLOWED

    unsigned int m;

    m = m_poServ[l].m_iServAcctOffset;
    Date date_sBeginDate(strBeginDate);
    Date date_sEndDate(strEndDate);

    while(m)
    {
        Date date_EffDate(m_poServAcct[m].m_sEffDate);
        long p = date_EffDate.diffSec(date_sEndDate);

        if(p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs) 
        {
            if( m_poAcctItemMgr->getBelongGroup(acctitemtype, m_poServAcct[m].m_iAcctItemGroup)) 
            {
                  return m_poServAcct[m].m_lAcctID;
            }
        }else
        {
            Date date_ExpDate(m_poServAcct[m].m_sExpDate);
            p = date_sBeginDate.diffSec(date_ExpDate);
            if (p>= UserInfoReader::m_beginOffsetSecs
            && p <= UserInfoReader::m_offsetSecs)
            {
                if( m_poAcctItemMgr->getBelongGroup(acctitemtype, m_poServAcct[m].m_iAcctItemGroup)) 
                {
                      return m_poServAcct[m].m_lAcctID;
                }
            }
        }
        m = m_poServAcct[m].m_iNextOffset;
    }
#endif

    return 0;
}

//##ModelId=42414DD400F5
int Serv::getOffers(int  offers[MAX_OFFERS], char *sdate)
{
	if(!m_poServInfo){
		return 0;
	}
    unsigned int k = m_poServInfo->m_iOfferInstanceOffset;
    int temp[MAX_OFFERS];//##输出要求从高层次－》低层次

    int i = 0;

    if (!sdate) sdate = m_sTime;

    while (k && i<MAX_OFFERS) {
        while (k) {
            if (strcmp(sdate, m_poOfferInstance[k].m_sEffDate) >= 0 &&
                strcmp(sdate, m_poOfferInstance[k].m_sExpDate) < 0) {

                temp[i] = m_poOfferInstance[k].m_iOfferID;
                i++;
                break;
            } else {
                k = m_poOfferInstance[k].m_iNextOffset;
            }
        }


        if (!k) break;

        k = m_poOfferInstance[k].m_iParentOffset;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    if ( i == 0 ) {

        k = m_poServInfo->m_iOfferInstanceOffset;
        
        Date date_sDate(sdate);

        while (k && i<MAX_OFFERS) {
            while (k) {
                Date date_EffDate(m_poOfferInstance[k].m_sEffDate);
                long p = date_EffDate.diffSec(date_sDate);
                if (p >= UserInfoReader::m_beginOffsetSecs
                    &&p <= UserInfoReader::m_offsetSecs) {

                    temp[i] = m_poOfferInstance[k].m_iOfferID;
                    i++;
                    break;
                } else {
                    Date date_ExpDate(m_poOfferInstance[k].m_sExpDate);
                    p = date_sDate.diffSec(date_ExpDate);
                    if (p >= UserInfoReader::m_beginOffsetSecs
                    &&p <= UserInfoReader::m_offsetSecs) {

                        temp[i] = m_poOfferInstance[k].m_iOfferID;
                        i++;
                        break;
                    }
                    k = m_poOfferInstance[k].m_iNextOffset;
                }
            }


            if (!k) break;

        k = m_poOfferInstance[k].m_iParentOffset;
    }
    }

    #endif

    int j = 0;
    for (j=0; j<i; j++) {
        offers[j] = temp[i-1-j];
    }
    return i;
}

//##ModelId=42414DF202E3
int Serv::getOfferInstances(long offerinstances[MAX_OFFERINSTANCES], char *sdate)
{
	if(!m_poServInfo){
		return 0;
	}
    unsigned int k = m_poServInfo->m_iOfferInstanceOffset;
    long temp[MAX_OFFERINSTANCES];

    int i = 0;

    if (!sdate) sdate = m_sTime;

    while (k && i<MAX_OFFERINSTANCES) {
        while (k) {
            if (strcmp(sdate, m_poOfferInstance[k].m_sEffDate) >= 0 &&
                strcmp(sdate, m_poOfferInstance[k].m_sExpDate) < 0) {
                temp[i] = m_poOfferInstance[k].m_lOfferInstanceID;
                i++;
                break;
            } else {
                k = m_poOfferInstance[k].m_iNextOffset;
            }
        }
        
        if (!k) break;

        k = m_poOfferInstance[k].m_iParentOffset;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    if ( i == 0 ) {

        k = m_poServInfo->m_iOfferInstanceOffset;
        
        Date date_sDate(sdate);

        while (k && i<MAX_OFFERINSTANCES) {
            while (k) {
                Date date_EffDate(m_poOfferInstance[k].m_sEffDate);
                long p = date_EffDate.diffSec(date_sDate);
                if (p >= UserInfoReader::m_beginOffsetSecs
                    &&p <= UserInfoReader::m_offsetSecs) {

                    temp[i] = m_poOfferInstance[k].m_lOfferInstanceID;
                    i++;
                    break;
                } else {
                    Date date_ExpDate(m_poOfferInstance[k].m_sExpDate);
                    p = date_sDate.diffSec(date_ExpDate);
                    if (p >= UserInfoReader::m_beginOffsetSecs
                    &&p <= UserInfoReader::m_offsetSecs) {
                        temp[i] = m_poOfferInstance[k].m_lOfferInstanceID;
                        i++;
                        break;
                    }
                    k = m_poOfferInstance[k].m_iNextOffset;
                }
            }


            if (!k) break;

        k = m_poOfferInstance[k].m_iParentOffset;
    }
    }

    #endif

    int j = 0;
    for (j=0; j<i; j++) {
        offerinstances[j] = temp[i-1-j];
    }

    return i;

}

bool UserInfoInterface::getServProduct(long lServProductID, ServProduct & servProduct)
{
    unsigned int k;

    if (!m_poServProductIndex->get (lServProductID, &k)) return false;

    servProduct.m_poServProductInfo = m_poServProduct + k;
    servProduct.m_poServInfo = m_poServ + m_poServProduct[k].m_iServOffset;
    return true;
}

bool Serv::getServProduct(long lServProductID, ServProduct & servProduct)
{
	if(!m_poServInfo){
		return false;
	}
    unsigned int k = m_poServInfo->m_iServProductOffset;

    ServProductInfo * pServProduct = *m_poServProductData;

    while (k) {
        if (pServProduct[k].m_lServProductID == lServProductID) {
            servProduct.m_poServProductInfo = pServProduct + k;
            servProduct.m_poServInfo = m_poServ + pServProduct[k].m_iServOffset;
            return true;
        }

        k = pServProduct[k].m_iNextOffset;
    }

    return false;
}
//##ModelId=42414E330142
bool Serv::getServProductIteration(ServProductIteration &servProductIteration, char *sdate, bool isall)
{
	if(!m_poServInfo){
		return false;
	}
    servProductIteration.m_bAll = isall;
    if (sdate)
        strcpy (servProductIteration.m_sTime, sdate);
    else
        strcpy (servProductIteration.m_sTime, m_sTime);
    
    servProductIteration.m_iCurOffset = m_poServInfo->m_iServProductOffset;

    return servProductIteration.m_iCurOffset ? true : false;
}

//##ModelId=4242BA380063
bool Serv::belongOrg(int orgid)
{
    int org = getOrgID ();
    if (org == -1) return false;

    return m_poOrgMgr->getBelong (org, orgid);    
}

bool ServGroupIteration::next(long & lGroupID)
{
    ServGroupBaseInfo * pGroupBase = (ServGroupBaseInfo *)(*m_poServGroupBaseData);

    if (m_iCurOffset) {
        lGroupID = pGroupBase[m_iCurOffset].m_lGroupID;
        m_iCurOffset++;

        if (m_iCurOffset == m_poServGroupBaseData->getCount ()+1) {
            m_iCurOffset = 0;
        }
        return true;
    }

    return false;
}

bool CustIteration::next(long & lCustID)
{
    while (m_iCurOffset) {
        if ((m_poCust[m_iCurOffset].m_sState[2]|0x20) == 'a') {
            lCustID = m_poCust[m_iCurOffset].m_lCustID;
            break;
        } else {
            m_iCurOffset++;
            if (m_iCurOffset == (m_poCustData->getCount ()+1)) {
                m_iCurOffset = 0;
            }
        }

    }

    if (m_iCurOffset) {
        m_iCurOffset++;
        if (m_iCurOffset == (m_poCustData->getCount ()+1)) {
            m_iCurOffset = 0;
        }
        return true;
    }

    return false;
}

//##ModelId=42414F0F010C
bool ServIteration::next(Serv &serv)
{
    if (!m_iCurOffset)
        return false;

    strcpy (serv.m_sTime, m_sTime);
    serv.m_poServInfo = m_poServ + m_iCurOffset;
    serv.m_poServProductInfo = m_poServProduct + m_poServ[m_iCurOffset].m_iServProductOffset;
    serv.m_poServAcctInfo = m_poServAcct + m_poServ[m_iCurOffset].m_iServAcctOffset;

    unsigned int k;
    if (m_poCustIndex->get(m_poServ[m_iCurOffset].m_lCustID, &k)) {
        serv.m_poCustInfo = m_poCust + k;
    } else {
        serv.m_poCustInfo = 0;
    }

    m_iCurOffset++;

    if (m_iCurOffset == m_poServData->getCount()+1)
        m_iCurOffset = 0;

    return true;
}

bool ServIteration::nextByOrg(Serv &serv)
{
    if (!m_iCurOffset) return false;

    ServLocationInfo *p = (ServLocationInfo *)(*m_poServLocationData);

    unsigned int l = (unsigned int)(p[m_iCurOffset].m_lServID);

    strcpy (serv.m_sTime, m_sTime);
    serv.m_poServInfo = m_poServ + l;
    serv.m_poServProductInfo = m_poServProduct + m_poServ[l].m_iServProductOffset;
    serv.m_poServAcctInfo = m_poServAcct + m_poServ[l].m_iServAcctOffset;

    unsigned int k;
    if (m_poCustIndex->get(m_poServ[l].m_lCustID, &k)) {
        serv.m_poCustInfo = m_poCust + k;
    } else {
        serv.m_poCustInfo = 0;
    }

    m_iLastOffset = m_iCurOffset;

    m_iCurOffset = p[m_iCurOffset].m_iOrgNext;

    return true;
}

bool ServIteration::belongCurServLocation(char *sTime)
{
    ServLocationInfo *p = (ServLocationInfo *)(*m_poServLocationData);

    p += m_iLastOffset;

    return (strcmp (sTime, p->m_sEffDate) >=0 &&
                strcmp (sTime, p->m_sExpDate) < 0);
}

//##ModelId=42414F1C02D8
bool ServProductIteration::next(ServProduct &servProduct)
{
    if (!m_iCurOffset)
        return false;

    

    if (!m_bAll) {
        while (m_iCurOffset) {
            if (strcmp(m_poServProduct[m_iCurOffset].m_sEffDate, m_sTime) > 0 ||
            strcmp(m_sTime, m_poServProduct[m_iCurOffset].m_sExpDate) >= 0) {
                
                m_iCurOffset = m_poServProduct[m_iCurOffset].m_iNextOffset;
            } else {
                break;
            }
        }
        
        if (!m_iCurOffset)
            return false;
    }

    servProduct.m_poServProductInfo = m_poServProduct+m_iCurOffset;
    servProduct.m_poServInfo = m_poServ+m_poServProduct[m_iCurOffset].m_iServOffset;
    m_iCurOffset = m_poServProduct[m_iCurOffset].m_iNextOffset;
    return true;
}


//##ModelId=42414DB202A5
bool UserInfoInterface::getServIteration(ServIteration &servIteration)
{
    unsigned int k = m_poServData->getCount();
    if (!k) return false;
    
    Date d;
    strcpy (servIteration.m_sTime, d.toString());

    servIteration.m_iCurOffset = 1;
    
    return true;
}

bool UserInfoInterface::getServIterByOrg(ServIteration &servIter, int iOrgID)
{
    unsigned int k;

    if (!m_poServLocationIndex->get (iOrgID, &k)) return false;

    servIter.m_iCurOffset = k;

    return true;
}

bool UserInfoInterface::getServGroupIteration(ServGroupIteration &groupIteration)
{
    unsigned int k = m_poServGroupBaseData->getCount();
    if (!k) return false;
    
    groupIteration.m_iCurOffset = 1;
    
    return true;

}

bool UserInfoInterface::getCustIteration(CustIteration &custIteration)
{
    unsigned int k = m_poCustData->getCount();
    if (!k) return false;
    
    custIteration.m_iCurOffset = 1;
    
    return true;

}

int UserInfoInterface::getAcctCreditAmount(long lAcctID)
{
    unsigned int k;
    
    if (!m_poAcctIndex->get (lAcctID, &k)) return 0;

    AcctInfo * pAcctInfo = *(m_poAcctData);
    return pAcctInfo[k].m_iCreditAmount;
}

int UserInfoInterface::getAcctCreditGrade(long lAcctID)
{
    unsigned int k;
    
    if (!m_poAcctIndex->get (lAcctID, &k)) return 0;

    AcctInfo * pAcctInfo = *(m_poAcctData);
    return pAcctInfo[k].m_iCreditGrade;
}

bool UserInfoInterface::getAcctBelongOrg(long lAcctID, int iOrgID)
{
    unsigned int k, l;

    if (!m_poServAcctIndex->get (lAcctID, &l)) return false;

    while (l) {
        if (m_poServIndex->get (m_poServAcct[l].m_lServID, &k)) {
            k = m_poServ[k].m_iLocationOffset;

            if (k) {
                return m_poOrgMgr->getBelong (m_poServLocation[k].m_iOrgID, iOrgID);
            }
        }

        l = m_poServAcct[l].m_iNextOffset;

    } 

    return false;
}

bool UserInfoInterface::getCustBelongOrg(long lCustID, int iOrgID)
{
    unsigned int k, l;

    if (!(m_poCustIndex->get(lCustID, &k))) return false;

        k = m_poCust[k].m_iServOffset;
        while (k) {
        l = m_poServ[k].m_iLocationOffset;

        if (l) {
            return m_poOrgMgr->getBelong (m_poServLocation[l].m_iOrgID, iOrgID);
        }

        k = m_poServ[k].m_iCustNext; 
        }

    return false;
}

bool UserInfoInterface::getServGroupBelongOrg(int iServGroupID, int iOrgID)
{
    unsigned int k, l;

        if (!m_poServGroupBaseIndex->get (iServGroupID, &k)) return false;

        ServGroupBaseInfo * pGroupBase = (ServGroupBaseInfo *)(*m_poServGroupBaseData);
        ServGroupInfo * pGroup = (ServGroupInfo *)(*m_poServGroupData);

        l = pGroupBase[k].m_iServOffset;

        while (l) {
        if (m_poServIndex->get (pGroup[l].m_lServID, &k)) {
                        k = m_poServ[k].m_iLocationOffset;

                        if (k) {
                                return m_poOrgMgr->getBelong (m_poServLocation[k].m_iOrgID, iOrgID);
                        }
                }

                l = pGroup[l].m_iGroupNext;
        }

        return false;
}

bool UserInfoInterface::getOfferInsIteration(OfferInsIteration &offerinsiter)
{
    unsigned int k = m_poProdOfferInsData->getCount();
    if (!k) return false;
    

    offerinsiter.m_iCurOffset = 1;
    
    return true;
}

//##ModelId=4248BC6B013E
bool Serv::belongServGroup(long groupid, char *sdate)
{
	if(!m_poServInfo){
		return false;
	}
    unsigned int k = m_poServInfo->m_iGroupOffset;

    if (!sdate) sdate = m_sTime;

    while (k) {
        if (groupid == m_poServGroup[k].m_lGroupID) {
            if (strcmp(sdate, m_poServGroup[k].m_sEffDate) >=0 &&
                strcmp(sdate, m_poServGroup[k].m_sExpDate) < 0)
                return true;
        } 

        if (groupid < m_poServGroup[k].m_lGroupID)
            break; //##sorted by m_lGroupID

        k = m_poServGroup[k].m_iNextOffset;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iGroupOffset;

    Date date_sDate(sdate);

    while (k) {
        if (groupid == m_poServGroup[k].m_lGroupID) {
            Date date_EffDate(m_poServGroup[k].m_sEffDate);
            long p = date_EffDate.diffSec(date_sDate);
            if (p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs)
            {
                return true;
            }
            else {
                Date date_ExpDate(m_poServGroup[k].m_sExpDate);
                p = date_sDate.diffSec(date_ExpDate);
                if (p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs)
                {
                    return true;
                }
            }
        } 

        if (groupid < m_poServGroup[k].m_lGroupID)
            break; //##sorted by m_lGroupID

        k = m_poServGroup[k].m_iNextOffset;
    }

    #endif

    return false;
}

//##ModelId=4248BC7600EA
bool Serv::belongCustGroup(long groupid)
{
	if(!m_poCustInfo){
		return false;
	}
    unsigned int k = m_poCustInfo->m_iGroupOffset;
    while (k) {
        if (groupid == m_poCustGroup[k].m_lGroupID) {
            /*if (strcmp(m_sTime, m_poCustGroup[k].m_sEffDate) >=0 &&
                strcmp(m_sTime, m_poCustGroup[k].m_sExpDate) < 0)*/
                return true;
        } 

        if (groupid < m_poCustGroup[k].m_lGroupID)
            return false; //##sorted by m_lGroupID

        k = m_poCustGroup[k].m_iNextOffset;
    }

    return false;
}


//##ModelId=424A47DC01D0
BillingCycleMgr * UserInfoReader::m_poBillingCycleMgr = 0;

//##ModelId=424A47DC01EE
AcctItemMgr * UserInfoReader::m_poAcctItemMgr = 0;

//##ModelId=424A47DC0202
OrgMgr * UserInfoReader::m_poOrgMgr = 0;

StateConvertMgr * UserInfoReader::m_poStateConvertMgr = 0;

NetworkPriorityMgr * UserInfoReader::m_poNetworkMgr = 0;
    
OfferMgr * UserInfoReader::m_poOfferMgr = 0;

//##ModelId=42525763028B
UserInfoReader::UserInfoReader() : UserInfoBase ()
{
    if (!m_poBillingCycleMgr) {
        m_poBillingCycleMgr = new BillingCycleMgr ();
        if (!m_poBillingCycleMgr) THROW(MBC_UserInfoReader+2);
    }

    if (!m_poAcctItemMgr) {
        m_poAcctItemMgr = new AcctItemMgr ();
        if (!m_poAcctItemMgr) THROW(MBC_UserInfoReader+2);
    }

    if (!m_poOrgMgr) {
        m_poOrgMgr = new OrgMgr ();
        if (!m_poOrgMgr) THROW(MBC_UserInfoReader+2);
    }

    if (!m_poStateConvertMgr) {
        m_poStateConvertMgr = new StateConvertMgr ();
        if (!m_poStateConvertMgr) THROW(MBC_UserInfoReader+2);
    }
    if(!m_poNetworkMgr) {
        m_poNetworkMgr = new NetworkPriorityMgr();
        if (!m_poNetworkMgr) THROW(MBC_UserInfoReader+2);
#ifdef PRIVATE_MEMORY
        m_poNetworkMgr->load();
#endif
    }
    if(!m_poOfferMgr){
		m_poOfferMgr = new OfferMgr;
		if(!m_poOfferMgr)
        {
            THROW(MBC_UserInfoReader+2);
        }
        m_poOfferMgr->load();
        m_poOfferMgr->loadSpecialAttr();
	}
	  	
    #ifdef USERINFO_OFFSET_ALLOWED
    if(m_offsetSecs==-1){
      loadOffsetSecs();
    }
    #endif
}

UserInfoReader::UserInfoReader(int iMode) : UserInfoBase ()
{
	if(iMode == USER_INFO_BASE)
	{
		if(!m_poNetworkMgr) {
			m_poNetworkMgr = new NetworkPriorityMgr();
			if (!m_poNetworkMgr) THROW(MBC_UserInfoReader+2);
#ifdef PRIVATE_MEMORY
			m_poNetworkMgr->load();
#endif
		}
		if(!m_poOfferMgr){
			m_poOfferMgr = new OfferMgr;
			if(!m_poOfferMgr)
			{
				THROW(MBC_UserInfoReader+2);
			}
			m_poOfferMgr->load();
			m_poOfferMgr->loadSpecialAttr();
		}
	}
	if(iMode == USRE_INFO_NOITEMGROUP)
	{

    	if (!m_poBillingCycleMgr) {
 	       m_poBillingCycleMgr = new BillingCycleMgr ();
   	     if (!m_poBillingCycleMgr) THROW(MBC_UserInfoReader+2);
	    }

	    if (!m_poAcctItemMgr) {
	        m_poAcctItemMgr = new AcctItemMgr (USRE_INFO_NOITEMGROUP);
	        if (!m_poAcctItemMgr) THROW(MBC_UserInfoReader+2);
	    }

	    if (!m_poOrgMgr) {
 	       m_poOrgMgr = new OrgMgr ();
	        if (!m_poOrgMgr) THROW(MBC_UserInfoReader+2);
	    }

 	   if (!m_poStateConvertMgr) {
  	      m_poStateConvertMgr = new StateConvertMgr ();
 	       if (!m_poStateConvertMgr) THROW(MBC_UserInfoReader+2);
	    }
 	   if(!m_poNetworkMgr) {
 	       m_poNetworkMgr = new NetworkPriorityMgr();
 	       if (!m_poNetworkMgr) THROW(MBC_UserInfoReader+2);
 	       m_poNetworkMgr->load();
	    }
 	   if(!m_poOfferMgr){
 	   		m_poOfferMgr = new OfferMgr;
 	   		if(!m_poOfferMgr)
 	   		{
 	   		THROW(MBC_UserInfoReader+2);
 	   		}
				m_poOfferMgr->load();
 	   		m_poOfferMgr->loadSpecialAttr();
 	   }
	}
    #ifdef USERINFO_OFFSET_ALLOWED
    if(m_offsetSecs==-1){
      loadOffsetSecs();
    }
    #endif
}

#ifdef USERINFO_OFFSET_ALLOWED

int UserInfoReader::m_offsetSecs =  -1;

int UserInfoReader::m_beginOffsetSecs = 0;

void UserInfoReader::loadOffsetSecs()
{
    char *pt;

    if ((pt = getenv ("PROCESS_ID")) == NULL) {
        m_offsetSecs = 0;
        return;
    }
    int temp_proc = atoi (pt);
    DEFINE_QUERY(qry);
    char sql[128];
    sprintf(sql,"select offset_mins*60 from b_cfg_userinfo_offset a,wf_process b where b.process_id = %d and b.billflow_id = a.billflow_id", temp_proc);
    qry.setSQL (sql);
    qry.open ();

    if(qry.next ()) {
        m_offsetSecs= qry.field(0).asInteger () ;
    } else {
        m_offsetSecs = 0;

        }
    
    qry.close ();
}
#endif

long UserInfoReader::execOperation(int iOperation,StdEvent * poEvent,int iAttrID)
{
	Value value;
	G_CURATTRID = iAttrID;
	Process::m_poOperationMgr->execute (iOperation, poEvent);
	IDParser::getValue ((EventSection*) poEvent, "51900", &value);
	return value.m_lValue;
}


char * Serv::getAttr(int attrid, char *sDate)
{
	if(!m_poServInfo){
		return 0;
	}
    unsigned int k = m_poServInfo->m_iAttrOffset;

    ServAttrInfo *pServAttr = (ServAttrInfo *)(*m_poServAttrData);
    while (k && pServAttr[k].m_iAttrID < attrid ) {
        k = pServAttr[k].m_iNextOffset;
    }

    if (!k || !(pServAttr[k].m_iAttrID==attrid)) return 0;

    
    if (!sDate) sDate = m_sTime;

    while (k && (pServAttr[k].m_iAttrID==attrid)) {

        if ( strcmp(pServAttr[k].m_sEffDate, sDate) <=0 &&
            strcmp(pServAttr[k].m_sExpDate, sDate) > 0 ) {
            return pServAttr[k].m_sValue;    
        }

        k = pServAttr[k].m_iNextOffset;
    }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iAttrOffset;

    while (k && pServAttr[k].m_iAttrID < attrid ) {
        k = pServAttr[k].m_iNextOffset;
    }

    Date date_sDate(sDate);

    while (k && (pServAttr[k].m_iAttrID==attrid)) {
        
        Date date_EffDate(pServAttr[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs  ) {
            return pServAttr[k].m_sValue;
        }
        else {
            Date date_ExpDate(pServAttr[k].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
            if(p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs  ) {
                return pServAttr[k].m_sValue;
            }
        }
        k = pServAttr[k].m_iNextOffset;
    }

    #endif


    return 0;
}

char * Serv::getAttrNew(int attrid, char * pDate, char * m_sValue, int iSize)
{
	if(!m_poServInfo){
		return 0;
	}
	char sDate[16]={0};
	unsigned int k = m_poServInfo->m_iAttrOffset;

	if((pDate)&&(!pDate[0]))
	{
		pDate =m_sTime;
	}

	ServAttrInfo *pServAttr = (ServAttrInfo *)(*m_poServAttrData);
	while (k && pServAttr[k].m_iAttrID < attrid ) {
		k = pServAttr[k].m_iNextOffset;
	}

	if (!pDate) {  // sDate为空表示获取最新的属性值
		while (k && (pServAttr[k].m_iAttrID==attrid)) {
			if(strcmp(pServAttr[k].m_sEffDate, sDate) >= 0) {
				strcpy(sDate, pServAttr[k].m_sEffDate);
				strncpy(m_sValue, pServAttr[k].m_sValue, iSize);
			}

			k = pServAttr[k].m_iNextOffset;
		}
	}
	else {  // sDate不为空表示获取系统当前时间/话单时间的属性值
		while (k && (pServAttr[k].m_iAttrID==attrid)) {
			if ( strcmp(pServAttr[k].m_sEffDate, pDate) <=0 &&
				strcmp(pServAttr[k].m_sExpDate, pDate) > 0 ) {
					strncpy(m_sValue, pServAttr[k].m_sValue, iSize);
					return m_sValue;
			}

			k = pServAttr[k].m_iNextOffset;
		}
	}

	return m_sValue;
}


//##ModelId=42414E99010D
char * ServProduct::getAttr(int attrid, char *sDate)
{
    unsigned int k = m_poServProductInfo->m_iAttrOffset;

    ServProductAttrInfo *pServProductAttr = (ServProductAttrInfo *)(*m_poServProductAttrData);
    while (k && pServProductAttr[k].m_iAttrID < attrid ) {
        k = pServProductAttr[k].m_iNextOffset;
    }

    if (!k || !(pServProductAttr[k].m_iAttrID==attrid)) return 0;

    while (k && (pServProductAttr[k].m_iAttrID==attrid) ) { 
        if (sDate) {
            if (strcmp(pServProductAttr[k].m_sEffDate, sDate) <=0 &&
                strcmp(pServProductAttr[k].m_sExpDate, sDate) > 0 ) {
                return pServProductAttr[k].m_sValue;
            }
        } else {
            return pServProductAttr[k].m_sValue;
        }

        k = pServProductAttr[k].m_iNextOffset;
    } 

    #ifdef USERINFO_OFFSET_ALLOWED

    if (sDate) return 0;

    k = m_poServProductInfo->m_iAttrOffset;

    while (k && pServProductAttr[k].m_iAttrID < attrid ) {
        k = pServProductAttr[k].m_iNextOffset;
    }

    Date date_sDate(sDate);
    while (k && (pServProductAttr[k].m_iAttrID==attrid) ) { 
        Date date_EffDate(pServProductAttr[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
        if (p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs ) {
            return pServProductAttr[k].m_sValue;
        }
        else{
            Date date_ExpDate(pServProductAttr[k].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
            if (p >= UserInfoReader::m_beginOffsetSecs
                    &&p <= UserInfoReader::m_offsetSecs ) {
                return pServProductAttr[k].m_sValue;
            }
        }

        k = pServProductAttr[k].m_iNextOffset;
    } 

    #endif

    return 0;
}

bool Serv::isAttrValue(int attrid, char *sValue, char *sDate)
{
	if(!m_poServInfo){
		return false;
	}
        unsigned int k = m_poServInfo->m_iAttrOffset;

        ServAttrInfo *pServAttr = (ServAttrInfo *)(*m_poServAttrData);
        while (k && pServAttr[k].m_iAttrID < attrid ) {
                k = pServAttr[k].m_iNextOffset;
        }

        if (!k || !(pServAttr[k].m_iAttrID==attrid)) return false;

        if (!sDate) sDate = m_sTime;

        while (k && (pServAttr[k].m_iAttrID==attrid)) {

                if ( strcmp(pServAttr[k].m_sEffDate, sDate) <=0 &&
                        strcmp(pServAttr[k].m_sExpDate, sDate) > 0 &&
                        !strcmp(pServAttr[k].m_sValue, sValue) ) {

                        return true;
                }

                k = pServAttr[k].m_iNextOffset;
        }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iAttrOffset;

    while (k && pServAttr[k].m_iAttrID < attrid ) {
                k = pServAttr[k].m_iNextOffset;
        }

    Date date_sDate(sDate);

    while (k && (pServAttr[k].m_iAttrID==attrid)) {

        Date date_EffDate(pServAttr[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
    if ( p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs &&
                      !strcmp(pServAttr[k].m_sValue, sValue) ) {

                      return true;
     }
     else{
         Date date_ExpDate(pServAttr[k].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
           if ( p >= UserInfoReader::m_beginOffsetSecs
                    &&p <= UserInfoReader::m_offsetSecs &&
                  !strcmp(pServAttr[k].m_sValue, sValue) ) {

                      return true;
         }
     }
             

              k = pServAttr[k].m_iNextOffset;
        }

    #endif

        return false;
}

bool Serv::isAttrValueMax(int attrid, char *sValue, char *sDate)
{
	if(!m_poServInfo){
		return false;
	}
        unsigned int k = m_poServInfo->m_iAttrOffset;

        ServAttrInfo *pServAttr = (ServAttrInfo *)(*m_poServAttrData);
        while (k && pServAttr[k].m_iAttrID < attrid ) {
                k = pServAttr[k].m_iNextOffset;
        }

        if (!k || !(pServAttr[k].m_iAttrID==attrid)) return false;

        if (!sDate) sDate = m_sTime;

        while (k && (pServAttr[k].m_iAttrID==attrid)) {

                if ( strcmp(pServAttr[k].m_sEffDate, sDate) <=0 &&
                        strcmp(pServAttr[k].m_sExpDate, sDate) > 0 ) {

                        char *p = pServAttr[k].m_sValue;
                        char *t = sValue;

                        while (*p) {
                            if (*t != *p) return false;
                            p++; t++;
                        }

                        return true;
                }

                k = pServAttr[k].m_iNextOffset;
        }

    #ifdef USERINFO_OFFSET_ALLOWED

    k = m_poServInfo->m_iAttrOffset;

    while (k && pServAttr[k].m_iAttrID < attrid ) {
                k = pServAttr[k].m_iNextOffset;
        }

    Date date_sDate(sDate);

    while (k && (pServAttr[k].m_iAttrID==attrid)) {

        Date date_EffDate(pServAttr[k].m_sEffDate);
        long p = date_EffDate.diffSec(date_sDate);
              if ( p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs ) {

            char *p = pServAttr[k].m_sValue;
                        char *t = sValue;

                        while (*p) {
                            if (*t != *p) return false;
                            p++; t++;
                        }

                     return true;
    }
    else{
        Date date_ExpDate(pServAttr[k].m_sExpDate);
            p = date_sDate.diffSec(date_ExpDate);
              if ( p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs ) {

            char *p = pServAttr[k].m_sValue;
                        char *t = sValue;

                        while (*p) {
                            if (*t != *p) return false;
                            p++; t++;
                        }

                     return true;
        }
      }

              k = pServAttr[k].m_iNextOffset;
        }

    #endif

        return false;
}

int Serv::getAttrNumber(int attrid, char *sDate)
{
	if(!m_poServInfo){
		return 0;
	}
        unsigned int k = m_poServInfo->m_iAttrOffset;

        ServAttrInfo *pServAttr = (ServAttrInfo *)(*m_poServAttrData);
        while (k && pServAttr[k].m_iAttrID < attrid ) {
                k = pServAttr[k].m_iNextOffset;
        }

        if (!k || !(pServAttr[k].m_iAttrID==attrid)) return 0;

        if (!sDate) sDate = m_sTime;

        int iRet (0);

        while (k && (pServAttr[k].m_iAttrID==attrid)) {

                if ( strcmp(pServAttr[k].m_sEffDate, sDate) <=0 &&
                        strcmp(pServAttr[k].m_sExpDate, sDate) > 0 ) {

                        iRet++;

                }

                k = pServAttr[k].m_iNextOffset;
        }

    #ifdef USERINFO_OFFSET_ALLOWED

    if ( iRet == 0 ) {
        k = m_poServInfo->m_iAttrOffset;
        while (k && pServAttr[k].m_iAttrID < attrid ) {
                    k = pServAttr[k].m_iNextOffset;
            }

        Date date_sDate(sDate);

        while (k && (pServAttr[k].m_iAttrID==attrid)) {

            Date date_EffDate(pServAttr[k].m_sEffDate);
            long p = date_EffDate.diffSec(date_sDate);
      if ( p >= UserInfoReader::m_beginOffsetSecs
                &&p <= UserInfoReader::m_offsetSecs ) {

                iRet++;
       }
       else {
            Date date_ExpDate(pServAttr[k].m_sExpDate);
               p = date_sDate.diffSec(date_ExpDate);
         if ( p >= UserInfoReader::m_beginOffsetSecs
                    &&p <= UserInfoReader::m_offsetSecs ) {
                    iRet++;
               }
       }

                  k = pServAttr[k].m_iNextOffset;
            }

    }
    #endif

        return iRet;
}

//##ModelId=42578E4F0129
int &UserInfoAttr::getAttrID()
{
    return m_iAttrID;
}

//##ModelId=42578E570383
char *UserInfoAttr::getAttrValue()
{
    return m_sAttrValue;
}

//##ModelId=42578E60015F
char *UserInfoAttr::getEffDate()
{
    return m_sEffDate;
}

//##ModelId=42578E6603B7
char *UserInfoAttr::getExpDate()
{
    return m_sExpDate;
}

int Serv::getAttrs(vector<UserInfoAttr> &ret)
{
	if(!m_poServInfo){
		return 0;
	}
        unsigned int k = m_poServInfo->m_iAttrOffset;
        int i = 0;

        UserInfoAttr temp;

        ServAttrInfo *pServAttr = (ServAttrInfo *)(*m_poServAttrData);

        while (k) {
                temp.getAttrID() = pServAttr[k].m_iAttrID;
                strcpy (temp.getAttrValue(), pServAttr[k].m_sValue);
                strcpy (temp.getEffDate(), pServAttr[k].m_sEffDate);
                strcpy (temp.getExpDate(), pServAttr[k].m_sExpDate);

                ret.push_back (temp);
                i++;

                k = pServAttr[k].m_iNextOffset;
        }

        return i;
}

//##ModelId=42578D9C03AC
int ServProduct::getAttrs(vector<UserInfoAttr> &ret)
{
    unsigned int k = m_poServProductInfo->m_iAttrOffset;
    int i = 0;

    UserInfoAttr temp;

    ServProductAttrInfo *pServProductAttr = (ServProductAttrInfo *)(*m_poServProductAttrData);

    while (k) {
        temp.getAttrID() = pServProductAttr[k].m_iAttrID;
        strcpy (temp.getAttrValue(), pServProductAttr[k].m_sValue);
        strcpy (temp.getEffDate(), pServProductAttr[k].m_sEffDate);
        strcpy (temp.getExpDate(), pServProductAttr[k].m_sExpDate);
                
        ret.push_back (temp);
        i++;

        k = pServProductAttr[k].m_iNextOffset;
    }

    return i;
}

//##ModelId=425A2BD10371
char *Serv::getRentDate()
{
	if(m_poServInfo){
		return m_poServInfo->m_sRentDate;
	}
	
	return 0;
}

//##ModelId=425A2BD902E6
char *Serv::getCreateDate()
{
	if(m_poServInfo){
		return m_poServInfo->m_sCreateDate;
	}
	
	return 0;
}

//##ModelId=425A2BEE0264
char *Serv::getNscDate()
{
	if(m_poServInfo){
		return m_poServInfo->m_sNscDate;
	}
	
	return 0;
}

//##ModelId=425A2BFD0374
char *Serv::getStateDate(char * sDate)
{
    char *sTemp;

    if (!sDate) {
        sTemp = m_sTime;
    } else {
        sTemp = sDate;
    }
	if(!m_poServInfo){
		return 0;
	}

/*
    昆明这里SERV表里面的STATE_DATE不是最后的状态变更时间，而是用户资料的变更时间
    2005-09-24 xueqt

    从模型上来看,应该从SERV_STATE_ATTR里面取就足够了

    if (strcmp(sTemp, m_poServInfo->m_sStateDate)>=0) {
        return m_poServInfo->m_sStateDate;
    }
*/

    unsigned k = m_poServInfo->m_iStateOffset;
    
  /*
    while (k && strcmp(sTemp, m_poServState[k].m_sEffDate)<0) {
        k = m_poServState[k].m_iNextOffset;
    }

    if (k) {
                return m_poServState[k].m_sEffDate;
        }
   */
   
    while(k) {
      if(strcmp(m_poServState[k].m_sEffDate,sTemp)<=0 && strcmp(m_poServState[k].m_sExpDate, sTemp)>0) {
        return m_poServState[k].m_sEffDate;
      }

      k = m_poServState[k].m_iNextOffset;
    }
       
    return 0; 
}

//##ModelId=426F002B014E
ServInfo *Serv::getServInfo()
{
    return m_poServInfo;
}

//##ModelId=4271EA030243
ServProductInfo *ServProduct::getServProductInfo()
{
    return m_poServProductInfo;
}

 //##ModelId=4271EA3D0051
CustInfo * Serv::getCustInfo()
{
    return m_poCustInfo;
}

void Serv::setTime(char *sDate)
{
    strcpy(m_sTime, sDate);
}

int Serv::getFreeType()
{
	if(m_poServInfo){
		return m_poServInfo->m_iFreeType;
	}
	
	return 0;
}


bool Serv::isServAttrChanged(const char *sBeginTime,const char *sEndTime)
{	
	unsigned int k = m_poServInfo->m_iAttrOffset;
	ServAttrInfo * pServAttr = (ServAttrInfo *)(*m_poServAttrData);
	
	while(k) {
		if( ( strcmp(pServAttr[k].m_sEffDate, sBeginTime) >= 0 
			   && strcmp(pServAttr[k].m_sEffDate, sEndTime) <= 0 )
			||(strcmp(pServAttr[k].m_sExpDate, sBeginTime) >= 0
			   && strcmp(pServAttr[k].m_sExpDate, sEndTime) <= 0)
			) {
		  	return true;
		 }
		
		k = pServAttr[k].m_iNextOffset;
	}
	
	return false;
}


bool Serv::isServStateAttrChanged(const char *sBeginTime,const char *sEndTime)
{
	unsigned int k = m_poServInfo->m_iStateOffset;

	while(k) {
		if( (strcmp(m_poServState[k].m_sEffDate, sBeginTime) >= 0 
			     && strcmp(m_poServState[k].m_sEffDate, sEndTime) <= 0 ) ||
			   (strcmp(m_poServState[k].m_sExpDate, sBeginTime) >= 0 
			    && strcmp(m_poServState[k].m_sExpDate, sEndTime) <= 0 )
		  ) {
			  return true;
     	}
		
		k = m_poServState[k].m_iNextOffset;	
   }
   
   return false;
}

bool Serv::isServTypeChanged(const char *sBeginTime,const char *sEndTime)
{
	int j;
	unsigned int k = m_poServInfo->m_iServTypeOffset;
	
	if(!g_bLoadGenCycleFee) loadGenCycleFeeIndex();
		
	ServTypeInfo * pServType = (ServTypeInfo *)(*m_poServTypeData);
	
	while(k) {
		if( m_poGenCycleFeeIndex.get(pServType[k].m_iProductID,&j) ) {
			
			if( ( strcmp(pServType[k].m_sEffDate, sBeginTime) >= 0
				 && strcmp(pServType[k].m_sEffDate, sEndTime) <= 0 ) || 
			   ( strcmp(pServType[k].m_sExpDate, sBeginTime) >= 0
				 && strcmp(pServType[k].m_sExpDate, sEndTime) <= 0 )
			   ) {
				return true;
			 }
		}
		
		k = pServType[k].m_iNextOffset;
	}

	return false;
}

bool Serv::isServProductChanged(const char *sBeginTime,const char *sEndTime)
{
	unsigned int k = m_poServInfo->m_iServProductOffset;
	
	ServProductInfo * pServProduct= (ServProductInfo *)(*m_poServProductData);

  int j;
	unsigned int i;
	  
	if(!g_bLoadGenCycleFee) loadGenCycleFeeIndex();
    	
	while(k) {
		if( m_poGenCycleFeeIndex.get(pServProduct[k].m_iProductID,&j) ) {		  
			if( ( strcmp(pServProduct[k].m_sEffDate, sBeginTime) >= 0
				   && strcmp(pServProduct[k].m_sEffDate, sEndTime) <= 0 ) || 
				  ( strcmp(pServProduct[k].m_sExpDate, sBeginTime) >= 0
				   && strcmp(pServProduct[k].m_sExpDate, sEndTime) <= 0 )
				  ) {
				      return true;
			   }
		}
	  
		k = pServProduct[k].m_iNextOffset;
	}

	return false;
}


bool Serv::isServProductAttrChanged(const char *sBeginTime,const char *sEndTime)
{
	int j;
	unsigned int l;
	
	unsigned int k = m_poServInfo->m_iServProductOffset;
	ServProductInfo * pServProduct= (ServProductInfo *)(*m_poServProductData);
	ServProductAttrInfo * pServProductAttr = (ServProductAttrInfo *)(*m_poServProductAttrData);
	
	if(!g_bLoadGenCycleFee) loadGenCycleFeeIndex();
	
	while(k) {
      if(m_poGenCycleFeeIndex.get(pServProduct[k].m_iProductID,&j)) {
		    l = pServProduct[k].m_iAttrOffset;
		    while(l) {
			    if( (strcmp(pServProductAttr[l].m_sEffDate,sBeginTime) >= 0 
				       && strcmp(pServProductAttr[l].m_sEffDate,sEndTime) <= 0)
				     ||(strcmp(pServProductAttr[l].m_sExpDate,sBeginTime) >= 0
				     && strcmp(pServProductAttr[l].m_sExpDate,sEndTime) <= 0)
				    ) {
				   return true;
			    } 
			    l = pServProductAttr[l].m_iNextOffset;
	    	}
		  }
		  
		k = pServProduct[k].m_iNextOffset;
	}
	
	return false;
}


bool Serv::isProdOffer_Ins_Attr_Detail_Changed(const char *sBeginTime,const char *sEndTime)
{
  vector<ProdOfferIns *> vOfferIns;
  vector<OfferDetailIns *> vDetailIns;
	
  int i;
  unsigned int k = m_poServInfo->m_iOfferDetailOffset;
	
  while(k) {
    vDetailIns.push_back(m_poOfferDetailIns + k);
    vOfferIns.push_back( m_poProdOfferIns + m_poOfferDetailIns[k].m_iParentOffset);

    k = m_poOfferDetailIns[k].m_iNextProdOfferOffset;
  }
	
  for(i = 0; i< vOfferIns.size(); ++i) {
    if(m_poOfferDetailInsOfferInsIndex->get(vOfferIns[i]->m_lProdOfferInsID,&k)) {
      while (k) {
        vDetailIns.push_back(m_poOfferDetailIns + k);
        vOfferIns.push_back( m_poProdOfferIns + m_poOfferDetailIns[k].m_iParentOffset );
        k = m_poOfferDetailIns[k].m_iNextProdOfferOffset;
      }
    }
  }
	
  k = m_poServInfo->m_iBaseOfferOffset;
  while(k){
    vDetailIns.push_back(m_poOfferDetailIns + k);
    vOfferIns.push_back(m_poProdOfferIns + m_poOfferDetailIns[k].m_iParentOffset);
    k = m_poOfferDetailIns[k].m_iNextProdOfferOffset;
  }
	
  for(i = 0; i < vDetailIns.size(); ++i) {
 	  if( ( strcmp(vDetailIns[i]->m_sEffDate, sBeginTime) >= 0
         && strcmp(vDetailIns[i]->m_sEffDate, sEndTime) <= 0 ) || 
        ( strcmp(vDetailIns[i]->m_sExpDate, sBeginTime) >= 0
         && strcmp(vDetailIns[i]->m_sExpDate, sEndTime) <= 0 )
       ) {	
       return true;
     }
  }
		
	for(i = 0; i < vOfferIns.size(); ++i) {
		if( ( strcmp(vOfferIns[i]->m_sEffDate, sBeginTime) >= 0
			   && strcmp(vOfferIns[i]->m_sEffDate, sEndTime) <= 0 ) || 
			  ( strcmp(vOfferIns[i]->m_sExpDate, sBeginTime) >= 0
				 && strcmp(vOfferIns[i]->m_sExpDate, sEndTime) <= 0 )
			) {	
			  	return true;
		}
		
		k = vOfferIns[i]->m_iOfferAttrOffset;	
		while(k) {
			if( ( strcmp(m_poProdOfferInsAttr[k].m_sEffDate, sBeginTime) >= 0
				 && strcmp(m_poProdOfferInsAttr[k].m_sEffDate, sEndTime) <= 0 ) || 
			    ( strcmp(m_poProdOfferInsAttr[k].m_sExpDate, sBeginTime) >= 0
				 && strcmp(m_poProdOfferInsAttr[k].m_sExpDate, sEndTime) <= 0 )
			   ) {	
			  	return true;
			 }
			k =  m_poProdOfferInsAttr[k].m_iNextOfferInsAttrOffset;
		}
		
				//协议期判断
		k = vOfferIns[i]->m_iOfferAgreementOffset;
		while (k) {
		  if( ( strcmp(m_poProdOfferInsAgreement[k].m_sEffDate, sBeginTime) >= 0
		  	   && strcmp(m_poProdOfferInsAgreement[k].m_sEffDate, sEndTime) <= 0 ) || 
		  	  ( strcmp(m_poProdOfferInsAgreement[k].m_sExpDate, sBeginTime) >= 0
		  	  && strcmp(m_poProdOfferInsAgreement[k].m_sExpDate, sEndTime) <= 0 )
		   ) {	
		   	  return true;
		  }
		  k = m_poProdOfferInsAgreement[k].m_iNextOfferInsAgreementOffset;
		}
		
	}
	
	return false;	
}

bool Serv::isServChanged (const char *sBeginTime,const char *sEndTime)
{
	if(!sBeginTime || !sEndTime) return false;
		
	bool bRet = ( isServAttrChanged(sBeginTime, sEndTime) ||  
	             isServStateAttrChanged(sBeginTime, sEndTime) ||
	             isServTypeChanged(sBeginTime, sEndTime) || 
	             isServProductChanged(sBeginTime, sEndTime) ||
	             isServProductAttrChanged(sBeginTime, sEndTime) || 
	             isProdOffer_Ins_Attr_Detail_Changed(sBeginTime, sEndTime)
	            );
	return bRet;
}

int OfferInsQO::getOfferInsAttr(ProdOfferInsAttr ret[MAX_OFFER_ATTRS], char * sDate)
{
    if(!sDate) sDate = m_sTime;
    int iRet = 0;
    unsigned int k = m_poOfferIns->m_iOfferAttrOffset;
    
    for (; k; k=m_poProdOfferInsAttr[k].m_iNextOfferInsAttrOffset)
    {
        if(sDate &&
            (strcmp (sDate, m_poProdOfferInsAttr[k].m_sEffDate)<0 ||
             strcmp (sDate, m_poProdOfferInsAttr[k].m_sExpDate)>=0))
        {
            continue;
        }
        ret[iRet].m_iAttrID = m_poProdOfferInsAttr[k].m_iAttrID;
        strcpy(ret[iRet++].m_sAttrValue, m_poProdOfferInsAttr[k].m_sAttrValue);
    }
    
    return iRet;
}

//add by zhaoziwei
bool OfferInsQO::getOfferInsAgreement(ProdOfferInsAgreement &ret,char * sDate)
{
    if(!sDate) sDate = m_sTime;
    
		unsigned int k = m_poOfferIns->m_iOfferAgreementOffset;
		
    for (; k; k=m_poProdOfferInsAgreement[k].m_iNextOfferInsAgreementOffset)
		{
			if(sDate &&
          (strcmp (sDate, m_poProdOfferInsAgreement[k].m_sEffDate)<0 ||
           strcmp (sDate, m_poProdOfferInsAgreement[k].m_sExpDate)>=0))
      {
          continue;
      }
			
			ret=m_poProdOfferInsAgreement[k];
			return true;
		}
    
    return false;
}
//end

int OfferInsQO::getOfferInsAttrAndDate(ProdOfferInsAttr ret[MAX_OFFER_ATTRS], char * sDate)
{
    if(!sDate) sDate = m_sTime;
    int iRet = 0;
    unsigned int k = m_poOfferIns->m_iOfferAttrOffset;
    
    for (; k; k=m_poProdOfferInsAttr[k].m_iNextOfferInsAttrOffset)
    {
        if(sDate &&
            (strcmp (sDate, m_poProdOfferInsAttr[k].m_sEffDate)<0 ||
             strcmp (sDate, m_poProdOfferInsAttr[k].m_sExpDate)>=0))
        {
            continue;
        }
        ret[iRet].m_iAttrID = m_poProdOfferInsAttr[k].m_iAttrID;
		strcpy(ret[iRet].m_sEffDate,m_poProdOfferInsAttr[k].m_sEffDate);
		strcpy(ret[iRet].m_sExpDate,m_poProdOfferInsAttr[k].m_sExpDate);
        strcpy(ret[iRet++].m_sAttrValue, m_poProdOfferInsAttr[k].m_sAttrValue);
    }    
    return iRet;
}

int OfferInsQO::getOfferInsAttrCntByAttrID(int iAttrID,char* sDate,StdEvent * poEvent)
{
    if(!sDate) sDate = m_sTime;
    int iRet = 0;
	int iOperation;
	if(poEvent && m_poOfferMgr->checkSpecialAttr(iAttrID,iOperation))
	{
		return 1;
	}
	unsigned int k = m_poOfferIns->m_iOfferAttrOffset;
    
    for (; k; k=m_poProdOfferInsAttr[k].m_iNextOfferInsAttrOffset)
    {
        if (m_poProdOfferInsAttr[k].m_iAttrID < iAttrID)
            continue;
        
        if (m_poProdOfferInsAttr[k].m_iAttrID > iAttrID)
            break;
            
        if(sDate &&
            (strcmp (sDate, m_poProdOfferInsAttr[k].m_sEffDate)<0 ||
             strcmp (sDate, m_poProdOfferInsAttr[k].m_sExpDate)>=0))
        {
            continue;
        }
        iRet++;
    }
    
    return iRet;
}
int OfferInsQO::getOfferInsAttrByAttrID(ProdOfferInsAttr ret[MAX_OFFER_ATTRS], int iAttrID, char * sDate,StdEvent * poEvent)
{
    int iRet = 0;
	int iOperation;
	long lResult;
	if(poEvent && m_poOfferMgr->checkSpecialAttr(iAttrID,iOperation))
	{
		lResult = execOperation(iOperation,poEvent,iAttrID);
		ret[iRet].m_iAttrID = iAttrID;
		sprintf(ret[iRet].m_sAttrValue,"%ld",lResult);
		return 1;
	}
    if(!sDate) sDate = m_sTime;
	unsigned int k = m_poOfferIns->m_iOfferAttrOffset;

    for (; k; k=m_poProdOfferInsAttr[k].m_iNextOfferInsAttrOffset)
    {
        if (m_poProdOfferInsAttr[k].m_iAttrID < iAttrID)
            continue;
        
        if (m_poProdOfferInsAttr[k].m_iAttrID > iAttrID)
            break;
            
        if(sDate &&
            (strcmp (sDate, m_poProdOfferInsAttr[k].m_sEffDate)<0 ||
             strcmp (sDate, m_poProdOfferInsAttr[k].m_sExpDate)>=0))
        {
            continue;
        }
        ret[iRet].m_iAttrID = m_poProdOfferInsAttr[k].m_iAttrID;
        strcpy(ret[iRet++].m_sAttrValue, m_poProdOfferInsAttr[k].m_sAttrValue);
    }
    
    return iRet;
}

char* OfferInsQO::getOfferAttr(int iAttrID, char *sDate,StdEvent * poEvent)
{
	int iOperation;
	long lResult;
	static char strVal[32];
	if(poEvent && m_poOfferMgr->checkSpecialAttr(iAttrID,iOperation))
	{
		lResult = execOperation(iOperation,poEvent,iAttrID);
		sprintf(strVal,"%ld",lResult);
		return strVal;
	}
	if(!sDate) sDate = m_sTime;
    unsigned int k = m_poOfferIns->m_iOfferAttrOffset;   
    for (; k; k=m_poProdOfferInsAttr[k].m_iNextOfferInsAttrOffset) {
        if (m_poProdOfferInsAttr[k].m_iAttrID < iAttrID)
            continue;
        
        if (m_poProdOfferInsAttr[k].m_iAttrID > iAttrID)
            break;

        if (strcmp (sDate, m_poProdOfferInsAttr[k].m_sEffDate)<0)
            continue;

        if (strcmp (sDate, m_poProdOfferInsAttr[k].m_sExpDate)>=0)
            continue;

        return m_poProdOfferInsAttr[k].m_sAttrValue;
    }
    
	  return m_poOfferMgr->getOfferAttrValue(m_poOfferIns->m_iOfferID,iAttrID,sDate);

    return 0;
}

bool OfferInsQO::isOfferAttrValue(int iAttrID, char* iAttrVal, char *sDate,StdEvent * poEvent)
{
	int iOperation;
	long lResult;
	char strVal[32];
	if(poEvent && m_poOfferMgr->checkSpecialAttr(iAttrID,iOperation))
	{
		lResult = execOperation(iOperation,poEvent,iAttrID);
		sprintf(strVal,"%ld",lResult);
		if(!strcmp(strVal,iAttrVal)){
			return true;
		}else{
			return false;
		}
	}
    if(!sDate) sDate = m_sTime;
    unsigned int k = m_poOfferIns->m_iOfferAttrOffset;   
    for (; k; k=m_poProdOfferInsAttr[k].m_iNextOfferInsAttrOffset) {
        if (m_poProdOfferInsAttr[k].m_iAttrID < iAttrID)
            continue;
        
        if (m_poProdOfferInsAttr[k].m_iAttrID > iAttrID)
            break;

        if (strcmp (sDate, m_poProdOfferInsAttr[k].m_sEffDate)<0)
            continue;

        if (strcmp (sDate, m_poProdOfferInsAttr[k].m_sExpDate)>=0)
            continue;

        if (!strcmp (iAttrVal, m_poProdOfferInsAttr[k].m_sAttrValue))
        {
            return true;
        }
    }

    return false;
}

bool OfferInsQO::isOfferMember(int iMemberID, char *sDate)
{
    if(!sDate) sDate = m_sTime;
    unsigned int k = m_poOfferIns->m_iOfferDetailInsOffset;  
    for (; k; k=m_poOfferDetailIns[k].m_iNextDetailOffset) {
        if (strcmp (sDate, m_poOfferDetailIns[k].m_sEffDate)<0)
            continue;

        if (strcmp (sDate, m_poOfferDetailIns[k].m_sExpDate)>=0)
            continue;
            
        if(m_poOfferDetailIns[k].m_iMemberID == iMemberID)
        {
            return true;
        }
        if(m_poOfferDetailIns[k].m_iInsType == OFFER_INSTANCE_TYPE)
        {
            if(__isOfferMember(m_poOfferDetailIns[k].m_lInsID, iMemberID, sDate))
            {
                return true;
            }
        }
    }
    return false;
}

int OfferInsQO::getOfferInsMember(OfferDetailIns * ret[MAX_OFFER_MEMBERS],
                            int iMemberID, char *sDate)
{
    if(!sDate) sDate = m_sTime;
    int iRet = 0;
    int iLastNum = 0;
    unsigned int p = 0;

    unsigned int k = m_poOfferIns->m_iOfferDetailInsOffset;  

    for (; k; k=m_poOfferDetailIns[k].m_iNextDetailOffset) {
        if (strcmp (sDate, m_poOfferDetailIns[k].m_sEffDate)<0)
            continue;
    
        if (strcmp (sDate, m_poOfferDetailIns[k].m_sExpDate)>=0)
            continue;

        if (iMemberID && m_poOfferDetailIns[k].m_iMemberID!=iMemberID)
            continue;

        ret[iRet++] = &(m_poOfferDetailIns[k]);

        if(iRet==MAX_OFFER_MEMBERS) {
            return iRet;
        }
    }
    
    for(; iLastNum<iRet; iLastNum++) {
        if(ret[iLastNum]->m_iInsType == OFFER_INSTANCE_TYPE) {
            if(m_poProdOfferInsIndex->get(ret[iLastNum]->m_lInsID, &p)) {
                k = m_poProdOfferIns[p].m_iOfferDetailInsOffset;
                for (; k; k=m_poOfferDetailIns[k].m_iNextDetailOffset) {
                    if (strcmp (sDate, m_poOfferDetailIns[k].m_sEffDate)<0)
                        continue;
                
                    if (strcmp (sDate, m_poOfferDetailIns[k].m_sExpDate)>=0)
                        continue;

                    ret[iRet++] = &(m_poOfferDetailIns[k]);

                    if(iRet==MAX_OFFER_MEMBERS) {
                        return iRet;
                    }
                }
            }
        }
    }

    return iRet;
}

OfferDetailIns * OfferInsQO::getOneMemberIns(int iMemberID, char *sDate)
{
    if(!sDate) sDate = m_sTime;
    int iRet = 0;
    int iLastNum = 0;
    unsigned int p = 0;

    unsigned int k = m_poOfferIns->m_iOfferDetailInsOffset;  
    for (; k; k=m_poOfferDetailIns[k].m_iNextDetailOffset) {
        if (strcmp (sDate, m_poOfferDetailIns[k].m_sEffDate)<0)
            continue;
    
        if (strcmp (sDate, m_poOfferDetailIns[k].m_sExpDate)>=0)
            continue;

        if (m_poOfferDetailIns[k].m_iMemberID != iMemberID) 
            continue;

        return &(m_poOfferDetailIns[k]);
    }
    
    return 0;
}

int OfferInsQO::getOfferInsMember(OfferDetailIns ret[MAX_OFFER_MEMBERS], 
char *sDate)
{
    if(!sDate) sDate = m_sTime;
    int iRet = 0;
    int iLastNum = 0;
    unsigned int p = 0;

    unsigned int k = m_poOfferIns->m_iOfferDetailInsOffset;  
    for (; k; k=m_poOfferDetailIns[k].m_iNextDetailOffset) {
        if (strcmp (sDate, m_poOfferDetailIns[k].m_sEffDate)<0)
            continue;
    
        if (strcmp (sDate, m_poOfferDetailIns[k].m_sExpDate)>=0)
            continue;
        ret[iRet].m_lInsID = m_poOfferDetailIns[k].m_lInsID;
        ret[iRet].m_iInsType = m_poOfferDetailIns[k].m_iInsType;
        ret[iRet++].m_iMemberID = m_poOfferDetailIns[k].m_iMemberID;
        if(iRet==MAX_OFFER_MEMBERS)
        {
            return iRet;
        }
    }
    
    for(; iLastNum<iRet; iLastNum++)
    {
        if(ret[iLastNum].m_iInsType == OFFER_INSTANCE_TYPE)
        {
            if(m_poProdOfferInsIndex->get(ret[iLastNum].m_lInsID, &p))
            {
                k = m_poProdOfferIns[p].m_iOfferDetailInsOffset;
                for (; k; k=m_poOfferDetailIns[k].m_iNextDetailOffset) 
                {
                    if (strcmp (sDate, m_poOfferDetailIns[k].m_sEffDate)<0)
                        continue;
                
                    if (strcmp (sDate, m_poOfferDetailIns[k].m_sExpDate)>=0)
                        continue;
                    ret[iRet].m_lInsID = m_poOfferDetailIns[k].m_lInsID;
                    ret[iRet].m_iInsType = m_poOfferDetailIns[k].m_iInsType;
                    ret[iRet++].m_iMemberID = m_poOfferDetailIns[k].m_iMemberID;
                    if(iRet==MAX_OFFER_MEMBERS)
                    {
                        return iRet;
                    }
                }
            }
        }
    }
    return iRet;
}

bool OfferInsQO::isOfferInsMember(long lServID, long lAcctID, int iMemberID, char *sDate)
{
    unsigned int k = m_poOfferIns->m_iOfferDetailInsOffset;  
    for (; k; k=m_poOfferDetailIns[k].m_iNextDetailOffset) {
        if(m_poOfferDetailIns[k].m_iMemberID != iMemberID)
            continue;
        if (strcmp (sDate, m_poOfferDetailIns[k].m_sEffDate)<0)
            continue;
        if (strcmp (sDate, m_poOfferDetailIns[k].m_sExpDate)>=0)
            continue;
            
        if((m_poOfferDetailIns[k].m_lInsID == lServID && 
            m_poOfferDetailIns[k].m_iInsType == SERV_INSTANCE_TYPE)||
            (m_poOfferDetailIns[k].m_iInsType == ACCT_TYPE && 
            m_poOfferDetailIns[k].m_lInsID == lAcctID))
        {
            return true;
        }
        ////增加客户级的判断
        if( m_poOfferDetailIns[k].m_iInsType == CUST_TYPE ){
            Serv oServ;
            if(  G_PUSERINFO->getServ(oServ, lServID, sDate)
              && oServ.getCustID() == m_poOfferDetailIns[k].m_lInsID){
                return true;
            }
         } 
////
        if(m_poOfferDetailIns[k].m_iInsType == OFFER_INSTANCE_TYPE)
        {
            if( __isOfferInsMember(m_poOfferDetailIns[k].m_lInsID,
                         lServID, lAcctID, iMemberID, sDate))
            {
                return true;
            }
        }
    }
    return false;
}

bool OfferInsQO::__isOfferInsMember(long lProdOfferInsID, long lServID, long lAcctID, int iMemberID, char *sDate)
{
    unsigned int p = 0;
    
    if(!m_poProdOfferInsIndex->get(lProdOfferInsID, &p)) return false;
    unsigned int k = m_poProdOfferIns[p].m_iOfferDetailInsOffset;
    for (; k; k=m_poOfferDetailIns[k].m_iNextDetailOffset) {
        if(m_poOfferDetailIns[k].m_iMemberID != iMemberID)
            continue;
        if (strcmp (sDate, m_poOfferDetailIns[k].m_sEffDate)<0)
            continue;
        if (strcmp (sDate, m_poOfferDetailIns[k].m_sExpDate)>=0)
            continue;
            
        if((m_poOfferDetailIns[k].m_lInsID == lServID && 
            m_poOfferDetailIns[k].m_iInsType == SERV_INSTANCE_TYPE)||
            (m_poOfferDetailIns[k].m_iInsType == ACCT_TYPE && 
            m_poOfferDetailIns[k].m_lInsID == lAcctID))
        {
            return true;
        }
        if(m_poOfferDetailIns[k].m_iInsType == OFFER_INSTANCE_TYPE)
        {
            if( __isOfferInsMember(m_poOfferDetailIns[k].m_lInsID,
                         lServID, lAcctID, iMemberID, sDate))
            {
                return true;
            }
        }
    }
    return false;
}

bool OfferInsQO::__isOfferMember(long lProdOfferInsID, int iMemberID, char *sDate)
{
    unsigned int p = 0;
    
    if(m_poProdOfferInsIndex->get(lProdOfferInsID, &p))
    {
        unsigned int k = m_poProdOfferIns[p].m_iOfferDetailInsOffset;  
        for (; k; k=m_poOfferDetailIns[k].m_iNextDetailOffset) 
        {
            if (strcmp (sDate, m_poOfferDetailIns[k].m_sEffDate)<0)
                continue;
    
            if (strcmp (sDate, m_poOfferDetailIns[k].m_sExpDate)>=0)
                continue;
                
            if(m_poOfferDetailIns[k].m_iMemberID == iMemberID)
            {
                return true;
            }
            if(m_poOfferDetailIns[k].m_iInsType == OFFER_INSTANCE_TYPE)
            {
                if(__isOfferMember(m_poOfferDetailIns[k].m_lInsID, iMemberID, sDate))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

#ifdef GROUP_CONTROL
bool UserInfoInterface::getAccNbrOfferType(char* pAccNbr,char* pStartDate,AccNbrOfferType& oAccNbrOfferType)
{
	if((0==pAccNbr)||(0== pAccNbr[0]))
		return false;
	unsigned int k =0;

	AccNbrOfferType* pAccNbrOfferType = (AccNbrOfferType*)(*m_poAccNbrData);
	if(0 !=m_poAccNbrIndex)
	{
		if(!m_poAccNbrIndex->get(pAccNbr,&k))
		{
			return false;
		}else
		{
			while(k>0)
			{
				
				if((strcmp(pStartDate,pAccNbrOfferType[k].m_sEffDate)>=0)&&
					(strcmp(pStartDate,pAccNbrOfferType[k].m_sExpDate)<=0))
				{
					memcpy(&oAccNbrOfferType,&pAccNbrOfferType[k],sizeof(AccNbrOfferType));
					return true;
				}
				k = pAccNbrOfferType[k].m_iNextOffset;
			}
		}
	}
	return false;
}

bool UserInfoInterface::checkAccNbrOfferType(char * pAccNbr,char* pStartDate,int iOfferType)
{
	if((0==pAccNbr)||(0== pAccNbr[0]))
		return false;
	unsigned int k =0;

	AccNbrOfferType* pAccNbrOfferType = (AccNbrOfferType*)(*m_poAccNbrData);
	if(0 !=m_poAccNbrIndex)
	{
		if(!m_poAccNbrIndex->get(pAccNbr,&k))
		{
			return false;
		}else
		{
			while(k>0)
			{
				if((strcmp(pStartDate,pAccNbrOfferType[k].m_sEffDate)>=0)&&
					(strcmp(pStartDate,pAccNbrOfferType[k].m_sExpDate)<=0)&&
					(pAccNbrOfferType[k].m_iOfferType == iOfferType))
					return true;
				k = pAccNbrOfferType[k].m_iNextOffset;
			}
		}
	}
	return false;
}
#endif
ProdOfferIns * UserInfoInterface::getOfferIns(long lOfferInstID)
{
    unsigned int k;

    if (!m_poProdOfferInsIndex->get (lOfferInstID, &k)) return 0;

    return m_poProdOfferIns + k;
}

bool OfferInsIteration::next(ProdOfferIns &offerins)
{
    if(!m_iCurOffset) return false;
    
    offerins.m_lProdOfferInsID = m_poProdOfferIns[m_iCurOffset].m_lProdOfferInsID;
    offerins.m_iOfferDetailInsOffset = m_poProdOfferIns[m_iCurOffset].m_iOfferDetailInsOffset;
    offerins.m_iAccumuOffset = m_poProdOfferIns[m_iCurOffset].m_iAccumuOffset;
    offerins.m_iOfferAttrOffset = m_poProdOfferIns[m_iCurOffset].m_iOfferAttrOffset;
    offerins.m_iOfferID = m_poProdOfferIns[m_iCurOffset].m_iOfferID;
    strcpy(offerins.m_sEffDate, m_poProdOfferIns[m_iCurOffset].m_sEffDate);
    strcpy(offerins.m_sExpDate, m_poProdOfferIns[m_iCurOffset].m_sExpDate);
    
    m_iCurOffset++;
    if (m_iCurOffset == (m_poProdOfferInsData->getCount ()+1)) 
    {
        m_iCurOffset = 0;
    }
    return true;
}

bool UserInfoInterface::clearServ(Serv &cur)
{
	cur.m_poServInfo=0;
	cur.m_poCustInfo=0;
	cur.m_poServProductInfo=0;
	cur.m_poServAcctInfo=0;
    memset(cur.m_sTime, 0, sizeof(cur.m_sTime) );






























	return true;
}

#ifdef GROUP_CONTROL
bool UserInfoInterface::getGroupInfoByID(GroupInfo &groupInfo,int iGroupID,char *sDate)
{
	bool bRet = false;
	unsigned int k;
	if(NULL == sDate)
	{
		Date d;
		sDate=d.toString();
	}
	if(m_poGroupInfoIndex->get(iGroupID,&k))
	{
		while(k)
		{
			if(strcmp(m_poGroupInfo[k].m_sEffDate,sDate)<=0 && strcmp(m_poGroupInfo[k].m_sExpDate,sDate)>0)
			{
				memcpy(&groupInfo,&m_poGroupInfo[k],sizeof(GroupInfo));
				bRet = true;
				return bRet;
			}
			k = m_poGroupInfo[k].m_iNextOffset;
		}
	}
	return bRet;
}

int UserInfoInterface::getGroupMemberByAccNbr(GroupMember ret[GROUP_NUM],char * pAccNbr,char *sDate)
{
	int iRet = 0;
	unsigned int k;
	if(NULL == pAccNbr)
	{
		return iRet;
	}
	if(NULL == sDate)
	{
		Date d;
		sDate=d.toString();
	}
	if(m_poGroupMemberIndex->get(pAccNbr,&k))
	{
		while(k)
		{
			if(strcmp(m_poGroupMember[k].m_sEffDate,sDate)<=0 && strcmp(m_poGroupMember[k].m_sExpDate,sDate)>0)
			{
				ret[iRet++]=m_poGroupMember[k];
				if(iRet == GROUP_NUM)
				{
					return iRet;
				}
			}
			k = m_poGroupMember[k].m_iNextOffset;
		}
	}
	return iRet;
}

int UserInfoInterface::getNumberGroupByAccNbr(NumberGroup ret[GROUP_NUM],char * pAccNbr,char *sDate)
{
	int iRet = 0;
	unsigned int k;
	if(NULL == pAccNbr)
	{
		return iRet;
	}
	if(NULL == sDate)
	{
		Date d;
		sDate=d.toString();
	}
	if(m_poNumberGroupIndex->get(pAccNbr,&k))
	{
		while(k)
		{
			if(strcmp(m_poNumberGroup[k].m_sEffDate,sDate)<=0 && strcmp(m_poNumberGroup[k].m_sExpDate,sDate)>0)
			{
				ret[iRet++]=m_poNumberGroup[k];
				if(iRet == GROUP_NUM)
				{
					return iRet;
				}
			}
			k = m_poNumberGroup[k].m_iNextOffset;
		}
	}
	return iRet;
}

int UserInfoInterface::getGroupResult(GroupInfoResult ret[GROUP_NUM],char * pCAccNbr,char * pGAccNbr,char * sDate)
{
	int iRet = 0;
	int iNumber;
	int iMember;
	GroupInfo groupInfo;
	NumberGroup aNumberInfo[GROUP_NUM];
	GroupMember aMemberInfo[GROUP_NUM];

	if(NULL == pCAccNbr || NULL == pGAccNbr)
	{
		return iRet;
	}
	if(NULL == sDate)
	{
		Date d;
		sDate=d.toString();
	}
	iNumber = getNumberGroupByAccNbr(aNumberInfo,pCAccNbr,sDate);
	if(iNumber>0)
	{
		iMember = getGroupMemberByAccNbr(aMemberInfo,pGAccNbr,sDate);
		if(iMember>0)
		{
			for(int i = 0;i<iNumber;i++)
			{
				for (int j = 0;j<iMember;j++)
				{
					if(aNumberInfo[i].m_iGroupID == aMemberInfo[j].m_iGroupID)
					{
						if(getGroupInfoByID(groupInfo,aNumberInfo[i].m_iGroupID,sDate))
						{
							ret[iRet].m_iGroupID = groupInfo.m_iGroupID;
							ret[iRet].m_iConditionID = groupInfo.m_iConditionID;
							ret[iRet].m_iStrategyID = groupInfo.m_iStrategyID;
							ret[iRet].m_iPriority = groupInfo.m_iPriority;
							ret[iRet++].m_lOfferInstID = aNumberInfo[i].m_lOfferInstID;
						}
					}
				}
			}
		}
	}
	return iRet;
}
#endif

#ifdef	IMSI_MDN_RELATION
bool UserInfoInterface::getImsiMdnRelationByImsi(ImsiMdnRelation &oRelation, char *sImsi,long iType, const char *sDate)
{
	unsigned int k;
	Date d, d2;
	
	if (!sImsi){
		return false;
	}
	if (!sDate){
		sDate = d.toString();
	}else{
		d = Date(sDate);
		sDate = d.toString();
	}
	
	if (!m_poImsiMdnRelationIndex->get(sImsi, &k)){
		return false;
	}

#ifdef USERINFO_OFFSET_ALLOWED
	d2 = d;
	d.addSec(UserInfoReader::m_offsetSecs);
	d2.addSec(-UserInfoReader::m_offsetSecs);
#endif

	while (k){
		if (strcmp(d2.toString(), m_poImsiMdnRelation[k].m_sExpDate)<0){
			if (strcmp(d.toString(), m_poImsiMdnRelation[k].m_sEffDate)>=0){
				//if(m_poImsiMdnRelation[k].m_itype == -1||m_poImsiMdnRelation[k].m_itype==iType ){	
				if(iType == 0 ||m_poImsiMdnRelation[k].m_itype==iType ){	
				oRelation = m_poImsiMdnRelation[k];
				return true;
				}
			}
			k = m_poImsiMdnRelation[k].m_iNextOffset;
		}
		/*	because the exp_date had been sorted in descending order,
			when sDate is larger then exp_date, stop searching. */
		else{
			return false;
		}
	}
	return false;
}
void UserInfoInterface::showImsiMdnRelationByImsi(char *sImsi, const char *sDate)
{
	unsigned int k;
	int iCnt=0, iFind=0;
	Date d, d2;
	
	if (!sImsi){
		return;
	}
	if (!sDate){
		sDate = d.toString();
	}else{
		d = Date(sDate);
		sDate = d.toString();
	}

	if (!m_poImsiMdnRelationIndex->get(sImsi, &k)){
		printf("没有找到关于IMSI: %s 的任何信息!\n", sImsi);
		return;
	}

#ifdef USERINFO_OFFSET_ALLOWED
	d2 = d;
	d.addSec(UserInfoReader::m_offsetSecs);
	d2.addSec(-UserInfoReader::m_offsetSecs);

#endif

	printf("  %-15s %-15s %-15s %-15s %-15s %-15s %-15s\n", 
		"RELATION_ID", "IMSI", "ACC_NBR", "AREA_CODE", "EFF_DATE", "EXP_DATE","TYPE");

	while (k){
		if (strcmp(d2.toString(), m_poImsiMdnRelation[k].m_sExpDate)<0 &&
			strcmp(d.toString(), m_poImsiMdnRelation[k].m_sEffDate)>=0){
			printf("* %-15ld %-15s %-15s %-15s %-15s %-15s %-15d\n", 
				m_poImsiMdnRelation[k].m_lRelationID, 
				m_poImsiMdnRelation[k].m_sImsi,
				m_poImsiMdnRelation[k].m_sAccNbr,
				m_poImsiMdnRelation[k].m_sAreaCode,
				m_poImsiMdnRelation[k].m_sEffDate,
				m_poImsiMdnRelation[k].m_sExpDate,
				m_poImsiMdnRelation[k].m_itype
				);	
			++iFind;
			++iCnt;
			k = m_poImsiMdnRelation[k].m_iNextOffset;
			continue;
		}
		/*	because the exp_date had been sorted in descending order,
			when sDate is larger then exp_date, stop searching. */
		printf("  %-15ld %-15s %-15s %-15s %-15s %-15s %-15d\n", 
			m_poImsiMdnRelation[k].m_lRelationID, 
			m_poImsiMdnRelation[k].m_sImsi,
			m_poImsiMdnRelation[k].m_sAccNbr,
			m_poImsiMdnRelation[k].m_sAreaCode,
			m_poImsiMdnRelation[k].m_sEffDate,
			m_poImsiMdnRelation[k].m_sExpDate,
			m_poImsiMdnRelation[k].m_itype
			);	
		k = m_poImsiMdnRelation[k].m_iNextOffset;
		++iCnt;
	}
	printf("共找到%d条与IMSI: %s 相关的信息\n", iCnt, sImsi);
	if (iFind){
#ifdef USERINFO_OFFSET_ALLOWED
		printf("其中有%d条符合时间(向后偏移:%d分钟): %s 要求(*标出), 第一条*记录将起作用。\n", 
				iFind, UserInfoReader::m_offsetSecs/60, sDate);
#else
		printf("其中有%d条符合时间: %s 要求(*标出), 第一条*记录将起作用。\n", iFind, sDate);
#endif
	}
	else{
#ifdef USERINFO_OFFSET_ALLOWED
		printf("但这些记录都不满足时间(向后偏移:%d分钟): %s 要求!\n",
				UserInfoReader::m_offsetSecs/60, sDate);
#else
		printf("但这些记录都不满足时间: %s 要求!\n", sDate);
#endif
	}
	return;
}
void UserInfoInterface::showAllImsiMdnRelation()
{
	unsigned int iCount = m_poImsiMdnRelationData->getCount();
	printf("%-15s %-15s %-15s %-15s %-15s %-15s %-15s %-15s\n", 
		"RELATION_ID", "IMSI", "ACC_NBR", "AREA_CODE", "EFF_DATE", "EXP_DATE", "TYPE","NEXTOFFSET");
	for (int i=1; i<=iCount; i++){
		printf("%-15ld %-15s %-15s %-15s %-15s %-15s %-15d %-15d\n", 
			m_poImsiMdnRelation[i].m_lRelationID, 
			m_poImsiMdnRelation[i].m_sImsi,
			m_poImsiMdnRelation[i].m_sAccNbr,
			m_poImsiMdnRelation[i].m_sAreaCode,
			m_poImsiMdnRelation[i].m_sEffDate,
			m_poImsiMdnRelation[i].m_sExpDate,
			m_poImsiMdnRelation[i].m_itype,
			m_poImsiMdnRelation[i].m_iNextOffset
			);	
	}
}
#endif	/* end if IMSI_MDN_RELATION */

//返回NP表中信息
	//add by jx 2009-10-26
bool UserInfoInterface::getNpInfo(NpInfo &oNpInfo,char *sAccNbr,char *sDate)
{
	unsigned int k;
	
	bool rtn = false;
	
	NpInfo *pNp = (NpInfo *)(*m_poNpData);
	
	if(!m_poNpIndex->get(sAccNbr,&k))
			return false;
	
	Date d;

	if (!sDate){
		sDate = d.toString();
	}else{
		d = Date(sDate);
		sDate = d.toString();
	}

	while( k )
	{
		
		if (strcmp(sDate, pNp[k].m_sEffDate) >= 0 &&
			strcmp(sDate, pNp[k].m_sExpDate) < 0) 
		{
			memcpy((void *)(&oNpInfo),(const void*)(&(pNp[k])),sizeof(NpInfo) );
			
			rtn = true;
			break;
		}
		else
			k = pNp[k].m_iNext;
	}

	return rtn;
}






































#ifdef	IMMEDIATE_VALID
int UserInfoInterface::getRollbackInstByServ(long lServID,OfferInsQO * pOfferBuf, int iOfferNum,
											 long ret[MAX_OFFER_INSTANCE_NUMBER],const char *sDate)
{
	int iRet = 0;
	if (!sDate){
		Date d;
		sDate = d.toString();
	}
	unsigned int i;
	if (m_poServInstIndex->get(lServID,&i)){
		ProdOfferIns *pOffer ;
		long lInstID = 0;
		while(i){
			lInstID = m_poServInstRelation[i].m_lNewInstID;
			for (int j=0; j<iOfferNum; j++){
				if ((lInstID ==  pOfferBuf[j].m_poOfferIns->m_lProdOfferInsID)
					&& (strcmp(sDate,m_poServInstRelation[i].m_sEffDate)>0)
					&& (m_poServInstRelation[i].m_iProcState == 0)){
					ret[iRet++] = m_poServInstRelation[i].m_lOldInstID;
					break;
				}
			}
			i = m_poServInstRelation[i].m_iNextOffset;
		}
	}
	return iRet;
}

bool UserInfoInterface::updateRollbackInstByServ(long lServID,long lInstID)
{
	bool bRet = false;
	unsigned int i;
	if (m_poServInstIndex->get(lServID,&i)){
		while(i){
			if (lInstID == m_poServInstRelation[i].m_lOldInstID){
				m_poServInstRelation[i].m_iProcState = 1;
				bRet = true;
			}
			i = m_poServInstRelation[i].m_iNextOffset;
		}
	}else{
		Log::log(0, "[ERROR] ServID=%ld，的优惠回退资料不存在！",lServID);
	}
	if (!bRet){
		Log::log(0, "[ERROR] ServID=%ld，ProductOfferInstID=%ld,的优惠回退资料不存在！",lServID,lInstID);
	}
	return bRet;
}
#endif

#endif
