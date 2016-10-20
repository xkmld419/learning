#pragma once
#ifndef _SHMPARAMCFG_H_HEAD_
#define _SHMPARAMCFG_H_HEAD_
#include "ThreeLogGroup.h"
#include "CommandCom.h"
#include "ReadIni.h"

class SHMParamCfg
{
public:
/*	static int getValue(const char *_sTab,const char *_sValue) {
		CommandCom _pCommandComm;
		_pCommandComm.InitClient();
		return _pCommandComm.readIniInteger(_sTab,_sValue,-22222222);
	}
	static char *getString(const char *_sTab,const char *_sValue,char *defstr) {
		CommandCom _pCommandComm;
		_pCommandComm.InitClient();
		return _pCommandComm.readIniString(_sTab,_sValue,defstr,NULL);
	}
*/
	static bool setValue(const char *_sValue) {
		CommandCom _pCommandComm;
		_pCommandComm.InitClient();
		return _pCommandComm.changeSectionKey(_sValue);
	}

	static int getValue( char const *_sTab, char const *_sValue,int _iValue = -22222222) {
		ReadIni reader;
		char sFileName[1024] = {0};
		char *penv = getenv("ABM_PRO_DIR");

		if (!penv) {
			return -1;
		}

		if (penv[strlen(penv)-1] != '/') {
			snprintf (sFileName, sizeof(sFileName)-1, "%s/etc/abmconfig", penv);
		} else {
			snprintf (sFileName, sizeof(sFileName)-1, "%setc/abmconfig", penv);
		}

		_iValue = reader.readIniInteger(sFileName,_sTab,_sValue,_iValue);

		if (_iValue == -22222222) {
			ThreeLogGroup _oLogObj;
			//_oLogObj.log(MBC_PARAM_THREAD_LOST_ERROR,-1,-1,-1,2,2,-1,NULL,"%s.%s核心参数缺少配置",_sTab,_sValue);
			_oLogObj.log(MBC_PARAM_THREAD_LOST_ERROR,-1,2,2,-1,NULL,"%s.%s核心参数缺少配置",_sTab,_sValue);
			THROW(MBC_PARAM_THREAD_LOST_ERROR);
		} else if (_iValue < 0) {
			ThreeLogGroup _oLogObj;
			//_oLogObj.log(MBC_PARAM_THREAD_VALUE_ERROR,-1,-1,-1,2,2,-1,NULL,"%s.%s核心参数配置错误",_sTab);
			_oLogObj.log(MBC_PARAM_THREAD_VALUE_ERROR,-1,2,2,-1,NULL,"%s.%s核心参数配置错误",_sTab);
			THROW(MBC_PARAM_THREAD_VALUE_ERROR);
		}
		return _iValue;
	}

	static char *getString( char const *_sTab, char const *_sValue,char *defstr) {
		ReadIni reader;
		char sFileName[1024] = {0};
		char sIniHeader[32] = {0};
		char *penv = getenv("BILLDIR");

		if (!penv) {
			return NULL;
		}

		if (penv[strlen(penv)-1] != '/') {
			snprintf (sFileName, sizeof(sFileName)-1, "%s/etc/sgwconfig", penv);
		} else {
			snprintf (sFileName, sizeof(sFileName)-1, "%setc/sgwconfig", penv);
		}
		return reader.readIniString (sFileName, _sTab,_sValue,defstr,NULL);
	}

};

#endif
