/*VER: 3*/ 
#include "Head2OrgIDMgr.h"
#include <iostream>
using namespace std;

#include "Environment.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif


KeyTree<HeadEx>* Head2OrgIDMgr:: m_ppoLocalHead[MAX_TYPE][MAX_TYPE] ={0};
bool Head2OrgIDMgr::m_bIsLoad = false;
vector<HeadEx*> Head2OrgIDMgr::vecSameHead;
// add by zhangch 20100603
OrgMgrEx* Head2OrgIDMgr::m_pOrgMgrEx = NULL;
    
//##ModelId=42803ACA03E7
#ifdef PRIVATE_MEMORY
Head2OrgIDMgr::Head2OrgIDMgr()
{
    loadLocalHead();
}
#else
Head2OrgIDMgr::Head2OrgIDMgr()
{
}
#endif

//##ModelId=42803ADA030E
Head2OrgIDMgr::~Head2OrgIDMgr()
{
//  unloadLocalHead();
}

//##ModelId=42803AE60329
void Head2OrgIDMgr::loadLocalHead()
{
    if (m_bIsLoad)
        return;
    
    cout<<"\nInit Local_Head ..."<<endl;
    
    
    
    HeadEx obLocalHead;
    HeadEx obLocalHeadTemp;
    vecSameHead.clear();
    vecSameHead.reserve(32);
    DEFINE_QUERY(qry);
        
    qry.close();
    qry.setSQL(" select REGION_ID,"
        "to_char(NVL(eff_date,to_date('19000101010101','yyyymmddhh24miss')),'yyyymmddhh24miss') eff_date,"
        "to_char(NVL(exp_date,to_date('20500101010101','yyyymmddhh24miss')),'yyyymmddhh24miss') exp_date, "
            "nvl(EMULATORY_PARTNER_ID,-1) EMULATORY_PARTNER_ID ,"
            "nvl(PARTY_ID,-1) PARTY_ID,"
            "NVL(TYPE_CODE,'52Z') TYPE_CODE ,head, ascii(LOWER(SUBSTR(NVL(TYPE_CODE,'52Z'),-1,1)))-97 POSITION," 
	    "b.REGION_TYPE_ID REGION_TYPE_ID "
            "from (select * from B_HEAD_REGION ) a, "
            "(select * from ( "
            "select ORG_ID,PARENT_ORG_ID ,ORG_LEVEL_ID,NAME,ORG_AREA_CODE ,ORG_CODE,1 REGION_TYPE_ID  from ORG a union  ALL "
            "select REGION_ID ORG_ID,PARENT_REGION_ID PARENT_ORG_ID,"
            " REGION_LEVEL ORG_LEVEL_ID ,NAME,ORG_AREA_CODE,ORG_CODE,"
            "2 REGION_TYPE_ID from STAT_REGION  b union all "
            "select REGION_ID ORG_ID,PARENT_REGION_ID PARENT_ORG_ID, "
            "REGION_LEVEL ORG_LEVEL_ID ,NAME ,REGION_AREA_CODE ORG_CODE, "
            " REGION_CODE ORG_CODE,REGION_TYPE_ID "
            " from B_SELF_DEFINE_REGION  )) b "
            " WHERE A.REGION_ID = B.ORG_ID " );
    qry.open();
    int iPos = 0;
    int iTypeID=0;
    char sKey[128]={0};
    while (qry.next()) {
        obLocalHead.m_pNext = NULL;
        iTypeID = qry.field("REGION_TYPE_ID").asInteger();
        iPos = qry.field("POSITION").asInteger();
        obLocalHead.m_iOrgID = qry.field("REGION_ID").asInteger();
        strcpy(obLocalHead.m_sHead, qry.field("head").asString());
        strcpy(obLocalHead.m_sEffDate, qry.field("eff_date").asString());
        strcpy(obLocalHead.m_sExpDate, qry.field("exp_date").asString());
        obLocalHead.m_iPartnerID = qry.field("EMULATORY_PARTNER_ID").asInteger();
        obLocalHead.m_iPartyID = qry.field("PARTY_ID").asInteger();
        strcpy(obLocalHead.m_sTypeCode,qry.field("TYPE_CODE").asString());
        strcpy(sKey,qry.field("head").asString());  
        if(m_ppoLocalHead[iTypeID][iPos]==0)
            m_ppoLocalHead[iTypeID][iPos] = new KeyTree<HeadEx>;
        if(m_ppoLocalHead[iTypeID][iPos]->get(sKey,&obLocalHeadTemp) ){
            obLocalHead.m_pNext = obLocalHeadTemp.m_pNext;
            obLocalHeadTemp.m_pNext = new HeadEx;
            if(!obLocalHeadTemp.m_pNext) THROW(MBC_KeyTree+1);
            *obLocalHeadTemp.m_pNext = obLocalHead;
            m_ppoLocalHead[iTypeID][iPos]->add(sKey, obLocalHeadTemp);
            vecSameHead.push_back(obLocalHeadTemp.m_pNext);
            
        }else{
            m_ppoLocalHead[iTypeID][iPos]->add(sKey, obLocalHead);
        }
        
    
    }
    
    qry.close();
    qry.setSQL(" select max(length(head )) max_len, "
        " min(length(head) ) min_len from B_HEAD_REGION ");
    qry.open();
    if (qry.next()) {
        m_iMaxHeadLen = qry.field("max_len").asInteger();
        m_iMinHeadLen = qry.field("min_len").asInteger();
    }
    qry.close();
    
    m_bIsLoad =true;
    cout<<"Local_Head inited.\n"<<endl;
}

//##ModelId=42803AF3021A
void Head2OrgIDMgr::unloadLocalHead()
{
    ///这里内存释放不完, 加了vec就释放完了
    if (!m_bIsLoad)
        return;
    for(int i=0;i <MAX_TYPE;i++)
    {
        for(int j=0;j<MAX_TYPE;j++)
        {
            if(m_ppoLocalHead[i][j])
                delete m_ppoLocalHead[i][j];
            m_ppoLocalHead[i][j] =0;
        }
    }
    
    for(int i=0; i<vecSameHead.size(); i++){
        if( vecSameHead[i] )
            delete vecSameHead[i];
    }
    vecSameHead.clear();
    
    m_bIsLoad = false;
}

bool Head2OrgIDMgr::reloadLocalHead()
{
    if(m_bIsLoad){
        unloadLocalHead();
        loadLocalHead();
        if(!m_bIsLoad)
            return false;
    }
    return true;

}

#ifdef PRIVATE_MEMORY
bool Head2OrgIDMgr::getOrgIDs(const char* pHead,const char *pTime, vector<int> & pvOrgID,
    const char* pType, const int iOrgType )
{
    char sType[4]="50Z";
    int  iType = iOrgType;
    
    
    if( !pType || pType[0]=='\0')
        pType = sType;
    if( iOrgType == 0 )
        iType == BILL_ORG ;
    
    if(!Head2OrgIDMgr::searchLocalHead(pHead,pTime,&Head2OrgIDMgr::m_goHeadEx,pType,iType))
        return false;
    HeadEx * pHeadEx = &Head2OrgIDMgr::m_goHeadEx;

    while( pHeadEx ){
        if ((strcmp(pTime,pHeadEx->m_sEffDate)<0)
            ||(strcmp(pTime,pHeadEx->m_sExpDate) >= 0)) {
            pHeadEx = pHeadEx->m_pNext;    
            continue;        
        }
        pvOrgID.push_back( pHeadEx->getOrgID() );
        pHeadEx = pHeadEx->m_pNext;

    }    

    return true;

}
#else
bool Head2OrgIDMgr::getOrgIDs(const char* pHead,const char *pTime, vector<int> & pvOrgID,
    const char* pType, const int iOrgType )
{
    char sType[4]="50Z";
    int  iType = iOrgType;
    
    
    if( !pType || pType[0]=='\0')
        pType = sType;
    if( iOrgType == 0 )
        iType == BILL_ORG ;
    
    if(!Head2OrgIDMgr::searchLocalHead(pHead,pTime,&Head2OrgIDMgr::m_goHeadEx,pType,iType))
        return false;

    HeadEx *pHeadEx = &Head2OrgIDMgr::m_goHeadEx;
    HeadEx *pHeadHeadEx = (HeadEx *)G_PPARAMINFO->m_poHeadExList;

    while( pHeadEx != pHeadHeadEx)
    {
        if ((strcmp(pTime,pHeadEx->m_sEffDate)<0) ||(strcmp(pTime,pHeadEx->m_sExpDate) >= 0)) 
        {
            pHeadEx = pHeadHeadEx + pHeadEx->m_iNextOffset;      
        }
        else
        {
            pvOrgID.push_back( pHeadEx->getOrgID() );
            pHeadEx = pHeadHeadEx + pHeadEx->m_iNextOffset;    
        }

    }    

    return true;

}
#endif

bool Head2OrgIDMgr::getOrgID(const char* pHead,const char *pTime,int& iOrgID,const char* pType,
                             const int iOrgType )
{
    char sType[4]="50Z";
    int  iType = iOrgType;
    
    if( !pType || pType[0]=='\0')
        pType = sType;
    if( iOrgType == 0 )
        iType == BILL_ORG ;

    if(!Head2OrgIDMgr::searchLocalHead(pHead,pTime,&Head2OrgIDMgr::m_goHeadEx,pType,iType))
        return false;
    iOrgID = m_goHeadEx.getOrgID();
    return true;

}

//##ModelId=42803B8001A4
#ifdef PRIVATE_MEMORY
bool Head2OrgIDMgr::searchLocalHead(const char *_sAccNbr,  const char *_sTime, 
        HeadEx *_pLocalHead,const char* pType,const int iOrgType )
{
    if (!_pLocalHead)
        return false;
    HeadEx oHeadExTmp ;
    HeadEx* pHeadExTmp = 0;
    if (!m_bIsLoad)
        loadLocalHead();
    if(iOrgType>=MAX_TYPE)
    {
        Log::log(0,"区域类型超界:%d",iOrgType);
        return false;
    }
    
    char sKey[MAX_HEAD_LEN_EX+1]={0};
    int iAccNbrLen = MAX_ACCNBR_LEN_EX>strlen(_sAccNbr)?strlen(_sAccNbr):MAX_ACCNBR_LEN_EX;

    strncpy(sKey,_sAccNbr,iAccNbrLen );
    sKey[iAccNbrLen] = 0;

    
    int iCmpNbrLen = iAccNbrLen<m_iMaxHeadLen ? iAccNbrLen:m_iMaxHeadLen;

    sKey[iCmpNbrLen] = 0;

    int iPosition =0;

    KeyTree<HeadEx>* pLocalHead=0;
    if(pType !=0)
    {
        iPosition = (int)(pType[2]|0x20);
        iPosition = iPosition - (int) 'a';
        pLocalHead = m_ppoLocalHead[iOrgType][iPosition];
    }else
        pLocalHead = m_ppoLocalHead[iOrgType][0];
    if(pLocalHead ==0)
        return false;
    
    while (iCmpNbrLen>0) {
        
        int iMatchLen;

        if (pLocalHead->getMax(sKey,&oHeadExTmp, &iMatchLen)) {            
            *_pLocalHead = oHeadExTmp;
            //// 号码长度不匹配
            //if (_pLocalHead->m_iLen && _pLocalHead->m_iLen != iAccNbrLen ) {
   //             iPos = iMatchLen - 1;
   //             sKey[iPos] = 0;
            //  continue;
            //}
            
            // 不在生效时间范围内
            pHeadExTmp = &oHeadExTmp;
            if ((strcmp(_sTime,pHeadExTmp->m_sEffDate)<0)
                ||(strcmp(_sTime,pHeadExTmp->m_sExpDate) >= 0)) {                    
                pHeadExTmp = pHeadExTmp->m_pNext;
                while(pHeadExTmp){
                    if ((strcmp(_sTime,pHeadExTmp->m_sEffDate)<0)
                       ||(strcmp(_sTime,pHeadExTmp->m_sExpDate) >= 0)) {
                       pHeadExTmp = pHeadExTmp->m_pNext; 
                       continue; 
                    }
                    *_pLocalHead = *pHeadExTmp;
                    break;                    
                }
                if( !pHeadExTmp ){
                    iCmpNbrLen = iMatchLen - 1;
                    sKey[iCmpNbrLen] = 0;
                    continue;
                }
                
            }            
            return true;
        } else {
            return false;
        }
    }
    
    return false;
}
#else
bool Head2OrgIDMgr::searchLocalHead(const char *_sAccNbr,  const char *_sTime, 
        HeadEx *_pLocalHead,const char* pType,const int iOrgType )
{
    if(!_pLocalHead)  //参数_pLocalHead有存储空间,函数里做拷贝函数
    {
        return false;
    }
    else if(iOrgType>=MAX_TYPE)
    {
        Log::log(0,"区域类型超界:%d",iOrgType);
        return false;
    }
    
    HeadEx* pHeadExTmp = 0; 
    HeadEx* pHead = 0; 
    char sKey[128];          //存储索引变量
    memset(sKey, '\0', 128);

    char sAccNbr[MAX_ACCNBR_LEN_EX+1];  //存储accnbr,参数中超出部分截断不用
    memset(sAccNbr, '\0', MAX_ACCNBR_LEN_EX+1);
    int iAccNbrLen = strlen(_sAccNbr);  //accnbr的字符串长度, 如果超过了最大长度就截断
    iAccNbrLen = iAccNbrLen < MAX_ACCNBR_LEN_EX ? iAccNbrLen : MAX_ACCNBR_LEN_EX;
    strncpy(sAccNbr, _sAccNbr, iAccNbrLen);

    int iPosition = 0;
    if(pType != 0)
    {
        iPosition = (int)(pType[2]|0x20);
        iPosition = iPosition - (int)'a';
    }
    else
    {
        iPosition = 0;
    }

    sprintf(sKey, "%d_%d_%s", iPosition, iOrgType, sAccNbr);
    int iStrLength = strlen(sKey);  //索引串的长度

    int i = 1;
    unsigned int iIdx = 0;
    pHead = (HeadEx *)G_PPARAMINFO->m_poHeadExList;
    while(i < iAccNbrLen)
    {
        if(G_PPARAMINFO->m_poHeadExIndex->get(sKey, &iIdx))
        {
            pHeadExTmp = (HeadEx *)G_PPARAMINFO->m_poHeadExList + iIdx;
            while(pHeadExTmp != pHead)
            {
                if((strcmp(_sTime,pHeadExTmp->m_sEffDate)<0)||(strcmp(_sTime,pHeadExTmp->m_sExpDate) >= 0))
                {
                    pHeadExTmp = pHead + pHeadExTmp->m_iNextOffset;
                    continue;
                }
                else
                {
                    *_pLocalHead = *pHeadExTmp;  //拷贝给返回参数
                    return true;
                }
            }
            
        }

        sKey[iStrLength-i] = '\0';
        i++;
    }
    
    return false;
}
#endif


// add by zhangch 20100603
bool Head2OrgIDMgr::isBelongOrg(const char *pHead, const char* pTime,int iOrgID,
            const char* pType,const int iOrgType)
{
    char sType[4]="50Z";
    int  iType = iOrgType;
    
    if( !pType || pType[0]=='\0')
        pType = sType;
    if( iOrgType == 0 )
        iType == BILL_ORG ;
    
    if(!Head2OrgIDMgr::searchLocalHead(pHead,pTime,&Head2OrgIDMgr::m_goHeadEx,pType,iType))
        return false;

    if (m_pOrgMgrEx == NULL)
        m_pOrgMgrEx = OrgMgrEx::GetInstance();
    // 判断归并关系
    if (!(m_pOrgMgrEx->getBelong(m_goHeadEx.getOrgID(), iOrgID, iOrgType)))
        return false;

    return true;
}


//##ModelId=42803E6602C3
int HeadEx::getOrgID() const
{
    return m_iOrgID;
}

//##ModelId=42803E810055
char* HeadEx::getAreaCode() const
{
    return (char *)m_sAreaCode;
}

//##ModelId=42803E940193
//int HeadEx::getAccNbrLen() const
//{
//  return m_iLen;
//}

//##ModelId=42803EAC00CF
char* HeadEx::getHead() const
{
    return (char *)m_sHead;
}

//##ModelId=4280447C0292
int Head2OrgIDMgr::m_iMaxHeadLen = 0;

//##ModelId=42804495031A
int Head2OrgIDMgr::m_iMinHeadLen = 0;

//##ModelId=4280316802FD
HeadEx Head2OrgIDMgr::m_goHeadEx;

