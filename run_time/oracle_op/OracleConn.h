/**
* (C) Copyright 2009, linkage
* @version 	v1.0
* @author 	
* @brief  otl连接环境管理类,提供了获取数据库连接,提交事务,回滚事务,
					断开连接等操作
*
*/
#ifndef _ORACLE_CONN_H_
#define _ORACLE_CONN_H_

#include <sstream>

#include "TOCIQuery.h"

using namespace std;

#define DEFINE_OCICMDBYCONN(x,y) 		TOCIQuery x(y.getOCIDatabase());
#define DEFINE_OCICMDPOINTBYCONN(x,y) 		x = new TOCIQuery (y.getOCIDatabase());
class OracleConn
{
  public:
  OracleConn()
  {
  	Connected = false;
  }
	~OracleConn()
	{
		oci_database.disconnect();
	}
  
  /*********************************************************
	* 函数说明：连接数据库,如果指定了sConnectstr串,就用sConnectstr
							连接数据库;否则使用user,pass,tnsconnstr属性拼筹成
							user/pass@connstr格式的连接数据库
	* 参数说明：[in]sConnectstr		user/pass@connstr格式的连接串
	* 返回值  ：true成功,false失败
	**********************************************************/
 	bool connectDb(const string &user="", const string &pass="", const string &tnsStr="");
 	/*********************************************************
	* 函数说明：返回OCI数据库连接应用
	* 参数说明：空
	* 返回值  ：otl_connect引用
	**********************************************************/
  TOCIDatabase* getOCIDatabase();
  
  /*********************************************************
	* 函数说明：设置数据库用户名
	* 参数说明：[in]user数据库用户名
	* 返回值  ：空
	**********************************************************/
  void setUser(const string& user);
  /*********************************************************
	* 函数说明：设置数据库密码
	* 参数说明：[in]pass	数据库密码
	* 返回值  ：空
	**********************************************************/
  void setPass(const string& pass);
  /*********************************************************
	* 函数说明：设置数据库tns连接串
	* 参数说明：[in]connStr	tns连接串
	* 返回值  ：空
	**********************************************************/
  void setConnStr(const string& connStr);
  	
  /*********************************************************
	* 函数说明：提交事务
	* 参数说明：空
	* 返回值  ：空
	**********************************************************/
  void commit();
  /*********************************************************
	* 函数说明：关闭事务自动提交
	* 参数说明：空
	* 返回值  ：空
	**********************************************************/
  void auto_commit_off();
  /*********************************************************
	* 函数说明：打开事务自动提交
	* 参数说明：空
	* 返回值  ：空
	**********************************************************/
  void auto_commit_on();
  /*********************************************************
	* 函数说明：回滚事务
	* 参数说明：空
	* 返回值  ：空
	**********************************************************/
	void rollback();
	/*********************************************************
	* 函数说明：断开数据库连接
	* 参数说明：空
	* 返回值  ：空
	**********************************************************/
	void disconnect();
	/*********************************************************
	* 函数说明：判断是否连接到数据库
	* 参数说明：空
	* 返回值  ：空
	**********************************************************/
	bool isConnected();
	
private:
  
  //otl数据库连接对象
 	TOCIDatabase	oci_database;
	bool Connected;
	
 	string	m_userName;//用户名
	string	m_passWord;//密码
	string	m_tnsStr;//tns连接串
  	
  stringstream stream_last_error;//用流纪录最近的错误信息
};

#endif
