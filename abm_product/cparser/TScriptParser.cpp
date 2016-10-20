/*VER: 1*/ 
/************************************************
  Copyright by Wholewise  2002
  File Name: TScriptParser.cpp
  Created Date: 2002.08.01
  Author: xiaozc
  Version: 0.0.01
  Create Version: 0.0.01
  Last Version Date: 2002.08.01
  Create Version Date: 2002.08.01
 ***********************************************/

#include "wwInterpreter.h"
#include "TScriptParser.h"
#include <stdio.h>

typedef TSymbol TValuePair;
 
TValuePair  ScriptParser::Param(char * name)
{
	Symbol sym;

	if ( ! ( wwParser::GetInputParam(name, sym))){
		throw TException("ScriptParser::Param: 指定的参数名不存在");
	};
	
	return sym;
};
 
TValuePair * & ScriptParser::Result(char * name)
{
	return  wwParser::GetOutput(name);
}; 

TValuePair * ScriptParser::Result(int index, char * name)
{
	return  wwParser::GetOutput(index, name);
}; 


//wwParser::yylex实现
int ScriptParser::yylex()
{
	return wwLex::yylex(&yylval,&yylloc);
}

//wwParser::yyerror实现
void ScriptParser::yyerror(char *m)
{ 

	cout<<m<<"line:"<<cur_line<<"column:"<<cur_column<<endl;
	return;
}

//脚本源代码输入
void ScriptParser::SetSource(char * buf, int size)
{
	wwLex::SetBuf(buf,size);		
}

//解析编译脚本
void ScriptParser::Parse()
{
	if ( yyparse()){
	    char sErrMsg[200];
	    sprintf(sErrMsg,"语法错误,行:%d,列:%d",cur_line,cur_column);
		throw TException(sErrMsg);
	}
}

//执行脚本
void ScriptParser::Run()
{
	wwParser::run();
}

/* end of add */

//输入单个字符串型值
void ScriptParser::SimpleInput(char * value)
{
	wwParser::SetInputData(symSTRING, value);
}

//输入单个数值型值
void ScriptParser::SimpleInput(double value)
{
	wwParser::SetInputData(symDOUBLE, value);
}

//输入多个字符串型值
void ScriptParser::MultiInput(char * value)
{
	wwParser::SetInputData(symSTRING_ARRAY, value);
}

//输入多个数值型值
void ScriptParser::MultiInput(double value)
{
	wwParser::SetInputData(symDOUBLE_ARRAY, value);
}

//数据清除(含编译的生成的指令)
void ScriptParser::Clear()
{
	wwParser::Clear();
}
               
//清除执行结果(不含指令)
void ScriptParser::ClearResult()
{
	wwParser::ClearResult();
}

//打印指令流	
void ScriptParser::PrintCommandFlow()
{
	wwParser::print();
}
