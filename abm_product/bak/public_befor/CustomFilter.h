/*VER: 1*/ 
#ifndef CUSTOM_FILTER_HEADER
#define CUSTOM_FILTER_HEADER

#include "StdEvent.h"
#include "KeyTree.h"


class CustomCheckResult {
  public:


  CustomCheckResult(int iEventTypeID, char * sEffDate, char * sExpDate);
  CustomCheckResult();


	int  m_iEventTypeID;
	char m_sEffDate[16];
	char m_sExpDate[16];	

};



class CustomCheck {
  public:
	int  check( );
	void insert( );
	void del( );    
	void setResult(CustomCheckResult* i_pResult);
	
      CustomCheck();

        CustomCheckResult* m_pResult;

  
  private:
    
	KeyTree <CustomCheck *> m_oCustomCheck;
	


	
};



class CustomFilter{
  public:
	void load();
        void unload();
	int  filter(StdEvent * pEvent);


  private:
	CustomCheck m_oCustomCheck;
        int  m_iNotFilterEventType;   /////如果没有匹配, 是不是填入一个事件类型.
		
};



#endif
