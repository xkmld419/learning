/*VER: 1*/
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.
///testlru 
////LRU落地文件限制文件大小， 分文件的策略；
////重启系统的话，怎么恢复 做checkpoint， 做内存的影像
////提供一个函数从文件中恢复，类初始化时，
////如果是创建: 如果文件存在,且可恢复，则从文件恢复数据，否则则将其改名，直接创建空的
////如果是恢复: 如果文件存在,且大小合适，则从文件中恢复上次的数据，如果文件出问题，或不存在，则恢复失败
////zhangap


///#define DEBUG_LRU


#include <stdio.h>
#include <time.h>
#include "SHMLruData.h"
#include "LruAggrData.h"
#include "CommandCom.h"
#include "ReadIni.h"

void lruPrintData(LruAggrData &t )
{
    printf("lServID:%d\n", t.lServID);
    printf("lAcctID:%ld\n", t.lAcctID);
    printf("lCustID:%ld\n", t.lCustID);
    printf("lProductID:%ld\n", t.lProductID);
    printf("lOfferID:%d\n", t.lOfferID);
    printf("sArea:%s\n", t.sArea);
    printf("sExpDate:%s\n", t.sExpDate);
    printf("sEffDate:%s\n", t.sEffDate);
}

void lruParseData(char * p, LruAggrData &t )
{
            memset(&t, 0, sizeof(LruAggrData) );

            if(p){ 
                t.lServID = atol(p);
            }
            
            if(p){ 
                p = strstr(p+1,"," );
                if(p) t.lAcctID = atol(p+1);
            }
            if(p){
                 p = strstr(p+1,"," );
                if(p) t.lProductID = atol(p+1);
            }
            if(p){
                 p = strstr(p+1,"," );
                if(p) t.lOfferID = atol(p+1);
            }
            if(p){
                p = strstr(p+1,"," );
                if(p) strncpy (t.sArea, p+1, sizeof(t.sArea) );
            }
            if(p){
                p = strstr(p+1,"," );
                if(p) strncpy (t.sExpDate, p+1, sizeof(t.sExpDate) );
            }
            if(p){
                p = strstr(p+1,"," );
                if(p) strncpy (t.sEffDate, p+1, sizeof(t.sEffDate) );
            }
    
}

void printHelp()
{
printf("%s",
    "使用参数说明:\n"
    "-c	创建内存，若存在落地文件，则从中恢复内存，否则创建落地文件\n"
    "-d	删除共享内存，且trunc落地文件\n"
    "-q	删除共享内存，保留落地文件\n"
    "-l	显示目前的内存状态信息\n"
    "-l offset	显示offset所在的内容\n"
    "-l offset1,offset2	显示从offset1到offset2中所有的内存内容\n"
    "-p offset	显示offset是在内存还是在磁盘，在则打印内容，不在也不调入内存\n"
    "-p offset1,offset2	显示从offset1到offset2是否在内存，在则显示，不在也不调入\n"
    "-v offset	删除offset所在的内容\n"
    "-v offset,offset2	删除从offset1到offset2\n"
    "-i x,x,x,x,x	插入一条数据到内存\n"
    "-u offset x,x,x,x,x	将指定offset的数据更新\n"
    "-a count	由程序自己生成count个数据插入到内存，如果不指定count，默认插入一个数据\n"
    "-x 	动态扩展内存大小\n"
    "-w 0|1|2|3  设置3种写方式 0:自动控制,  1:定时写,  2:定量写,  3:实时写\n\n"
    );
}

    
int main( int argc, char **argv )
{
    ReadIni oRead;
    char * pPath = getenv("BILLDIR");
    char sTestLruFile[256]={0};
    char sFileName[512]={0};
    sprintf(sFileName, "%s/etc/abmconfig", pPath);
    long lTestLruKey =  oRead.readIniLong(sFileName, "MEMORY", "testlru.ipc_key_value", 1000);
    unsigned int iExCnt = oRead.readIniInteger(sFileName, "MEMORY", "testlru.max_extents", 4);
    unsigned int iNextSize = oRead.readIniInteger(sFileName, "MEMORY", "testlru.ext_data_size",1024*1024*10);
    oRead.readIniString(sFileName,"MEMORY","testlru.ext_file_name",sTestLruFile,NULL);
    //CommandCom* pCmdCom = new CommandCom();
      
    SHMLruData<LruAggrData> *  pLruAggr 
      = new SHMLruData<LruAggrData>(lTestLruKey, 
        sTestLruFile , 1,
        iNextSize, iExCnt) ;

    if( argc<=1 || argv[1][0]!='-'){
        printHelp();
        return 0;
    }

try{
    if( strcmp(&argv[1][1], "commit" )==0 ){
        if(!pLruAggr->exist() ){printf("内存不存在。"); return 0;}
        pLruAggr->commit();
        printf("提交内存。\n");
        return 0;
    }

    
    if( strcmp(&argv[1][1], "rollback" )==0 ){
        if(!pLruAggr->exist() ){printf("内存不存在。"); return 0;}
        pLruAggr->rollback();
        printf("回滚内存。\n");
        return 0;
    }
    
    
    switch(argv[1][1]){
        case 'c':
        {
            if(! pLruAggr->exist() ) {
            	//long lInitSize = pCmdCom->readIniInteger("MEMORY","testlru.init_data_size",1000);
               long lInitSize= oRead.readIniLong(sFileName, "MEMORY","testlru.init_data_size",1000);
                pLruAggr->create(lInitSize);
                if(! pLruAggr->exist() ) {
                    printf("创建共享内存失败。\n");
                    return 0;
                }
                printf("创建共享内存成功,数据容量1000条。\n");
            }else{
                printf("内存已经存在，无需创建。\n");
            }
            return 0;
        }
        break;

        case 'd':
        {
            if( pLruAggr->exist() ){
                printf("内存删除。\n");
                pLruAggr->remove();
        
                printf("LRU落地文件清空。\n");
                pLruAggr->truncFile();
            }else{
                printf("内存本来就不存在。\n");
            }
            return 0;
        }
        break;
        case 'q':
        {
            if(!pLruAggr->exist() ){printf("内存不存在。"); return 0;}
            printf("内存删除。\n");
            pLruAggr->remove();
            return 0;
        }
        break;
        case 'l':    
        case 'p': 
        {
            if(!pLruAggr->exist() ){printf("内存不存在。"); return 0;}
            if(argc == 2 ){
                pLruAggr->showDetail();

                
                return 0;
            }
            unsigned int iOffset1= atoi( &argv[2][0] );
            unsigned int iOffset2=  iOffset1;
            char * p = strstr( &argv[2][0],"," );
            if(p) 
                iOffset2= atoi( p+1);
            LruAggrData t;
            memset(&t, 0, sizeof(LruAggrData) );
            for(unsigned int i=iOffset1; i<=iOffset2 ; i++ ){
                try{
                    if('l' == argv[1][1]){
                        t = pLruAggr->getForRead( i );
                        printf("偏移量%u处值为：\n", i);
                        lruPrintData(t);
                    }else{
                        if( !pLruAggr->getForMemRead( i, t ) ){
                            printf("偏移量%u处值不在内存。。\n", i);
                        }else{
                            printf("偏移量%u处在内存，值为：\n", i);
                            lruPrintData(t);
                        }
                        
                    }

                }catch(Exception &e){
                    printf("%s\n" , e.descript() );
                }

            }
            
        }
        break;
   

        case 'v':    
        {
            if(!pLruAggr->exist() ){printf("内存不存在。"); return 0;}

            unsigned int iOffset1= atoi(&argv[2][0] );
            unsigned int iOffset2=  iOffset1;
            char * p = strstr(&argv[2][0],"," );
            if(p) 
                iOffset2= atoi( p+1);

            for(unsigned int i=iOffset1; i<=iOffset2 ; i++ ){
                try{
                    pLruAggr->revoke( i );
                    printf("删除 %u\n", i);
                }catch( Exception &e ){
                    printf("%s\n" , e.descript() );
                }

            }
            
        }
        break;
        case 'i':    
        {
            if(!pLruAggr->exist() ){printf("内存不存在。"); return 0;}
            if(argc < 3){
                printf("请输入插入数据。\n");
                return 0;
            }
            LruAggrData t;
            lruParseData(&argv[2][0], t);
            unsigned int uiOffset = pLruAggr->malloc( t );
            printf("插入成功，偏移量为：%u,请稍候手工提交\n", uiOffset);
            return 0;

        }
        break;
        case 'u':    
        {
            if(!pLruAggr->exist() ){printf("内存不存在。"); return 0;}
            if(argc < 4){
                printf("请输入偏移量和更新数据。\n");
                return 0;
            }
            LruAggrData t;
            lruParseData(&argv[3][0], t);
            unsigned int uiOffset = atoi( &argv[2][0] ) ;
            (*pLruAggr)[uiOffset ] = t;
            
            printf("更新成功，请稍候手工提交\n", uiOffset);
            return 0;
        }
        break;
        case 'a':    
        {
            if(!pLruAggr->exist() ){printf("内存不存在。"); return 0;}
            int j=1;
            if(argc >= 3){
                j = atoi(&argv[2][0] );
                if(j<=0){
                    j =1;
                }
            }
                
            
            unsigned int     uiOffset = 0;
            LruAggrData t;
            memset(&t, 0, sizeof(LruAggrData) );

            strcpy(t.sArea,"025");
            strcpy(t.sExpDate,"20100101000000");
            strcpy(t.sEffDate,"20501231595959");
            for(int i=1; i<=j; i++) {
                t.lServID=10000000+i;
                t.lAcctID=20000000+i;
                t.lCustID=30000000+i;
                t.lProductID=10000+i%1000;
                t.lOfferID=20000+i%1000;;
                uiOffset = pLruAggr->malloc( );
                if(!uiOffset){
                    printf("malloc faile  \n");
                    return 0;
                }
                (*pLruAggr)[uiOffset] = t;
                pLruAggr->commit();
            }
            
            if( j==1) printf("插入数据1条,偏移量为%u \n", uiOffset);
            else printf("插入数据%d条.\n",j );
            return 0;
        }
        break;
        case 'x':    
        {
            if(!pLruAggr->exist() ){printf("内存不存在。"); return 0;}
            unsigned int iexpand = pLruAggr->expandMem( );
            printf("扩展内存成功，新增%u个结构体空间\n", iexpand);
        }
        break;

        case 'w':    
        {
            if(!pLruAggr->exist() ){printf("内存不存在。"); return 0;}
            unsigned int j=0;
            
            if(argc >= 3)
                j = atoi(&argv[2][0] );
            else{
                printf("请输入写方式\n");
                return 0;
            }
            unsigned int old = 0;
            long ltime=0;
            pLruAggr->getState( old, ltime);
            if(  pLruAggr->setState( j) ){
                printf("上次写方式为%u, 现置成%u \n", old, j);
            }else{
                printf("更新写方式失败。");
            }
        }
                
        break;
        
        default : 
            printHelp();
            break;
       }
       
}catch( ...){
    printf("异常退出\n" );
}

    return 0;
}
