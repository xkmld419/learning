#include "p_ParseBuff.h"




void PETRI::AllTrim(char sBuf[])
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








//构造函数1
PETRI::P_ParseBuff::P_ParseBuff()
{
    reset(); //成员变量复位
}

//构造函数2
/*
P_ParseBuff::P_ParseBuff(char *sInBuff)
{
    parseBuff(sInBuff);
}
*/

//解析字符串 sInBuff
bool PETRI::P_ParseBuff::parseBuff(char *sInBuff)
{
    reset();
    if (strlen(sInBuff)>MAX_PARSEBUFF_LEN ) return false;
    strcpy(m_Str,sInBuff);
    return parse();
}


void PETRI::P_ParseBuff::reset()//数值型的初始化为 MEMBER_NULL = -99, 字符串为空
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
		// 变迁
		m_lTransactionID	= 0;
		m_lTransitionID		= 0;
		m_lDrawID		= 0;
		m_iNeedAck				= MEMBER_NULL;
		memset (m_sSrcState,0,sizeof(m_sSrcState));
		memset (m_sDescState,0,sizeof(m_sDescState));
		memset (m_sStartTime,0,sizeof(m_sStartTime));
		memset (m_sEndTime,0,sizeof(m_sEndTime));

    memset (m_sMsg,0,MSG_MAXCNT*(MSG_MAXLEN+1));
}


//解析格式串
//<iBillFlowID=2><iProcessID=1023>...<sMsg=message_str1><sMsg=message_str2>...<sMsg=message_strN>
bool PETRI::P_ParseBuff::parse() //if (m_str) parse it.
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
void PETRI::P_ParseBuff::formatBuff()
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

unsigned long PETRI::P_ParseBuff::stringToUL (char *pValue)
{
    char *pt;
    for (pt=pValue; *pt; pt++) {
        if(*pt>='0' && *pt<='9')
        break;
    }
    return strtoul(pt,0,10);
}

//保存一个成员变量 *pAtt(变量名) *pValue(变量值)
void PETRI::P_ParseBuff::parseResultSave(char *pAtt, char *pValue)
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

		//begin
    else if (strcmp(pAtt,"lTransactionID") == 0 ){
			this->m_lTransactionID = stringToUL(pValue);
    }    
    else if (strcmp(pAtt,"lTransitionID") == 0 ){
			this->m_lTransitionID = stringToUL(pValue);
		}
		else if (strcmp(pAtt,"lDrawID") == 0 ){
			this->m_lDrawID = stringToUL(pValue);
		}
		else if (strcmp(pAtt,"iNeedAck") ==0 ) {
			this->m_iNeedAck = atoi(pValue);
		}
    else if (strcmp(pAtt,"sSrcState") == 0 ){
			strncpy(m_sSrcState,pValue,sizeof(m_sSrcState)-1);
		}
    else if (strcmp(pAtt,"sDescState") == 0 ){
			strncpy(m_sDescState,pValue,sizeof(m_sDescState)-1);
		}
    else if (strcmp(pAtt,"sStartTime") == 0 ){
			strncpy(m_sStartTime,pValue,sizeof(m_sStartTime)-1);
		}
    else if (strcmp(pAtt,"sEndTime") == 0 ){
			strncpy(m_sEndTime,pValue,sizeof(m_sEndTime)-1);
		}
		//end

		else {
			//do nothing
			//
		}

}


//压入文本消息到m_sMsg
void PETRI::P_ParseBuff::addMsg(const char *msg) //压入文本消息到m_sMsg
{
    char sTemp[MSG_MAXLEN];
    strncpy (sTemp,msg,MSG_MAXLEN-1);
    sTemp[MSG_MAXLEN-1] = 0;
    
    if (m_iMsgOff>= MSG_MAXCNT ) return;
    strcpy(this->m_sMsg[m_iMsgOff], sTemp);
    m_iMsgOff++;
}

//将内部成员变量build成字符流 => m_Str
bool PETRI::P_ParseBuff::build()
{
    formatBuff();
    m_Str[0] = 0;


		//new member
		if (m_lTransactionID != 0) 
        sprintf(m_Str,"%s<lTransactionID=%lu>",m_Str,m_lTransactionID );
		if (m_lTransitionID != 0) 
        sprintf(m_Str,"%s<lTransitionID=%lu>",m_Str,m_lTransitionID );
		/*
		if (m_lTransitionID != MEMBER_NULL) 
        sprintf(m_Str,"%s<iNeedAck=%d>",m_Str,m_iNeedAck);
				*/
		if (m_sSrcState[0] )
        sprintf(m_Str,"%s<sSrcState=%s>",m_Str,m_sSrcState);
		if (m_sDescState[0] )
        sprintf(m_Str,"%s<sDescState=%s>",m_Str,m_sDescState);
 		if (m_sStartTime[0] )
        sprintf(m_Str,"%s<sStartTime=%s>",m_Str,m_sStartTime);
 		if (m_sEndTime[0] )
        sprintf(m_Str,"%s<sEndTime=%s>",m_Str,m_sEndTime);
		//end
       
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
        sprintf(m_Str,"%s<iProcessCnt=%lu>",m_Str,m_lProcessCnt);
    
    if (m_lNormalCnt != 0)
        sprintf(m_Str,"%s<iNor=%lu>",m_Str,m_lNormalCnt);
    
    if (m_lErrorCnt != 0)
        sprintf(m_Str,"%s<iErr=%lu>",m_Str,m_lErrorCnt);
    
    if (m_lBlackCnt != 0)
        sprintf(m_Str,"%s<iBlk=%lu>",m_Str,m_lBlackCnt);
    
    if (m_lOtherCnt != 0)
        sprintf(m_Str,"%s<iOth=%lu>",m_Str,m_lOtherCnt);
    
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
bool PETRI::P_ParseBuff::getBuiltStr(char *sOutBuff)
{
    if (!sOutBuff) return false;
    build();    
    strcpy(sOutBuff,m_Str);    
    return true;
}

//仅获取文本消息
char * PETRI::P_ParseBuff::getBuiltMsg()
{
    formatBuff();
    m_Str[0] = 0;
    
    for (int iPos = 0; iPos<m_iMsgOff; iPos++) {
        int iLen = strlen(m_Str)+ strlen(m_sMsg[iPos]);
        if (iLen > MAX_PARSEBUFF_LEN) break;
        sprintf(m_Str,"%s<sMsg=%s>",m_Str,m_sMsg[iPos]);
    }
    
    return m_Str;
};


