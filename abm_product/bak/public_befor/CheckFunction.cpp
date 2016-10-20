/*VER: 2*/ 
#include "CheckFunction.h"
#include "Head2OrgIDMgr.h"
#include "OrgMgrEx.h"
#include "EdgeRoamMgr.h"
#include "EventToolKits.h"
#include "FormatMethod.h"
#include "EventTypeMgrEx.h"
#include "Head2OrgIDMgr.h"
#include "Date.h"
#include "ZoneMgr.h"
//#include "../app_prep/Tap3FileInfo.h"
#include <string.h>
#include <string>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include "ParamInfoInterface.h"

//CheckFunction Class
CheckFunction::CheckFunction()
{
}

CheckFunction::~CheckFunction()
{
}

///* Public Function */
bool CheckFunction::doFunction(int iFuncID, const EventSection *poEventSection, 
		int iTAG, const CheckArgument *poFirstArgument, string *psResult, 
		string *psInput)
{
	#ifndef PRIVATE_MEMORY 
	CheckArgument *pArgNull = &(G_PPARAMINFO->m_poCheckArgumentList[0]);
	CheckArgument *pArgument = (G_PPARAMINFO->m_poCheckArgumentList);
	#endif
    int iAttrID = -1;
    string sAttrValue;
    char sTime[15]={0};

    char m_sFileHead[10]={0};
	//   char *p;
    //char *p = strrchr(poEventSection->m_oEventExt.m_sFileName,'_');
    //strncpy(m_sFileHead,poEventSection->m_oEventExt.m_sFileName,p-poEventSection->m_oEventExt.m_sFileName);

    //当psInput不为空时，表明此次调用是作为管道的下游函数来使用
    if (psInput != NULL){
        sAttrValue = *psInput;
    }else{
	    EventToolKits toolKits;
	    iAttrID = TagSeqMgr::getArrtID(poEventSection->m_iGroupID, iTAG);
	    sAttrValue = toolKits.get_param(poEventSection, iAttrID);  
	    /*if (iTAG>=100)
	    	sAttrValue = toolKits.get_param(poEventSection, iAttrID);   
	   	else
	    {
	   	 sAttrValue =  toolKits.get_param (&poEventSection, 
	   		 Tap3FileInfo::getEventAttrID (m_sFileHead, C_Head_Block, C_record_type_Tag)).c_str();
		 if (sTValue[0] = 0)
			 sAttrValue = toolKits.get_param (&poEventSection, 
			 Tap3FileInfo::getEventAttrID (m_sFileHead, C_Tail_Block, C_record_type_Tag)).c_str();
	    }*/
    }
	
    long lValue = atoi(sAttrValue.c_str());
    const char *sValue = sAttrValue.c_str();
        
    const char *pTime  = poEventSection->m_sStartDate;
    //char sTmpName[250]={0};
    //strcpy(sTmpName,poEventSection->m_oEventExt.m_sFileName);
    const char *p = strrchr(poEventSection->m_oEventExt.m_sFileName,'/');
    char sFileName[64]={0};
    if (!p)
	strcpy(sFileName,poEventSection->m_oEventExt.m_sFileName);
    else
	   strcpy(sFileName,p+1);
		
	strcpy(sTime,pTime);
	const CheckArgument *pArgu = poFirstArgument;
        
    int  iTmp, iTmp2;
    float fTmp;
    char sTmp[32];
    char cTmp;
    const char *p1;
    const char *p2;
    bool  bRet;
    memset(sTmp, 0, sizeof(sTmp));
    Date td; //tmp date
    
    
	switch (iFuncID){
	case CK_GET_ATTR:
	    if (psResult!=NULL)
	        *psResult = sAttrValue;
	    return true;
	case CK_DO_FUNC:
	#ifdef PRIVATE_MEMORY 
	    if (pArgu==NULL || pArgu->m_sValue[0]==0 
	        || pArgu->m_poNextSibArgument==NULL
	        || pArgu->m_poNextSibArgument->m_sValue[0]==0)
	        return false;
	    if (atoi(pArgu->m_sValue)==CK_DO_FUNC)
	        return false;
	    iTmp = atoi(pArgu->m_poNextSibArgument->m_sValue);
	    return doFunction(atoi(pArgu->m_sValue), poEventSection, iTmp, 
	        pArgu->m_poNextSibArgument->m_poNextSibArgument, psResult); 
	#else
		if (pArgu==NULL || pArgu== pArgNull || pArgu->m_sValue[0]==0 
	        || pArgu->m_iNextSibArgument==0
	        || G_PPARAMINFO->m_poCheckArgumentList[pArgu->m_iNextSibArgument].m_sValue[0]==0)
	        return false;
	    if (atoi(pArgu->m_sValue)==CK_DO_FUNC)
	        return false;
	    iTmp = atoi(G_PPARAMINFO->m_poCheckArgumentList[pArgu->m_iNextSibArgument].m_sValue);
	    return doFunction(atoi(pArgu->m_sValue), poEventSection, iTmp, 
	        &(pArgument[pArgument[pArgu->m_iNextSibArgument].m_iNextSibArgument]), psResult); 
	#endif
	case CK_CMP_TWO_FUNC:
	    return cmpTwoFunc(poEventSection, pArgu);
	case CK_IS_DEC:	       
	    return isNumber(sValue, pArgu, "DEC");
	case CK_IS_OCT:	       
	    return isNumber(sValue, pArgu, "OCT");
	case CK_IS_HEX:	       
	    return isNumber(sValue, pArgu, "HEX");
	case CK_BEGIN_WITH:
	    if( beginWith(sValue, pArgu, sTmp)){
	        if (psResult!=NULL)
	            *psResult = string(sTmp, strlen(sTmp));
	        return true;
	    }
	    return false;
	case CK_GET_ORG:
	    if (strlen(sTime) == 0)
		strcpy(sTime,td.toString());
	    if (!getOrgID(sValue, iTmp, sTime, pArgu))
	        return false;
	     sprintf(sTmp, "%d", iTmp);
	     if (psResult!=NULL)
	        *psResult = string(sTmp, strlen(sTmp));
	    return true;
	case CK_AREA_CODE:
	    if (strlen(sTime) == 0)
		strcpy(sTime,td.toString());
	    if (!getAreaCode(sValue, sTmp, sizeof(sTmp), sTime, pArgu))
	        return false;
	    if (psResult!=NULL)
	        *psResult = string(sTmp, strlen(sTmp));
	    return true;
	case CK_IS_AREACODE:
	    if (strlen(sTime) == 0)
		strcpy(sTime,td.toString());
	    if(!isAreaCode(sValue, sTmp, sizeof(sTmp), sTime, pArgu))
	        return false;
	    if (psResult!=NULL)
	        *psResult = string(sTmp, strlen(sTmp));
	    return true;
	case CK_PROVIN_CODE:
	    if (strlen(sTime) == 0)
		strcpy(sTime,td.toString());
	    p1 = strrchr(poEventSection->m_oEventExt.m_sFileName, '.')-3;
	    if (!getAreaCode(sValue, sTmp, sizeof(sTmp), sTime, pArgu))
	        return false;
	    iTmp = strlen(sTmp);
	    p2 = &(sTmp[iTmp-3]);//指向倒数第三位
	    if (p1[0]==p2[0] && p1[1]==p2[1] && p1[2]==p2[2]){
	        if (psResult!=NULL)
	            *psResult = string(sTmp, strlen(sTmp));
	        return true;
	    }
	    return false;
    case CK_DATE_FORMAT:
	if (strlen(sValue) == 0)
		return false;
		#ifdef PRIVATE_MEMORY 
        if (pArgu==NULL || pArgu->m_sValue[0]==0){
		#else
		if (pArgu==NULL || pArgu==pArgNull || pArgu->m_sValue[0]==0){
		#endif
            if (!td.parse(sValue, NULL))
                return false;    
            string s1 = string(td.toString()).substr(0, 8);
            string s2 = string(td.toString()).substr(8);
            		
	     if (td.checkDate(s1.c_str())!=-1 && td.checkTime( s2.c_str())!=-1)
                return true;            
            else
                return false;
        }
        else{
            if (!td.parse(sValue, pArgu->m_sValue))
                return false;
            string s1,s2;
            if (strlen(sValue) == 14 || strlen(sValue) == 8)  //包含年月日的情况
	     {
            		s1 = string(td.toString()).substr(0, 8);
            		s2 = string(td.toString()).substr(8);
            		if (td.checkDate(s1.c_str())!=-1 && td.checkTime( s2.c_str())!=-1)
                		return true;
            		else 
            			return false;
            	}
            else   //仅有时分秒的形式
            	{
			s1 = string(td.toString("HHMISS"));
			if (td.checkTime( s1.c_str())!=-1)
                		return true;
			else
				return false;
            	}
        }
    case CK_BEYOND_DATE:
        return isBeyondDate(sValue, pArgu,sFileName);
    case CK_LARGER_THEN:
        if (pArgu==NULL || pArgu->m_sValue[0]==0)
            iTmp = 0;
        else if (!isNumber(pArgu->m_sValue, NULL, "DEC"))
            return false;
        else
            iTmp = atoi(pArgu->m_sValue);
        return lValue>iTmp;
    case CK_LESS_THEN:
		#ifdef PRIVATE_MEMORY
        if (pArgu==NULL || pArgu->m_sValue[0]==0)
		#else
		if (pArgu==NULL || pArgu==pArgNull || pArgu->m_sValue[0]==0)
		#endif
            iTmp = 0;
        else if (!isNumber(pArgu->m_sValue, NULL, "DEC"))
            return false;
        else
            iTmp = atoi(pArgu->m_sValue);
        return lValue<iTmp;
    case CK_EQUAL:
		#ifdef PRIVATE_MEMORY
        if (pArgu==NULL || pArgu->m_sValue[0]==0)
		#else
		if (pArgu==NULL || pArgu==pArgNull || pArgu->m_sValue[0]==0)
		#endif
            iTmp = 0;
        else if (!isNumber(pArgu->m_sValue, NULL, "DEC"))
            return false;
        else
            iTmp = atoi(pArgu->m_sValue);
	if (strlen(sValue) == 0) return false;
	
        return lValue==iTmp;
    case CK_GET_ORG_2:
        if (!getOrgID(poEventSection, iTmp, pArgu, NULL))
            return false;
        sprintf(sTmp, "%d", iTmp);
        if (psResult!=NULL)
	        *psResult = string(sTmp, strlen(sTmp));
	    return true;
    case CK_LENGTH:
		#ifdef PRIVATE_MEMORY
        if (pArgu==NULL || pArgu->m_sValue[0]==0)
            iTmp = 0;
        //else if (!isNumber(pArgu->m_sValue, NULL, "DEC"))
        //    return false;
        else
            iTmp = atoi(pArgu->m_sValue);
        if (pArgu->m_poNextSibArgument == NULL || strcmp(pArgu->m_poNextSibArgument->m_sValue,"=") == 0) 
        	return strlen(sValue)==iTmp;
        else if (strcmp(pArgu->m_poNextSibArgument->m_sValue,"<") == 0)
        		return strlen(sValue)<iTmp;
        else if (strcmp(pArgu->m_poNextSibArgument->m_sValue,">") == 0)
        		return strlen(sValue)>iTmp;
        else 
        	return false;
		#else
		if (pArgu==NULL || pArgu==pArgNull || pArgu->m_sValue[0]==0)
			iTmp = 0;
        else
            iTmp = atoi(pArgu->m_sValue);
        if (pArgu->m_iNextSibArgument == 0 || strcmp(pArgument[pArgu->m_iNextSibArgument].m_sValue,"=") == 0) 
        	return strlen(sValue)==iTmp;
        else if (strcmp(pArgument[pArgu->m_iNextSibArgument].m_sValue,"<") == 0)
        		return strlen(sValue)<iTmp;
        else if (strcmp(pArgument[pArgu->m_iNextSibArgument].m_sValue,">") == 0)
        		return strlen(sValue)>iTmp;
        else 
        	return false;
		#endif
    case CK_MUX:
		#ifdef PRIVATE_MEMORY
        if (pArgu==NULL || pArgu->m_sValue[0]==0)
		#else
		if (pArgu==NULL || pArgu==pArgNull || pArgu->m_sValue[0]==0)
		#endif
            fTmp = 0;
        else
            fTmp = atof(pArgu->m_sValue);
        //向上取整    
        iTmp = ceil(fTmp*lValue);
        sprintf(sTmp, "%d", iTmp);
        if (psResult!=NULL)
	        *psResult = string(sTmp, strlen(sTmp));
	    return true;
	case CK_DIV:
		#ifdef PRIVATE_MEMORY
	    if (pArgu==NULL || pArgu->m_sValue[0]==0)
		#else
		if (pArgu==NULL || pArgu==pArgNull || pArgu->m_sValue[0]==0)
		#endif
            fTmp = 1;
        else
            fTmp = atof(pArgu->m_sValue);
        if (fTmp == 0.0) return false;
        //向上取整
        iTmp = (int)((lValue+fTmp-1)/fTmp);
        sprintf(sTmp, "%d", iTmp);
        if (psResult!=NULL)
	        *psResult = string(sTmp, strlen(sTmp));
	    return true;
    case CK_GET_CARRIER:
        if (!formatCarrier(poEventSection, iTmp))
            return false;
        sprintf(sTmp, "%d", iTmp);
        if (psResult!=NULL)
            *psResult = string(sTmp, strlen(sTmp));
        return true;
    case CK_FORMAT_CARRIER:
        bRet = formatCarrier(poEventSection, iTmp, sTmp);
        if (psResult!=NULL)
            *psResult = string(sTmp, strlen(sTmp));
        return bRet;
    case CK_FORMAT_AREACODE:
        bRet = formatAreaCode(poEventSection, sTmp);
        if (psResult!=NULL)
            *psResult = string(sTmp, strlen(sTmp));
        return bRet;
    case CK_IS_MSC:
        if (!isMSC(sValue, pArgu, sTmp))
            return false;
        if (psResult!=NULL)
	        *psResult = string(sTmp, strlen(sTmp));
	    return true;
	case CK_GET_PARTNER:
	    if (!(bRet=getPartnerID(sValue, pTime, iTmp)))
            iTmp = -1;
        sprintf(sTmp, "%d", iTmp);
        if (psResult!=NULL)
            *psResult = string(sTmp, strlen(sTmp));
        return bRet;
    case CK_PIPE:
        bRet = pipe(poEventSection, pArgu, sTmp);
        if (psResult!=NULL)
            *psResult = string(sTmp, strlen(sTmp));
        return bRet;
    case CK_IS_INNER:
        return isInner(poEventSection, pTime);
    case CK_FORMAT_0086:
        format0086(poEventSection->m_sOrgCalledNBR, sTmp);
        if (psResult!=NULL)
            *psResult = string(sTmp, strlen(sTmp));
        return true;
    case CK_GET_LONG_TYPE:
        if (!(bRet=getLongTypeAndRoamID(poEventSection, iTmp, iTmp2)))
            iTmp = -1;
        sprintf(sTmp, "%d", iTmp);
        if (psResult!=NULL)
            *psResult = string(sTmp, strlen(sTmp));
        return bRet;        
    case CK_GET_ROAM_TYPE:
        if (!(bRet=getLongTypeAndRoamID(poEventSection, iTmp, iTmp2)))
            iTmp2 = -1;
        sprintf(sTmp, "%d", iTmp2);
        if (psResult!=NULL)
            *psResult = string(sTmp, strlen(sTmp));
        return bRet;
    case CK_CMP_CHAROFSTRING:
    	if (!cmpCharofString(sValue, pArgu, cTmp))
    		return false;
    	return true;
    case CK_SUBSTR:
    	 if (!cmpSubStr(sValue,pArgu))
    	 	return false;
    	 return true;
    case CK_ZONE:	
	if (!check_zone(sValue,pArgu,0))
		return false;
	return true;
    case CK_ZONE_MAX:	
	if (!check_zone(sValue,pArgu,1))
		return false;
	return true;
	default: 
		return false;  //没有对应的函数，则默认不通过	
	} /* End of switch (iFuncID) */

	return true;
}

#ifdef PRIVATE_MEMORY
bool CheckFunction::isNumber(const char* str, 
    const CheckArgument *poTable, const char *pFormat)
{
    if (str==NULL || str[0]==0)
        return true;
        
    const CheckArgument  *pArg = poTable;
    const CheckArgument  *psbArg = NULL;    //sibArgument
    const CheckArgument  *pntArg = NULL;    //nextArgument
    const char  *p;
    const int length = strlen(str);
    
    //默认的检测位置和长度
    int i, pos = 0;
    int len = -1;
        
    //参数表不为空
	if (pArg != NULL){                 
	    pos = atoi(pArg->m_sValue); 
	    
	    //参数表的第二个长度参数不为空或其值不为空
	    psbArg = pArg->m_poNextSibArgument;
	    if (psbArg!=NULL && psbArg->m_sValue[0]!=0){
	        i = atoi(psbArg->m_sValue);
	        len = i>0?i:len;
	    }
	    
	    //pos<0时，需特殊处理，处理后，pos应该不小于0
	    if (pos<0)
	        pos = length+pos;
	        
	    len = (len==-1)?(length-pos):len;
	    //判断pos, len是否非法
        if (pos<0 || (len+pos)>length)
            return false;   	    
    }
    len = (len==-1)?length:len;
    
    //目前支持：十、八、十六进制
    enum Format {
        DEC,            //decimal
        OCT,            //octal
        HEX,            //hex
        UNKNOW          //unknow format
    } eFmt;    
    
    if (pFormat==NULL){
        eFmt = DEC;    
    }else if (strcmp(pFormat, "DEC")==0)
        eFmt = DEC;
    else if (strcmp(pFormat, "OCT")==0)
        eFmt = OCT;
    else if (strcmp(pFormat, "HEX")==0)
        eFmt = HEX;
    else
        eFmt = UNKNOW;
    
     
    p = &(str[pos]);
    
    
    i = 0;
    //处理负号,单独一个'-'字符，不能算是数字；负号应在字符串首位
    if (len>=2 && pos==0 && p[0]=='-') i++; 
        
    switch(eFmt){
    case DEC:
        for (; i<len; i++){
            if (p[i]<'0' || p[i]>'9')
                return false;
        }
        return true;
    case OCT:
        for (; i<len; i++){
            if (p[i]<'0' || p[i]>'7')
                return false;
        }
        return true;
    case HEX:
        for (; i<len; i++){
            if (p[i]<'0' || (p[i]>'9' && p[i]<'A') 
                || (p[i]>'F' && p[i]<'a') || p[i]>'z' )
                return false;
        }
        return true;
    default:
        return false;
    }
}
#else
bool CheckFunction::isNumber(const char* str, 
    const CheckArgument *poTable, const char *pFormat)
{
#ifndef PRIVATE_MEMORY 
	CheckArgument *pArgNull = &(G_PPARAMINFO->m_poCheckArgumentList[0]);
	CheckArgument *pArgument = (G_PPARAMINFO->m_poCheckArgumentList);
#endif
    if (str==NULL || str[0]==0)
        return true;
        
    const CheckArgument  *pArg = poTable;
    const CheckArgument  *psbArg = NULL;    //sibArgument
    const CheckArgument  *pntArg = NULL;    //nextArgument
    const char  *p;
    const int length = strlen(str);
    //默认的检测位置和长度
    int i, pos = 0;
    int len = -1;
    //参数表不为空
	if (pArg != NULL){                 
	    pos = atoi(pArg->m_sValue); 
	    //参数表的第二个长度参数不为空或其值不为空
		if(pArg->m_iNextSibArgument)
	    	psbArg = &(pArgument[pArg->m_iNextSibArgument]);
	    if (psbArg!=NULL && psbArg->m_sValue[0]!=0){
	        i = atoi(psbArg->m_sValue);
	        len = i>0?i:len;
	    }
	    //pos<0时，需特殊处理，处理后，pos应该不小于0
	    if (pos<0)
	        pos = length+pos;	  
	    len = (len==-1)?(length-pos):len;
	    //判断pos, len是否非法
        if (pos<0 || (len+pos)>length)
            return false;   	    
    }
    len = (len==-1)?length:len;    
    //目前支持：十、八、十六进制
    enum Format {
        DEC,            //decimal
        OCT,            //octal
        HEX,            //hex
        UNKNOW          //unknow format
    } eFmt;    
    if (pFormat==NULL){
        eFmt = DEC;    
    }else if (strcmp(pFormat, "DEC")==0)
        eFmt = DEC;
    else if (strcmp(pFormat, "OCT")==0)
        eFmt = OCT;
    else if (strcmp(pFormat, "HEX")==0)
        eFmt = HEX;
    else
        eFmt = UNKNOW;   
    p = &(str[pos]);
    i = 0;
    //处理负号,单独一个'-'字符，不能算是数字；负号应在字符串首位
    if (len>=2 && pos==0 && p[0]=='-') i++; 
    switch(eFmt){
    case DEC:
        for (; i<len; i++){
            if (p[i]<'0' || p[i]>'9')
                return false;
        }
        return true;
    case OCT:
        for (; i<len; i++){
            if (p[i]<'0' || p[i]>'7')
                return false;
        }
        return true;
    case HEX:
        for (; i<len; i++){
            if (p[i]<'0' || (p[i]>'9' && p[i]<'A') 
                || (p[i]>'F' && p[i]<'a') || p[i]>'z' )
                return false;
        }
        return true;
    default:
        return false;
    }
}
#endif

#ifdef PRIVATE_MEMORY
bool CheckFunction::beginWith(const char* str, 
    const CheckArgument *poTable, char *pHead)
{
    if (str==NULL || str[0]==0 || poTable==NULL)
        return false;
       
    const CheckArgument  *pArg = poTable;
    const CheckArgument  *pntArg = NULL;    //m_poNextArgument
    const CheckArgument  *psbArg = NULL;    //m_poNextSibArgument
    const char  *p1;
    const char  *p2;   
       
    const int length = strlen(str);
    int len;
    int pos = 0;
    
    //若不为NULL，应为十进制字符串
    if (pArg->m_sValue[0]!=0 && !isNumber(pArg->m_sValue, NULL, "DEC"))
        return false;
        
    //求pos, m_sValue为NULL时返回0, 
    pos = atoi(pArg->m_sValue);
    
    if (pos<0 || pos>=length)
        return false;
    
    pntArg = pArg->m_poNextSibArgument;    
    while (pntArg!=NULL && pntArg->m_sValue[0]!=0){
        if (pntArg->m_sValue[0]==0 || length<strlen(pntArg->m_sValue)+pos){
            pntArg = pntArg->m_poNextArgument;
            continue;
        }
            
        p1 = &(str[pos]);
        p2 = pntArg->m_sValue;
        while (*p2!='\0' && *p1!='\0' && *p1==*p2){
            p1++;
            p2++;
        }
        if (*p2=='\0'){  //found it!
            if (pHead!=NULL)
                strcpy(pHead, pntArg->m_sValue);
            break;
        }
            
        pntArg = pntArg->m_poNextArgument;
    }
    
    if (pntArg == NULL) //not match all
        return false;
    
    return true;    
}
#else
bool CheckFunction::beginWith(const char* str, 
    const CheckArgument *poTable, char *pHead)
{
#ifndef PRIVATE_MEMORY 
	CheckArgument *pArgNull = &(G_PPARAMINFO->m_poCheckArgumentList[0]);
	CheckArgument *pArgument = (G_PPARAMINFO->m_poCheckArgumentList);
#endif
	if (str==NULL || str[0]==0 || poTable==NULL || poTable == pArgNull)
        return false;
    const CheckArgument  *pArg = poTable;
    const CheckArgument  *pntArg = NULL;    //m_poNextArgument
    const CheckArgument  *psbArg = NULL;    //m_poNextSibArgument
    const char  *p1;
    const char  *p2;   
    const int length = strlen(str);
    int len;
    int pos = 0;	
    //若不为NULL，应为十进制字符串
    if (pArg->m_sValue[0]!=0 && !isNumber(pArg->m_sValue, NULL, "DEC"))
        return false;
    //求pos, m_sValue为NULL时返回0, 
    pos = atoi(pArg->m_sValue);
    if (pos<0 || pos>=length)
        return false;
    if(pArg->m_iNextSibArgument){
   	 	pntArg = &(pArgument[pArg->m_iNextSibArgument]); 
	} 
    while (pntArg!=NULL && pntArg->m_sValue[0]!=0){
        if (pntArg->m_sValue[0]==0 || length<strlen(pntArg->m_sValue)+pos){
			if(pntArg->m_iNextArgument){
            	pntArg = &(pArgument[pntArg->m_iNextArgument]);
			}else{
				pntArg = 0;
			}
            continue;
        }
        p1 = &(str[pos]);
        p2 = pntArg->m_sValue;
        while (*p2!='\0' && *p1!='\0' && *p1==*p2){
            p1++;
            p2++;
        }
        if (*p2=='\0'){  //found it!
            if (pHead!=NULL)
                strcpy(pHead, pntArg->m_sValue);
            break;
        }
		if(pntArg->m_iNextArgument){       
        	pntArg = &(pArgument[pntArg->m_iNextArgument]);
		}else{
			pntArg = NULL;
		}
    }
    if (pntArg == NULL) //not match all
        return false;  
    return true;    
}
#endif

#ifdef PRIVATE_MEMORY
bool CheckFunction::getOrgID(const char *sNumber, int &iOrgID, 
        const char* pTime, const CheckArgument *poTable, char *pHead, int *piShift)
{
    if (sNumber==NULL || sNumber[0]==0
        || poTable==NULL || poTable->m_sValue[0]==NULL)
        return false;
    
    int iPos = 0;
    int iOffset=0; 
    char sFlag[32];  
    char  sHead[32];
    char  sTmp[50];
    
    memset(sFlag, 0, sizeof(sFlag));
    strcpy(sFlag, poTable->m_sValue);
    poTable = poTable->m_poNextSibArgument;
    
    //判断号头，如果有号头，需改变iOffset
    memset(sHead, 0, sizeof(sHead));
    if (poTable->m_poNextSibArgument!=NULL && poTable->m_poNextSibArgument->m_sValue[0]!=0){
        if (!beginWith(sNumber, poTable, sHead))
            //return false;
            iOffset = 0;
        else
        {
        	iOffset = strlen(sHead);
        	if (pHead!=NULL)
            		strcpy(pHead, sHead);	
        }
     }
        
        //get iPos
        iPos=atoi(poTable->m_sValue);  
    
    
    const char *p = &(sNumber[iPos+iOffset]);
    int j = 0;
    
    //去除号头前的0086国家代码
    char toNBR[64];
    if (!format0086(p, toNBR, &j, piShift))
        return false;
    
    //pHead可能需要增长一些内容
    if (j>0)
        strncpy(&(pHead[iOffset]), &(sNumber[iPos+iOffset]), j);
    
    //test code
    /* printf("sNumber: %s\niPos: %d\niOffset: %d\nsHead: %s\np1: %s\np2: %s\n",
            sNumber, iPos, iOffset, sHead, &(sNumber[iPos+iOffset]), p);     
    return true;
    */

    return Head2OrgIDMgr::getOrgID(p, pTime, iOrgID,sFlag);

}
#else
bool CheckFunction::getOrgID(const char *sNumber, int &iOrgID, 
        const char* pTime, const CheckArgument *poTable, char *pHead, int *piShift)
{
	CheckArgument *pArgNull = &(G_PPARAMINFO->m_poCheckArgumentList[0]);
	CheckArgument *pArgument = (G_PPARAMINFO->m_poCheckArgumentList);
    if (sNumber==NULL || sNumber[0]==0 || poTable==NULL || poTable->m_sValue[0]==NULL)
        return false;
    int   iPos = 0;
    int   iOffset = 0; 
    char  sFlag[32] = {0};  
    char  sHead[32] = {0};
    char  sTmp[50] = {0};

    memset(sFlag, 0, sizeof(sFlag));
    strcpy(sFlag, poTable->m_sValue);
	if(poTable->m_iNextSibArgument){
    	poTable = &(pArgument[poTable->m_iNextSibArgument]);
	}else{
		poTable = NULL;
	}
    //判断号头，如果有号头，需改变iOffset
    memset(sHead, 0, sizeof(sHead));
    if (poTable->m_iNextSibArgument!=0 && pArgument[poTable->m_iNextSibArgument].m_sValue[0]!=0){
        if (!beginWith(sNumber, poTable, sHead)){
            iOffset = 0;
        }else{
        	iOffset = strlen(sHead);
        	if (pHead!=NULL)
            	strcpy(pHead, sHead);	
        }
     }
    iPos=atoi(poTable->m_sValue);  
    const char *p = &(sNumber[iPos+iOffset]);
    int j = 0;
    //去除号头前的0086国家代码
    char toNBR[64];
    if (!format0086(p, toNBR, &j, piShift))
        return false;
	//pHead可能需要增长一些内容 
    if (j>0)  
        strncpy(&(pHead[iOffset]), &(sNumber[iPos+iOffset]), j);
    return Head2OrgIDMgr::getOrgID(p, pTime, iOrgID,sFlag);
}
#endif

#ifdef PRIVATE_MEMORY
bool CheckFunction::getAreaCode(const char *sNumber, char *pAreaCode, int iSize,
        const char *pTime, const CheckArgument *poTable)
{
    if (sNumber==NULL || sNumber[0]==0 || pAreaCode==NULL)
        return false;
    
    //参数表的Arg1,Arg2不能为空
    int iLevelID = 0;
    CheckArgument *psbArg = poTable->m_poNextSibArgument;
    if (poTable==NULL || poTable->m_sValue[0]==0 
        || psbArg==NULL || psbArg->m_sValue[0]==0)
        return false;
        
    int iOrgID = 0;
    int iShift = 0;
    char sHead[32];
    
    //获取OrgID
    memset(sHead, 0, sizeof(sHead));
    if (!getOrgID(sNumber, iOrgID, pTime, psbArg, sHead, &iShift))
        return false;

    //test code
    //printf("sHead:%s iShift:%d\n", sHead, iShift);   
        
    //由OrgID及levelID获取归属局号
    if (!isNumber(poTable->m_sValue, NULL, NULL))
        return false;
    
    iLevelID = atoi(poTable->m_sValue);
    OrgMgrEx *poOrgMgrEx = OrgMgrEx::GetInstance(); 
    if (!poOrgMgrEx->getAreaCode(iOrgID, pAreaCode, iSize, iLevelID))
        return false;    
            
    //校验向后匹配的号头
    int iPos = 0;
    int offset;
    const char *p1;
    const char *p2;
    CheckArgument *pntArg;
    
    //判断是否有向后匹配的号头，若没有，返回true.
    psbArg = poTable->m_poNextSibArgument->m_poNextSibArgument;
    if (psbArg==NULL || psbArg->m_poNextSibArgument==NULL
        || (pntArg=psbArg->m_poNextSibArgument->m_poNextSibArgument)==NULL)
        return true;

    iPos = atoi(psbArg->m_sValue);    
    offset = iPos+strlen(sHead)+strlen(pAreaCode)-iShift;  
    
    while (pntArg!=NULL){
        if (pntArg->m_sValue[0]==0 
            || strlen(sNumber)<strlen(pntArg->m_sValue)+offset){
            pntArg = pntArg->m_poNextArgument;
            continue;
        }
        
        p1 = &(sNumber[offset]);
        p2 = pntArg->m_sValue;
        while (*p2!='\0' && *p1!='\0' && *p1==*p2){
            p1++;
            p2++;
        }
        
        if (*p2=='\0'){ //found it 
            printf("sHead:%s\n", pntArg->m_sValue);
            break;
        }
            
        pntArg = pntArg->m_poNextArgument;
    }
   
    if (pntArg == NULL)
        return false;
        
    return true;
        
}
#else
bool CheckFunction::getAreaCode(const char *sNumber, char *pAreaCode, int iSize,
        const char *pTime, const CheckArgument *poTable)
{
	CheckArgument *pArgument = (G_PPARAMINFO->m_poCheckArgumentList);
    CheckArgument *pTmp = NULL;
	CheckArgument *pTmp1 = NULL;
	CheckArgument *pArgNull = &(G_PPARAMINFO->m_poCheckArgumentList[0]);
	if (sNumber==NULL || sNumber[0]==0 || pAreaCode==NULL)
        return false;  
    //参数表的Arg1,Arg2不能为空
    int iLevelID = 0;
	if (poTable==NULL || poTable == pArgNull || poTable->m_sValue[0]==0) return false;
	CheckArgument *psbArg = NULL;
	if(poTable->m_iNextSibArgument){
    	psbArg = &(pArgument[poTable->m_iNextSibArgument]);
	}
    if (poTable==NULL || poTable->m_sValue[0]==0 || psbArg==NULL || psbArg->m_sValue[0]==0)
        return false;    
    int iOrgID = 0;
    int iShift = 0;
    char sHead[32];   
    //获取OrgID
    memset(sHead, 0, sizeof(sHead));
    if (!getOrgID(sNumber, iOrgID, pTime, psbArg, sHead, &iShift))
        return false;      
    //由OrgID及levelID获取归属局号
    if (!isNumber(poTable->m_sValue, NULL, NULL))
         return false;
    iLevelID = atoi(poTable->m_sValue);
    OrgMgrEx *poOrgMgrEx = OrgMgrEx::GetInstance(); 
    if (!poOrgMgrEx->getAreaCode(iOrgID, pAreaCode, iSize, iLevelID))
        return false;              
    //校验向后匹配的号头
    int iPos = 0;
    int offset;
    const char *p1;
    const char *p2;
    CheckArgument *pntArg;   
    //判断是否有向后匹配的号头，若没有，返回true.
	pTmp = &(pArgument[poTable->m_iNextSibArgument]);
	if(pTmp->m_iNextSibArgument){
    	psbArg = &(pArgument[pTmp->m_iNextSibArgument]);
	}else{
		psbArg = NULL;
	}
	unsigned int iTmp2 = 0;
	if(psbArg->m_iNextSibArgument){
		iTmp2 = pArgument[psbArg->m_iNextSibArgument].m_iNextSibArgument; 
	}
	if(iTmp2){
		pTmp1 = &(pArgument[iTmp2]);
    }else{
		pTmp1 = NULL; 
	}
	if (psbArg == NULL || psbArg->m_iNextSibArgument==0
        || (pntArg=pTmp1)== NULL)
        return true;
    iPos = atoi(psbArg->m_sValue);    
    offset = iPos+strlen(sHead)+strlen(pAreaCode)-iShift;    
    while (pntArg!=NULL && pntArg!=pArgNull){
        if (pntArg->m_sValue[0]==0 
            || strlen(sNumber)<strlen(pntArg->m_sValue)+offset){
			if(pntArg->m_iNextArgument){
            	pntArg = &(pArgument[pntArg->m_iNextArgument]);
			}else{
				pntArg = NULL;
			}
            continue;
        }       
        p1 = &(sNumber[offset]);
        p2 = pntArg->m_sValue;
        while (*p2!='\0' && *p1!='\0' && *p1==*p2){
            p1++;
            p2++;
        }        
        if (*p2=='\0'){ //found it 
            printf("sHead:%s\n", pntArg->m_sValue);
            break;
        }        
		if(pntArg->m_iNextArgument){
        	pntArg = &(pArgument[pntArg->m_iNextArgument]);
		}else{
			pntArg = NULL;
		}
    }
    if (pntArg == NULL) //|| pntArg == pArgNull)
        return false;
    return true;  
}
#endif

bool CheckFunction::isBeyondDate(const char *pTime, const CheckArgument *poTable,char *sFileName)
{
    if (pTime==NULL || poTable==NULL )
        return false;
            
    if (poTable->m_sValue[0]!=0 && !isNumber(poTable->m_sValue, NULL, "DEC"))
        return false;
    
    Date oStartDate;    
    Date oRightDate;  //当前时间    
    oStartDate.parse(pTime, NULL);

    char sDiffType[5]={0};

    strcpy(sDiffType,poTable->m_poNextSibArgument->m_poNextSibArgument->m_sValue);
    sDiffType[1]=0;
    sDiffType[0] = toupper(sDiffType[0]);

    if (sDiffType[0] !='Y' && sDiffType[0] !='M' && sDiffType[0] !='D'
    	&& sDiffType[0] !='H' && sDiffType[0] !='I' && sDiffType[0] !='S' )
    	return false;
    
    //如果poTable的第二个参数不为空，且值不为空
    if (poTable->m_poNextSibArgument!=NULL 
        && poTable->m_poNextSibArgument->m_sValue[0]!=0){
	char sFileTime[15]={0};
	if (strncmp(poTable->m_poNextSibArgument->m_sValue,"$FILE_NAME$",11) == 0)
	{
		if (!getFileTime(poTable->m_poNextSibArgument->m_sValue,sFileTime,sFileName))
		return false;

		strcpy(poTable->m_poNextSibArgument->m_sValue,sFileTime);
	}
        
        //如果poTable的第二个参数与“YYYYMMDDHHMISS”标准格式不符
        if (!oRightDate.parse(poTable->m_poNextSibArgument->m_sValue,"YYYYMMDDHH24MISS"))
            return false;
            
        string s1 = string(oRightDate.toString()).substr(0, 8);
        string s2 = string(oRightDate.toString()).substr(8);
        if (oRightDate.checkDate(s1.c_str())==-1 || oRightDate.checkTime( s2.c_str())==-1)
            return false;
    }
    
    int iDiffMin = atoi(poTable->m_sValue);
    
    //oStartDate.addMin(iDiffMin);
    
    //printf("oStartDate:   %s\n", oStartDate.toString());
    //printf("oRightDate:   %s\n", oRightDate.toString());

//规整日期，如果按天则时分秒为0，如果为月则日为1号
    if (sDiffType[0] =='Y')
    	{
    		oStartDate.setMonth(1);
    		oStartDate.setDay(1);
    		oRightDate.setMonth(1);
    		oRightDate.setDay(1);    		
    	}
    if (sDiffType[0] =='M')
    	{
    		oStartDate.setDay(1);    	
    		oRightDate.setDay(1);    		
    	}
    if (sDiffType[0] =='D')
    	{
    		oStartDate.setHour(0);
    		oStartDate.setMin(0);
    		oStartDate.setSec(0);
    		oRightDate.setHour(0);
    		oRightDate.setMin(0);
    		oRightDate.setSec(0);
    	}    
    if (sDiffType[0] =='H')
    	{
    		oStartDate.setMin(0);
    		oStartDate.setSec(0);    	
    		oRightDate.setMin(0);
    		oRightDate.setSec(0);
    	}
    if (sDiffType[0] =='I')
    	{
    		oStartDate.setSec(0);    	
    		oRightDate.setSec(0);    	
    	}
    
    //向前越界
    if (iDiffMin<=0 )
    {
    		if (sDiffType[0] =='Y' )
    			{
				oRightDate.addYear(iDiffMin);
    				if (oStartDate< oRightDate)
    					return true;
    			}
			
    		if (sDiffType[0] =='M' )
    			{
				oRightDate.addMonth(iDiffMin);
    				if (oStartDate < oRightDate)
    					return true;
    			}
			
    		if (sDiffType[0] =='D' )
    			{
				oRightDate.addDay(iDiffMin);
    				if (oStartDate < oRightDate)
    					return true;
    			}
			
    		if (sDiffType[0] =='H' )
    			{
    				oRightDate.addHour(iDiffMin);
    				if (oStartDate < oRightDate)
    					return true;
    			}			
    		if (sDiffType[0] =='I' )
    			{
    				oRightDate.addMin(iDiffMin);
    				if (oStartDate < oRightDate)
    					return true;
    			}
			
    		if (sDiffType[0] =='S' )
    			{
    				oRightDate.addSec(iDiffMin);
    				if (oStartDate < oRightDate)
    					return true;   
    			}
			
    }      
    //向后越界
    else if (iDiffMin>0 )
	    	{
	    		if (sDiffType[0] =='Y' )
    			{
				oRightDate.addYear(iDiffMin);
    				if (oStartDate> oRightDate)
    					return true;
    			}
			
    		if (sDiffType[0] =='M' )
    			{
				oRightDate.addMonth(iDiffMin);
    				if (oStartDate > oRightDate)
    					return true;
    			}
			
    		if (sDiffType[0] =='D' )
    			{
				oRightDate.addDay(iDiffMin);
    				if (oStartDate > oRightDate)
    					return true;
    			}
			
    		if (sDiffType[0] =='H' )
    			{
    				oRightDate.addHour(iDiffMin);
    				if (oStartDate > oRightDate)
    					return true;
    			}			
    		if (sDiffType[0] =='I' )
    			{
    				oRightDate.addMin(iDiffMin);
    				if (oStartDate > oRightDate)
    					return true;
    			}
			
    		if (sDiffType[0] =='S' )
    			{
    				oRightDate.addSec(iDiffMin);
    				if (oStartDate >oRightDate)
    					return true;   
    			}
	    		
	    }      
    //未越界
     return false;
}

bool CheckFunction::getOrgID(const EventSection *poEventSection, int &iOrgID, 
       const CheckArgument *poTable, char *pHead)
{
    const char *sMSC = poEventSection->m_sMSC;
    const char *sLAC = poEventSection->m_sLAC;
    const char *sCellID = poEventSection->m_sCellID;
    
    if (sMSC==NULL)
        return false;
        
    int iPos = 0;
    char sHead[32];
    CheckArgument *psbArg = NULL;
    memset(sHead, 0, sizeof(sHead));
    
    //若参数表不为空
    if (poTable!=NULL){
        iPos = atoi(poTable->m_sValue);
     
        CheckArgument *psbArg = poTable->m_poNextSibArgument;
        
        //若参数表第二个号头参数不为空， 判断号头是否匹配
        if (psbArg!=NULL && !beginWith(sMSC, psbArg, sHead))
            return false;
            
        //处理可选参数pHead
        if (pHead != NULL && sHead[0]!=0)
            strcpy(pHead, sHead);
    }
    
   int iHeadLen = strlen(sHead);
   int iOffset = iPos + iHeadLen;
   
   if (iOffset>=strlen(sMSC))
        return false;
   
   //调用蒋涛的接口
   const char* szTime =0;
   return MobileRegionMgr::GetOrgID(&(sMSC[iOffset]),sLAC,sCellID,szTime,iOrgID);
   
}

bool CheckFunction::isMSC(const char *sMSC, const CheckArgument *poTable, 
    char *pHead)
{
    if (sMSC==NULL)
        return false;
        
    int iPos = 0;
    char sHead[32];
    CheckArgument *psbArg = NULL;
    memset(sHead, 0, sizeof(sHead));
    
    //若参数表不为空
    if (poTable!=NULL){
        iPos = atoi(poTable->m_sValue);
     
        CheckArgument *psbArg = poTable->m_poNextSibArgument;
        
        //若参数表第二个号头参数不为空， 判断号头是否匹配
        if (psbArg!=NULL && psbArg->m_sValue[0]!=0 && !beginWith(sMSC, poTable, sHead))
            return false;
            
        //处理可选参数pHead
        if (pHead != NULL && sHead[0]!=0)
            strcpy(pHead, sHead);
    }
    
   int iHeadLen = strlen(sHead);
   int iOffset = iPos + iHeadLen;
   
   if (iOffset>=strlen(sMSC))
        return false;
   
   //get switch ID
   //调用蒋涛的接口
   return MobileRegionMgr::MscHasExists(&(sMSC[iOffset]));
   
}

bool CheckFunction::isAreaCode(const char *sNumber, char *pAreaCode, int iSize, 
        const char *pTime, const CheckArgument *poTable)
{
    if (sNumber==NULL || sNumber[0]==0
        || poTable==NULL || poTable->m_sValue[0]==0)
        return false;
    
    int iPos = 0;
    int iOffset = 0; 
    int iOrgID = 0;
    char  sFlag[32];  
    char  sHead[32];
    char  sCode[50];
    
    memset(sFlag, 0, sizeof(sFlag));
    strcpy(sFlag, poTable->m_sValue);
    poTable = poTable->m_poNextSibArgument;
    
    //判断号头，如果有号头，需改变iOffset
    memset(sHead, 0, sizeof(sHead));
    if (poTable->m_poNextSibArgument!=NULL || poTable->m_poNextSibArgument->m_sValue[0]!=0){
        if (!beginWith(sNumber, poTable, sHead))
            return false;
                 
        iOffset = strlen(sHead);
        //get iPos
        iPos=atoi(poTable->m_sValue);       
     }
    
    const char *p = &(sNumber[iPos+iOffset]);
    memset(sCode, 0, sizeof(sCode));
    
    //自动给区号前加‘0’
    if (p[0]!='0'){
    		sCode[0]='0';
    		strncpy(&(sCode[1]), p, sizeof(sCode)-2 );
    	}
    else
    	strncpy(sCode,p,sizeof(sCode)-1);
    
    //test code
    //printf("sCode:%s", sCode);       
       
   if (!Head2OrgIDMgr::getOrgID(sCode, pTime, iOrgID, sFlag))
        return false;
 
   if (pAreaCode!=NULL)
        strncpy(pAreaCode, sCode, iSize-1);

    return true;    
}

bool CheckFunction::cmpTwoFunc(const EventSection *poEventSection, 
        const CheckArgument *poTable)
{
    if (poTable==NULL || poTable->m_sValue[0]==0)
        return false;
    
    const CheckArgument *psbArg = poTable;
    int iFuncID1, iFuncID2;
    int iTAG1, iTAG2;
    string sRet1, sRet2;
    
    iFuncID1 = atoi(psbArg->m_sValue);
    if (iFuncID1==CK_CMP_TWO_FUNC)
        return false;
    
    psbArg = psbArg->m_poNextSibArgument;
    if (psbArg==NULL || psbArg->m_sValue[0]==0)
        return false;
        
    iTAG1 = atoi(psbArg->m_sValue);    
    //执行第一个函数
    psbArg = psbArg->m_poNextSibArgument;
    doFunction(iFuncID1, poEventSection, iTAG1, psbArg, &sRet1);
    
    //执行第二个函数
    while (psbArg!=NULL){
        if (strcmp(psbArg->m_sValue, "NULL")==0)
            break;
        psbArg = psbArg->m_poNextSibArgument;
    }
    
    if (psbArg==NULL)
        return false;
    
    psbArg = psbArg->m_poNextSibArgument;
    if (psbArg==NULL || psbArg->m_sValue[0]==0)
        return false;
        
    iFuncID2 = atoi(psbArg->m_sValue);
    if (iFuncID1==CK_CMP_TWO_FUNC)
        return false;
        
    psbArg = psbArg->m_poNextSibArgument;
    if (psbArg==NULL || psbArg->m_sValue[0]==0)
        return false;
        
    iTAG2 = atoi(psbArg->m_sValue);
    psbArg = psbArg->m_poNextSibArgument;
    doFunction(iFuncID2, poEventSection, iTAG2, psbArg, &sRet2);
    
    //test code
    //printf("sRet1: %s\nsRet2: %s\n", sRet1.c_str(), sRet2.c_str());
    
    return sRet1==sRet2;
}

bool CheckFunction::formatCarrier(const EventSection *poEventSection, 
        int &iCarrierType, char *psResult)
{
    EventSection oEvent = *poEventSection;
    FormatMethod::formatCarrier(&oEvent);
    
    iCarrierType = oEvent.m_oEventExt.m_iCarrierTypeID;
    
    if (psResult!=NULL){
        strcpy(psResult, oEvent.m_sCalledNBR);        
    }
    
    if (iCarrierType==0)
        return false; 
    
    return true;
        
}

bool CheckFunction::pipe(const EventSection *poEventSection, 
        const CheckArgument *poTable, char *pResult)
{
    if (poTable==NULL || poTable->m_sValue[0]==0)
        return false;
    
    const CheckArgument *psbArg = poTable;
    int iFuncID1, iFuncID2;
    int iTAG1, iTAG2;
    string sPrev, sCurr;
    bool bRet = false;
    
    iFuncID1 = atoi(psbArg->m_sValue);
    if (iFuncID1==CK_PIPE)
        return false;
    
    psbArg = psbArg->m_poNextSibArgument;
    if (psbArg==NULL || psbArg->m_sValue[0]==0)
        return false;
        
    iTAG1 = atoi(psbArg->m_sValue);    
    //执行第一个函数
    psbArg = psbArg->m_poNextSibArgument;
    doFunction(iFuncID1, poEventSection, iTAG1, psbArg, &sPrev);
    
    //执行第后继函数
    while (1){
        while (psbArg!=NULL){
            if (strcmp(psbArg->m_sValue, "NULL")==0){
                psbArg = psbArg->m_poNextSibArgument;
                break;
            }
            psbArg = psbArg->m_poNextSibArgument;
        }
        
        if (psbArg==NULL)
            break;
            
        iFuncID2 = atoi(psbArg->m_sValue); 
        
        //支持嵌套调用CK_PIPE
        if (iFuncID2 == CK_PIPE)
            break;
               
        iTAG2 = -1;
        psbArg = psbArg->m_poNextSibArgument;
        
        bRet = doFunction(iFuncID2, poEventSection, iTAG2, psbArg, &sCurr, &sPrev);
        sPrev = sCurr;
    }
    
    if (pResult!=NULL)
            strcpy(pResult, sCurr.c_str());
    return bRet;
}

bool CheckFunction::formatAreaCode(const EventSection *poEventSection,
        char *psResult)
{
    EventSection oEvent = *poEventSection;
    int iCarrierType = oEvent.m_oEventExt.m_iCarrierTypeID;
    
    
    char sTmp[64];
    char sAccNBR[64];
    strcpy(sAccNBR, oEvent.m_sCalledNBR);
    
    //如果iCarrierType=0，则规整承载类型
    if (iCarrierType==0){
        formatCarrier(&oEvent, iCarrierType, sAccNBR);
    }
    
    //去除号头前的0086国家代码
    char toNBR[64];
    const char *p = sAccNBR;
    if (!format0086(p, toNBR))
        return false;

    //规整区号
    strcpy(oEvent.m_sCalledNBR, p);
    EventTypeMgrEx *poEvtTypeMgrEX = EventTypeMgrEx::GetInstance();
    poEvtTypeMgrEX->fillAreaCode(&oEvent);
    
    
    if (psResult!=NULL){
        strcpy(psResult, oEvent.m_sCalledNBR);        
    }
    
    return strcmp(p, oEvent.m_sCalledNBR)!=0;
}

bool CheckFunction::getPartnerID(const char *sAccNBR, const char *pTime, int &iPartnerID)
{

    if (sAccNBR==NULL)
        return false;
    
    //去除号头前可能存在的0086国家代码
    char toNBR[64];
    if (!format0086(sAccNBR, toNBR))
        return false;
    
    //规整号头    
    HeadEx oLocalHead;
    bool bRet = Head2OrgIDMgr::searchLocalHead(toNBR, pTime, &oLocalHead);
     
    //获取运营商号
    iPartnerID = oLocalHead.getPartnerID();
    
    return bRet;
}

bool CheckFunction::format0086(const char *fromNBR, char *toNBR, int *trimLen, int *piShift)
{

    if (fromNBR==NULL || toNBR==NULL)
        return false;
    
    const char *p = fromNBR;
    char sTmp[64];
    bool bHead86 = false;
    
    //当p字符串以为0086开头时，需分情况进行处理：
    if (strlen(p)>=7 && p[0]=='0' && p[1]=='0' && p[2]=='8' && p[3]=='6'){
        
        //008610为北京区号，00861[2-9]代表手机号
        if (p[4]=='1' && p[5]!='0')
            p = &(p[4]);
        
        //008610,或0086[2-9]，是非手机用户，转换为: 0+国内区号的格式。
        else if (p[4]!='0'){
            memset(sTmp, 0, sizeof(sTmp));
            sTmp[0]='0';
            strcpy(&(sTmp[1]), &(p[4]));
            p = sTmp;
            if (piShift!=NULL) *piShift = 1;
        }
        
        //0086+0+国内区号/手机号， 转换为: 0+国内区号/手机号的格式
        else
            p = &p[4];
            
        if (trimLen!=NULL) *trimLen = 4;
        bHead86 = true;
    }

    //对于013，015，018，13，15，18开头的不进行判断
    if(strncmp(p,"013",3) == 0 ||strncmp(p,"015",3) == 0 || strncmp(p,"018",3) == 0  )
    {
		p = &p[1];	
		if (bHead86)
    			*trimLen = 5;
		else
			*trimLen = 1;
    }
    
    strcpy(toNBR, p);
    
    return true;
}

bool CheckFunction::isInner(const EventSection *poEventSection, 
    const char *pTime)
{
    if (poEventSection==NULL)
        return false;
    
    int iTmp, iTmp2;
    
    //获取运营商ID
    if (!getPartnerID(poEventSection->m_sOrgCallingNBR, pTime, iTmp))
        return false;
        
    //规整被叫区号
    char sTmp[64];
    formatAreaCode(poEventSection, sTmp);
   
    if (!getPartnerID(sTmp, pTime, iTmp2))
        return false;
        
    return iTmp==iTmp2;
    
       
}

bool CheckFunction::getLongTypeAndRoamID(const EventSection *poEventSection, 
    int &iLongType, int &iRoamID)
{
    if (poEventSection==NULL)
        return false;    
    
    EventSection oEvtSect = *poEventSection;
    EventTypeMgrEx *poEvtTypeMgrEx = EventTypeMgrEx::GetInstance();
    poEvtTypeMgrEx->fillLongTypeAndRoamID(&oEvtSect);
       
    iLongType = oEvtSect.m_oEventExt.m_iLongTypeID;
    iRoamID   = oEvtSect.m_oEventExt.m_iRoamTypeID;
    
    return true;
    
}

bool CheckFunction::cmpCharofString(const char *str,const CheckArgument *poTable,char &cNotFind)
{
	string sHeadList;
	char sCmpStr[50]={0};
	int iPos = 0,i = 0;
	CheckArgument *p;
	if (poTable == NULL)
		return false;

	sHeadList.clear();
	iPos = atoi(poTable->m_sValue);
	if (iPos>=strlen(str))
		return false;
	
	p = poTable->m_poNextSibArgument;
	while (p)
	{
		if (strlen(p->m_sValue) > 1)
			return false;
		sHeadList.append(p->m_sValue);
		p = p->m_poNextArgument;
	}

	strcpy(sCmpStr,str+iPos);
	for (i = 0;i<strlen(sCmpStr);i++)
	{
		iPos = sHeadList.find(sCmpStr[i]);
		if (iPos == string::npos)
		{
			cNotFind = sCmpStr[i];
			return false;
		}
	}

	return true;
}

void CheckFunction::to_Lower(char * sStr)
{
	int i=0;	
	
	for (i = 0; i<strlen(sStr); i++)
		sStr[i] = tolower(sStr[i]);	
}

void CheckFunction::to_Upper(char * sStr)
{
	int i=0;	
	
	for (i = 0; i<strlen(sStr); i++)
		sStr[i] = toupper(sStr[i]);	
}
	
bool CheckFunction::cmpSubStr(const char *str,const CheckArgument *poTable)
{
	char sCmpStr[30]={0},sDesStr[30]={0};
	int iPos=0,iLen=0;
	CheckArgument *poArg;	

	if (poTable->m_poNextSibArgument == NULL || poTable->m_poNextSibArgument->m_poNextSibArgument== NULL)
		return false;

	iPos = atoi(poTable->m_sValue);
	if (poTable->m_poNextSibArgument->m_sValue[0] == 0)
	{
		if (iPos>=0)
			iLen = strlen(str+iPos);
		else
			iLen = 0-iPos;
	}
	else
		iLen = atoi(poTable->m_poNextSibArgument->m_sValue);

	if (iLen < 0 ) return false;

	poArg = poTable->m_poNextSibArgument->m_poNextSibArgument;
	strcpy(sDesStr,poArg->m_sValue);

	if (iPos>=0)
	{
		if ((iPos+iLen) > strlen(str))
			return false;
		strncpy(sCmpStr,str+iPos,iLen);
	}
	else
	{
		if ((iPos+iLen) > 0 || (strlen(str)+iPos) <0 )
			return false;

		strncpy(sCmpStr,str+strlen(str)+iPos,iLen);
	}

	to_Upper(sCmpStr);
	to_Upper(sDesStr);
	if (strcmp(sCmpStr,sDesStr) == 0 )
		return true;
	return false;
}

bool CheckFunction::check_zone(const char * str, const CheckArgument * poTable,int iMaxFlag)
{
	static ZoneMgr * zone = 0;
        if (!zone) {
        	zone = new ZoneMgr ();
        }

	char sZoneValue[MAX_STRING_VALUE_LEN]={0};
	strcpy(sZoneValue,str);
	to_Upper(sZoneValue);

	const char *sZoneID=poTable->m_sValue;
	const char *sZoneItemID = poTable->m_poNextSibArgument->m_sValue;
	if (!isNumber(sZoneID, NULL, "DEC"))
		return false;
	if (!isNumber(sZoneItemID, NULL, "DEC"))
		return false;

	int iZoneID = atoi(sZoneID);
	int iZoneItemID = atoi(sZoneItemID);

	if (iMaxFlag == 0)
	{
		if (zone->check (iZoneID, sZoneValue,iZoneItemID)) 
		{
                 	return true;   
        	} 
	}
	else
	{
		if (zone->checkMax(iZoneID,sZoneValue,iZoneItemID))
		{
			return true;
		}
	}
	
	return false;
}

bool CheckFunction::getFileTime(char *sTag,char *sFileTime,char *sFileName)
{
	char sLow[3]={0},sLen[10]={0};
	Date td;
	//$FILE_NAME$
	char *sFlag=strrchr(sTag,'$')+1;
	char *sSep=strrchr(sTag,',');
	if (!sSep)
		return false;

	strncpy(sLow,sFlag,sSep-sFlag);
	strcpy(sLen,sSep+1);
	if (!isNumber(sLow,NULL) || !isNumber(sLen,NULL))
		return false;

	int iLow=atoi(sLow);
	int iLen = atoi(sLen);
	if (iLow<0)
	{
		iLow=strlen(sFileName)+iLow;
		if (iLow<0)
			iLow=0;
	}
	else
	{
		if (iLow && iLow>strlen(sFileName))
			return false;
	}

	if (iLen>14)
		iLen=14;

	strncpy(sFileTime,sFileName+iLow,iLen);
	if (strlen(sFileTime)<14)
	{
		char sSuffix[15]={0};
		strncpy(sSuffix,"00000000000000",14-strlen(sFileTime));
		strcat(sFileTime,sSuffix);
	}
	sFileTime[14]=0;

	if (!isNumber(sFileTime,NULL))
		return false;

	if (!td.parse(sFileTime))
		return false;
		
	
}


