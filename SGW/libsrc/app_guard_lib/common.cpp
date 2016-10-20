// common.cpp
#include "common.h"

#ifdef	XML_SQL_PRINT_DEBUG
#include "userlog.h"
#endif
#include "platform.h"
#include <strstream>
#include "platform.h"
//#include "Timesten_Env.h"
//根据传入的不同的结构体,插入到对应的表中
/*void Pprintf(int DataNodeId,void *LogInfo)
{
	switch(DataNodeId)
	{
		case 1: //请求异常
			{
				MessageRequest *pMessageRequest = (MessageRequest*)LogInfo;
				try
				{
					DEFINE_TTCMD(qry);
					qry.Prepare(" Insert into abm_message_monitor \
  							(service_id, \
   							service_type, \
   							ABM_REQUEST_ID, \
  							OUTER_REQUEST_ID, \
   							RESULT_CODE, \
   							deal_time,abm_message_monitor_seq) \
							values \
  							(:serviceId, \
  							 :serviceType, \
   							:abmrequestid, \
   							:outrequestid, \
   							:requestcode, \
  							sysdate,abm_message_monitor_seq.nextval)");
					qry.setParam(1,pMessageRequest->m_lServiceId);
					qry.setParam(2,pMessageRequest->m_lServiceType);
					qry.setParam(3,pMessageRequest->m_sAbmRequestId);
					qry.setParam(4,pMessageRequest->m_sOutRequestId);
					qry.setParam(5,1002);
					qry.Execute(); 
					qry.Close();
					TimesTenEnv::commit();             
				}
				catch(TTError err)
				{
					TimesTenEnv::rollback();
					return ;
				}
				break;
			}
		case 3:   //消息交互性能监控
			{
				MessageMonitor *pMessage = (MessageMonitor*)LogInfo;
				try
				{
				DEFINE_TTCMD(qry);
				qry.Prepare("Insert into abm_message_monitor \
  							(service_id, \
   							service_type, \
   							request_time, \
  							request_wait_cnt, \
   							answer_wait_cnt, \
   							deal_time,abm_message_monitor_seq) \
							values \
  							(:serviceId, \
  							 :serviceType, \
   							:requestTime, \
   							:RequestWaitCnt, \
   							:AnswerWaitCnt, \
  							sysdate,abm_message_monitor_seq.nextval);");
				qry.setParam(1,pMessage->m_lServiceId);
				qry.setParam(2,pMessage->m_sServiceType);
				qry.setParam(3,pMessage->m_lRequestWaitCnt);
				qry.setParam(4,pMessage->m_lAnswerWaitCnt);
				qry.Execute(); 
				TimesTenEnv::commit();
				qry.Close();
				}
				catch(TTError err)
				{
					TimesTenEnv::rollback();
					return ;
				}
				break;        
			}   
			              
		case 5:   //鉴权异常日志
			{   
				ACLExceptionLog *pAclExcp = (ACLExceptionLog*)LogInfo;
				try
				{
					DEFINE_TTCMD(qry);
					qry.Prepare(" Insert into abm_AclException_log \
     (SERVICE_ID, OUTER_REQUEST_ID, ABM_REQUEST_ID,SERVICE_TYPE, RESULT_DESC, DEAL_DATE,abm_AclException_log_seq) \
   values \
     (:serviceId, :outrequestId, :abmRequestId,:serviceType, :resultDesc, sysdate,abm_AclException_log_seq.nextval); \
");
					qry.setParam(1,pAclExcp->m_lServiceId);
					qry.setParam(2,pAclExcp->m_sRequestId);
					qry.setParam(3,pAclExcp->m_sRequestId);
					qry.setParam(4,pAclExcp->m_lServiceType);
					qry.setParam(5,pAclExcp->m_sResultDesc);
					qry.Execute(); 
					qry.Close();
					TimesTenEnv::commit();             
				}
				catch(TTError err)
				{
					TimesTenEnv::rollback();
					return ;
				}
				break;        
			}                 
		case 50:   //进程日志信息点
			{
				ProcessLog *pProcessLig = (ProcessLog *)LogInfo;
				try
				{
				DEFINE_TTCMD(qry);
				qry.Prepare("insert into abm_process_log \
  							(PROCESS_CODE, \
   							PROCESS_NAME, \
   							LOG_CODE_ID, \
   							LOG_GRADE, \
   							LOG_CON, \
   							HOST_NAME,\
   							HOST_IP,abm_process_log_seq) \
							values \
  							(:processCode, \
  							 :processName, \
  							 :logCodeId,\
  							 :logGrade, \
  							 :logCon, \
  							 :hostName, \
  							 :hostIp,abm_process_log_seq.nextval);");

				qry.setParam(1,pProcessLig->m_lProcessCode);
				qry.setParam(2,pProcessLig->m_sProcessName);
				qry.setParam(3,pProcessLig->m_lLogCodeId);
				qry.setParam(4,pProcessLig->m_sLogGrade);
				qry.setParam(5,pProcessLig->m_sLogCon);
				qry.setParam(6,pProcessLig->m_sHostName);
				qry.setParam(7,pProcessLig->m_sHostIp);
				qry.Execute(); 
				qry.Close();
				TimesTenEnv::commit(); 
				}
				catch(TTError err)
				{
					TimesTenEnv::rollback();
					return ;
				}
				break;
			}
		default:
			break;
	}
}*/


/*
输出应用进程日志、告警信息到前台调度界面
    iAlertCode == 0时 表示非告警信息
        (AlertCode 在表BILL.WF_ALERT_DEFINE 中有解释,各模块增加告警代码时,
        需要在CommonMacro.h这个头文件和数据库表中增加相应的记录)
    bInsertDB: 是否需要入告警日志表(wf_alert_log)

*/
/*bool Pprintf(int iAlertCode,bool bInsertDB,int iFileID,char const *fmt,...)
{
    va_list argptr;
    char sMsgSrc[1024],sMsgBuilt[1024];

    //    如果数据库没有成功连接， 强制不入库*******要讨论，by xueqt 
    if (!(DB_CONNECTED)) bInsertDB = false;
    
    va_start(argptr, fmt);
    vsprintf(sMsgSrc,fmt, argptr);
    va_end(argptr);

    if (iAlertCode) {
        sprintf(sMsgBuilt,"[WARNING]: 告警编码:%d |",iAlertCode);
    } else {
        sprintf(sMsgBuilt,"[MESSAGE]: ");
    }

    if (iFileID>0) {
        sprintf(sMsgBuilt,"%s 处理文件ID:%d ",sMsgBuilt,iFileID);
    }

    sprintf(sMsgBuilt,"%s%s",sMsgBuilt,sMsgSrc);
    
    //替换sMsgSrc中的单引号
    TreatSingleQuotes(sMsgSrc);
    
    sMsgBuilt[MAX_APPINFO__LEN-1]=0;
    
    int iProcLogID=0;
        
    if (g_pAppInfo) 
		{
        g_pAppInfo->pushInfo(sMsgBuilt);
        iProcLogID = g_pAppInfo->m_iProcessLogID;
    }
    
    if (bInsertDB) 
    {
        char sql[2048];
        DEFINE_QUERY(qry);
        qry.close();
        qry.setSQL("select seq_alert_log_id.nextval log_id from dual");
        qry.open();         
        qry.next();         
        int iLogID = qry.field("log_id").asInteger();
 
        qry.close();
        sprintf(sql,
          " insert into wf_alert_log(alert_log_id,process_log_id,msg_date,msg_time, \n"
          " process_id,billflow_id,file_id,alert_code,alert_content) values ( \n"
          " %d,%d,to_char(sysdate,'yyyymmdd'),to_char(sysdate,'hh24miss'), \n"
          " %d,%d,%d,%d,'%s' )",iLogID,iProcLogID,g_iProcessID,
          g_iBillflowID,iFileID,iAlertCode,sMsgSrc);

        qry.setSQL(sql);
        qry.execute();
        qry.commit();
        qry.close();
    }
    
    return true;      
}*/

//## 应用程序退出监控
/*bool DisConnectMonitor(char _cMode)
{
    
    if (g_iRegistered == 0)
        return -1;
    

    Log::log (0, "退出工作流监控!");
    TWfLog::log (0, "退出工作流监控!");
    cout<<endl<<" 退出工作流监控 ..."<<endl;
        
    LockSafe(g_pShmTable->m_iAppLock,0);
    
    g_pShmTable->AppReqInfo.m_iProcessID = g_iProcessID;
    
    if (g_pAppInfo)
        g_pShmTable->AppReqInfo.m_iAppPID = g_pAppInfo->m_iAppPID;
        
    g_pShmTable->AppReqInfo.m_cState = _cMode;
    g_pShmTable->AppReqInfo.m_iStatus = APP_REG_UNREGISTER;
    
    while( g_pShmTable->AppReqInfo.m_iStatus == APP_REG_UNREGISTER ) {
        static int i = 0;
        usleep(10000);
        if (i++>1000)
            break;
    }
    
    g_iRegistered = 0;
    UnlockSafe(g_pShmTable->m_iAppLock,0);
    shmdt( (char*)g_pShmTable );
    
    g_pAppInfo = 0x0;
    
    cout<<"[MESSAGE]: Process:"<<g_iProcessID<<" Monitor Disconnected . "<<endl<<endl;
    return true;
}
*/
/*
int DisconnectTerminalMonitor ()
{
    Log::log (0, "into DisconnectTerminalMonitor()");
    //TWfLog::log (0, "%s(%d) into DisconnectTerminalMonitor()", g_argv[0], g_iProcessID);
    cout<<endl<<g_argv[0]<<"("<<g_iProcessID<<")DisconnectTerminalMonitor() ..."<<endl;
    if (pCurProc)
        pCurProc->iState = ST_SLEEP;

    DetachShm ();
    pCurProc = NULL;
    Log::log (0, "DetachShm ok");
    return 0;
}
*/
char *ltoa(long InNum,long sumlen)
{
	static char tmpstr[20];
	char	strformat[10];

	sprintf(strformat,"%%0%dld",sumlen);

	sprintf(tmpstr,strformat,InNum);
	
	return tmpstr;
}
char * ltoa(const short lValue)
{
	static char sTemp[16];
	sprintf(sTemp,"%d",lValue);
	return (char*)sTemp;
}
char * ltoa(const long lValue)
{
	static char sTemp[32];
	sprintf(sTemp,"%ld",lValue);
	return (char*)sTemp;
}
char * ltoa(const long long lValue)
{
	static char sTemp[32];
	sprintf(sTemp,"%lld",lValue);
	return (char*)sTemp;
}
char * stoa(const short lValue)
{
	static char sTemp[16];
	sprintf(sTemp,"%d",lValue);
	return (char*)sTemp;
}
short atos(const char *lValue)
{
	return (short)atol(lValue);
}

extern "C" {
	
typedef struct
{
	const char* pXMLStr;
	const char* pSQLStr;
	long  lXML_Len;
	long  lSQL_Len;

} T_XML_SQL_t;
char toUpper(const char& ch) 
{ 
	return ch & 0x5F; 
} 
char toLower(const char& ch) 
{ 
	return ch | 0x20; 
} 

std::string Upper(const std::string s) 
{ 
	std::string sResult; 
	for(int i=0;i<s.length();i++) 
		sResult+=( toUpper(s[i])); 
	return sResult; 
} 

std::string Lower(const std::string s) 
{ 
	std::string sResult; 
	for(int i=0;i<s.length();i++) 
		sResult+=( toLower(s[i])); 
	return sResult; 
}	

std::string trim(std::string const& source, char const* delims) {

  std::string result(source);
  std::string::size_type index = result.find_last_not_of(delims);

  if(index != std::string::npos)
    result.erase(++index);

  index = result.find_first_not_of(delims);
  if(index != std::string::npos)
    result.erase(0, index);
  else
    result.erase();
  return result;
}

std::string delAll(std::string const& source, char const* delims){
	std::string result(source);
	
	unsigned int len = strlen(delims);
	unsigned int pos = 0;
	unsigned int index = result.find(delims, pos);

	while(index != std::string::npos)
	{
		result.erase(index, len);
		
		pos = ++index;
		index = result.find(delims, pos);
	}
	

	return result;
}



long Memcpy(StdString &sStdStr,const char *sValue,const long lLen)
{
        static long lExtendLen = 0;     //每次扩展长度
        if((sStdStr.m_lCurPos+lLen)>sStdStr.m_lTotalLen)
        {//长度不足
            if(lLen>EXTEND_LEN)
            {
                lExtendLen = lLen;
            }
            else
                lExtendLen = EXTEND_LEN;
                //分配空间
                sStdStr.m_lTotalLen+=lExtendLen;
                sStdStr.m_pStr = (char *)realloc((void *)sStdStr.m_pStr,sStdStr.m_lTotalLen);
                if(sStdStr.m_pStr == NULL)
                {
                    return -1;
                }
        }
        memcpy(&(sStdStr.m_pStr[sStdStr.m_lCurPos]),sValue,lLen);
        sStdStr.m_lCurPos+=lLen;
        sStdStr.m_pStr[sStdStr.m_lCurPos]=0;   
        return 0;
}

//ptr指向开始位置
long Memset(char *ptr,const char *Value,long lLen)
{
    char *caTemp = ptr;
    int iLen = strlen(Value);
    int i=lLen-1;
    
        
    for(int j=iLen-1;j>=0;j--)
    {
        memset(&(caTemp[i]),(int)Value[j],1);
        i--;
    }
        
    return 0;
}

/*long Memcpy(StdString &sStdStr,const char *sValue,const long lLen)
{
	static long lExtendLen = 0;	//每次扩展长度
	 char *caTemp ; 
	if((sStdStr.m_lCurPos+lLen)>sStdStr.m_lTotalLen)
	{//长度不足
		if(lLen>EXTEND_LEN)
		{
			lExtendLen = lLen;
		}
		else
			lExtendLen = EXTEND_LEN;
		//分配空间
		
		sStdStr.m_lTotalLen+=lExtendLen;
		
		if(sStdStr.m_pStr != NULL)
		{
			strcpy(caTemp,sStdStr.m_pStr);
			delete sStdStr.m_pStr;
			sStdStr.m_pStr = NULL;
		}
		sStdStr.m_pStr = new char[sStdStr.m_lTotalLen];
		if(caTemp != NULL)
		{
			strcpy(sStdStr.m_pStr,caTemp);
		}
	}
	for(int i = 0;i<lLen; i++)
	{
		sStdStr.m_pStr[sStdStr.m_lCurPos]=sValue[i];
		sStdStr.m_lCurPos++;
	}
	sStdStr.m_pStr[sStdStr.m_lCurPos]=0;
	return 0;
}
*/
string toString(StdString &sStdStr)
{
    stringstream strsem;
    if(sStdStr.m_pStr != NULL)
	    strsem<<sStdStr.m_pStr;
	else
	    strsem<<"";
	return strsem.str();
	        
}

static T_XML_SQL_t strXML_Sql_arr[]={ {"&lt;","<",0}, {"&gt;",">",0}, {"&apos;","'",0}, {"&amp;","&",0} };

#define strXML_Sql_arr_SIZE (sizeof(strXML_Sql_arr) / sizeof(strXML_Sql_arr[0]))

static long lXMLFirst_Flag = 1;

/*
	ret Value
	-1:参数出错
	0:不必替换
	1:有特殊符号，发生替换
*/

/* 记录集中分隔符定义 */
static char cSQL_WORD_SEP = 1;  /* 列分隔符 */

/* 2005.05.13 ADD 添加其他的(除了32..126之外, ASCII码值在1..31之间 ) SQL合法字符 
	TAB
*/
static const char gsOtherValidCharArr[] = {'\t',STRING_END_FLAG};  

static inline long IsValidCh(int ch);

long specialXML_Sql_recover(const char*sSqlIn, char** ppSqlOut)
{		
	long  lRet =0;
	char  *pCHAR = NULL;
	char  *pBegin = NULL;
	char  *pEnd = NULL;
	
	char *sBuffer = NULL;
	
	long lStrlen = 0;
	long lCur = 0;
	long k =0;
	long lReplaceFlag  =   FALSE ;
	int iFlag = (0x01+0x02) ; //右填空格

	if (!sSqlIn || !ppSqlOut) return   -1;

	// 初始化XML String Length, 只执行一次
	if(lXMLFirst_Flag)	
	{
		for(k = 0; k < strXML_Sql_arr_SIZE; k++)
			 strXML_Sql_arr[k].lXML_Len = strlen(strXML_Sql_arr[k].pXMLStr);
		
		lXMLFirst_Flag =0;
	}
	lStrlen = strlen(sSqlIn);
	
	sBuffer = (char*)malloc(sizeof(char)*(lStrlen+32));
	if (NULL == sBuffer) return -2;

	memset(sBuffer, 0, sizeof(char)*(lStrlen+32));

	lCur = WriteFixLenInfo(sBuffer, sSqlIn, lCur, lStrlen);

	for (k = 0; k < strXML_Sql_arr_SIZE; k++)
	{
		pBegin = sBuffer;

		while (pBegin && (*pBegin))
		{
			pCHAR = strstr(pBegin, strXML_Sql_arr[k].pXMLStr);

			//有需要替换的特殊XML字符串
			if (pCHAR)
			{
				lReplaceFlag = TRUE;
				MemoryFill(pCHAR, strXML_Sql_arr[k].pSQLStr, strXML_Sql_arr[k].lXML_Len, \
					iFlag, cSQL_WORD_SEP, strlen(strXML_Sql_arr[k].pSQLStr));

				pBegin = pCHAR + strXML_Sql_arr[k].lXML_Len;
			}
			else
			{
				break;
			}
		}

	}

	sBuffer[lStrlen+128-1] = STRING_END_FLAG ;

	if (lReplaceFlag)
	{
		lRet  = 1;	
		// 把cSQL_WORD_SEP特殊符号去掉
		pEnd = pBegin = pCHAR = sBuffer;
		while (pCHAR && *pCHAR)
		{
			pBegin = strchr(pCHAR, cSQL_WORD_SEP);
			if (pBegin)
			{
				pEnd = pBegin+1;
				pCHAR = pBegin+1;
				while(cSQL_WORD_SEP == *pEnd) pEnd++;

				//后面的字符串向前移动
				while(*pEnd)
				{
					*pBegin = *pEnd;
					pBegin++;
					pEnd++;
				}

				*pBegin = STRING_END_FLAG;
			}
			else
			{
				break;
			}

		}
	}
	else	// 没有任何替换XML传送来的SQL不必改变 
	{
		
	}
	*ppSqlOut = sBuffer;
	return lRet;
} 


//static char *
//ltoa(long l)
//{
//	static char str[20];
//	char *sp = &str[18];	/* u370 */
//	int i;
//	int neg = 0;
//
//	if ((unsigned long)l == 0x80000000UL)
//		return ("-2147483648");
//	if (l < 0)
//		++neg, l = -l;
//	str[19] = '\0';
//	do {
//		i = l % 10;
//		*sp-- = '0' + i;
//		l /= 10;
//	} while (l);
//	if (neg)
//		*sp-- = '-';
//	return (++sp);
//}
//
//static char *
//lltoa(long long l)
//{
//	static char str[25];
//	char *sp = &str[23];
//	int i;
//	int neg = 0;
//
//	if (l == 0x8000000000000000ULL)
//		return ("-9223372036854775808");
//	if (l < 0)
//		++neg, l = -l;
//	str[24] = '\0';
//	do {
//		i = l % 10;
//		*sp-- = '0' + i;
//		l /= 10;
//	} while (l);
//	if (neg)
//		*sp-- = '-';
//	return (++sp);
//}
 

void AllTrim(char sBuf[])
{
	long i, lFirstChar, lEndPos, lFirstPos;
	
	lEndPos=lFirstChar=lFirstPos=0;

	for(i=0; sBuf[i]!='\0'; i++)
	{
		if (sBuf[i]==' ')
		{
			if (lFirstChar==0)
				lFirstPos++;
		}
   		else
   		{
   			lEndPos=i;
   			lFirstChar=1;
   		}
	}

	for(i=lFirstPos;i<=lEndPos;i++)
		sBuf[i-lFirstPos]=sBuf[i];

	sBuf[i-lFirstPos]='\0';
}


/*获取当前日期时间
sDateTime为返回日期时间字符串格式为YYYYMMDDHHMISS
lDateTimeFlag为日期时间标识
                为0时返回当前日期时间，为1返回当前日期，为2返回当前时间
*/
void GetNowDateTime(char *sDateTime,long lDateTimeFlag)
{
	time_t timeNow;
	struct tm *ptmNow;
    
	time(&timeNow);
	ptmNow=localtime(&timeNow);

	/*日期时间*/
	if(lDateTimeFlag==0)
	{
		sprintf(sDateTime,"%04d-%02d-%02d %02d:%02d:%02d",ptmNow->tm_year+1900,ptmNow->tm_mon+1, \
			ptmNow->tm_mday, ptmNow->tm_hour,ptmNow->tm_min,ptmNow->tm_sec);
	}
	else
		/*日期*/
		if (lDateTimeFlag==1)
		{
			sprintf(sDateTime,"%04d-%02d-%02d",ptmNow->tm_year+1900,ptmNow->tm_mon+1, \
				ptmNow->tm_mday);
		}
		else
			/*时间*/
			if(lDateTimeFlag==2)
			{
				sprintf(sDateTime,"%02d:%02d:%02d",ptmNow->tm_hour,ptmNow->tm_min, \
					ptmNow->tm_sec);
			}
	return;
}

/*
保留字符串中的字符和下划线，其余按空格和TAB裁减
sBuf为需要截取的字符串
*/
void StrTrim(char sBuf[])
{
	long i, lFirstChar, lEndPos, lFirstPos;

	lEndPos=lFirstChar=lFirstPos=0;

	for (i=0; sBuf[i]!='\0'; i++)
	{
		if ((sBuf[i]==' ') || (sBuf[i]=='\t'))
		{
			if(lFirstChar==0) lFirstPos++;
		}
   		else
			if ((sBuf[i]>='a')&&(sBuf[i]<='z')||((sBuf[i]>='A')&&(sBuf[i]<='Z'))
				||(isdigit(sBuf[i]))|(sBuf[i]=='_'))
			{
				lEndPos=i;
				lFirstChar=1;
			}
	}

	for (i=lFirstPos; i<=lEndPos; i++) sBuf[i-lFirstPos]=sBuf[i];

	sBuf[i-lFirstPos]='\0';
}

/*16进制字符转换成10进制整形
cHex为0-9 A-F a-f字符,表示16进制
返回值<0表示输入字符错误,>0为转换的值
*/
int HexCharToDecInt(char cHex)
{
	int iRet=0;
	
	if(isxdigit(cHex)==0)
	{/*不是16进制数字*/
		return -1;
	}
	else if (isdigit(cHex)!=0)
	{/*数字*/
		iRet=cHex-'0';
	}
	else if ((toupper(cHex)>='A')&&(toupper(cHex)<='F'))
	{/*字符*/
		iRet=10+toupper(cHex)-'A';
	}
	
	return iRet;
}

/*10进制数字转换成16进制字符
iDec为0-15的整数
返回值为0-9,A-F的字符值,其余范围内的数字绝对值后转换成取256模后对应的字符
*/
char DecIntToHexChar(int iDec)
{
	char cRet=0;
	int iTemp=0;
	
	if((iDec>=0)&&(iDec<=15))
	{
		iTemp=iDec;
	}
	else
	{/*超出范围的数字*/
		iTemp=abs(iDec)%0x100;
	}
	
	if(iTemp<10)
	{/*0-9*/
		cRet='0'+iTemp;
	}
	else
	{/*A-F*/
		cRet='A'+iTemp-10;
	}
	
	return cRet;
}

/*字符串填充为一定长度
sDest为目标字符串
sSrc为源字符串
lLen为指定目标字符串的长度(不含'\0')
iFlag为1时右补空格，为0时左补空格
cFilled填充的的字符
*/
void StringFill(char *sDest,char *sSrc,long lLen,int iFlag,char cFilled)
{
	long lCnt=0;
	long lFillLen=0;
	
	strncpy(sDest,sSrc,lLen);  	
	lFillLen=lLen-strlen(sDest);
    
	if(lFillLen>0)
	{
		if(iFlag==0)
		{/*左补*/
			for(lCnt=lLen-1;lCnt>=lFillLen;lCnt--)
			{/*字符串右移*/
				sDest[lCnt]=sDest[lCnt-lFillLen];    
			}

			for(lCnt=0;lCnt<lFillLen;lCnt++)
			{
				sDest[lCnt]=cFilled;
			}
		}

		if(iFlag==1)
		{/*右补*/
			for(lCnt=0;lCnt<lFillLen;lCnt++)
			{
				sDest[lLen-1-lCnt]=cFilled;
			}
		}
	}

	sDest[lLen]='\0';

	return;
}

/*--------------------------------------------------------------------------------------*/
/*将长度（十进制）转换成符合要求的16进制字符串											*/
/*入参：iByte：字节数;iNum:十进制数											*/
/*出参：szHexNum：长度为iByte的16进制的字符串									*/
/*--------------------------------------------------------------------------------------*/
void DecimalIntoHex(char *szHexNum,int iByte,int iNum)
{
	char pszNumX[8];

	memset(szHexNum,'0',iByte);
	memset(pszNumX,0,iByte);

	sprintf(pszNumX,"%x",iNum);
	memcpy(szHexNum+(iByte-strlen(pszNumX)),pszNumX,strlen(pszNumX) );

	szHexNum[iByte] = 0;
	szHexNum[iByte+1] = 0;

	//strupr(szHexNum);
}





/*
sDest为目标字符串
sSrc为源字符串
lLen为指定目标字符串的长度(不含'\0')
iFlag为1时右补空格，为0时左补空格
cFilled填充的的字符
*/
void MemoryFill(char *sDest, const char *sSrc, long lLen, int iFlag, char cFilled, long lSrcLen)
{
	long lCnt=0;
	long lFillLen=0;

	memcpy(sDest,sSrc,lLen);  /* 2004.04.02 Modify by fuxf  */	 
	lFillLen=lLen- lSrcLen;
    
	if (lFillLen>0)
	{
		if (iFlag==0)
		{/*左补*/
			for(lCnt=lLen-1;lCnt>=lFillLen;lCnt--)
			{/*字符串右移*/
				sDest[lCnt]=sDest[lCnt-lFillLen];    
			}

			for(lCnt=0;lCnt<lFillLen;lCnt++)
			{
				sDest[lCnt]=cFilled;
			}
		}

		if(iFlag==1)
		{/*右补*/
			for(lCnt=0;lCnt<lFillLen;lCnt++)
			{
				sDest[lLen-1-lCnt]=cFilled;
			}
		}
	}
	sDest[lLen]='\0';
	return;
}

/*ReadFixLenInfo从sSrc字符串的第lBegin个字符不多于lLen个字符拷贝到sDest字符串
sDest为目的字符串
sSrc为源字符串      
lBegin为起始字符从0开始
lLen为拷贝的字符串长度
返回值lBegin+实际字段长度
*/
long ReadFixLenInfo(char *sDest,const char *sSrc,long lBegin,long lLen)
{       
	memcpy(sDest,sSrc+lBegin,lLen);
	
	sDest[lLen]='\0';

	return lBegin+lLen;
}

/*WriteFixLenInfo从sSrc字符串不多于lLen个字符拷贝到sDest字符串的第lBegin个字符开始
sDest为目的字符串
sSrc为源字符串      
lBegin为起始字符从0开始
lLen为拷贝的字符串长度
返回值lBegin+lLen
*/
long WriteFixLenInfo(char *sDest,const char *sSrc,long lBegin,long lLen)
{       
	memcpy(sDest+lBegin,sSrc,lLen);
	
	sDest[lBegin+lLen]='\0';
	
	return lBegin+lLen;
}

long WriteFixLenInfo_EX(char **ppDest,const  char *sSrc,long lBegin,long lLen , long  *plDestLen)
{       
	char *sDest =NULL;
	long lNewLen = 0;

	if (!ppDest ||!sSrc || !plDestLen || (*plDestLen< 0))
	{
		return -1; /* 参数出错*/
	}

	/* 需要复制的字符串的长度<=0 则直接返回 */
	if (lLen <=0)
	{
		return  lBegin;
	}

	sDest = *ppDest;
	lNewLen = *plDestLen ;

	/* 检查空间是否足够 */
	if(lBegin+lLen >= lNewLen)
	{
		lNewLen = (lBegin+lLen)*2;
		sDest = (char*)realloc(*ppDest, sizeof(char) * lNewLen);
		if (NULL == sDest)
		{
			free(*ppDest);
			*ppDest = NULL;
			*plDestLen = 0;
			return -2;
		}

		*ppDest = sDest;
		*plDestLen = lNewLen;
	}
	
	memcpy(sDest+lBegin,sSrc,lLen);	
	sDest[lBegin+lLen]='\0';
	
	return lBegin+lLen;
}

static inline long IsValidCh(int ch)
{
	const int iChBegin = 32;
	const int iChEnd = 126;
	
	return (!((iChBegin> ch &&ch>=0)));  
}

/*  " !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~"  */
long rmInvalidChar(char *sSqlIn)
{
	char *p = NULL;
	char *pNew = NULL;
	
	const int iChBegin = 32;
	const int iChEnd = 126;

	if (NULL == sSqlIn ) return -1;


	pNew = p = sSqlIn;
	while (*p)
	{
		while (*p && IsValidCh(*p))
		{
			/* 合法字符并且pNew, p位置不重复则复制*/
			if (pNew != p)
			{
				*pNew = *p;				
			}
			p++;
			pNew++;
		}
		
		while(*p && !IsValidCh(*p)) p++;  /* 跳过非法字符*/
	}

	*pNew = STRING_END_FLAG;

	return  0 ;
}

//## 字符串替换 
    //## 	ret Value
    //## 	-1:参数出错,   <0  err
    //## 	0:不必替换
    //## 	>0:有特殊符号，发生替换(新串的长度)
long  replace(const char* m_sSqlIn, const char* sOriginStr, const char* sNewStr, char** ppSqlOut)
{
	long  lRet =0;
	char  *pBegin = NULL;
	size_t  szOriginStr = 0;
	size_t  szNewStr = 0;
	long	lCur = 0;
	
	char	*m_sBuffer = NULL;	//缓冲区
	long	m_lStrLen = 0;		//缓冲区长度

	if (!m_sSqlIn || !sOriginStr || !sNewStr || !ppSqlOut)
	{
		return  -1;
	}
	//初始化缓冲区
	if (NULL == m_sBuffer)
	{
		m_lStrLen =  strlen(m_sSqlIn)+128 ;
		m_sBuffer = (char*)malloc(sizeof(char) * (m_lStrLen));
		if (NULL == m_sBuffer)
		{
			m_lStrLen = 0;
			return  -2;
		}
	}
	
	szOriginStr = strlen(sOriginStr);
	szNewStr = strlen( sNewStr);
	if (0==szOriginStr||0==szNewStr)
	{
		m_sSqlIn = (const char*)(pBegin);
	}
	else
	{
		while (*m_sSqlIn)
		{
    	
			pBegin = strstr(const_cast<char *>(m_sSqlIn),const_cast<char *>(sOriginStr));
			if (pBegin)
			{
				// m_sSqlIn中sOriginStr 之前的字符串
				lCur = WriteFixLenInfo_EX(&m_sBuffer, m_sSqlIn, lCur, (long)(pBegin-m_sSqlIn), &m_lStrLen); 
				if (lCur < 0)
				{
					free(m_sBuffer);
					m_sBuffer = NULL;
					return lCur;
				}
				lCur = WriteFixLenInfo_EX(&m_sBuffer, sNewStr, lCur, (long)szNewStr, &m_lStrLen); 
				if (lCur < 0)
				{
					free(m_sBuffer);
					m_sBuffer = NULL;
					return lCur;
				}
				m_sSqlIn = (const char*)(pBegin+szOriginStr);
			}
			else
			{
				break;
			}
		}
	}
	//处理m_sSqlIn 的串尾
	lRet = WriteFixLenInfo_EX(&m_sBuffer, m_sSqlIn, lCur, (long)strlen(m_sSqlIn), &m_lStrLen ); 
	if(lRet<0)
	{
		free(m_sBuffer);
		m_sBuffer = NULL;
		return lRet;
	}
	*ppSqlOut  = m_sBuffer;	
	return lRet;
}

long specialXML2Sql_replace(const char*sSqlIn, char** ppSqlOut)
{		
	long  lRet =0;
	char  *pNew = NULL;

	char *sBuffer = NULL;
	
	long lStrlen = 0;
	long lCur = 0;
	long k =0;
	long lReplaceFlag  =   FALSE ;
	if(!sSqlIn || !ppSqlOut)
	{
		return   -1;
	}

	// 初始化XML String Length  ， 只执行一次*
	if (lXMLFirst_Flag)
	{
		for (k = 0; k < strXML_Sql_arr_SIZE; k++)
		{
			 strXML_Sql_arr[k].lXML_Len = strlen(strXML_Sql_arr[k].pXMLStr);
			 strXML_Sql_arr[k].lSQL_Len = strlen(strXML_Sql_arr[k].pSQLStr);
		}
		lXMLFirst_Flag =0;
	}

	lStrlen = strlen(sSqlIn);
	sBuffer = (char*)malloc(sizeof(char)*(lStrlen+128));
	if (NULL == sBuffer) return -2;

	memset(sBuffer, 0, sizeof(char)*(lStrlen+128));
	
	lCur = 0; 
	lStrlen =0;
	lCur = WriteFixLenInfo_EX(&sBuffer, sSqlIn, lCur, strlen(sSqlIn),  &lStrlen);
	if (lCur < 0)
	{
		free(sBuffer);
		sBuffer = NULL;
		return lCur;
	}

	for (k = 0; k < strXML_Sql_arr_SIZE; k++)
	{

		lRet = replace(sBuffer, strXML_Sql_arr[k].pXMLStr, strXML_Sql_arr[k].pSQLStr, &pNew);
		if (lRet < 0)
		{  
				if(sBuffer)
				{
					free(sBuffer);  
					sBuffer = NULL;
				}
				return  lRet;
		}
		else
		{
			if (lRet > 0)
			{  
				lReplaceFlag = TRUE;	
				lCur = WriteFixLenInfo_EX(&sBuffer, pNew, 0, lRet, &lStrlen);
				if(pNew)
				{
					free(pNew);
					pNew = NULL;
				}
				if(lCur < 0 )
				{
					if(sBuffer)
					{
						free(sBuffer);
						sBuffer = NULL;
					}
					return lCur;
				}
			}
		}
		if(pNew)
		{
			free(pNew);
			pNew = NULL;
		}
	} //end for


	*ppSqlOut  = sBuffer;
	lRet = lCur;
	return lRet;
} 

/* 	返回INT  串的长度 
*/	
long  Str2IntString(const char*sSqlIn,  const char cConnChar, char** ppSqlOut)
{		
	long lRet =0;

	char *sBuffer = NULL;
	const char *p=NULL;
	char sTemp[32];
	long lStrlen = 0;
	long lCur = 0;

	if(!sSqlIn || !ppSqlOut)
	{
		return   -1;
	}

	*ppSqlOut = NULL_CONST_P;
	lCur = 0;
	lStrlen = 0;
	p = sSqlIn;

	while(*p)
	{
		sprintf(sTemp, "%d%c", *p++, cConnChar);
		lCur = WriteFixLenInfo_EX(&sBuffer, sTemp, lCur, strlen(sTemp), &lStrlen);
		if(lCur < 0) return lCur;
	}

	*ppSqlOut = sBuffer;

	return  lCur;
}

long   IntString2Str(char*sSqlIn, const char cConnChar)
{		
	long  lRet =0;

	char *sBuffer = NULL;
	char *p=NULL;
	char *pBegin =NULL;
	char  ch = 0;
	
	char sTemp[32];
	long lStrlen = 0;
	long lCur = 0;

	if (!sSqlIn) return  -1;
	
	lCur = 0;
	lStrlen = 0;
	p = sSqlIn;    /*   "32|16|23|56|\0"    */

	while(*p)
	{
		pBegin = strchr(p, cConnChar);
		if(pBegin)
		{
			*pBegin = STRING_END_FLAG;
			ch= (char)atoi(p);
			lCur = WriteFixLenInfo_EX(&sBuffer, &ch, lCur, 1, &lStrlen);
			if(lCur < 0) return lCur; 
			
			p = pBegin +1;
		}
		else
		{
			break;
		}
	}

	if(sBuffer)
	{ 
		lCur = WriteFixLenInfo(sSqlIn, sBuffer, 0, lCur);
		free(sBuffer);
		sBuffer = NULL_CONST_P;
	}

	return  lCur;
}

long stol(string str)
{
	long result;
	istringstream is(str);
	is >> result;
	return result;
}

string ltos(long InNum,long sumlen)
{
	string	result;
	char 	tmpstr[20];
	char	strformat[10];
	//int		lenformat;

	sprintf(strformat,"%%0%dld",sumlen);
	//lenformat  = strlen(strformat);
	//strformat[lenformat]='\0';

	sprintf(tmpstr,strformat,InNum);
	result = tmpstr;
	return result;
}
#ifdef  DEBUG_rmInvalidChar

static  long ReadFile2Mem(const char* sFileName ,  char** ppBuffer )
{
	long lRet =0;
	FILE  *fp =NULL;
	char sBuffer[1024];
	long  lDestLen = 0;
	long lCur =0;


	fp = fopen(sFileName, "r" );
	if(NULL_CONST_P == fp ){
		printf("Error fopen(sFileName[%s] , 'r' ) \n" ,  sFileName );
		return  -1;
	}

	lCur =0;

	while(  0==lRet && (NULL  != fgets(sBuffer, sizeof(sBuffer)  , fp) )     )
	{
		sBuffer[ sizeof(sBuffer)-1 ]  = STRING_END_FLAG;
		lCur = WriteFixLenInfo_EX( ppBuffer , sBuffer , lCur , strlen(sBuffer) , &lDestLen );
		if(lCur < 0 ){
			 break;
		}
	}

	if(fp)
		fclose( fp );  
	fp = NULL;


	return  lCur;
}
//
//去掉字符串中的特定的字符，例如"1.0.0" 处理之后１００
char  *delGivenChar(char *version,char *givenChar)
{
	 char *sNewString=new char[strlen(version)];
	 memset(sNewString,'\0',strlen(version));
	 char *sNewStringPrefix = new char[strlen(version)];
	 memset(sNewStringPrefix,'\0',strlen(version));
	 
   while( 1 )
   {
	   char *sTemp;
  	 sTemp = strstr(version,givenChar);
  	 if(sTemp==NULL)
  	 {
  	   strcpy(sNewStringPrefix,version);
  	   strcat(sNewString,sNewStringPrefix);
  	   break;
  	 }
  	 else
  	 {
  	   strcpy(sNewStringPrefix,version);
  	 	 sNewStringPrefix[sTemp-version]='\0';
  	   strcat(sNewString,sNewStringPrefix);
  	   version=++sTemp;
  	 } 
   }
	 return sNewString;
}

#endif
}

