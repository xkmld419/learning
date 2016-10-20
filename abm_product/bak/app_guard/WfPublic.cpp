/*VER: 2*/ 
#include "WfPublic.h"

#include "Guard.h"


Date TWfLog::m_oDate;

void TWfLog::log(char const *fmt,...) {
    char sMsg[1000];
    va_list argptr;        
    va_start(argptr, fmt);
    vsprintf( sMsg, fmt, argptr);
    va_end(argptr);
    
    sprintf(sMsg,"%s\n",sMsg);
    //cout<<endl<<">>"<< sMsg <<endl;
    
    //写文件
    writeLog2(sMsg);
}

void TWfLog::log(int iErrCode,char const *fmt,...) {
    char sMsg[1000];
    va_list argptr;        
    va_start(argptr, fmt);
    vsprintf( sMsg, fmt, argptr);
    va_end(argptr);
    
    char _sMsg[1000];
    
    if (iErrCode)
        sprintf(_sMsg,"ErorrCode:%d, %s \n",iErrCode, sMsg);
    else
        sprintf(_sMsg,"%s \n",sMsg);
    strcpy(sMsg,_sMsg);
    
    //cout<<endl<<">>"<< sMsg <<endl;
    
    // 写文件
    writeLog(sMsg);
}

void TWfLog::log(char const *sFileName, int iLine, char const *fmt,...)
{
    char sMsg[1000];
    va_list argptr;        
    va_start(argptr, fmt);
    vsprintf(sMsg, fmt, argptr);
    va_end(argptr);
    
    sprintf(sMsg,"%s\n[File(line):] %s(%d)\n",
        sMsg,sFileName,iLine);
    
    cout<<endl<<sMsg<<endl;
    
    //write file
    writeLog(sMsg);
}

void TWfLog::writeLog(char *sMsg) {
    
    FILE  * fp;
    
    char sFileName[200], sDirFileName[200], sCmd[200];
    
    m_oDate.getCurDate();
    
    sprintf(sFileName,"wf%d%02d%02d.log",m_oDate.getYear(),m_oDate.getMonth(),m_oDate.getDay());
    
    if (!sMsg) return;
    
    char *p = getenv ("TIBS_HOME");
    if (!p) {
        chdir("../log/");
    }
    else {
        char sPath[100];
        sprintf (sPath,"%s/log",p);
        //chdir(sPath);
        sprintf (sCmd,"mkdir -p %s",sPath);
        system (sCmd);
        sprintf (sDirFileName,"%s/%s",sPath,sFileName);
        strcpy (sFileName, sDirFileName);
    }
    
    if ( (fp = fopen( sFileName,"a")) == NULL){
        perror("open file wfYYYYMMDD.log");
    }
    
    fprintf(fp,"[Date:] %d/%02d/%02d %02d:%02d:%02d \n[Message:]\n %s"
        "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n",
        m_oDate.getYear(),m_oDate.getMonth(),m_oDate.getDay(),
        m_oDate.getHour(),m_oDate.getMin(),m_oDate.getSec(), sMsg );
    
    fclose(fp);
    
    return;
}

void TWfLog::writeLog2(char *sMsg) {
    
    FILE  * fp;
    char sFileName[200], sDirFileName[200], sCmd[200];
    
    m_oDate.getCurDate();
    
    sprintf(sFileName,"ws%d%02d%02d.log",m_oDate.getYear(),m_oDate.getMonth(),m_oDate.getDay());
    
    if (!sMsg) return;
    
    char *p = getenv ("TIBS_HOME");
    if (!p) {
        chdir("../log/");
    }
    else {
        char sPath[100];
        sprintf (sPath,"%s/log",p);
        //chdir(sPath);
        sprintf (sCmd,"mkdir -p %s",sPath);
        system (sCmd);
        sprintf (sDirFileName,"%s/%s",sPath,sFileName);
        strcpy (sFileName, sDirFileName);
    }
    
    if ( (fp = fopen( sFileName,"a")) == NULL){
        perror("open file wsYYYYMMDD.log");
    }
    
    fprintf(fp,"[Date:] %d/%02d/%02d %02d:%02d:%02d \n[Message:]\n %s"
        "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n",
        m_oDate.getYear(),m_oDate.getMonth(),m_oDate.getDay(),
        m_oDate.getHour(),m_oDate.getMin(),m_oDate.getSec(), sMsg );
    
    fclose(fp);
    
    return;
}



//---- implementaion of Class TParseBuff ----

//构造函数1
TParseBuff::TParseBuff()
{
    reset(); //成员变量复位
}

//构造函数2
TParseBuff::TParseBuff(char *sInBuff)
{
    parseBuff(sInBuff);
}

//解析字符串 sInBuff
bool TParseBuff::parseBuff(char *sInBuff)
{
    reset();
    if (strlen(sInBuff)>MAX_PARSEBUFF_LEN ) return false;
    strcpy(m_Str,sInBuff);
    return parse();
}


void TParseBuff::reset()//数值型的初始化为 MEMBER_NULL = -99, 字符串为空
{
    m_iBillFlowID      = MEMBER_NULL;
    m_iProcessID       = MEMBER_NULL;
    m_iHostID          = MEMBER_NULL;
    m_iStaffID         = MEMBER_NULL;
    m_iPlanID          = MEMBER_NULL;
    m_iLogMode         = MEMBER_NULL;
    m_iLogID           = MEMBER_NULL;
    m_iAuthID          = MEMBER_NULL;
    m_iMainGuardIndex  = MEMBER_NULL;
    m_iOperation       = MEMBER_NULL;
    m_iResult          = MEMBER_NULL;
    m_lProcessCnt      = 0;
    m_lNormalCnt       = 0;
    m_lErrorCnt        = 0;
    m_lBlackCnt        = 0;
    m_lOtherCnt        = 0;
    m_iMsgOff          = 0; //消息缓冲m_sMsg的下一条消息存储区偏移量
    m_sHostAddr[0]     = 0;
    m_sSysUserName[0]  = 0;
    m_Str[0]           = 0;
    m_cState           = '0';
    memset (m_sMsg,0,MSG_MAXCNT*(MSG_MAXLEN+1));
}


//解析格式串
//<iBillFlowID=2><iProcessID=1023>...<sMsg=message_str1><sMsg=message_str2>...<sMsg=message_strN>
bool TParseBuff::parse() //if (m_str) parse it.
{
    int iStrLen = strlen(m_Str);

    if (!iStrLen) return true;

    char* pt = m_Str;
    char  sAttribute[MAX_PARSEBUFF_LEN];
    char  sValue[MAX_PARSEBUFF_LEN];
    int   iAttrOff = 0;
    int   iValueOff = 0;
    
    for (int iPos = 0; iPos<iStrLen; iPos++,pt++){

        static int s_iFlag = 0; // 1-已找到'<'; 2-已找到'='; 

/*
        if (*pt == '<' && s_iFlag == 0 ) { 
            s_iFlag = 1;
        }
*/
        if (*pt == '<')
        { 
            s_iFlag = 1;
            iAttrOff = 0;
        }

        else if (*pt == '=' && s_iFlag == 1 ) { 
            s_iFlag = 2; 
            sAttribute[iAttrOff] =  0;
            iAttrOff = 0;
        }
        else if (*pt == '>' && s_iFlag == 2 ) { 
            s_iFlag = 0;  
            sValue[iValueOff] = 0;
            iValueOff = 0;
            AllTrim(sValue);
            AllTrim(sAttribute);
            parseResultSave(sAttribute,sValue);
        }
        else { //非系统定义分格符
            if (s_iFlag == 1) 
                sAttribute[iAttrOff++] = *pt;
            else if (s_iFlag == 2)
                sValue[iValueOff++] = *pt;
        }
    }    
    return true;
}

//将 m_sMsg 缓冲中的分隔符替换掉
//{'<' '>' '='} == > {'[' ']' '~'}
void TParseBuff::formatBuff()
{
    for (int iPos = 0; iPos<m_iMsgOff; iPos++) {
        int iMsgLen = strlen( m_sMsg[iPos]);
        char *pt = m_sMsg[iPos];
        for (int j = 0;  j<iMsgLen; j++,pt++) {
            if (*pt == '<') { *pt = '['; continue; }
            if (*pt == '>') { *pt = ']'; continue; }
            if (*pt == '=') { *pt = '~'; continue; }
            if (*pt == '$') { *pt = '@'; continue; } // add 2005.06.13
        }
    }
}

unsigned long TParseBuff::stringToUL (char *pValue)
{
    char *pt;
    for (pt=pValue; *pt; pt++) {
        if(*pt>='0' && *pt<='9')
        break;
    }
    return strtoul(pt,0,10);
}

//保存一个成员变量 *pAtt(变量名) *pValue(变量值)
void TParseBuff::parseResultSave(char *pAtt, char *pValue)
{
    if (strcmp(pAtt,"iBillFlowID") == 0)           this->m_iBillFlowID = atoi(pValue);
    else if (strcmp(pAtt,"iProcessID") == 0)       this->m_iProcessID = atoi(pValue);
    else if (strcmp(pAtt,"iHostID") == 0)          this->m_iHostID = atoi(pValue);
    else if (strcmp(pAtt,"iStaffID") == 0)         this->m_iStaffID = atoi(pValue);
    else if (strcmp(pAtt,"iPlanID") == 0)          this->m_iPlanID = atoi(pValue);
    else if (strcmp(pAtt,"iLogMode") == 0)         this->m_iLogMode = atoi(pValue);
    else if (strcmp(pAtt,"iLogID") == 0)           this->m_iLogID = atoi(pValue);
    else if (strcmp(pAtt,"iAuthID") == 0)          this->m_iAuthID = atoi(pValue);
    else if (strcmp(pAtt,"iMainGuardIndex") == 0)  this->m_iMainGuardIndex = atoi(pValue);
    else if (strcmp(pAtt,"iOperation") == 0)       this->m_iOperation = atoi(pValue);
    else if (strcmp(pAtt,"iResult") == 0)          this->m_iResult = atoi(pValue);

    else if (strcmp(pAtt,"iProcessCnt") == 0)      this->m_lProcessCnt = stringToUL(pValue);
    else if (strcmp(pAtt,"iNor") == 0)             this->m_lNormalCnt = stringToUL(pValue);
    else if (strcmp(pAtt,"iErr") == 0)             this->m_lErrorCnt = stringToUL(pValue);
    else if (strcmp(pAtt,"iBlk") == 0)             this->m_lBlackCnt = stringToUL(pValue);
    else if (strcmp(pAtt,"iOth") == 0)             this->m_lOtherCnt = stringToUL(pValue);

    else if (strcmp(pAtt,"sHostAddr") == 0)  {
        strncpy(this->m_sHostAddr, pValue,15);
        this->m_sHostAddr[15] = 0;
    }
    else if (strcmp(pAtt,"sSysUserName") == 0)  {
        strncpy(this->m_sSysUserName, pValue,31);
        this->m_sSysUserName[31] = 0;
    }
    else if (strcmp(pAtt,"sMsg") == 0)  {
        this->addMsg(pValue);
    }
    else if (strcmp(pAtt,"cState") == 0 ) {
        this->m_cState = pValue[0];
    }
    else if (strcmp(pAtt,"sOrgID") == 0 ) {
        strncpy(this->m_sOrgIDStr, pValue, ORGID_STR_LEN);
        this->m_sOrgIDStr[ORGID_STR_LEN] = 0;
    }
    else {
        //do nothing.
    }    
}


//压入文本消息到m_sMsg
void TParseBuff::addMsg(const char *msg) //压入文本消息到m_sMsg
{
    char sTemp[MSG_MAXLEN];
    strncpy (sTemp,msg,MSG_MAXLEN-1);
    sTemp[MSG_MAXLEN-1] = 0;
    
    if (m_iMsgOff>= MSG_MAXCNT ) return;
    strcpy(this->m_sMsg[m_iMsgOff], sTemp);
    m_iMsgOff++;
}

//将内部成员变量build成字符流 => m_Str
bool TParseBuff::build()
{
    formatBuff();
    m_Str[0] = 0;
    
    if (m_iBillFlowID != MEMBER_NULL) 
        sprintf(m_Str,"%s<iBillFlowID=%d>",m_Str,m_iBillFlowID );
    if (m_iProcessID != MEMBER_NULL) 
        sprintf(m_Str,"%s<iProcessID=%d>",m_Str,m_iProcessID );
    if (m_iHostID != MEMBER_NULL) 
        sprintf(m_Str,"%s<iHostID=%d>",m_Str,m_iHostID );
    if (m_iStaffID != MEMBER_NULL) 
        sprintf(m_Str,"%s<iStaffID=%d>",m_Str,m_iStaffID );
    if (m_iPlanID != MEMBER_NULL)
        sprintf(m_Str,"%s<iPlanID=%d>",m_Str,m_iPlanID);
    if (m_iLogMode != MEMBER_NULL) 
        sprintf(m_Str,"%s<iLogMode=%d>",m_Str,m_iLogMode );
    if (m_iLogID != MEMBER_NULL) 
        sprintf(m_Str,"%s<iLogID=%d>",m_Str,m_iLogID );
    if (m_iAuthID != MEMBER_NULL) 
        sprintf(m_Str,"%s<iAuthID=%d>",m_Str,m_iAuthID );
    if (m_iMainGuardIndex != MEMBER_NULL) 
        sprintf(m_Str,"%s<iMainGuardIndex=%d>",m_Str,m_iMainGuardIndex );
    if (m_iOperation != MEMBER_NULL) 
        sprintf(m_Str,"%s<iOperation=%d>",m_Str,m_iOperation );
    if (m_iResult != MEMBER_NULL) 
        sprintf(m_Str,"%s<iResult=%d>",m_Str,m_iResult );
    
    if (m_lProcessCnt != 0)
    {
        if (m_lProcessCnt >100000000 ) sprintf(m_Str,"%s<iProcessCnt=%lu>",m_Str,0);
          else sprintf(m_Str,"%s<iProcessCnt=%lu>",m_Str,m_lProcessCnt);        
    }
    if (m_lNormalCnt != 0)
    {
        if ( m_lNormalCnt > 100000000 ) sprintf(m_Str,"%s<iNor=%lu>",m_Str,0);
        else sprintf(m_Str,"%s<iNor=%lu>",m_Str,m_lNormalCnt);
    }
    
    if (m_lErrorCnt > 100000000 ) sprintf(m_Str,"%s<iErr=%lu>",m_Str,0);
        else sprintf(m_Str,"%s<iErr=%lu>",m_Str,m_lErrorCnt);
    
    if (m_lBlackCnt > 100000000 ) sprintf(m_Str,"%s<iBlk=%lu>",m_Str,0);
        else sprintf(m_Str,"%s<iBlk=%lu>",m_Str,m_lBlackCnt);
    
    if (m_lOtherCnt > 100000000 ) sprintf(m_Str,"%s<iOth=%lu>",m_Str,0);
        else sprintf(m_Str,"%s<iOth=%lu>",m_Str,m_lOtherCnt);
    
    if (m_sHostAddr[0] != 0 )
        sprintf(m_Str,"%s<sHostAddr=%s>",m_Str,m_sHostAddr );
    
    if (m_sSysUserName[0] != 0)
        sprintf(m_Str,"%s<sSysUserName=%s>",m_Str,m_sSysUserName);
    
    if (m_cState != '0')
        sprintf(m_Str,"%s<cState=%c>",m_Str,m_cState);
    
    for (int iPos = 0; iPos<m_iMsgOff; iPos++) {
        int iLen = strlen(m_Str)+ strlen(m_sMsg[iPos]);
        if (iLen > MAX_PARSEBUFF_LEN) break;
        sprintf(m_Str,"%s<sMsg=%s>",m_Str,m_sMsg[iPos]);
    }    
    return true;
}

//获取m_Str
bool TParseBuff::getBuiltStr(char *sOutBuff)
{
    if (!sOutBuff) return false;
    build();    
    strcpy(sOutBuff,m_Str);    
    return true;
}

//仅获取文本消息
char * TParseBuff::getBuiltMsg()
{
    formatBuff();
    m_Str[0] = 0;
    
    for (int iPos = 0; iPos<m_iMsgOff; iPos++) {
        int iLen = strlen(m_Str)+ strlen(m_sMsg[iPos]);
        if (iLen > MAX_PARSEBUFF_LEN) break;
        sprintf(m_Str,"%s<sMsg=%s>",m_Str,m_sMsg[iPos]);
    }
    
    return m_Str;
}


//---- end implementaion of Class TParseBuff ----
bool TAppInfo::checkNeedOutput()
{ 
    return bNewMsg||bNewState;
}


bool TAppInfo::putRetInfo(int iRet, char *sRetInfo)
{
    if(iRet && m_iRet)
        return false;
    
    m_iRet= iRet;
    if(sRetInfo){
        strncpy(m_sErrMsg,sRetInfo,MAX_APPINFO__LEN );
        m_sErrMsg[MAX_APPINFO__LEN-1]=0;
    }else{
        m_sErrMsg[0]=0;
    }
    return true;
    
}

bool TAppInfo::getRetInfo(int & iRet, char *sOutInfo)
{
    if(!sOutInfo)
        return false;
    
    iRet = m_iRet;
    if(sOutInfo ){
        strncpy(sOutInfo, m_sErrMsg, MAX_APPINFO__LEN );

    return true;
    }
    
}



//void TAppInfo::pushInfo(int _iAlertCode, int _iFileID, char *_sMsg)
void TAppInfo::pushInfo(char *_sMsg)
{
	int iLen = (sizeof(m_asMsg[m_iInfoHead])-1)>strlen(_sMsg)?strlen(_sMsg):(sizeof(m_asMsg[m_iInfoHead])-1);
	strncpy((char *)(m_asMsg[m_iInfoHead]), _sMsg,iLen );
	m_asMsg[m_iInfoHead][iLen]=0;
    //strcpy ((char *)(m_asMsg[m_iInfoHead]), _sMsg );
    
    m_iInfoHead++;
    m_iInfoHead = m_iInfoHead%INFO_Q_SIZE;
    
    //m_iMsgCnt++;
    
    //队列满
    if (m_iInfoHead == m_iInfoTail) {
        m_iInfoTail++;
        m_iInfoTail = m_iInfoTail%INFO_Q_SIZE;    
        //m_iMsgCnt--;    
    }
    
    bNewMsg = true;
    return;
}

bool TAppInfo::getInfo( char *_sMsg)
{
    if (m_iInfoHead == m_iInfoTail) {
        bNewMsg = false;
        return false;
    }
    if(m_iInfoTail<0 || m_iInfoTail>=INFO_Q_SIZE)
      	return false;


    if (!_sMsg){return false; }
    
    //strcpy( _sMsg,m_asMsg[m_iInfoTail]);
   strncpy( _sMsg,m_asMsg[m_iInfoTail],sizeof(m_asMsg[m_iInfoTail])-1 );
    _sMsg[sizeof(m_asMsg[m_iInfoTail])-1]=0; 
    m_iInfoTail++;
    m_iInfoTail = m_iInfoTail%INFO_Q_SIZE;
    
    //m_iMsgCnt--;
    
    return true;
}

//检查当前进程在后台是否真的活着(根据m_iAppPID).  
//alive-return true, dead-return false
bool TAppInfo::checkAlive()
{
    if (m_iAppPID<=0)
        return false;
    
    if (kill(m_iAppPID,0) == 0) {
        //进程号为pid的进程存在；
        return true;
    }
    else {
        //进程号为pid的进程不存在！
        return false;
    }
}

void TAppInfo::getOtherProcessInfo()
{
	if (m_iProcessID == -1)
		return;

	DEFINE_QUERY(qry);
	qry.setSQL("select sum(normal_cnt),sum(error_cnt),sum(black_cnt),sum(other_cnt) from wf_process_info where process_id=:PROCESSID ");
	qry.setParameter("PROCESSID",m_iProcessID);
	qry.open();
	if (qry.next())
	{
		m_lNormalCnt = qry.field(0).asLong();
		m_lErrorCnt = qry.field(1).asLong();
		m_lBlackCnt = qry.field(2).asLong();
		m_lOtherCnt = qry.field(3).asLong();
		m_lProcessCnt = m_lNormalCnt+m_lErrorCnt+m_lBlackCnt+m_lOtherCnt;
	}
	qry.close();
	return;
}


//###################################################################3

TAppInfo*  TSHMCMA::getAppInfoByProcID(int _iProcID) {
    for (int i=0; i<MAX_APP_NUM; i++ )
        if (AppInfo[i].m_iProcessID == _iProcID)
            return &(AppInfo[i]);
    return NULL;
}

TAppSaveBuff* TSHMCMA::getAppSaveBuffByProcID(int _iProcID)
{
    for (int i=0; i<MAX_APP_NUM; i++ )
        if (AppSaveBuff[i].m_iProcessID == _iProcID)
            return &(AppSaveBuff[i]);
    return NULL;
}

int TSHMCMA::changeState(int _iProcID, char _cState) {
    TAppInfo *pt_app = getAppInfoByProcID(_iProcID);
    if (!pt_app) {
        TWfLog::log(WF_FILE,"TSHMCMA::changeState, g_pShmTable.TAppInfo[?]中未找到 ProcessID = %d 对应的进程实例!修改状态为%c",_iProcID,_cState);
        return 0;
    }
    pt_app->setState(_cState);
        
    return 1;
}

//获取一个空闲的 TAppInfo, found:return 1; notFound:return 0; running:return -1;
int TSHMCMA::getIdleAppInfo(int _iProcID, int &_iIndex) {
    
    //[如果_iProcID已存在AppInfo[x]中]
    for (int i=0; i<MAX_APP_NUM; i++) {
        if (AppInfo[i].m_iProcessID == _iProcID){
            if (AppInfo[i].m_cState == WF_START ) {//该进程已经启动中
                if (AppInfo[i].checkAlive()) //后台进程确实存在,应用尚未连接主监控
                    return -1;
            }
            if (AppInfo[i].m_cState == WF_RUNNING ) {//该进程在共享内存中状态为运行
                if (AppInfo[i].checkAlive()) //后台进程确实存在
                    return -1;
            }
            
            //该节点空闲,可用
            _iIndex = i;
            return 1;
        }
    }
    
    //[如果_iProcID不存在AppInfo[x]中]
    for (int i=0; i<MAX_APP_NUM; i++) {
        if (AppInfo[i].m_cState == 'A' || AppInfo[i].m_cState == 0) {
            _iIndex = i;
            return 1;
        }
    }        
    return 0;
}

void TSHMCMA::setMainGuardIndex(int _iProcID,int _iMainGuardIndex) {
    TAppInfo* pt = getAppInfoByProcID(_iProcID);
    if (!pt) {
        TWfLog::log(WF_FILE,"setMainGuardIndex: Can't find AppInfo which ProcessID = %d",_iProcID);
        return;
    }
    pt->m_iMainGuardIndex = _iMainGuardIndex;
}


//////////////////////////////////////////////////////////////

void TLogProcessMgr::updateLogProcess(int _iProcessLogID, char _cState, int _iAppPid) 
{
    DEFINE_QUERY(qry);
    char sql[1000];
    if (_iAppPid == 0) {
        sprintf( sql," update wf_log_process set state = '%c',end_date = sysdate "
            " where process_log_id = %d ", _cState, _iProcessLogID);
    }
    else {
        sprintf( sql," update wf_log_process set state = '%c',end_date = sysdate, app_pid = %d "
            " where process_log_id = %d ", _cState, _iAppPid, _iProcessLogID );
    }
    qry.close();
    qry.setSQL(sql);
    qry.execute();
    qry.commit();        
}

//守护进程启动后台应用进程前调用(目前守护进程执行的指令全部放入,包括 停止,改变状态)
int TLogProcessMgr::insertLogProcess( int _iProcID, const char *sCommand,
        int iStaffID, int iPlanID)
{
    DEFINE_QUERY(qry); 
    
    int iProcessLogID = getProcessLogID();
    
    TWfLog::log(0,"ProcID:%d, Command:%s, StaffID:%d, PlanID:%d",_iProcID,sCommand,iStaffID,iPlanID);
    
    char sql[1000];
    sprintf( sql,"insert into wf_log_process( "
        " process_log_id,process_id,start_date,state,start_command,staff_id,run_plan_id) "
        " values(%d,%d,sysdate,'B','%s',%d,%d)",iProcessLogID,_iProcID,sCommand,iStaffID,iPlanID );
    qry.close();
    qry.setSQL(sql);
    qry.execute();
    qry.commit();
    
    return iProcessLogID;
}

int TLogProcessMgr::getProcessLogID() {
    int iLogID;
             
//    DEFINE_QUERY(qry);         
    TOCIQuery qry (Environment::getDBConn(true));
    qry.close();
    qry.setSQL("select seq_process_log_id.nextval log_id from dual");
    qry.open();         
    qry.next();         
    iLogID = qry.field("log_id").asInteger();
    
    return iLogID;
}


extern "C" {

void AllTrim(char sBuf[])
{
    int iFirstChar = 0;
    int iEndPos = 0;
    int i;
    int iFirstPos = 0;
    for (i = 0;sBuf[i] != '\0';i++){
        if (sBuf[i] == ' '){
            if (iFirstChar == 0) iFirstPos++;
        }
        else{
            iEndPos = i;
            iFirstChar = 1;
        }
    }
    for (i = iFirstPos;i <= iEndPos;i++)  sBuf[i-iFirstPos] = sBuf[i];
    sBuf[i-iFirstPos] = '\0';
    return ;
}

void TrimHeadNull(char *src,int ilen)
{
    int iPos;
    for (iPos=0; iPos<ilen; iPos++) {
        if (src[iPos]!=0)
            break;        
    }
    if (iPos<ilen)
        strcpy(src,src+iPos);
}

void replace(char * sql,char *src,char *des)
{
	char tmp_all[MAX_SQL_LEN]={0},tmp1[MAX_SQL_LEN]={0},tmp2[MAX_SQL_LEN]={0};//sql;
	char *p=NULL;
	/*string newStr=B;
	string::size_type   pos;
	pos = tmp_all.find(A);
	if(pos==string::npos)return;
	while(pos!=string::npos){
		int len = strlen(A);
		tmp_all.replace(pos, len, newStr);
		pos = tmp_all.find(A);
	}
	strcpy(sql,tmp_all.c_str());*/

	strcpy(tmp_all,sql);
	while ((p=strstr(tmp_all,src))!=NULL)
	{
		strncpy(tmp1,tmp_all,p-tmp_all);
		strncpy(tmp2,p+strlen(src),strlen(tmp_all)-strlen(src)-strlen(tmp1));
		strcat(tmp1,des);
		strcat(tmp1,tmp2);
		strcpy(tmp_all,tmp1);
	}	
	strcpy(sql,tmp_all);
	
}

} //end extern "C"

//------------------------------------------



void TSHMCMA::printShm() {
    
    char str[2000];
    sprintf(str,"\n--------------------------------------------------------");

#if 0
    sprintf(str,"%s\n\n==== Monitor ====",str);
    sprintf(str,"%s\n m_iMonitorLock ---- : %d",str,m_iMonitorLock);
    sprintf(str,"%s\n MntReqInfo.m_iStatus: %d",str,MntReqInfo.m_iStatus);
    sprintf(str,"%s\n MntReqInfo.m_iRegNum: %d",str,MntReqInfo.m_iRegNum);
    sprintf(str,"%s\n MntReqInfo.m_iFlowID: %d",str,MntReqInfo.m_iBillFlowID);
    sprintf(str,"%s\n MntReqInfo.m_LogMode: %d",str,MntReqInfo.m_iLogMode);
    sprintf(str,"%s\n MntReqInfo.m_iIndex : %d",str,MntReqInfo.m_iIndex);
    sprintf(str,"%s\n MntReqInfo.m_gAuthID: %d",str,MntReqInfo.m_igAuthID);
    sprintf(str,"%s\n MntReqInfo.m_iAuthID: %d",str,MntReqInfo.m_iAuthID);
    sprintf(str,"%s\n MntReqInfo.ProcessID: %d",str,MntReqInfo.m_iProcessID);
    sprintf(str,"%s\n MntReqInfo.Operation: %d",str,MntReqInfo.m_iOperation);
    sprintf(str,"%s\n MntReqInfo.m_cState : %c",str,MntReqInfo.m_cState);
    sprintf(str,"%s\n MntReqInfo.m_sMsgBuf: %s",str,MntReqInfo.m_sMsgBuf);
#endif

#if 1    
    sprintf(str,"%s\n\n==== AppReqInfo ====",str);
    sprintf(str,"%s\n m_iAppLock -------- : %d",str,m_iAppLock);
    sprintf(str,"%s\n AppReqInfo.m_iStatus: %d",str,AppReqInfo.m_iStatus);
    sprintf(str,"%s\n AppReqInfo.ProcessID: %d",str,AppReqInfo.m_iProcessID);
    sprintf(str,"%s\n AppReqInfo.m_iAppPID: %d",str,AppReqInfo.m_iAppPID);
    sprintf(str,"%s\n AppReqInfo.m_iIndex : %d",str,AppReqInfo.m_iIndex);
    sprintf(str,"%s\n AppReqInfo.m_cState : %c",str,AppReqInfo.m_cState);
    sprintf(str,"%s\n AppReqInfo.SavBufIdx: %d",str,AppReqInfo.m_iSaveBufIdx);
#endif

#if 1
  sprintf(str,"%s\n\n==== AppInfo ====\n",str);
  for (int i=0; i<8; i++) {
    sprintf(str,"%s \n",str);
    //sprintf(str,"%s ProcLogID:% 6d  |",str,AppInfo[i].m_iProcessLogID);
    sprintf(str,"%s ProcessID:% 6d  |",str,AppInfo[i].m_iProcessID);
    sprintf(str,"%s m_iAppPID:% 6d  |",str,AppInfo[i].m_iAppPID);
    sprintf(str,"%s ProcessCt:% 6d  |",str,AppInfo[i].m_lProcessCnt);
    sprintf(str,"%s NormalCnt:% 6d  |",str,AppInfo[i].m_lNormalCnt);
    sprintf(str,"%s m_cState:%c",str,AppInfo[i].m_cState);
  }
#endif

#if 0
    sprintf(str,"%s\n\n==== Client  ====",str);
    sprintf(str,"%s\n m_iClientLock --------- :%d",str,m_iClientLock);
    sprintf(str,"%s\n ClientReqInfo.m_iStatus : %d",str,ClientReqInfo.m_iStatus);
    sprintf(str,"%s\n ClientReqInfo.m_cState  : %c",str,ClientReqInfo.m_cState);
    sprintf(str,"%s\n ClientReqInfo.m_iRegNum : %d",str,ClientReqInfo.m_iRegNum);
    sprintf(str,"%s\n ClientReqInfo.m_iHostID : %d",str,ClientReqInfo.m_iHostID);
    sprintf(str,"%s\n ClientReqInfo.m_iIndex  : %d",str,ClientReqInfo.m_iIndex);
    sprintf(str,"%s\n ClientReqInfo.ProcessID : %d",str,ClientReqInfo.m_iProcessID);
    sprintf(str,"%s\n ClientReqInfo.m_iStaffID: %d",str,ClientReqInfo.m_iStaffID);
    sprintf(str,"%s\n ClientReqInfo.m_iPlanID : %d",str,ClientReqInfo.m_iPlanID);
    sprintf(str,"%s\n ClientReqInfo.m_iResult : %d",str,ClientReqInfo.m_iResult);
    sprintf(str,"%s\n ClientReqInfo.Operation : %d",str,ClientReqInfo.m_iOperation);
    sprintf(str,"%s\n ClientReqInfo.MainGdIndx: %d",str,ClientReqInfo.m_iMainGuardIndex);
    sprintf(str,"%s\n ClientReqInfo.m_sMsgBuf : %s",str,ClientReqInfo.m_sMsgBuf);
#endif

    sprintf(str,"%s\n",str);
    cout<<str<<endl;
}

