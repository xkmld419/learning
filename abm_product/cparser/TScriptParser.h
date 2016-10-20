/*VER: 1*/ 
/************************************************
  Copyright by Wholewise  2002
  File Name: TScriptParser.h
  Created Date: 2002.08.01
  Author: xiaozc
  Version: 0.0.01
  Create Version: 0.0.01
  Last Version Date: 2002.08.01
  Create Version Date: 2002.08.01
 ***********************************************/

#ifndef _TSCRIPTPARSER_H_
#define _TSCRIPTPARSER_H_
#include <string.h>
#include "wwInterpreter.h"
#include "wwLex.h"
#include "wwParser.h"
#include "exception.h"

//脚本解释器类定义
class ScriptParser :public wwLex, public wwParser
{
private:
	
public:
	ScriptParser(){input_len =0;};
	~ScriptParser(){

		Clear();
	};
	virtual int yylex();
	virtual void yyerror(char *m);
	//virtual double SysinputHandler(const char *){};
	//virtual char * SysinputHandlerS(const char *){};
	
	void SetSource(char * buf,int size);        //脚本源代码输入
	void Parse();                               //解析编译脚本
	void Run();                                 //执行脚本
	
	typedef  TSymbol TValuePair;
	
	 //输入函数所需的参数
	TValuePair   Param(char * name);
	 
	//输出结果
	TValuePair * & Result(char * name); 
	TValuePair * Result(int index, char * name); 
	
	void SimpleInput(char * value); //输入单个字符串型值
	void SimpleInput(double value); //输入单个数值型值

	void MultiInput(char * value);  //输入多个字符串型值
	void MultiInput(double value);  //输入多个数值型值
	

	void Clear();                   //数据清除(含编译的生成的指令)
	void ClearResult();             //清除执行结果(不含指令)
		
	void PrintCommandFlow();        //打印指令流
	
};

#endif
