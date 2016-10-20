#include "ThreeLogGroup.h"
#include "MBC_ABM.h"
#include <unistd.h>
#include "Date.h"



int main(int argc,char *argv[])
{
	
	Date d;
	ThreeLogGroup Log;
	
	//每秒插入一条，可用于测试log.interval
  if(strcmp(argv[1],"-seconds") == 0)
  {
  	int i = 1;
  	while (1) {
  		Log.log(MBC_FILE_COPY_FAIL,-1,501,LOG_LEVEL_FATAL,LOG_TYPE_BUSI,
					"recv","Fatal:%d",i);
			d.getCurDate();
			cout << d.toString() << ": 插入第" << i << "条数据" << endl;
			i++;
			sleep(1);
		}
  }
  
  if(strcmp(argv[1],"-level") == 0)
  {
 	  Log.log(MBC_FILE_COPY_FAIL,-1,501,LOG_LEVEL_FATAL,LOG_TYPE_BUSI,
				"recv","Fatal:%s","LOG_LEVEL_FATAL");
				
	  Log.log(MBC_FILE_COPY_FAIL,-1,501,LOG_LEVEL_ERROR,LOG_TYPE_PARAM ,
					"recv","Error:%s","LOG_LEVEL_ERROR");
  	Log.log(MBC_FILE_COPY_FAIL,-1,501,LOG_LEVEL_WARNING,LOG_TYPE_SYSTEM,
				"recv","Waring:%s","LOG_LEVEL_WARNING");
 	  Log.log(MBC_FILE_COPY_FAIL,-1,502,LOG_LEVEL_INFO,LOG_TYPE_PROC ,
				"recv","Info:%s","LOG_LEVEL_INFO");
   	Log.log(MBC_FILE_COPY_FAIL,-1,502,LOG_LEVEL_DEBUG,LOG_TYPE_DATEDB,
				"recv","Debug:%s","LOG_LEVEL_DEBUG"); 
		
		cout << "各个level的日志各请求插入一条, 是否插入内存则依赖于level" << endl;
		return 0;
  }
  
  
  if(strcmp(argv[1],"-times") == 0)
  {
  	int times = atoi(argv[2]);
  	
  	for(int i=1;i<=times;i++) 
  	{
   		Log.log(MBC_FILE_COPY_FAIL,-1,501,LOG_LEVEL_FATAL,LOG_TYPE_BUSI,
					"recv","Fatal:times %d",i);
  	}
  	cout << "一次性插入" << times << "条日志数据" << endl;
  	
  	return 0;
  }
  
  return 0;	
}



