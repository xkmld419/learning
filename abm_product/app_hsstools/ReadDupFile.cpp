/*VER: 1*/ 
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <dirent.h>
#include <sstream>
#include "TOCIQuery.h"  
#include "Environment.h"


using namespace std;
//const string DATAPATH = "/bill/dupdata";

struct TicketFileStruct
{
    char sCallingNbr[41];
    char sStartTime[15];
    int  iDuration;
    
    char sCalledNbr[41];
    int  iExchID;

    long  lEventID;
    char  sMixStr[80];
	char   sSeqKey[DUP_CHK_MAX_SEQKEY_LEN];
    char  cNewLine;
};

int ReadFile(const char *filename, const char *tofilename)
{
    FILE * fp = NULL;
    FILE *wfp = NULL;
    struct TicketFileStruct TicketRead;
    if ( (fp = fopen( filename,"r")) == NULL)
    {
        
        printf("打开读文件失败,请检查文件是否存在\n");
        exit(0);
    }
    
    if( (wfp = fopen(tofilename,"w+")) == NULL)
    {
        printf("打开写文件失败!\n");
        exit(0);
    }
    fprintf(wfp,"\t\t\t\t ================文件查询结果====================\n");
    int begin = 0;
    rewind(fp);
    long whence = 0;
    while(1)
    {
        if(fseek(fp,begin*sizeof( struct TicketFileStruct),SEEK_SET)<0)
        {
            break;
        }
        if(fread(&TicketRead,sizeof(struct TicketFileStruct),1,fp)<0)
        {
            break;
        }
        
        if(feof(fp)!=0)
        {
            break;
        }    
        
        fprintf(wfp,"\t\t\t\t *\t\t主叫号码： %s\n",TicketRead.sCallingNbr);
        fprintf(wfp,"\t\t\t\t *\t\t开始时间： %s\n",TicketRead.sStartTime); 
        fprintf(wfp,"\t\t\t\t *\t\t时长    ： %d\n",TicketRead.iDuration);  
     
        fprintf(wfp,"\t\t\t\t *\t\t被叫号码： %s\n",TicketRead.sCalledNbr); 
        fprintf(wfp,"\t\t\t\t *\t\t交换机  ： %d\n",TicketRead.iExchID);    
     
        fprintf(wfp,"\t\t\t\t *\t\t事件号  :  %ld\n",TicketRead.lEventID);  
        fprintf(wfp,"\t\t\t\t *\t\t混合域  ： %s\n",TicketRead.sMixStr);    
		fprintf(wfp,"\t\t\t\t *\t\t序列排重字段  ： %s\n",TicketRead.sSeqKey);    
        fprintf(wfp,"\t\t\t\t ================================================\n");
        
        begin++;
    }   
    
    printf("已经处理了记录数=%ld\n",begin);
    
    fclose(fp);
    fclose(wfp);
    return 1;
}

void printUsag()
{
    printf("\t\t\t\t====================重单文件检查程序====================\n");
    printf("\t\t\t\t*                                                      *\n");
    printf("\t\t\t\t*  使用方法：                                          *\n");
    printf("\t\t\t\t*  (1).readdupfile -l 进程名 列出重单文件              *\n");
    printf("\t\t\t\t*  (2).readdupfile 进程名 源格式文件名 目标格式文件名  *\n");
    printf("\t\t\t\t*   For example:                                       *\n");
    printf("\t\t\t\t*   readdupfile -l 1103                                *\n");
    printf("\t\t\t\t*   readdupfile  1103 20070612023000.chk  example.txt  *\n");
    printf("\t\t\t\t========================================================\n");
}

int CheckFilePath(const char *filepath)
{
    DIR * dp;
    ///struct dirent *dirp;

    if ( (  dp=opendir(filepath) )  ==NULL )
    {
        printf("你要检查的重单文件目录不存在，请先创建!\n");
        return 0;
    }

    if(dp) 
        closedir(dp);
    return 1;
}

/*
//去除前后的空格
std::string trim(std::string& s,const std::string& drop = " ")  
{                                                             
    std::string r=s.erase(s.find_last_not_of(drop)+1);           
    return r.erase(0,r.find_first_not_of(drop));                 
}                                                             
*/ 
  
string SearchFileName(const char *processID)
{
    char checkDataPath[100];
    //ostringstream ocheckdatapath;  
    //string   checkDataPath;
    DEFINE_QUERY(qry);
    qry.close();
    qry.setSQL("select trim(check_data_path) from b_checkdup_filelist "
               "where process_id=:processID");
    qry.setParameter("processID",processID);
    qry.open(); 
    qry.next();
    strncpy(checkDataPath , qry.field(0).asString(), 100);
   
    return checkDataPath;

}


int main(int argc, char *argv[])
{
    if(argc == 3)
    {
        if((strncmp("-l",argv[1] , 2) ==0) )
		{
			string datapath;
			datapath = SearchFileName(argv[2]);
			if(CheckFilePath(datapath.c_str())) //目录存在
			{
			    datapath = "ls -l "+datapath;
			    system(datapath.c_str());
			}
			else
			{
			    return 0;
			}
		}
		else
		{
		    printUsag();
		}
		
    }
    else if(argc == 4)
    {
        char *processid = argv[1];
        char *filename = argv[2];
        char *tofilename = argv[3];
        
        ostringstream oSstream;
        string datapath;
	    datapath = SearchFileName(processid);
            
        oSstream<<datapath;
        cout<<"查询路径："<<oSstream.str()<<endl;
        oSstream<<"/"<<filename;
        ReadFile(oSstream.str().c_str(),tofilename);
      
    }
    else
    {
        printUsag();
    }

    return 0;
}

