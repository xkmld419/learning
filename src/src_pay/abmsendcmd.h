#ifndef __ABMSENDCMD_H_INCLUDED_
#define __ABMSENDCMD_H_INCLUDED_

#include "abmobject.h"
#include "abmcmd.h"

//#define UTF8_MAX  32

//#define TEXT_MAX  256
//typedef unsigned int ui32;
//typedef unsigned long time_s;

#define ABMCMD_QRY_SRV_PSWD  8   //二次认证密码下发
/*
struct PasswordResponsStruct
{
   char m_ReqID[UTF8_MAX];
    char  m_ReqTime[UTF8_MAX];
	//int m_iAlign; 
};

//3.4.2.5 二次认证密码下发
struct QryPasswordCond
{
	char m_sDestAct[UTF8_MAX];
	int m_DestType;
	int m_DestAttr;
	char m_SevPlatID[UTF8_MAX];
	char m_RanAccNbr[UTF8_MAX];
	char m_OrdID[UTF8_MAX];
};

struct QryPasswordData
{
	ui32 m_SevResCode;
	char ResID[UTF8_MAX];
    char ResTime[UTF8_MAX];
};

class QryPasswordCCR : public ABMTpl<__DFT_T1_, PasswordResponsStruct, QryPasswordCond, ABMCMD_QRY_SRV_PSWD>
{
public:
    void clear() {
        return ABMObject::clear();
    }
    
    bool addpasswdRespons(PasswordResponsStruct &data) {
    return __add2(data);
	}
	
    bool addServPassWd(QryPasswordCond &data) {
      return __add3(data);    
   }
  
     int getpasswdRespons(vector<PasswordResponsStruct*> &vec) {
      return __getValue2(vec);     
   }
  //   int  getpasswdCond(vector<QryPasswordCond*> &vec){
  //   	return __getValue2(vec);	
  //  }
   
     int getServPassWd(PasswordResponsStruct *pPtr, vector<QryPasswordCond*> &vec) {
      return __getValue3(pPtr, vec);    
   }
};

class QryPasswordCCA :public ABMTpl<QryPasswordData, __DFT_T2_, __DFT_T3_, ABMCMD_QRY_SRV_PSWD>
{
public:
    void clear() {
        return ABMObject::clear();    
    } 
    
    bool addServPassWdData(QryPasswordData &data) {
        return __add1(data);    
    }
    
    int getServPassWdData(vector<QryPasswordData *> &vec) {
        return __getValue1(vec);    
    }
};   
*/

#endif/*__ABMSENDCMD_H_INCLUDED_*/
