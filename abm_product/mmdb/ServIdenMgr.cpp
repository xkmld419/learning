/*VER: 1*/ 
#include <string.h>
#include "Environment.h"
#include "ServIdenMgr.h"
#include "IpcKey.h"
#include "ParamDefineMgr.h"

SHMData<ServIdenCenterData> * ServIdenMgr::m_pocServIdenData = 0;
SHMStringTreeIndex * ServIdenMgr::m_pocServIdenIndex = 0;

bool ServIdenMgr::m_bUploaded = false;
#ifdef USERINFO_OFFSET_ALLOWED
static int g_iOffsetSecs = -1;

int ServIdenMgr::m_offsetSecs =  -1;

int ServIdenMgr::m_beginOffsetSecs = 0;
#endif
static long getIpcKey(int i_iBillFlowID,char * i_sIpcName)
{
  return IpcKeyMgr::getIpcKey (i_iBillFlowID, i_sIpcName);
}

ServIdenMgr::ServIdenMgr()
{
	if (m_bUploaded) return;

	m_pocServIdenData = new SHMData<ServIdenCenterData> (IpcKeyMgr::getIpcKey(-1,"SHM_CServIdenBuf"));
	m_pocServIdenIndex = new SHMStringTreeIndex (IpcKeyMgr::getIpcKey(-1,"SHM_CServIdenIdxBuf"));


	if (!m_pocServIdenData->exist () || !m_pocServIdenIndex->exist ()) {
		m_bUploaded = false;
		load ();
	} else {
		m_bUploaded = true;
	}
	m_poUserLogMgr =new UserInfoLogMgr();
	
	#ifdef USERINFO_OFFSET_ALLOWED
    //if(m_offsetSecs==-1){
    if(g_iOffsetSecs==-1){
      loadOffsetSecs();
    }
  #endif
    
}

ServIdenMgr::~ServIdenMgr()
{
	 delete m_poUserLogMgr;
}

void ServIdenMgr::unload()
{
	if (m_pocServIdenData->exist()) m_pocServIdenData->remove ();
	if (m_pocServIdenIndex->exist ()) m_pocServIdenIndex->remove ();
}

void ServIdenMgr::load()
{
	DEFINE_QUERY (qry);
	int iCount, iTemp;
	ServIdenCenterData * pData;
	unsigned int k;
  char	 sql[2000],sDate[16];
  Date d;
  
  char countbuf[32];
	memset(countbuf,'\0',sizeof(countbuf));
	
	if (m_pocServIdenData->exist ()) m_pocServIdenData->remove ();
	if (m_pocServIdenIndex->exist ()) m_pocServIdenIndex->remove ();

  strncpy(sDate,d.toString(),8);
  sDate[8]=0;
  
  if(!ParamDefineMgr::getParam("USERINFO_UPLOAD", "SERV_IDEN_CENTER", countbuf))
	{
	    THROW(MBC_UserInfoCtl+100);
	}
	iCount = atoi(countbuf);
	
	m_pocServIdenData->create (CAL_COUNT(iCount));
	m_pocServIdenIndex->create (CAL_COUNT(iCount),MAX_CALLING_NBR_LEN);

	pData = *m_pocServIdenData;

	sprintf (sql, "select serv_id, acc_nbr, "
        "eff_date,"
        "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date, '50Z' acc_nbr_type "
        "from serv_identification_vw "
        "union all "
        "select serv_id, acc_nbr, "
        "eff_date, "
        "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date, acc_nbr_type "
        "from a_serv_ex_vw");

	qry.setSQL (sql);
	
	qry.open();
	
	while (qry.next ()) {
		iTemp = m_pocServIdenData->malloc ();
		
		if (!iTemp) THROW (MBC_UserInfoCtl+9);

    pData[iTemp].m_lServID = qry.field (0).asLong ();
    strcpy(pData[iTemp].m_sAccNbr,qry.field(1).asString());
    strcpy(pData[iTemp].m_sEffDate , qry.field(2).asString());
    strcpy(pData[iTemp].m_sExpDate , qry.field(3).asString());
    char sType[8];
    strcpy (sType, qry.field (4).asString ());
    pData[iTemp].m_iType = (sType[2] | 0x20) - 'a';    
    
    char * s = pData[iTemp].m_sAccNbr;
    while (*s) {
        if (*s>='A' && *s<='Z') *s |= 0x20;

        s++;
    }
    if(m_pocServIdenIndex->get(pData[iTemp].m_sAccNbr,&k))
    {
    	pData[iTemp].iNextServOffset =  pData[k].iNextServOffset;
    	pData[k].iNextServOffset = iTemp;
    } else {
    	m_pocServIdenIndex->add (pData[iTemp].m_sAccNbr , iTemp);
    }
   }

	qry.close ();
}


int ServIdenMgr::getServIdenData(char *sAccNbr,char sTime[16])
{
	if (!m_bUploaded) THROW (MBC_UserInfoCtl+2);

	ServIdenCenterData * pData =  *m_pocServIdenData;
	unsigned int k;
  
  char sTemp[MAX_BILLING_NBR_LEN];
  memset (sTemp, 0, sizeof(sTemp));

  char *p = (char *)sTemp;   

   while (*sAccNbr) {
        if (*sAccNbr >= 'A' && *sAccNbr <= 'Z') {
            *p = (*sAccNbr | 0X20);
        } else {
            *p = *sAccNbr;
        }
        
        p++;
        sAccNbr++;
    }
    
	if (!m_pocServIdenIndex->get (sTemp, &k)) return 0;
	unsigned int begin_k = k;
	
	while (k) {
		if (strcmp(sTime, pData[k].m_sEffDate) < 0 ||
			strcmp (sTime, pData[k].m_sExpDate) >=0 ) {
                k = pData[k].iNextServOffset;
                continue;
            }
    else return 1;		
	} 
  
#ifdef USERINFO_OFFSET_ALLOWED
    if(ServIdenMgr::m_offsetSecs==-1){
        ServIdenMgr::loadOffsetSecs();
    }
    k = begin_k;
    Date date_sDate(sTime);
    while (k) {
        Date date_EffDate(pData[k].m_sEffDate);
        long pEff = date_EffDate.diffSec(date_sDate);

        Date date_ExpDate(pData[k].m_sExpDate);
        long pExp = date_sDate.diffSec(date_ExpDate);

	if(pEff >= ServIdenMgr::m_beginOffsetSecs && pEff <= ServIdenMgr::m_offsetSecs)
		return 1;
	if(pExp >= ServIdenMgr::m_beginOffsetSecs && pExp <= ServIdenMgr::m_offsetSecs)
		return 1;
	k = pData[k].iNextServOffset;
    }
#endif
	return 0;
}

int ServIdenMgr::UpdateCServIdenMemory()
{
    DEFINE_QUERY (qry);
    int iCount, iTemp, iNewFlag;
    ServIdenCenterData strData;
    unsigned int k;
    char sSql[2000],sDate[16];
    Date d;
    long lOpSeq; 
    int iOpType = 0;
    
    strncpy(sDate,d.toString(),8);
    sDate[8]=0;

    ServIdenCenterData * pData = (ServIdenCenterData *)(*m_pocServIdenData);

    strcpy (sSql, "select b.serv_id, lower(b.acc_nbr) acc_nbr, "
                "b.eff_date,"
                "nvl(b.exp_date, to_date('20500101', 'yyyymmdd')) exp_date, "
                "a.op_seq, a.op_type "
                "from user_info_oplog a, serv_identification_vw b where a.memop_date is null and "
        "a.table_name='SERV_IDENTIFICATION' " 
                "and b.serv_id=a.table_column_1 and b.agreement_id=a.table_column_2 order by op_seq");

    qry.setSQL (sSql);

    qry.open();

    while (qry.next ()) {
        strcpy(strData.m_sAccNbr,qry.field (1).asString());
        strData.m_lServID = qry.field (0).asLong ();
        strcpy(strData.m_sEffDate , qry.field(2).asString());
        strcpy(strData.m_sExpDate , qry.field(3).asString());
        lOpSeq = qry.field(4).asLong();
        iOpType = qry.field(5).asInteger();
        
        strData.iNextServOffset = 0;
        
        if(iOpType == 2){
         if (!m_pocServIdenIndex->get (strData.m_sAccNbr, &k)){
        	  THROW (MBC_UserInfoCtl+32);  
        	  //m_poUserLogMgr->updateOPLog (lOpSeq);
            //continue;
          }
         while (k){
           if ((pData[k].m_lServID == strData.m_lServID) && 
           	   !strcmp (pData[k].m_sEffDate, strData.m_sEffDate) ){//&&
                 //   pData[k].m_iType == strData.m_iType) {
            	strcpy (pData[k].m_sExpDate, strData.m_sExpDate);
            	m_poUserLogMgr->updateOPLog (lOpSeq);
            	break;
            }
           k = pData[k].iNextServOffset;
          }
          continue;
        }
        
        if (iOpType == 1){
           iTemp = m_pocServIdenData->malloc ();
           if (!iTemp) THROW (MBC_UserInfoCtl+31);
	         pData[iTemp].m_lServID = strData.m_lServID;
	         strcpy (pData[iTemp].m_sAccNbr, strData.m_sAccNbr);
	         strcpy (pData[iTemp].m_sEffDate, strData.m_sEffDate);
	         strcpy (pData[iTemp].m_sExpDate, strData.m_sExpDate);
           pData[iTemp].m_iType = 25;
           if (m_pocServIdenIndex->get (strData.m_sAccNbr, &k))
           {
           	if (strcmp (strData.m_sEffDate, pData[k].m_sEffDate) >= 0){
           			pData[iTemp].iNextServOffset = k;
           			m_pocServIdenIndex->add (pData[iTemp].m_sAccNbr, iTemp);
           	}
           	else {
           		unsigned int * temp = &(pData[k].iNextServOffset);
           		while (*temp && (strcmp(pData[*temp].m_sEffDate, strData.m_sEffDate)>0)) {
               	temp = &(pData[*temp].iNextServOffset);
               }
               pData[iTemp].iNextServOffset = *temp;
               *temp = iTemp;
           	}
           		
           } else m_pocServIdenIndex->add (pData[iTemp].m_sAccNbr, iTemp);
           m_poUserLogMgr->updateOPLog (lOpSeq);
       }
    }
    m_poUserLogMgr->commit();
    return 0;
}

UserInfoLogMgr::UserInfoLogMgr() : m_iMax (2000)
{
    m_iCursor = 0;
    m_plBuf = new long[m_iMax];
}

UserInfoLogMgr::~UserInfoLogMgr()
{
    if (m_iCursor) commit ();

    delete [] m_plBuf;
}

void UserInfoLogMgr::commit()
{
    DEFINE_QUERY (qry);
    char sSQL[1024];

    if (!m_iCursor) return;

    sprintf (sSQL, "update user_info_oplog set memop_date=sysdate where op_seq=:OP_SEQ");
    qry.setSQL (sSQL);

    qry.setParamArray ("OP_SEQ", m_plBuf, sizeof (m_plBuf[0]));
    qry.execute (m_iCursor); qry.commit (); qry.close ();

    m_iCursor = 0;
}

void UserInfoLogMgr::updateOPLog(long lOpSeq)
{
    m_plBuf[m_iCursor] = lOpSeq;
    m_iCursor++;

    if (m_iCursor == m_iMax) commit ();
}


#ifdef USERINFO_OFFSET_ALLOWED


void ServIdenMgr::loadOffsetSecs()
{
    char *pt;

    if ((pt = getenv ("PROCESS_ID")) == NULL) {
        m_offsetSecs = 0;
        return;
    }
    int temp_proc = atoi (pt);
    DEFINE_QUERY(qry);
    char sql[128];
    sprintf(sql,"select offset_mins*60 from b_cfg_userinfo_offset a,wf_process b where b.process_id = %d and b.billflow_id = a.billflow_id",temp_proc);
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
