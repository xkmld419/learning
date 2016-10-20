/*VER: 1*/ 
/************************************************
  Copyright by Wholewise  2002
  File Name: wwInterpreter.h
  Created Date: 2002.08.01
  Author: xiaozc
  Version: 0.0.01
  Create Version: 0.0.01
  Last Version Date: 2002.08.01
  Create Version Date: 2002.08.01
  Add: ruanyj 2005.6.13
	GetOutput (int index, char *name);
	Result (int index, char *name);
 ***********************************************/
 
#ifndef __WWINTERPRETER_H_
#define __WWINTERPRETER_H_

#include <string.h>
#include <stdio.h>
#include <list>
#include <stack>
#include <string>
#include "exception.h"
using namespace std;

#ifndef STR_MAX
#define STR_MAX 255
#endif

class Interpreter;

//内部数据类型定义
enum cmd_type { // 指令类型 
	cFIRST_COMMAND, /* no command, just marks start of list*/ 
	
	cASSIGN, // 赋值
	cPOP,    // 出栈
	cJUMP,   // 跳转
	cPOS,    // 正号
	cNEG,    // 负号
	cNOT,    // 非
	cINC,    // ++
	cDEC,    // --
	cPINC,   // 右++
	cPDEC,   // 右--
	
	cLT,     // 大于 >
	cGT,     // 小于 <
	cLE,     // 小于等于 <=
	cGE,     // 大于等于 >=
	cEQ,     // 等于 == 
	cNE,     // 不等于 !=
	cAND,    // 与 &&
	cOR,     // 或 ||
	
	cLABEL,  //标号
	cDECIDE, //条件判断

	
	 /* double operations */
	cDBLADD, // 加 + 
	cDBLMIN, // 减 -
	cDBLMUL, // 乘 *
	cDBLDIV, // 除 /
	cDBLMOD, // 模 %
	
	cPUSHSYM, //符号压栈
	cPOPSYM,  //符号出栈
	cPUSHDBL,    //数值压栈
	
	/* string operations */
	cPUSHSTRSYM,
	cPOPSTRSYM,
	cPUSHSTR,
	cCONCAT,           
	cPUSHSTRPTR,
	cCHANGESTRING,
	cTOKEN,

	
	cDEFSYMBOL,  //定义符号
	cDEFSYMBOL_ARRAY, //定义符号数组
	cARRAYREF,   //数组引用
	cOUTPUT,     //输出
	cINPUT,      //输入
	cINPUTPARAM, //输入参数定义
	cPREPAREINPUT, //准备输入
	cSYSINPUT,   //输入系统变量
	cSYSINPUTS,  //输入系统变量(字符串型)
			
	cLAST_COMMAND /* no command, just marks end of list */
};

enum stackentries { /* different types of stackentries */
  stSTRING,stNUMBER,stLABEL,stSYMBOL,stARRAYMEMBER, stINPUTPARAM
};

// 指令的结构定义
typedef struct  Command {  
  int     type;              // 指令类型
  char  * str_data;     // 指令的特定数据
  double  dbl_data;
  int    args;  // 函数的参数个数
  char   tag;   // 杂项参数
  int    line;  // 指令对应的行号
}TCommand;

//符号的类型
enum symboltype {
	symDOUBLE, symDOUBLE_ARRAY,   /*数值*/ 
	symSTRING, symSTRING_ARRAY   /*字符串*/
};

typedef struct DblList {
	double dbl_value;
	struct DblList * pNext;
}TDblList;

typedef struct StrList {
	char   str[STR_MAX];
	struct StrList * pNext;
}TStrList;


//符号定义
typedef class  Symbol {
       
public:
	int     type;            //符号类型
	char    name[STR_MAX];      //符号名称
	double  dbl_data;       //数值
	char    str_data[STR_MAX];       //字符串值
	
	TDblList * dbl_list;  //数值数组
	TStrList * str_list;  //字符串数组
	
	//取数值
	double get_dbl (int array_ind);
	
	//取字符串
	char * get_str (int array_ind);
	
	//取数值数组
	TDblList * get_dblarray();
	
	//取字符串数组
	TStrList * get_strarray();
	
	//数值数组赋值
	void set_array_member( int array_ind, double dbl);
	
	//字符串数值赋值
	void set_array_member( int array_ind, char * str);
	
	//数值赋值
	void set_value(int array_ind,double value);
	
	//字符串赋值
	void set_value(int array_ind,char * value);
	
	//转换为字符串返回值
	const char * asString();
	//转换为双精度数值返回
	double asDouble();
	
	//转换为整型值返回
	long    asInteger();
	
	const char * getName();

	Symbol();
	~Symbol();
	Symbol(const Symbol & Ori);
}TSymbol;

//堆栈的节点结构
typedef class Stackentry {  
public:
	int        type;             //节点类型
	TSymbol *  symbol_ptr;      //指向变量的指针
	double dbl_data;            //数值
	char   str_data[STR_MAX];
	
	//取得数值
	double get_dbl();
	
	//取得字符串
	char * get_str();
	
	//取得数值数组
	TDblList * get_dblarray();
	
	//取得字符串数组
	TStrList * get_strarray();
	
	//赋数值
	void set_value(double value);
	
	//赋字符串
	void set_value(char * value);
	
	//判断是不是
	bool is_number();
	
	Stackentry();
	~Stackentry();
}TStackentry;


typedef struct  SymList {
	TSymbol        * pSym;
	struct SymList * pNext;
}TSymList;

typedef struct CmdList {
	TCommand  * pCmd;
	struct CmdList * pNext;
}TCmdList;

typedef struct StackList {
	TStackentry * pStackItem;
	struct StackList * pNext;
}TStackList;


//解释器定义
typedef class Interpreter {

public:
	
	bool GetInputParam(char * name,TSymbol & sym);          //根据名字取得输入参数值(转换为字符串)
	TSymbol * & GetOutput(char * name);
	TSymbol * GetOutput(int index, char *name);
	
	void SetInputData(int type, double value);             //设置数值(数值数组)类型输入数据
	void SetInputData(int type, char * value);             //设置字符串(字符串数组)类型输入数据

	/* end of add */
	
	virtual double Sysinput(const char * ){return 0;};
	virtual char * SysinputS(const char * ){return NULL;};
	//virtual bool InputHandler();
	
	TSymbol Param(char * name)
	{
		TSymbol sym;
	
		if ( ! (GetInputParam(name, sym))){
			throw TException("ScriptParser::Param: 指定的参数名不存在");
		};
		
		return sym;
	};
 
	TSymbol * & Result(char * name)
	{
		return GetOutput(name);
	}; 

	TSymbol * Result(int index, char *name)
	{
		return GetOutput(index, name);
	};

	//输入单个字符串型值
	void SimpleInput(char * value)
	{
		SetInputData(symSTRING, value);
	}
	
	//输入单个数值型值
	void SimpleInput(double value)
	{
		SetInputData(symDOUBLE, value);
	}
	
	//输入多个字符串型值
	void MultiInput(char * value)
	{
		SetInputData(symSTRING_ARRAY, value);
	}
	
	//输入多个数值型值
	void MultiInput(double value)
	{
		SetInputData(symDOUBLE_ARRAY, value);
	}
	
	
	Interpreter();   //constructor
	virtual ~Interpreter();  //destructor
	
	
	void print(); //输出生成的代码
	bool run();   //执行
	
	void Clear();
	void ClearResult();
	
	void StartRet();
	bool NextRet();
	TSymbol * Ret;
	
protected:
    TSymList  *  cur_result;
    
    	
	TStackList * selection_stack; //条件标号堆栈
	TStackList * iteration_stack; //循环标号堆栈
	//指令生成
	void add_command(TCommand & cmd); //生成一条指令
	void create_pushsym(char * symbol);    //生成一条 "向堆栈压入一个符号" 指令
	void create_pushdbl(double value);     //生成一条 "向堆栈压入一个数值" 指令	
	void create_pushstr(char * value);     //生成一条 "向堆栈压入一个字符串值" 指令	
	void create_dblbin(char c);            //生成一条 "二元运算" 指令
	void create_assign();                  //生成一条 "赋值" 指令
	void create_pop();                     //生成一条 "出栈" 指令
	void create_decide();                  //生成一条 "条件判断" 指令
	void create_jump(int dest_addr);                  //生成一条 "条件判断" 指令
	void create_unary_op(int op);          //生成一条 "一元运算" 指令
	void create_postinc();                 //生成一条 "右加一" 指令
	void create_postdec();                 //生成一条 "右减一" 指令
	void create_reop(int op);              //生成一条 "关系运算" 指令
	void create_logicalop(int op);         //生成一条 "逻辑运算" 指令
	
	void create_break();                   //生成跳转指令,实现break 
	void create_continue();                //生成跳转指令,实现continue
	
	
	void create_symbol_def(int type, char * name);          //生成一条 "符号定义"　指令
	void create_symbol_array_def(int type, char * name);    //生成一条 "数组符号定义" 指令
	void create_array_ref();                                //生成一条 "数组引用" 指令
	void create_output(char * name);                        //生成一条 "输出" 指令
	void create_input_param(char * name);                   //生成一条 "增加输入参数" 指令
	void create_input();                                    //生成一条 "输入" 指令
	void create_prepare_input();                            //生成一条 "准备输入" 指令
	
	// add 2003/1/21
	void create_sysinput(char * name);                      //生成一条 "输入系统变量" 指令
	void create_sysinputs(char * name);                      //生成一条 "输入系统变量(字符串型)" 指令
	void create_sysoutput(char * name);                     //生成一条 "输出系统变量" 指令
	// end of add 
	
	void stackswap(TStackList * & stk);             //交换栈顶两个元素

	void pushlabel_jump(TStackList * & stk);        //符号入栈,生成一条JUMP指令
	void poplabel(TStackList * & stk);              //符号出栈,生成一条LABEL指令

	void pushlabel(TStackList * & stk);             //生成一条LABEL指令, 符号入栈
	void poplabel_jump(TStackList * & stk);         //符号出栈,生成一条JUMP指令到当前符号
	
	void pushnolabel(TStackList * & stk);           //生成一个符号入栈
	void nopop_jump(TStackList * & stk);            //从栈顶取一个符号,生成一条JUMP

private:
	int labelcount;
	int cmd_count;
	TCmdList * command_list;       //指令流
	TSymList * symbol_list;        //符号流
	TStackList *  runtime_stack;  //运行时堆栈

	
	TSymList * input_param_list;  //输入读取参数列表
	TSymList * output_data_list;  //输出数据列表
	
	void FreeSymList(TSymList * & );
	void FreeStrList(TStrList * &);
	void FreeDblList(TDblList * &);
	void FreeStackList(TStackList * &);
	void FreeCmdList(TCmdList * &);
	
	//add 2003/1/21
	TStrList * sysinput_list;    //输入系统变量列表
	//end of add 
	
	TSymbol  input_data;   //输入数据

	TSymList * find_symbol(char * s);
	
	//指令执行
	void exec_pushsym(TCmdList *);  //执行一条 "向堆栈压入一个符号" 指令
	void exec_pushdbl(TCmdList *);     //执行一条 "向堆栈压入一个数值" 指令	
	void exec_pushstr(TCmdList *);     //执行一条 "向堆栈压入一个字符串值" 指令	
	void exec_dblbin(TCmdList *);      //执行一条 "二元运算" 指令
	void exec_assign(TCmdList *);      //执行一条 "赋值" 语句
	void exec_pop(TCmdList *);         //执行一条 "出栈" 指令
	void exec_decide(TCmdList * &);      //执行一条 "条件判断" 指令
	void exec_jump(TCmdList * &);      //执行一条 "跳转" 指令
	void exec_unary(TCmdList *);    //执行一条 "一元运算" 指令
	void exec_postinc(TCmdList *);   //执行一条 "右加一" 指令
	void exec_postdec(TCmdList *);   //执行一条 "右减一" 指令
	void exec_reop(TCmdList *);      //执行一条 "关系运算" 指令
	void exec_logicalop(TCmdList *);        //执行一条 "逻辑运算" 指令
	void exec_symbol_def(TCmdList *);       //执行一条 "符号定义" 指令
	void exec_symbol_array_def(TCmdList *); //执行一条 "数组符号定义" 指令
	void exec_array_ref(TCmdList *);    //执行一条 "数组引用" 指令
	void exec_output(TCmdList *);       //执行一条 "输出" 指令
	void exec_input(TCmdList *);       //执行一条 "输入" 指令
	void exec_input_param(TCmdList *);       //执行一条 "输入参数" 指令
	void exec_prepare_input(TCmdList *);  //执行一条 "准备输入"指令

	// add 2003/1/21
	void exec_sysinput(TCmdList *);  //执行一条 "输入系统变量"(double型) 指令
	void exec_sysinputs(TCmdList *);  //执行一条 "输入系统变量(字符串型)" 指令
	  void exec_sysinputInt(TCmdList *); //执行一条 "输入系统变量"(整型) 指令
	// end of add
	
	char *  get_cmd_exp(int type);   //取得指令名称
	void print_cmd(TCmdList *);
	
}TInterpreter;



#endif
