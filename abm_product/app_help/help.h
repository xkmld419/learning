#include <string.h>
#include <vector>
using namespace std;

class version
{
public :
	string m_sVersion;        //版本
	string m_sEnvironment;    //环境
};

class subcommand
{
public :
	string m_sSubCode;        //命令编码
	string m_sFunction;       //命令功能
	string m_sFormat;         //命令格式
	string m_sDescription;    //命令描述
};

class command
{
public :
	string m_sCode;                        //命令编码
	vector <subcommand *> m_pSubcommand;   //子命令
};

class attributes
{
public :
	string m_sRule;   //生效规则
	string m_sRange;  //取值范围
  string m_sValue;  //建议取值
};

class param
{
public :
	string m_sCode;   //参数编码
	string m_sName;   //参数名称
	string m_sDescription;  	  //参数描述
	attributes m_attributes;		//参数属性
};

class help
{
public :
	version  m_versionHelp;
	vector <command *> m_pCommandHelp;
	vector <param *>   m_pParamHelp;
};
