#include <string.h>
#include <stdio.h>

#include "AutoClearABMTT.h"
#include "AutoClearABMTT.h"
//#include "ThreeLogGroup.h"
//#include "Log.h"
#include "ABMException.h"


void PrintHelpInfo(){
    printf("使用方法：\n");
    printf("======================= 数据导出工具:abmarchive version1.0.0 =======================\n"
           "abmarchive -m -d -k -f \n"
           "  -m 内存数据库中归档表名 \n"
           "  -d 指定归档和删除时间 \n"
           "  -k 指定归档具体内存数据库名称 \n"
           "  -f 指定归档文件存放的位置\n "
           "示例：\n"
           "  abmclear -m tablename -d 20110908 -k tt_node -f /root/abm/deploy/bin \n"
           "解释：\n"
           "  同时将tt_node内存数据库的表tablename中20110908之前的数据归档到目录/root/abm/deploy/bin下面,\n"
           "  并删除掉内存数据库中表tablename已经归档的数据！\n");
}

int main(int argc, char *argv[])
{
    int oc=0;
    char sTableListName[1024+1];
    char sDBListName[1024+1]; 
    char sDBFileName[1024+1];
    ABMException oExp; 
    ABMClearProduct *poABMArchive=new ABMClearProduct();
       
    if(3 >= argc) {
        PrintHelpInfo();
        return -1;
    }
    
    memset(sTableListName, 0x00, sizeof(sTableListName));
    memset(sDBListName, 0x00, sizeof(sDBListName));
    
    char *paraOpt = "m:d:M:D:K:k:f:";
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
                strcpy(poABMArchive->m_sDateTime, optarg);
                poABMArchive->m_sDateTime[14] = '\0';
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
                strcpy(poABMArchive->m_sDateTime, optarg);
                poABMArchive->m_sDateTime[14] = '\0';                
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
            case 'f':
                strcpy(sDBFileName, optarg);
                sDBListName[strlen(optarg)] = '\0';
                break;
            default:
                printf("程序发现未知参数 %c\n", oc);
                break;            
        }
    }
    if(0 == strcspn(sDBFileName,"/")){
	    //m_poLogGroup->log(MBC_PARAM_THREAD_LOST_ERROR,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"输出文件为全路径");
        //Log::log(0, "输出文件为全路径!");
	} else {
	    //m_poLogGroup->log(MBC_PARAM_THREAD_LOST_ERROR,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"输出文件路径按照配置路径输出");
	    //Log::log(0, "输出文件路径按照配置路径输出!");
	} 
	poABMArchive->m_poArchiveData=fopen(sDBFileName,"w+"); 
	if(poABMArchive->m_poArchiveData == NULL) {
		//m_poLogGroup->log(MBC_FILE_OPEN_FAIL,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"打开文件失败[%s]",m_sFileName);
		return -1;
	}    
    poABMArchive->clearInit(oExp);
    poABMArchive->executeArchiveSQL(oExp, sTableListName, sDBListName);
    poABMArchive->executeArchive(oExp, sDBListName);
    poABMArchive->getABMClearConf(oExp, sTableListName, sDBListName);
    poABMArchive->executeClear(oExp);
    return 0;    
}