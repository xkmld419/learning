#include <string.h>
#include <stdio.h>

#include "AutoClearABMTT.h"
#include "AutoClearABMTT.h"
//#include "ThreeLogGroup.h"
//#include "Log.h"
#include "ABMException.h"


void PrintHelpInfo(){
    printf("使用方法：\n");
    printf("======================= 数据导出工具:abmclear version1.0.0 =======================\n"
           "abmclear -m -d -k\n"
           "  -m 内存数据库中表名 \n"
           "  -d 指定删除时间 \n"
           "  -k 指定具体内存数据库名称 \n"
           "示例：\n"
           "  abmclear -m tablename1,tablename2 -d 20110908 -k tt_n01,tt_n02\n"
           "解释：\n"
           "  同时将tt_n01和tt_n02内存数据库的表tablename1和表tablename2中20110908之前的数据删除掉！\n");
}

int main(int argc, char *argv[])
{
    int oc=0;
    char sTableListName[1024+1];
    char sDBListName[1024+1]; 
    ABMException oExp; 
    ABMClearProduct *poABMclear=new ABMClearProduct();
       
    if(3 >= argc) {
        PrintHelpInfo();
        return -1;
    }
    
    memset(sTableListName, 0x00, sizeof(sTableListName));
    memset(sDBListName, 0x00, sizeof(sDBListName));
    memset(poABMclear->m_sDateTime, 0x00, sizeof(poABMclear->m_sDateTime));
    
    char *paraOpt = "m:d:M:D:K:k:";
    while((oc=getopt(argc, argv, paraOpt)) != -1) {
        switch (oc) {
            case 'm':
                if(1024 < strlen(optarg)){
                    printf("数据库表名集合大于1024\n");
                    exit(1);    
                }
                strcpy(sTableListName, optarg);
                sTableListName[strlen(optarg)] = '\0';
                break;
            case 'd':
                if(14 < strlen(optarg)){
                    printf("时间格式不对\n");
                    printf("eg:-d 20110908112233\n");
                }
                strcpy(poABMclear->m_sDateTime, optarg);
                poABMclear->m_sDateTime[14] = '\0';
                break;
            case 'M':
                if(1024 < strlen(optarg)){
                    printf("数据库表名集合大于1024\n");
                    exit(1);    
                }
                strcpy(sTableListName, optarg);
                sTableListName[strlen(sTableListName)] = '\0';
                break;
            case 'D':
                if(14 < strlen(optarg)){
                    printf("时间格式不对\n");
                    printf("eg:-d 20110908112233\n");
                }
                strcpy(poABMclear->m_sDateTime, optarg);
                poABMclear->m_sDateTime[14] = '\0';                
                break;
            case 'k':
                if(1024 < strlen(optarg)){
                    printf("数据库名集合大于1024\n");
                    exit(1);    
                }
                strcpy(sDBListName, optarg);
                sDBListName[strlen(optarg)] = '\0';
                break;
                //g_iClearNodeID = atoi(optarg);
                //break;
            default:
                printf("程序发现未知参数 %c\n", oc);
                break;            
        }
    }
    poABMclear->clearInit(oExp);
    char sTableName[30+1];
    char sDBName[30+1];
    char *p = sTableListName;
    char *q = NULL;
    char *s = NULL;
    while(p) {
        s = sDBListName;
        q = p;
        p = strchr(p, ',');
        if (NULL != p) {
			memset(sTableName, 0x00, 31);
			memcpy(sTableName, q, p - q);
		    while(s){
		        q = s;
		        s = strchr(s, ','); 
		        if (NULL != s) {
			        memset(sDBName, 0x00, 31);
			        memcpy(sDBName, q, s - q);
			        poABMclear->getABMClearConf(oExp,sTableName,sDBName);
		        } else {
			        memset(sDBName, 0x00, 31);
			        strcpy(sDBName, q);
			        poABMclear->getABMClearConf(oExp,sTableName,sDBName);
			        break;
                }		    
		        s++;  
		    }
		} else {
			memset(sTableName, 0x00, 31);
			strcpy(sTableName, q);
		    while(s){
		        q = s;
		        s = strchr(s, ','); 
		        if (NULL != s) {
			        memset(sDBName, 0x00, 31);
			        memcpy(sDBName, q, s - q);
			        poABMclear->getABMClearConf(oExp,sTableName,sDBName);
		        } else {
			        memset(sDBName, 0x00, 31);
			        strcpy(sDBName, q);
			        poABMclear->getABMClearConf(oExp,sTableName,sDBName);
			        break;
                }		    
		        s++;  
		    }
			break;
        }   
        p++;
    }
    
    poABMclear->executeClear(oExp);
    printf("内存数据清理成功...\n");    
    return 0;    
}