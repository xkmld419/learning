/*VER: 3******************************************** 
* Copyright (c) 2009,联创科技股份有限公司电信事业部*
*All rights reserved.*******************************
* modified by zhougc******************************/

#include "Environment.h"
#include "AcctItemOweMgr.h"
#include "ParamDefineMgr.h"
#include "Log.h"


#define SHM_ACCTITEMOWE_DATA		(IpcKeyMgr::getIpcKey(-1,"SHM_AcctitemOweBuf"))
#define SHM_ACCTITEMOWE_INDEX1		(IpcKeyMgr::getIpcKey(-1,"SHM_AcctitemOweIndex1"))
#define SHM_ACCTITEMOWE_INDEX2		(IpcKeyMgr::getIpcKey(-1,"SHM_AcctitemOweIndex2"))


#ifdef  USERINFO_REUSE_MODE
SHMData_A<AcctItemOweData> * AcctItemOweMgr::m_poOweData = 0;
SHMIntHashIndex_A * AcctItemOweMgr::m_poOweAcctIndex = 0;
SHMIntHashIndex_A * AcctItemOweMgr::m_poOweServIndex = 0;
#else

SHMData<AcctItemOweData> * AcctItemOweMgr::m_poOweData = 0;
SHMIntHashIndex * AcctItemOweMgr::m_poOweAcctIndex = 0;
SHMIntHashIndex * AcctItemOweMgr::m_poOweServIndex = 0;
#endif


bool AcctItemOweMgr::m_bUploaded = false;

#define ACCTITEMOWEBUF_DATA_COUNT (ParamDefineMgr::getLongParam("MEMORY_DB", "AcctItemOweDataCnt"))

AcctItemOweMgr::AcctItemOweMgr()
{

	if (m_bUploaded) return;

#ifdef  USERINFO_REUSE_MODE
	m_poOweData = new SHMData_A<AcctItemOweData> (SHM_ACCTITEMOWE_DATA);
	m_poOweAcctIndex = new SHMIntHashIndex_A (SHM_ACCTITEMOWE_INDEX1);
	m_poOweServIndex = new SHMIntHashIndex_A (SHM_ACCTITEMOWE_INDEX2);
#else

	m_poOweData = new SHMData<AcctItemOweData> (SHM_ACCTITEMOWE_DATA);
	m_poOweAcctIndex = new SHMIntHashIndex (SHM_ACCTITEMOWE_INDEX1);
	m_poOweServIndex = new SHMIntHashIndex (SHM_ACCTITEMOWE_INDEX2);
#endif

	if (!m_poOweData->exist () || !m_poOweAcctIndex->exist () || !m_poOweServIndex->exist ()) {
		m_bUploaded = false;
		load ();
	} else {
		m_bUploaded = true;
	}
}

AcctItemOweMgr::~AcctItemOweMgr()
{
}

void AcctItemOweMgr::unload()
{
	if (m_poOweData->exist()) m_poOweData->remove ();
	if (m_poOweServIndex->exist ()) m_poOweServIndex->remove ();
	if (m_poOweAcctIndex->exist ()) m_poOweAcctIndex->remove ();
	
	cout<<"删除历史欠费成功!"<<endl;
}

void AcctItemOweMgr::load()
{	
	int iCount, i = 0, iTemp, ltempAcctID = 0;	
	unsigned int k,t;
	long lAcctID = 0 ;
	char sSql[400]= {0};	
	char sTableName [30][30] = {0};
	unload();
		
	cout<<"加载历史欠费中，请耐心等待......"<<endl;
	m_poOweData->create (ACCTITEMOWEBUF_DATA_COUNT);
	m_poOweServIndex->create (ACCTITEMOWEBUF_DATA_COUNT);
	m_poOweAcctIndex->create (ACCTITEMOWEBUF_DATA_COUNT);
	
	AcctItemOweData * pData = *m_poOweData;
	ServInfo *pServData = (ServInfo*)*m_poServData;
		
	DEFINE_QUERY (qry);

	sprintf(sSql, "SELECT A.TABLE_NAME FROM  A_SPLIT_TABLE_CONFIG A "
					" WHERE A.TABLE_TYPE = 1");	
	qry.setSQL (sSql);
    qry.open ();
    while (qry.next())
    {
    	strcpy(sTableName[i++], qry.field("TABLE_NAME").asString());	
    }
    i--;
	for(;i>=0;)
   	{   		
   		sprintf(sSql, " SELECT ACCT_ID,nvl(SERV_ID,-1) SERV_ID,SUM(AMOUNT) AMOUNT "
   						" FROM %s WHERE STATE IN  ('5JA','5JC','5JD','5JF') "
   						"  GROUP BY ACCT_ID,NVL(SERV_ID,-1)", sTableName[i--]);	
		qry.close();		  
		qry.setSQL (sSql);
		qry.open();
		while (qry.next ()) {
	
			iTemp = m_poOweData->malloc ();		
			if (!iTemp) THROW (MBC_OweMgr+3);
    	
			pData[iTemp].iAcctID = qry.field (0).asLong();
			pData[iTemp].iServID = qry.field (1).asLong();
			pData[iTemp].iAmount = qry.field (2).asLong();
			pData[iTemp].iParTID = i;
			//pData[iTemp].iBillingCnt = qry.field (3).asInteger ();
			pData[iTemp].isValid = 'Y';	
			strcpy(pData[iTemp].m_sExpDate,"80000000");			
	
			if(!strcmp(pServData[t].m_sState,"2HX"))
			{
				strcpy(pData[t].m_sExpDate,pServData[t].m_sStateDate);
				pData[iTemp].m_sExpDate[14] = 0;
			}	
			if (m_poOweAcctIndex->get (pData[iTemp].iAcctID, &k)) {
				pData[iTemp].iNextAcctOffset = pData[k].iNextAcctOffset;
				pData[k].iNextAcctOffset  = iTemp;
				} else {
					m_poOweAcctIndex->add (pData[iTemp].iAcctID, iTemp);
			}
    	
			if (m_poOweServIndex->get (pData[iTemp].iServID, &k)) {
				pData[iTemp].iNextServOffset = pData[k].iNextServOffset;
				pData[k].iNextServOffset  = iTemp;
			} else {
				m_poOweServIndex->add (pData[iTemp].iServID, iTemp);
			}		
			if ((iTemp % 40000) == 0 )
			{
				Log::log(0,"加载了%ld",iTemp);
			}
		}
		Log::log(0,"加载了%ld",iTemp);
	}
	
	qry.close ();

	Log::log(0,"加载历史欠费结束");
	
	printOweMemory();
		
}

int AcctItemOweMgr::getServOwe(long iServID)
{
	AcctItemOweData * pData =  *m_poOweData;
	unsigned int k;
	int iRet = 0;

	if (!m_poOweServIndex->get (iServID, &k))
	{
		 return 0;
	}
	while (k)
	{
		 if(pData[k].isValid == 'Y')
		{
			iRet += pData[k].iAmount;
		}
		k = pData[k].iNextServOffset;
	}
	return iRet;
}

int AcctItemOweMgr::getAcctOwe(long iAcctID)
{			
	unsigned int k;
	int iRet = 0;

	AcctItemOweData * pData =  *m_poOweData;
	if (!m_poOweAcctIndex->get (iAcctID, &k))
	{
		return 0;
	}
	while (k)
	{
		if(pData[k].isValid == 'Y')
		{
			iRet += pData[k].iAmount;
		}
		k = pData[k].iNextAcctOffset;
	}
	
	return iRet;
}

int AcctItemOweMgr::getBillingCnt(long iAcctID)
{		
	AcctItemOweData * pData =  *m_poOweData;
	unsigned int k;
	int iRet = 0;
	
	if (!m_poOweAcctIndex->get (iAcctID, &k)) 
	{
		return 0;
	}
	
	while (k)
	{
		if (iRet<pData[k].iBillingCnt)
			iRet = pData[k].iBillingCnt;
		k = pData[k].iNextAcctOffset;
	}
	
	return iRet;
}
/*考虑销账分表的问题,暂不适用*/
int AcctItemOweMgr::updateOweMemory()
{
	int iCount,flag,i = 0;
	unsigned int iTemp,temp,k,t;
	long lAcctID = 0 ;
	char sSql[300]= {0};	
	char sTableName [30][30] = {0};
	AcctItemOweData tempData;
	AcctItemOweData * pData = *m_poOweData;
	ServInfo *pServData = (ServInfo*)*m_poServData;
	
	DEFINE_QUERY (qry);	 
	
	cout<<"正在更行历史欠费中,请耐心等待......."<<endl;
		
    
    sprintf(sSql, "SELECT A.TABLE_NAME FROM  A_SPLIT_TABLE_CONFIG A "
					" WHERE A.TABLE_TYPE = 1");	
	qry.setSQL (sSql);
    qry.open ();
    while (qry.next())
    {
    	strcpy(sTableName[i++], qry.field("TABLE_NAME").asString());	
    }
    i--;
	qry.close();
	for(;i>=0;)
   	{
		sprintf(sSql, "SELECT ACCT_ID,nvl(SERV_ID,-1) SERV_ID,SUM(AMOUNT) AMOUNT FROM "
						" %s WHERE STATE IN ('5JA','5JC','5JD','5JF')"
						"GROUP BY ACCT_ID,NVL(SERV_ID,-1)", sTableName[i--]);	
		qry.close();
		qry.setSQL (sSql);	  
		qry.open();	
		while (qry.next ())
		{		
			tempData.iAcctID = qry.field (0).asLong();
			tempData.iServID = qry.field (1).asLong();			
			tempData.iAmount = qry.field(2).asLong();
			tempData.iParTID = i;
			//tempData.iBillingCnt = qry.field (3).asInteger();				
			flag  = 0;	

			if ( m_poOweAcctIndex->get (tempData.iAcctID, &k))
			{
				temp = k;
				while(temp)  
				{
					if ( pData[temp].iServID == tempData.iServID )
					{
						flag  = 1;							
						
						if ( tempData.iParTID == pData[temp].iParTID )
						{
							if(pData[temp].iAmount != tempData.iAmount)
							{
								pData[temp].iAmount = tempData.iAmount;
							}
						}
						if(!m_poServIndex->get(tempData.iServID,&t))
						{
							Log::log(0,"找不到用户%ld",pData[temp].iServID);
							//	continue;
						}	
						//已经拆机的
						if(!strcmp(pServData[t].m_sState,"2HX"))
						{
							pData[temp].isValid = 'N';
							strcpy(pData[temp].m_sExpDate, pServData[t].m_sStateDate);
							pData[temp].m_sExpDate[14] = 0;
						}
					}
					temp = pData[temp].iNextAcctOffset;
				}
				if ( 0 == flag )
				{
					iTemp = m_poOweData->malloc ();		
		    		if (!iTemp) THROW (MBC_OweMgr+3);
					pData[iTemp].iAcctID = tempData.iAcctID ;
					pData[iTemp].iServID = tempData.iServID;
					pData[iTemp].iAmount = tempData.iAmount ;
					//pData[iTemp].iBillingCnt = tempData.iBillingCnt ;	
					strcpy(pData[iTemp].m_sExpDate,"80000000");					
					pData[iTemp].isValid = 'Y';

					pData[iTemp].iNextAcctOffset = pData[k].iNextAcctOffset;
					pData[k].iNextAcctOffset  = iTemp;

					//已经拆机的
					if(!strcmp(pServData[t].m_sState,"2HX"))
					{
						pData[iTemp].isValid = 'N';
						strcpy(pData[iTemp].m_sExpDate, pServData[t].m_sStateDate);
						pData[iTemp].m_sExpDate[14] = 0;
					}
				}
			}
			else
			{
				iTemp = m_poOweData->malloc ();
		  		if (!iTemp) THROW (MBC_OweMgr+3);
        		
				pData[iTemp].iAcctID = tempData.iAcctID ;
				pData[iTemp].iServID = tempData.iServID;
				pData[iTemp].iAmount = tempData.iAmount ;
				//pData[iTemp].iBillingCnt = tempData.iBillingCnt ;
				strcpy(pData[iTemp].m_sExpDate,"80000000");
				pData[iTemp].isValid = 'Y';
				m_poOweAcctIndex->add (pData[iTemp].iAcctID, iTemp);

				
				//队已经拆机的回收
				if(!strcmp(pServData[t].m_sState,"2HX"))
				{
					pData[iTemp].isValid = 'N';
					strcpy(pData[iTemp].m_sExpDate, pServData[t].m_sStateDate);
					pData[iTemp].m_sExpDate[14] = 0;
				}
			}
		
			/*index for serv */		
			if ( m_poOweServIndex->get (tempData.iServID, &k))
			{
				if( 0 == flag)
				{
					pData[iTemp].iNextServOffset = pData[k].iNextServOffset;
					pData[k].iNextServOffset  = iTemp;
				}
			}
			else
			{
				m_poOweServIndex->add (pData[iTemp].iServID, iTemp);
			}	
		}
	}
	
	qry.close ();
	
	#ifdef  USERINFO_REUSE_MODE
  		revokeOwe();
	#endif

	printOweMemory();

	cout<<"更新历史欠费结束！"<<endl;
	
	return 1;
}


void AcctItemOweMgr::printOweMemory()
{

	unsigned int iTemp,temp;
	
	iTemp = m_poOweData->getTotal();
	temp = m_poOweData->getCount();
	cout<<"数据区总数："<<iTemp<<"  已使用："<<temp<<endl;
	
	iTemp = m_poOweServIndex->getTotal();
	temp = m_poOweServIndex->getCount();
	cout<<"索引区A总数："<<iTemp<<"  已使用："<<temp<<endl;
	
	iTemp = m_poOweAcctIndex->getTotal();
	temp = m_poOweAcctIndex->getCount();
	cout<<"索引区B总数："<<iTemp<<"  已使用："<<temp<<endl;
		
/*
	AcctItemOweData * pData = *m_poOweData;
	DEFINE_QUERY (qry);	 
	
	m_poOweData->showDetail();
	m_poOweServIndex->showDetail();
	m_poOweAcctIndex->showDetail();

	cout<<"\t"<<endl;
	cout<<"正在打印内存欠费信息明细......."<<endl;
  temp = 0;
  cout<<"number\tacctid\tservid\tamount\tbillingcnt\tisvalid\texpdate\tnextacctoffset\tnextservoffset"<<endl;
  while(temp <= ACCTITEMOWEBUF_DATA_COUNT)
    {
      pData++;
      temp++;
      if (pData->iAcctID > 0)
      	 {
            cout<<temp<<"\t"<<pData->iAcctID<<"\t"<<pData->iServID<<"\t"<<pData->iAmount<<"\t"<<pData->iBillingCnt
                <<"\t"<<pData->isValid<<"\t"<<pData->m_sExpDate<<"\t"<<pData->iNextAcctOffset<<"\t"<<pData->iNextServOffset<<endl;
         }
    }		
	cout<<"完成打印内存欠费明细......."<<endl<<endl;
	

	cout<<"打印帐户内存欠费......."<<endl;
	qry.setSQL (" select distinct acct_id " \
		" from acct_item_owe where (state in ('5JA','5JC','5JD','5JF') or (state='5JB' and trunc(state_date)>=trunc(sysdate-30))) ");
	qry.open();	
	temp = 0;
  cout<<"number\tacctid\tamount"<<endl;
	while (qry.next ())
	{		
    cout<<++temp<<"\t"<<qry.field(0).asLong()<<"\t"<<getAcctOwe(qry.field(0).asLong())<<endl;
	}
	qry.close ();
	cout<<"\完成打印帐户内存欠费......."<<endl<<endl;


	cout<<"打印用户内存欠费......."<<endl;
	qry.setSQL (" select distinct nvl(serv_id,-1) " \
		" from acct_item_owe where (state in ('5JA','5JC','5JD','5JF') or (state='5JB' and trunc(state_date)>=trunc(sysdate-30))) ");
	qry.open();	
	temp = 0;
  cout<<"number\tservid\tamount"<<endl;
	while (qry.next ())
	{		
    cout<<++temp<<"\t"<<qry.field(0).asLong()<<"\t"<<getServOwe(qry.field(0).asLong())<<endl;
	}
	qry.close ();
	cout<<"\完成打印用户内存欠费......."<<endl<<endl;
*/
}

int AcctItemOweMgr::getAcctOweV(long lAcctID,vector<AcctItemOweData> & vData)
{
	unsigned int k;

	AcctItemOweData * pData =  *m_poOweData;
	if (!m_poOweAcctIndex->get (lAcctID, &k))
	{
		return 0;
	}
	while (k)
	{
		if(pData[k].isValid == 'Y')
		{
			if (pData[k].iAmount != 0)
				vData.push_back(pData[k]);
		}
		k = pData[k].iNextAcctOffset;
	}
	
	return 0;
	
}

#ifdef  USERINFO_REUSE_MODE
void  AcctItemOweMgr::revokeOwe()
{	
	tidyUserInfo(m_poOweData, "ACCTITEMOWE",	
	m_poOweAcctIndex,(SHMData_A<int> *)0,0,(size_t)offsetof(AcctItemOweData,iNextAcctOffset),
	m_poOweServIndex,(SHMData_A<int> *)0,0,(size_t)offsetof(AcctItemOweData,iNextServOffset),
	0,(SHMData_A<int> *)0,0,-1,
	0,(SHMData_A<int> *)0,0,-1,
	0,(SHMData_A<int> *)0,0,-1 );

/*
	UserInfoAdmin *prevoke =  new UserInfoAdmin();	
	prevoke->tidyUserInfo(m_poOweData, "ACCTITEMOWE",	
	m_poOweAcctIndex,(SHMData_A<int> *)0,0,offsetof(AcctItemOweData,iNextAcctOffset),
	m_poOweServIndex,(SHMData_A<int> *)0,0,offsetof(AcctItemOweData,iNextServOffset),
	(SHMIntHashIndex_A*)0,(SHMData_A<int> *)0,0,-1,
	(SHMIntHashIndex_A*)0,(SHMData_A<int> *)0,0,-1,
	(SHMIntHashIndex_A*)0,(SHMData_A<int> *)0,0,-1 );
	delete prevoke;
*/}
#endif

//============================================================================
////iChkOffset为-1 表示 对应的idx是主键
#ifdef  USERINFO_REUSE_MODE
template < template<class > class dataT,class Y ,template<class > class getT1,class Z1,template<class > class getT2,class Z2,template<class > class getT3,class Z3,template<class > class getT4,class Z4,template<class >class getT5,class Z5 >
unsigned int AcctItemOweMgr::tidyUserInfo(dataT<Y> * pData, const char * sParamCode,
    SHMIntHashIndex_A * pIdx1, getT1<Z1> * pGet1, int iGetOffset1, int iChkOffset1, 
    SHMIntHashIndex_A * pIdx2, getT2<Z2> * pGet2, int iGetOffset2, int iChkOffset2,
    SHMIntHashIndex_A * pIdx3, getT3<Z3> * pGet3, int iGetOffset3, int iChkOffset3,
    SHMIntHashIndex_A * pIdx4, getT4<Z4> * pGet4, int iGetOffset4, int iChkOffset4,
    SHMIntHashIndex_A * pIdx5, getT5<Z5> * pGet5, int iGetOffset5, int iChkOffset5)
{
    long key;
    unsigned int uiKeyOffset;
    char countbuf[32];    
    char sLimitDate[16];
    unsigned int iKickCnt[6];
    unsigned int iRevokeCnt=0;
    
    memset( sLimitDate,0,sizeof(sLimitDate) );
    int iNum = 3;
    if(ParamDefineMgr::getParam(USERINFO_LIMIT, sParamCode, countbuf))
    {
			iNum = atoi(countbuf);
		}
		
    if(iNum<=0)
        return 0;

    memset(iKickCnt, 0, sizeof(iKickCnt) );
    int  iExpOffset = offsetof(Y, m_sExpDate);
    Y* p = (Y*)( * pData);
    
    getBillingCycleOffsetTime(iNum,sLimitDate);
    SHMIntHashIndex_A * pIdx[]={pIdx1, pIdx2, pIdx3,pIdx4,pIdx5, 0 };
    int  iChkOffset[]={iChkOffset1, iChkOffset2, iChkOffset3,iChkOffset4,iChkOffset5, -1};
    void * pGet[]={pGet1,pGet2,pGet3,pGet4,pGet5, 0};
    int  iGetOffset[]={iGetOffset1,iGetOffset2,iGetOffset3,iGetOffset4,iGetOffset5, -1};
    
    int iNxtOffset = -1;
    int iLoop=0;
    unsigned int uiDataOff=0;
    for( iLoop=0; iLoop<5; iLoop++){
        
        if(0== pIdx[iLoop+1] )
            break;
        SHMIntHashIndex_A::Iteration iter = pIdx[iLoop]->getIteration();
        iNxtOffset =  iChkOffset[iLoop];
        if( iNxtOffset<0 ){
            while( iter.next(key, uiKeyOffset) ){
                /////主键索引，直接更新为0
                /////如果是主键，只在index上面记录了 offset值，数据区没有next的存放
                ////主键不必支持再从另外的偏移获取offset了
                char * pExp = ( ( (char *)&p[uiKeyOffset] ) + iExpOffset);
                if(  strncmp(pExp, sLimitDate, 8)<0 ){
                    iter.removeMe( );
                    iKickCnt[iLoop]++;
                }
            
            }
            
            ////for循环继续
            continue;
        }
        
        while( iter.next(key, uiKeyOffset) ){
            unsigned int *pFirst = 0;
            if(!pGet[iLoop]){
                uiDataOff = uiKeyOffset;
            }else{
                switch(iLoop){
                case 0:
                    {
                    Z1* pGetFrom = (Z1*)(* pGet1);
                    
                    pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                    uiDataOff = *pFirst;
                    }
                    break;
                case 1:
                    {
                    Z2* pGetFrom = (Z2*)(* pGet2);
                    pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                    uiDataOff = *pFirst;
                    }
                    break;
                case 2:
                    {
                    Z3* pGetFrom = (Z3*)(* pGet3);
                    pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                    uiDataOff = *pFirst;
                    }
                    break;
                case 3:
                    {
                    Z4* pGetFrom = (Z4*)(* pGet4);
                    pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                    uiDataOff = *pFirst;
                    }
                    break;
                case 4:
                    {
                    Z5* pGetFrom = (Z5*)(* pGet5);
                    pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                    uiDataOff = *pFirst;
                    }
                    break;
                default:
                    uiDataOff = 0;
                    break;
                }
            }
            if(0==uiDataOff)
                continue;

            unsigned int uiFirst=uiDataOff;
            unsigned int uiDataNext = *(unsigned int *)( ( (char *)&p[uiDataOff] )+ iNxtOffset);

            while(uiDataNext){
                char * pExp = ( ( (char *)&p[uiDataNext] ) + iExpOffset);
                if( strncmp(pExp, sLimitDate, 8)<0 ){
                    ///过期的数据
                    *(unsigned int *)( ( (char *)&p[uiDataOff] )+ iNxtOffset) =
                    *(unsigned int *)( ( (char *)&p[uiDataNext])+ iNxtOffset);
                    
                    *(unsigned int *)( ( (char *)&p[uiDataNext])+ iNxtOffset)=0;

                    iKickCnt[iLoop]++;
                    uiDataNext=*(unsigned int *)( ( (char *)&p[uiDataOff] )+ iNxtOffset);
                    continue;
                }
                
                uiDataOff = uiDataNext;
                uiDataNext=*(unsigned int *)( ( (char *)&p[uiDataNext] )+ iNxtOffset);
                
            }
            char * pExp = ( ( (char *)&p[uiFirst] ) + iExpOffset);
            if(  strncmp(pExp, sLimitDate, 8)<0 ){
                ////第一个也要删掉，此时得更新索引
                uiDataNext=*(unsigned int *)( ( (char *)&p[uiFirst] )+ iNxtOffset);
                if(pFirst){
                    ////更新第一个的索引，他是另外的类的成员
                    *pFirst = uiDataNext;
                }else{
                    ////是类本身的
                    if( 0 == uiDataNext){
                        iter.removeMe();
                    }else{
                        iter.setValue( uiDataNext );
                    }
                }
                *(unsigned int *)( ( (char *)&p[uiFirst] )+ iNxtOffset) = 0;
                iKickCnt[iLoop]++;
            }

        }
        

    }
    
    if(iLoop>=0 && iLoop<=4 ){

        SHMIntHashIndex_A::Iteration iter = pIdx[iLoop]->getIteration();
        iNxtOffset =  iChkOffset[iLoop];

        if( iNxtOffset<0 ){
            while( iter.next(key, uiKeyOffset) ){
                /////主键索引，直接更新为0
                /////如果是主键，只在index上面记录了 offset值，数据区没有next的存放
                ////主键不必支持再从另外的偏移获取offset了
                char * pExp = ( ( (char *)&p[uiKeyOffset] ) + iExpOffset);
                if(  strncmp(pExp, sLimitDate, 8)<0 ){
                    iter.removeMe( );
                    iKickCnt[iLoop]++;
                    unsigned int iDelete = uiKeyOffset;
                    if(    ( iChkOffset1<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset1) )
                        && ( iChkOffset2<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset2) )
                        && ( iChkOffset3<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset3) )
                        && ( iChkOffset4<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset4) )
                        ){
                        ////需要revoke掉
                        pData->revoke(uiKeyOffset);
                        iRevokeCnt++;
                    }
                }
            
            }
            
            ////主键的处理结束
        }else{

            while( iter.next(key, uiKeyOffset) ){
                unsigned int *pFirst = 0;
                if(!pGet[iLoop]){
                    uiDataOff = uiKeyOffset;
                }else{
                    switch(iLoop){
                    case 0:
                        {
                        Z1* pGetFrom = (Z1*)(* pGet1);
                        pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                        uiDataOff = *pFirst;
                        }
                        break;
                    case 1:
                        {
                        Z2* pGetFrom = (Z2*)(* pGet2);
                        pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                        uiDataOff = *pFirst;
                        }
                        break;
                    case 2:
                        {
                        Z3* pGetFrom = (Z3*)(* pGet3);
                        pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                        uiDataOff = *pFirst;
                        }
                        break;
                    case 3:
                        {
                        Z4* pGetFrom = (Z4*)(* pGet4);
                        pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                        uiDataOff = *pFirst;
                        }
                        break;
                    case 4:
                        {
                        Z5* pGetFrom = (Z5*)(* pGet5);
                        pFirst = (unsigned int *)( ( (char *)&pGetFrom[uiKeyOffset] )+ iGetOffset[iLoop]);
                        uiDataOff = *pFirst;
                        }
                        break;
                    default:
                        uiDataOff = 0;
                        break;
                    }
                }
                if(0==uiDataOff)
                    continue;
                
                unsigned int uiFirst=uiDataOff;
                unsigned int uiDataNext = *(unsigned int *)( ( (char *)&p[uiDataOff] )+ iNxtOffset);
    
                while(uiDataNext){
                    char * pExp = ( ( (char *)&p[uiDataNext] ) + iExpOffset);
                    if( strncmp(pExp, sLimitDate, 8)<0 ){
                        ///过期的数据
                        unsigned int iDelete = uiDataNext;
                        *(unsigned int *)( ( (char *)&p[uiDataOff] )+ iNxtOffset) =
                        *(unsigned int *)( ( (char *)&p[uiDataNext])+ iNxtOffset);
                        
                        *(unsigned int *)( ( (char *)&p[uiDataNext])+ iNxtOffset)=0;
    
    
                        iKickCnt[iLoop]++;
    
                        if(    ( iChkOffset1<0 
                                || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset1) )
                            && ( iChkOffset2<0 
                                || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset2) )
                            && ( iChkOffset3<0 
                                || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset3) )
                            && ( iChkOffset4<0 
                                || 0 == *(unsigned int *)( ( (char *)&p[iDelete] )+ iChkOffset4) )
                            ){
                            ////需要revoke掉
                            pData->revoke(iDelete);
                            iRevokeCnt++;
                        }
                        
                        uiDataNext=*(unsigned int *)( ( (char *)&p[uiDataOff] )+ iNxtOffset);                        
                        continue;
                    }
                    
                    uiDataOff = uiDataNext;
                    uiDataNext=*(unsigned int *)( ( (char *)&p[uiDataNext] )+ iNxtOffset);
                    
                }
                char * pExp = ( ( (char *)&p[uiFirst] ) + iExpOffset);
                if(  strncmp(pExp, sLimitDate, 8)<0 ){
                    ////第一个也要删掉，此时得更新索引
                    uiDataNext=*(unsigned int *)( ( (char *)&p[uiFirst] )+ iNxtOffset);
                    if(pFirst){
                        ////更新第一个的索引，他是另外的类的成员
                        *pFirst = uiDataNext;
                    }else{
                        ////是类本身的
                        if( 0 == uiDataNext){
                            iter.removeMe();
                        }else{
                            iter.setValue( uiDataNext );
                        }
                    }
                    *(unsigned int *)( ( (char *)&p[uiFirst])+ iNxtOffset)=0;
                    
                    iKickCnt[iLoop]++;
                    
                      if(
                           ( iChkOffset1<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[uiFirst] )+ iChkOffset1) )
                        && ( iChkOffset2<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[uiFirst] )+ iChkOffset2) )
                        && ( iChkOffset3<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[uiFirst] )+ iChkOffset3) )
                        && ( iChkOffset4<0 
                            || 0 == *(unsigned int *)( ( (char *)&p[uiFirst] )+ iChkOffset4) )
                       ){
                        pData->revoke(uiFirst);
                        iRevokeCnt++;
                    }            
                }
            }
        }////非主键的处理结束

        cout<< "kick "<<sParamCode<<":" << iKickCnt[iLoop] <<endl;
        for(int iTmp=0;iTmp<=iLoop; iTmp++ ){
            if(iRevokeCnt != iKickCnt[iTmp] ){
                cout<< "memery index somewhere wrong,"
                <<"or revoke operation executed by another program at the same time."
                <<endl;
            }
        }
    }

    return iRevokeCnt;
}


#endif

/*add end*/

