

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <sys/timeb.h>

#include "LogV2.h"
#include "OracleConn.h"

using namespace std;

#define TOTALPAREACODE     31     //31个省
/*   */
typedef struct{
    char longCode[4+1];
    int  ccrCount;
    int  errCount;
    
}typeAllMsg;
typeAllMsg allMsg[TOTALPAREACODE];

/*   */
typedef struct{
    char longCode[4+1];
    char sessionId[48+1];
    char errCode[16+1];
    char resultCode[32+1];
    
}typeErrMsgDetail;
typeErrMsgDetail errMsgDetail[204800];


/* 本省长途区号与业务区对应关系  */
typedef struct{
    char provName[8+1];
    char provCode[4+1];
}typeProvCode;


//typeDetailMsg detailMsg[205600];

const typeProvCode gAreaCode[TOTALPAREACODE]={
"北京","010",
"广东","020",
"上海","021",
"天津","022",
"重庆","023",
"辽宁","024",
"江苏","025",
"湖北","027",
"四川","028",
"陕西","029",
"河北","311",
"山西","351",
"河南","371",
"吉林","431",
"黑龙江","451",
"内蒙古","471",
"山东","531",
"安徽","551",
"浙江","571",
"福建","591",
"湖南","731",
"广西","771",
"江西","791",
"贵州","851",
"云南","871",
"西藏","891",
"海南","898",
"甘肃","931",
"宁夏","951",
"青海","971",
"新疆","991"
};

/* 取系统时间，格式为YYYYMMDDHHMISS */
char * getSysDate(char * sysDate)
{
time_t sec;
struct tm t;

sec = time(NULL);
localtime_r(&sec,&t);
sprintf(sysDate,"%04d%02d%02d%02d%02d%02d",\
	t.tm_year+1900,t.tm_mon+1,t.tm_mday,\
	t.tm_hour,t.tm_min,t.tm_sec);
sysDate[14]='\0';
return sysDate;
}

/* 取前一天的系统时间，格式为YYYYMMDDHHMISS */
char * getYestoday(char * yestoDay)
{
time_t sec;
struct tm t;

sec = time(NULL);
sec = sec - 3600 * 24;
localtime_r(&sec,&t);
sprintf(yestoDay,"%04d%02d%02d%02d%02d%02d",\
	t.tm_year+1900,t.tm_mon+1,t.tm_mday,\
	t.tm_hour,t.tm_min,t.tm_sec);
yestoDay[14]='\0';
return yestoDay;
}

int getAllmsg(char * statisDate1, char * statisDate2,TOCIQuery *pQryCursor)
{
	int iRet=0;
	string sql;
	try{
		
		pQryCursor->close();
		
		
		
		sql.clear();
			
			sql="select z.orgid,z.ccrmsg,x.errmsg from "                                                   ;
			sql+="("                                                                                                 ;
			    sql+=" select b.org_id orgid, count(*) errmsg from abm_result a, abm_origin_host b, abm_result c"    ;
			    sql+=" where a.origin_host = b.origin_host"                                                         ;
			    sql+=" and c.session_id like 'ABM@%'"                                                         ;
			    sql+=" and a.session_id like 'ABM@%'"                                                         ;
			    sql+=" and a.session_id = c.session_id"                                                             ;
			    sql+=" and a.info_flag is null "                                                                     ;
			    sql+=" and c.info_flag = 'R'"                                                                         ;
			    sql+=" and a.RESULT_DATE > to_date(:p0,'yyyymmddhh24miss')"                                        ;
			    sql+=" and c.RESULT_DATE > to_date(:p0,'yyyymmddhh24miss')"                                        ;
			    sql+=" and a.RESULT_DATE <=to_date(:p1,'yyyymmddhh24miss')"                                        ;
			    sql+=" and c.RESULT_DATE <=to_date(:p1,'yyyymmddhh24miss')"                                        ;
			    sql+=" group by  b.org_id"                                                                           ;
			sql+=" )"                                                                                                ;
			 sql+=" x,"                                                                                              ;
			sql+=" ("                                                                                                ;
			    sql+=" select b.org_id orgid,count(*) ccrmsg"                                                        ;
			    sql+=" from abm_origin_host_tmp b, abm_result c"                                                     ;
			    sql+=" where c.session_id like 'ABM@%' "                                                    ;
			    sql+=" and substr(c.DEST_REALM,0,3) = b.ORG_ID"                                                    ;
			    sql+=" and c.info_flag = 'R' "                                                                       ;
			    sql+=" and c.RESULT_DATE > to_date(:p0,'yyyymmddhh24miss')"                                        ;
			    sql+=" and c.RESULT_DATE <=to_date(:p1,'yyyymmddhh24miss')"                                        ;
			    sql+=" group by  b.org_id"                                                                           ;
			sql+=" ) z"                                                                                              ;
			sql+=" where z.orgid=x.orgid(+) "                                                  ;
			sql+=" order by z.orgid"                                                                                 ;                                                                              ;
			
		pQryCursor->setSQL(sql.c_str());
		printf("|汇总数据查询脚本：%s|\n",sql.c_str());
	    pQryCursor->setParameter("p0", statisDate1);
	    pQryCursor->setParameter("p1", statisDate2);
	
	    	memset(&allMsg,0,sizeof(allMsg));
	    	
	    int i=0;
	    pQryCursor->open();
	    printf("|提取汇总数据内容|\n");
	    while (pQryCursor->next()) 
	    {
	    	
	        strcpy(allMsg[i].longCode, pQryCursor->field(0).asString());
	        allMsg[i].ccrCount = pQryCursor->field(1).asLong();
	        allMsg[i].errCount = pQryCursor->field(2).asLong();
	        
	        i++;
	         
	        printf("%d|%s|%d|%d\n", i,pQryCursor->field(0).asString(),pQryCursor->field(1).asLong(),pQryCursor->field(2).asLong());
	    }
	   
	 
	    //pQryCursor->execute();
	    pQryCursor->close();
	    
	}
	catch (TOCIException &oe) {
        printf("UpdateDetail:[ORACLE EXCEPTION]: code[%d]=%s \n", oe.getErrCode(),
            oe.GetErrMsg());
    }
	 printf("|汇总数据统计结束|\n");
	return iRet;
}

int getErrMsgDetail(char * statisDate1, char * statisDate2,TOCIQuery *pQryCursor2,int * num)
{
	int iRet=0;
	string sql;
	try{
		
		pQryCursor2->close();
		
		sql.clear();
			
			sql=" select b.org_id ,a.SESSION_ID , a.service_result_code ,a.RESULT_DATE "           ;
			sql+=" from abm_result a, abm_origin_host b,abm_result c  "                            ;                                                                ;
			    sql+=" where a.origin_host = b.origin_host"    							           ;
			    sql+="  and a.session_id=c.session_id"                                             ;         
			    sql+="  and a.info_flag is null "                                                  ;        
			    sql+="  and c.info_flag='R'   "                                                    ;              
			    sql+="  and a.service_result_code<>0  "                                            ;                         
			    sql+="  and a.RESULT_DATE > to_date(:p0,'yyyymmddhh24miss')"          ;                     
			    sql+=" and a.RESULT_DATE <= to_date(:p1,'yyyymmddhh24miss') "         ;    
			    
		pQryCursor2->setSQL(sql.c_str());
		printf("|错误明细查询脚本：%s|\n",sql.c_str());
	    pQryCursor2->setParameter("p0", statisDate1);
	    pQryCursor2->setParameter("p1", statisDate2);
	
	    memset(&errMsgDetail,0,sizeof(errMsgDetail));
	    	
	    int  i=0;
	    pQryCursor2->open();
	    printf("|提取错误明细数据内容|\n");
	    while (pQryCursor2->next()) 
	    {
	    	
	        strcpy(errMsgDetail[i].longCode,  pQryCursor2->field(0).asString());
	        strcpy(errMsgDetail[i].sessionId ,pQryCursor2->field(1).asString());
	        strcpy(errMsgDetail[i].errCode ,  pQryCursor2->field(2).asString());
	        strcpy(errMsgDetail[i].resultCode,pQryCursor2->field(3).asString());
	        
	        i++;
	        
	       printf("%d|%s|%s|%s|%s\n", i,pQryCursor2->field(0).asString(),pQryCursor2->field(1).asString(),pQryCursor2->field(2).asString(),pQryCursor2->field(3).asString());
	    }
	    *num=i;
	  printf("|11错误消息数：%d|\n",*num);
	    pQryCursor2->close();
    
	}
	catch (TOCIException &oe) {
        printf("UpdateDetail:[ORACLE EXCEPTION]: code[%d]=%s \n", oe.getErrCode(),
            oe.GetErrMsg());
    }
	 printf("|错误明细统计结束|\n");
	return iRet;
}

int main(int argc,char* argv[])
{
	char statisDate1[14+1];
	char statisDate2[14+1];
	char yestoday[14+1];
	char sysdate[14+1];
	int ret;
	FILE * fp;
	FILE * fp1;
	char sumFileName[128+1];
	char detailFileName[128+1];
	char tmpChar[512+1];
	
	__DEBUG_LOG0_(0, "[余额划拨规则]:查询SEQ失败!");
	string sORACLEName = "ABM";
    string sORACLEuser = "abm1";
    string sORACLEpwd = "abm1";
	
	OracleConn *pOracleConn = NULL;
    TOCIQuery *pQryCursor = NULL;  
    TOCIQuery *pQryCursor2 = NULL;
    pOracleConn = new OracleConn;        
    if (!pOracleConn->connectDb(sORACLEuser, sORACLEpwd, sORACLEName)) {
		printf("ORACLE CONNECT ERROR");
		return  - 1;
    }
         
	DEFINE_OCICMDPOINTBYCONN(pQryCursor, (*pOracleConn));
    if (pQryCursor == NULL) {
    	printf("pQryCursor ERROR");
		return  - 1;
	}
	
	DEFINE_OCICMDPOINTBYCONN(pQryCursor2, (*pOracleConn));
    if (pQryCursor2 == NULL) {
    	printf("pQryCursor2 ERROR");
		return  - 1;
	}
	
    memset(statisDate1,0,sizeof(statisDate1));
    memset(statisDate2,0,sizeof(statisDate2));
    memset(yestoday,0,sizeof(yestoday));
    memset(sysdate,0,sizeof(sysdate));
    
    getSysDate(sysdate);
    getYestoday(yestoday);
    
    sprintf(statisDate1,"%-8.8s000000",yestoday);
    sprintf(statisDate2,"%-8.8s235959",yestoday);
    
    printf("|今天统计时间区间:%s---%s|\n",statisDate1,statisDate2);

	printf("|开始提取每天汇总数据，日期:%-8.8s|",statisDate1);
	
    ret=getAllmsg(statisDate1,statisDate2,pQryCursor);
    
    memset(sumFileName,0,sizeof(sumFileName));
    sprintf(sumFileName,"/ABM/app/deploy/monitorfile/input/BOSSNM.50-30-70-00-00-001.3070.%-8.8s.0001.0.1000",sysdate);

    fp=fopen(sumFileName,"w");
    
    for(int i=0;i<TOTALPAREACODE;i++)
    {
    	memset(tmpChar,0,sizeof(tmpChar));
    	sprintf(tmpChar,"0|%s|%d|%d|%-8.8s\n",allMsg[i].longCode,allMsg[i].ccrCount,allMsg[i].errCount,yestoday);
    	fputs(tmpChar,fp); 
    }
    
    fclose(fp);

    printf("|开始提取每天错误明细数据，日期:%-8.8s|",statisDate1);
    int num=0;
    ret=getErrMsgDetail(statisDate1,statisDate2,pQryCursor2,&num);
    
    memset(detailFileName,0,sizeof(detailFileName));
    sprintf(detailFileName,"/ABM/app/deploy/monitorfile/input/BOSSNM.50-30-70-00-00-002.3070.%-8.8s.0001.0.1000",sysdate);
    fp1=fopen(detailFileName,"w");
    
    printf("|22错误消息数：%d|\n",num);
    
    for(int i=0;i<num;i++)
    {
    	memset(tmpChar,0,sizeof(tmpChar));
    	sprintf(tmpChar,"0|%s|%s|%s|%s\n",errMsgDetail[i].longCode,errMsgDetail[i].sessionId,errMsgDetail[i].errCode,sysdate);
    	fputs(tmpChar,fp1); 
    	
    }
    
    fclose(fp1);
    
    exit(0);
}
