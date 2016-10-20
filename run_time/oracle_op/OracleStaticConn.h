/**
* (C) Copyright 2007, linkage
* @version 	v1.0
* @author 	chenli
* @brief  otl连接环境管理类,提供了获取数据库连接,提交事务,回滚事务,
					断开连接等操作
*
* history:
* <p>chenli 2007-04-3 1.0 build this moudle</p>
*/
#ifndef _ORACLE_STATIC_CONN_H_
#define _ORACLE_STATIC_CONN_H_

#include <sstream>

#include "TOCIQuery.h"

using namespace std;

#define DEFINE_OCICMD(x) 		TOCIQuery x(OracleStaticConn::getInstance()->getOCIDatabase());
#define DEFINE_OCICMDPOINT(x) 		x = new TOCIQuery (OracleStaticConn::getInstance()->getOCIDatabase());
class OracleStaticConn
{
  public:
  
  static OracleStaticConn * getInstance();
  /*********************************************************
	* 函数说明：连接数据库,如果指定了sConnectstr串,就用sConnectstr
							连接数据库;否则使用user,pass,tnsconnstr属性拼筹成
							user/pass@connstr格式的连接数据库
	* 参数说明：[in]sConnectstr		user/pass@connstr格式的连接串
	* 返回值  ：true成功,false失败
	* 作  者  ：ChenLi
	* 编写时间：2007-07-18
	**********************************************************/
 	bool connectDb(const string &user="", const string &pass="", const string &tnsStr="");
 	/*********************************************************
	* 函数说明：返回OCI数据库连接应用
	* 参数说明：空
	* 返回值  ：otl_connect引用
	* 作  者  ：ChenLi
	* 编写时间：2007-07-18
	**********************************************************/
  TOCIDatabase* getOCIDatabase();
  
  /*********************************************************
	* 函数说明：设置数据库用户名
	* 参数说明：[in]user数据库用户名
	* 返回值  ：空
	* 作  者  ：ChenLi
	* 编写时间：2007-07-18
	**********************************************************/
  void setUser(const string& user);
  /*********************************************************
	* 函数说明：设置数据库密码
	* 参数说明：[in]pass	数据库密码
	* 返回值  ：空
	* 作  者  ：ChenLi
	* 编写时间：2007-07-18
	**********************************************************/
  void setPass(const string& pass);
  /*********************************************************
	* 函数说明：设置数据库tns连接串
	* 参数说明：[in]connStr	tns连接串
	* 返回值  ：空
	* 作  者  ：ChenLi
	* 编写时间：2007-07-18
	**********************************************************/
  void setConnStr(const string& connStr);
  	
  /*********************************************************
	* 函数说明：提交事务
	* 参数说明：空
	* 返回值  ：空
	* 作  者  ：ChenLi
	* 编写时间：2007-07-18
	**********************************************************/
  void commit();
  /*********************************************************
	* 函数说明：关闭事务自动提交
	* 参数说明：空
	* 返回值  ：空
	* 作  者  ：ChenLi
	* 编写时间：2007-07-18
	**********************************************************/
  void auto_commit_off();
  /*********************************************************
	* 函数说明：打开事务自动提交
	* 参数说明：空
	* 返回值  ：空
	* 作  者  ：ChenLi
	* 编写时间：2007-07-18
	**********************************************************/
  void auto_commit_on();
  /*********************************************************
	* 函数说明：回滚事务
	* 参数说明：空
	* 返回值  ：空
	* 作  者  ：ChenLi
	* 编写时间：2007-07-18
	**********************************************************/
	void rollback();
	/*********************************************************
	* 函数说明：断开数据库连接
	* 参数说明：空
	* 返回值  ：空
	* 作  者  ：ChenLi
	* 编写时间：2007-07-18
	**********************************************************/
	void disconnect();
	/*********************************************************
	* 函数说明：判断是否连接到数据库
	* 参数说明：空
	* 返回值  ：空
	* 作  者  ：ChenLi
	* 编写时间：2007-07-18
	**********************************************************/
	bool isConnected();
	
private:
	~OracleStaticConn()
	{
		oci_database.disconnect();
	}
  OracleStaticConn()
  {
  	Connected = false;
  	//otl_connect::otl_initialize();
  }//阻止外部OracleStaticConn例化
  
  //otl数据库连接对象
 	TOCIDatabase	oci_database;
	bool Connected;
	
 	string	m_userName;//用户名
	string	m_passWord;//密码
	string	m_tnsStr;//tns连接串
  //static 	string	sConnStr;//连接串格式user/pwd@tnsStr
  	
  stringstream stream_last_error;//用流纪录最近的错误信息
  //static	string  last_error;//最近错误信息 
};//全局静态类

#endif
