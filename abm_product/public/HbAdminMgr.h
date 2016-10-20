#ifndef __HB_ADMIN__MGR_H__
#define __HB_ADMIN__MGR_H__

class HbAdminMgr
{	
	public:
		
		/*构造函数*/
		HbAdminMgr();
		
		/*析构函数*/
		virtual ~HbAdminMgr();
		
		/*获取登录信息*/
		static  int  getLogInfo(int argc, char **argv);
		
		/*从配置文件获取信息*/
		static int  getConfInfo(const char * sIniHeader);
		
		/*获取登录成功，失败信息*/
		static	int getAuthenticate();
		
		/*字符串切割*/
		static	int  CutString(const char * sInputUserInfo);
		
		/*剔除解密后的空格*/
		static int tickOutTab (char *ptempWd, const char *sPwd);
		
		/*登录的初始化*/
		static int  loginInit(const int ihasLog);
		
		/*登录成功，失败状态的变更*/
		static  int authenticateInit(const int ihasSucess);
		
		/*把登录信息和数据信息验证*/
		static int doJude();
		
	private:
		
		/*登入的用户*/
		static char m_sLogUserName[20];
		
		/*登入的密码*/
		static char m_sLogPassWord[20];
		
		/*配置文件用户名*/
		static char m_sUserName[20];
		
		/*配置文件密码*/
		static char  m_sPassWord[20];
		
		/*配置文件目录*/
		static char m_sTIBS_HOME[20];
};
#endif