#ifndef _PARAMACCESSOR_H
#define _PARAMACCESSOR_H
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "Date.h"
#include "Log.h"

#ifndef LINEMAXCHARS
#define LINEMAXCHARS	150
#endif
using namespace std;
//参数访问器
//通过ParamAccessor::GetInstance()得到参数访问接口,ParamAccessor->GetParamValue()取得参数的具体值
//ParamAccessor->ModifyParamValue()修改配置文件sys.ini的参数值。如果是定期规则，同时更新newValue、effDate。
class ParamAccessor
{
  public:
    ~ParamAccessor();

    static ParamAccessor* GetInstance(char *sFileName);
   
    bool GetParamValue(char* strParamName, char* oValue,int iMaxLen=150,int iMode=0);//获得参数的value值
    bool ModifyParamValue(char* strParamName, char* oModifyValue,char *oModifyDate=0,int iMode=0);//修改参数的value值
		
		    void BeginTrans();
    void rollback();
    void commit();
    char m_sFileName[150];
    char m_sTmpFileName[150];
    char m_sBakFileName[150];
  private:
    ParamAccessor(char *sFileName);

    bool Init();

    static ParamAccessor* m_gpParamAccessor;
    
    char *TruncStr( char *sp , char *sDst , int len);
    char * trim( char *sp );
    
    bool GetFieldValue(const char* strParamName,const char* field,char *oValue,int iMaxLen=150);//获得参数某字段的值
    bool ModifyFieldValue(char *strParamName,char * field,char * oModifyValue);//修改参数某字段的值
    

};



#endif
