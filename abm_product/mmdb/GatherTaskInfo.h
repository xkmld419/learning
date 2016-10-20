 /*VER: 3*/
// Copyright (c) 20091214,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef GATHERTASKINFO_H_HEADER_INCLUDED_BDAE6AE7
#define GATHERTASKINFO_H_HEADER_INCLUDED_BDAE6AE7


class GatherTaskInfo
{
    public:
		    GatherTaskInfo();
            ~GatherTaskInfo();
            
			int  m_iOrgID;
            int  m_iTaskID;
    	    
		    char m_sMinCreateDate[16];
		    char m_sMaxCreateDate[16];	
            
            char m_sDescPath[500];
            int  m_iCheckInterVal;
            char m_sComments[50];
            char m_sTaskType[10];
            int  m_iProcessID;
            
            //采集是否子目录及文件  0:是,默认  1:文件 2  子目录  
    	    int m_iGatherMode; 
			int m_iBegin; //时间开始匹配的偏移 
    	    int m_iEnd;   //时间结束匹配的偏移 
            
			//## 默认的交换机ID
    		int m_iDefaultSwitchID;
			//## 当前 exch_id
    	    int m_iSwitchID;
    	    //文件名通配符
    	    char m_sFileNameWildcard[50];  //file_name_mask
    	    //目录名通配符
    	    char m_sPathNameWildcard[50];  //path_name_mask
			///文件名通配符备份
    		char m_sFileNameWcbak[50] ;
			//##增加修改采集文件名
     		char m_sModifyFileName[50] ;
    	    //采集目录路径
    	    //Directory m_oSrcDirectory;
    	    unsigned int m_iSrcDirectory;//增加一个下标
			unsigned int m_iDesDirectory;
			//增加远程备份目录
			char m_sBackupPath[500];
			
			//临时放置
			char m_sTaskName[50];        	
    	    int  m_iFileNum;
			
    	    //FTP相关
    	    char m_sSrcPath[500];
			char m_sOrgSrcPath[500]; ////原始的路径串，可能带日期变量 
    	    int  m_iHostID;
    	    int  m_iPasv;
			int m_iListMode;
    	    char m_sIP[50];
    	    int  m_iPort;
    	    char m_sUserName[50];
    	    char m_sPassword[50];
    
            //## 源文件创建时间
            char m_sFileDate[15];
    	    	    
    	    char m_sSubPath[500];
    	    
    	    int m_iCheckInterval;
            int m_iTaskNum;
            
            int  m_iQcSend;
            char m_sQcPath[200]; 
            char m_sQcOrgPath[200]; ////原始路径配置信息
            int  m_iQcHostID;

            char m_sQcIP[50];
            char m_sQcUserName[50];
            char m_sQcPassword[50];
            int  m_iQcPort;
			int  m_iProcessID2;
            int  m_iBusiClass;
			int  m_iDataSource;
            unsigned int m_iNext;//下一个
};

class GatherTaskListInfo
{
    public:
		    GatherTaskListInfo();
            ~GatherTaskListInfo();
            
			int  m_iTaskID;
			int  m_iMinInterval;
			long  m_iOrgStr;
			int  m_iCheckIntervalStr;
            unsigned int m_iNext;//下一个
};

#endif