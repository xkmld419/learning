/*VER: 1*/ 
/************************************************
  Copyright by Wholewise  2002
  File Name: wwInterpreter.cpp
  Created Date: 2002.08.01
  Author: xiaozc
  Version: 0.0.01
  Create Version: 0.0.01
  Last Version Date: 2002.08.01
  Create Version Date: 2002.08.01
 ***********************************************/

#include "wwInterpreter.h"
#include <algorithm>
#include <math.h>
#include "wwParser.h"
#include <stdio.h>
using namespace std;

bool operator==(TSymbol  s1,TSymbol  s2 )
{
	return !strcmp ( s1.name, s2.name);
}

const char * Symbol::getName()
{
    return name;
}


double Symbol::get_dbl (int array_ind)
{
	switch (type) {

	case symDOUBLE:
		return dbl_data;

	case symDOUBLE_ARRAY:
		{
			TDblList ** iter;
			int array_size;
			
			for ( iter = &dbl_list, array_size=0; *iter != NULL; iter = &((*iter)->pNext)){
				array_size++;
			}
			
			if ( array_size <= array_ind){ //数组长度自动增长
				for (int i=array_size; i<=array_ind; i++){
					
					*iter = new TDblList;
					
					if ( *iter == NULL){
						throw TException("Memory Overflow!");
					}
					(**iter).dbl_value = 0;
					(**iter).pNext =NULL;
					iter = &((**iter).pNext);
				}
			}
			
			iter = &dbl_list;
			
			for (int i=0;  i<array_ind; i++,iter = &((*iter)->pNext));
			
			return (*iter)->dbl_value;
		}
		
	default:
		throw TException("Symbol::get_dbl:试图从非数值符号中取数值");
	}
};

char * Symbol::get_str (int array_ind)
{
	switch (type) {
	case symSTRING:
		return str_data;
	case symSTRING_ARRAY:
	{
		TStrList ** iter;
		
		
		int array_size;
		
		for ( iter = &str_list, array_size=0; *iter != NULL; iter = &((*iter)->pNext)){
			array_size++;
		}
		
		if ( array_size <= array_ind){ //数组长度自动增长
			for (int i=array_size; i<=array_ind; i++){
				
				*iter = new TStrList;
				
				if ( *iter == NULL){
					throw TException("Memory Overflow!");
				}
				strcpy ( (**iter).str, "");
				(**iter).pNext =NULL;
				iter = &((**iter).pNext);
			}
		}

		iter = & str_list;
		for (int i=0;  i<array_ind; i++,iter = & ((**iter).pNext));
		return (*iter)->str;
	}
	default:
		throw TException("Symbol::get_str:试图从非字符串符号中取字符串");
	}
};

TDblList * Symbol::get_dblarray()
{
	switch (type) {
	case symDOUBLE_ARRAY:
		return dbl_list;
	default:
		throw TException("Symbol::get_dblarray:不是一个数值数组符号");
	}
	
};

TStrList * Symbol::get_strarray()
{
	switch (type) {
	case symSTRING_ARRAY:
		return str_list;
	default:
		throw TException("Symbol::get_strarray:不是一个字符串数组符号");
	}
	
};


void Symbol::set_array_member( int array_ind, double dbl)
{
	TDblList ** iter;
	
	int array_size;
	
	for ( iter = &dbl_list, array_size=0; *iter != NULL; iter = &((*iter)->pNext)){
		array_size++;
	}
	
	if ( array_size <= array_ind){ //数组长度自动增长
		for (int i=array_size; i<=array_ind; i++){
			
			*iter = new TDblList;
			
			if ( *iter == NULL){
				throw TException("Memory Overflow!");
			}
			(**iter).dbl_value = 0;
			(**iter).pNext =NULL;
			iter = &((**iter).pNext);
		}
	}
	
	iter = &dbl_list;
	for (int i=0;  i<array_ind; i++,iter = & ((**iter).pNext));
	
	(*iter)->dbl_value = dbl;
};

void Symbol::set_array_member( int array_ind, char * str)
{

	TStrList ** iter;
	
	
	int array_size;
	
	for ( iter = &str_list, array_size=0; *iter != NULL; iter = &((*iter)->pNext)){
		array_size++;
	}
	
	if ( array_size <= array_ind){ //数组长度自动增长
		for (int i=array_size; i<=array_ind; i++){
			
			*iter = new TStrList;
			
			if ( *iter == NULL){
				throw TException("Memory Overflow!");
			}
			strcpy ( (**iter).str , "");
			(**iter).pNext =NULL;
			iter = &((**iter).pNext);
		}
	}

	iter = & str_list;
	for (int i=0;  i<array_ind; i++,iter = & ((**iter).pNext));
	
	strcpy ( (*iter)->str,str);
};

void Symbol::set_value(int array_ind,double value)
{
	switch ( type){
		case symDOUBLE:
			dbl_data = value;
			break;
		case symDOUBLE_ARRAY:
			set_array_member(array_ind, value);
			break;
		default:
			throw TException("TSymbol::set_value(double):不是数值符号");
		
	}
	return;
};

//字符串赋值
void Symbol::set_value(int array_ind,char * value)
{
	switch ( type){
		case symSTRING:
			strcpy ( str_data, value);
			break;
		case symSTRING_ARRAY:
			set_array_member(array_ind, value);
			break;
		default:
			throw TException("TSymbol::set_value(string):不是字符串符号");
	}
	return;
};

//转换为整型值返回
long    Symbol::asInteger()
{
	switch (type) {
		case symDOUBLE:
			return (long)dbl_data;
		case symSTRING:
			return atol(str_data);
		default:
			throw TException("TSymbol::asDouble:是一个数组符号");
	};
};

//转换为字符串返回值
const char * Symbol::asString()
{
	switch (type) {
		case symSTRING:
			return str_data;
			
		case symDOUBLE:
			{
				char sTemp[20];
				sprintf(sTemp,"%f",dbl_data);
				
				strcpy ( str_data ,sTemp);
				return str_data;
			};
		default:
			throw TException("TSymbol::asString:是一个数组符号");
	};

}

double Symbol::asDouble()
{
	switch (type) {
		case symDOUBLE:
			return dbl_data;
		case symSTRING:
			return atof(str_data);
		default:
			throw TException("TSymbol::asDouble:是一个数组符号");
	};
}

Symbol::Symbol()
{
	name[0] = '\0';
	dbl_data = 0;
	str_data[0] = '\0';
	dbl_list = NULL;
	str_list = NULL;
};


Symbol::Symbol(const Symbol & Ori)
{
	dbl_data = Ori.dbl_data;
	strcpy ( name, Ori.name);
	strcpy ( str_data, Ori.str_data);
	dbl_list = NULL;
	str_list = NULL;
	type = Ori.type;
};

Symbol::~Symbol()
{
	
	//释放DBL数组
	TDblList * iter;
	for ( iter  = dbl_list; iter != NULL;){
		TDblList * pSave;
		pSave = iter->pNext;
		delete iter;
		iter = pSave;
	}
	//释放STR数组
	TStrList * iter2;
	for ( iter2  = str_list; iter2 != NULL;){
		TStrList * pSave;
		pSave = iter2->pNext;
		
		delete iter2;
		
		iter2 = pSave;
	}
};

//取得数值
double TStackentry::get_dbl()
{
 	 switch ( type) {
 	 case stNUMBER:
 	 	return dbl_data;
 	 case stSYMBOL:
 	 	return (*symbol_ptr).get_dbl(0);
 	 case stARRAYMEMBER:
 	 	return (*symbol_ptr).get_dbl((int)dbl_data);
 	 default:
 	 	throw TException("TStackentry::get_dbl:堆栈数据类型无效");
 	 }
};

//取得字符串
char * TStackentry::get_str()
{
	switch ( type) {
	case stSTRING:
		return str_data;
	case stSYMBOL:
		return (*symbol_ptr).get_str(0);
	case stARRAYMEMBER:
		return (*symbol_ptr).get_str((int)dbl_data);
	default:
		throw TException("TStackentry::get_str:堆栈数据类型无效");
	}
};


//取得数值数组
TDblList * TStackentry::get_dblarray()
{
	switch ( type){
	case stSYMBOL:
		return (*symbol_ptr).get_dblarray();
	default:
		throw TException("TStackentry::get_dblarray:不是一个符号");
	}
};

//取得字符串数组
TStrList * TStackentry::get_strarray()
{
	switch ( type){
	case stSYMBOL:
		return (*symbol_ptr).get_strarray();
	default:
		throw TException("TStackentry::get_dblarray:不是一个符号");
	}
};

//赋数值
void TStackentry::set_value(double value)
{
	switch (type) {
	case stSYMBOL:
		(*symbol_ptr).set_value(0,value);
		break;
	case stARRAYMEMBER:
		(*symbol_ptr).set_value((int)dbl_data,value);
		break;
	default:
		throw TException("TStackentry::set_value(double):不是一个符号");
	}
};

//赋字符串
void TStackentry::set_value(char * value)
{
	switch (type) {
	case stSYMBOL:
		(*symbol_ptr).set_value(0,value);
		break;
	case stARRAYMEMBER:
		(*symbol_ptr).set_value((int)dbl_data,value);
		break;
	default:
		throw TException("TStackentry::set_value(string):不是一个符号");
	}
};

//判断是不是
bool TStackentry::is_number()
{
	if (type == stNUMBER 
		|| ( type == stSYMBOL && ( (*symbol_ptr).type==symDOUBLE ||(*symbol_ptr).type==symDOUBLE_ARRAY))
	)		
		return true;
	else
		return false;	
};


TStackentry::Stackentry()
{
	strcpy ( str_data, "");
};

TStackentry::~Stackentry()
{
};

/////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////

//取得指令名称
char *  Interpreter::get_cmd_exp(int type)
{
	switch (type)
	{
	case cFIRST_COMMAND:return "FIRST";
	case cLABEL: return "LABEL";
	case cDECIDE: return "DECIDE";
	
	              
	/* double operations */
	case cDBLADD: return "DBLADD";
	case cDBLMIN: return "DBLMIN";
	case cDBLMUL: return "DBLMUL";
	case cDBLDIV: return "DBLDIV";
	case cDBLMOD: return "DBLMOD";

	case cPUSHSYM: return "PUSHSYM";
	case cPOPSYM: return "POPSYM";
	case cPUSHDBL: return "PUSHDBL";
	
	/* comparisons */
	case cAND: return "AND";
	case cOR: return "OR";
	case cLT: return "LT";
	case cGT: return "GT";
	case cLE: return "LE";
	case cGE: return "GE";
	case cEQ: return "EQ";
	case cNE: return "NE";            

	/* string operations */
	case cPUSHSTRSYM: return "PUSHSTRSYM";
	case cPOPSTRSYM: return "POPSTRSYM";
	case cPUSHSTR: return "PUSHSTR";
	case cCONCAT: return "CONCAT";           
	case cPUSHSTRPTR: return "PUSHSTRPTR";
	case cCHANGESTRING: return "CHANGESTRING";
	case cTOKEN: return "TOKEN";
	
	
	case cASSIGN: return "ASSIGN";
	case cPOP:    return "POP";
	case cJUMP:   return "JUMP";
	case cPOS:    return "POS";
	case cNEG:    return "NEG";
	case cNOT:    return "NOT";
	
	/* no command: return ""; just marks end of list */
	case cLAST_COMMAND: return "LAST";
	}
	return "???";
}
void Interpreter::print()
{
	TCmdList * curr_cmd;
	
	cout<<"BEGIN:"<<endl;
	for ( curr_cmd = command_list; curr_cmd != NULL; curr_cmd = curr_cmd->pNext)
	{
		print_cmd(curr_cmd);
	}
	cout<<"END."<<endl;
	
}

void Interpreter::print_cmd(TCmdList * curr_cmd)
{
	cout<<curr_cmd->pCmd->line<<":\t";
	
	switch (curr_cmd->pCmd->type)
	{
	case cPUSHSTR:
		cout<<"PUSHSTR\t"<<curr_cmd->pCmd->str_data<<endl;	  
		break;
	case cPUSHDBL:
		cout<<"PUSHDBL\t"<<curr_cmd->pCmd->dbl_data<<endl;	  
		break;
	case cPUSHSYM:
		cout<<"PUSHSYM\t"<<curr_cmd->pCmd->str_data<<endl;   
		break;
	case cDBLADD:
	 	cout<<"ADD"<<endl;
	 	break;
	case cDBLMIN:
	 	cout<<"MIN"<<endl;
	 	break;
	case cDBLMUL: 
		cout<<"MUL"<<endl;
		break;
	case cDBLDIV: 
		cout<<"DIV"<<endl;
		break;
	case cDBLMOD:
		cout<<"MOD"<<endl;
		break;
	case cASSIGN:
		cout<<"ASSIGN"<<endl;
		break;
	case cDECIDE:
		cout<<"DECIDE"<<endl;
		break;
	case cJUMP:
		cout<<"JUMP\t"<<curr_cmd->pCmd->dbl_data<<endl;
		break;
	case cPOP:
		cout<<"POP"<<endl;
		break;
	case cLABEL:
		cout<<"LABEL\t"<<curr_cmd->pCmd->dbl_data<<endl;
		break;
	case cNOT:
		cout<<"NOT"<<endl;
		break;
	case cNEG:
		cout<<"NEG"<<endl;
		break;
	case cPOS:
		cout<<"POS"<<endl;
		break;
	case cINC:
		cout<<"INC"<<endl;
		break;
	case cDEC:
		cout<<"DEC"<<endl;
		break;
	case cPINC:
		cout<<"PINC"<<endl;
		break;
	case cPDEC:
		cout<<"PDEC"<<endl;
		break;
	case cLT:
		cout<<"LT"<<endl;
		break;
	case cGT:
		cout<<"GT"<<endl;
		break;
	case cLE:
		cout<<"LE"<<endl;
		break;
	case cGE:
		cout<<"GE"<<endl;
		break;
	case cEQ:
		cout<<"EQ"<<endl;
		break;
	case cNE:
		cout<<"NE"<<endl;
		break;
	case cAND:
		cout<<"AND"<<endl;
		break;
	case cOR:
		cout<<"OR"<<endl;
		break;
	case cDEFSYMBOL:
		cout<<"DEF_SYMBOL\t"<<curr_cmd->pCmd->str_data<<endl;
		break;
	case cDEFSYMBOL_ARRAY:
		cout<<"DEF_SYMBOL_ARRAY\t"<<curr_cmd->pCmd->str_data<<"[]"<<endl;
		break;
	case cARRAYREF:
		cout<<"ARRAY_REF"<<endl;
		break;
	case cOUTPUT:
		cout<<"OUTPUT\t"<<curr_cmd->pCmd->str_data<<endl;
		break;
	case cINPUT:
		cout<<"INPUT"<<endl;
		break;
	case cINPUTPARAM:
		cout<<"INPUT_PARAM"<<endl;
		break;
	case cPREPAREINPUT:
		cout<<"PREPARE_INPUT"<<endl;
		break;
	case cSYSINPUT:
		cout<<"SYSINPUT"<<endl;
		break;
	case cSYSINPUTS:
		cout<<"SYSINPUTS"<<endl;
		break;
	default:
		cout<<"???"<<endl;
	}
	return;
}

//执行
bool Interpreter::run()
{
	TCmdList * curr_cmd;
	
	for ( curr_cmd = command_list; curr_cmd != NULL; )
	{
		//print_cmd(curr_cmd);
		
		switch (curr_cmd->pCmd->type)
		{
		case cPUSHSTR:
			exec_pushstr(curr_cmd);	  
			curr_cmd=curr_cmd->pNext;
			break;
		case cPUSHDBL:
			exec_pushdbl(curr_cmd);   
			curr_cmd=curr_cmd->pNext;
			break;
		case cPUSHSYM:
			exec_pushsym(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cDBLADD: case cDBLMIN: case cDBLMUL: case cDBLDIV: case cDBLMOD:
			exec_dblbin(curr_cmd); 
			curr_cmd=curr_cmd->pNext;
			break;
		case cASSIGN:
			exec_assign(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cDECIDE:
			exec_decide(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cJUMP:
			exec_jump(curr_cmd); 
			//curr_cmd=curr_cmd->pNext;
			break;
		case cPOP:
			exec_pop(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cLABEL:
			curr_cmd=curr_cmd->pNext;
			break;
		case cNEG: case cPOS: case cNOT: case cINC: case cDEC:
			exec_unary(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cPINC:
			exec_postinc(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cPDEC:
			exec_postdec(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cLT: case cGT: case cLE: case cGE: case cEQ: case cNE:
			exec_reop(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cDEFSYMBOL:
			exec_symbol_def(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cDEFSYMBOL_ARRAY:
			exec_symbol_array_def(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cARRAYREF:
			exec_array_ref(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cOUTPUT:	
			exec_output(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cINPUT:
			exec_input(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cINPUTPARAM:
			exec_input_param(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cPREPAREINPUT:
			exec_prepare_input(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cOR:
		case cAND:
			exec_logicalop(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cSYSINPUT:
			exec_sysinput(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		case cSYSINPUTS:
			exec_sysinputs(curr_cmd);
			curr_cmd=curr_cmd->pNext;
			break;
		default:
			print_cmd(curr_cmd);
			throw TException("run:unknow command");
		};
		
	};
	
	return true;
}

//////////////////////////////////////////////////////////
//
//指令生成和执行代码
//
//////////////////////////////////////////////////////////

//取得变量的地址
TSymList * Interpreter::find_symbol(char * s)
{
	TSymList ** result;

	//result = find(symbol_list.begin(), symbol_list.end(), sym);
	
	for ( result = &symbol_list; *result != NULL; result = & ( (*result)->pNext)){
		if( strcmp ( (*result)->pSym->name, s) ==0)
			break;
	}
	
	if (*result == NULL){
		
		*result  = new  TSymList;
		(*result)->pNext =NULL;
		(*result)->pSym = new TSymbol;
		(*result)->pSym->type = symDOUBLE;	
		(*result)->pSym->dbl_data = 0;
		strcpy ((*result)->pSym->name , s);

	}
	//cout<<"\t\t\t\tfind_symbol:"<<(*result).name<<":"<< (*result).dbl_data<<endl;
	
	return *result;
}


//生成一条 "跳转" 指令
void Interpreter::create_jump (int dest_addr)
{
	TCommand cmd;
	cmd.type = cJUMP;
	cmd.dbl_data = dest_addr;
	add_command(cmd);
}

//执行一条 "跳转" 指令
void Interpreter::exec_jump(TCmdList * & curr_cmd)
{
	TCmdList * new_cmd;
	
	for (new_cmd = command_list; new_cmd != NULL; new_cmd = new_cmd->pNext){
		if ( new_cmd->pCmd->type == cLABEL && new_cmd->pCmd->dbl_data == curr_cmd->pCmd->dbl_data ){
			curr_cmd = new_cmd; //更改当前指令到新的地址
			return;
		}
	}
	throw TException("exec_jump:jump label not found");
	return;
}

//产生一个LABEL入栈
void Interpreter::pushnolabel(TStackList * & stk )
{
	TStackList * pNewItem;
	pNewItem = new TStackList;
	
	pNewItem->pStackItem = new TStackentry;
	
	pNewItem->pStackItem->dbl_data = ++labelcount;
	pNewItem->pStackItem->type = stLABEL;
	
	pNewItem->pNext = stk;
	stk = pNewItem;
}

//从栈顶取一个符号,生成一条JUMP
void Interpreter::nopop_jump(TStackList * & stk )
{
	TStackentry * st;
	st = stk->pStackItem;
	create_jump((int)(*st).dbl_data);
}

//符号入栈,生成一条JUMP指令
void Interpreter::pushlabel_jump(TStackList * & stk )
{
	
	pushnolabel(stk);
	
	create_jump(labelcount);
	
}

//符号出栈,生成一条LABEL指令
void Interpreter::poplabel(TStackList *  & stk)
{
	TStackList * top;
	
	top = stk;
	stk = stk->pNext;
	
	if ( top->pStackItem->type != stLABEL){
		throw TException("poplabel: not a LABEL on stack");
	}
	
	TCommand cmd;
	
	cmd.type = cLABEL;
	cmd.dbl_data = top->pStackItem->dbl_data;
	
	delete top->pStackItem;
	delete top;
	
	add_command(cmd);
	
}

//生成一条LABEL指令, 符号入栈
void Interpreter::pushlabel(TStackList * & stk)
{
	TStackList * st;
	TCommand    cmd;
	
	++labelcount;
	
	//生成LABEL指令
	cmd.type = cLABEL;
	cmd.dbl_data = labelcount;
	
	add_command(cmd);
	
	//符号入栈
	
	st = new TStackList;
	
	st->pStackItem = new TStackentry;
	st->pStackItem->dbl_data = labelcount;
	st->pStackItem->type = stLABEL;
	
	st->pNext = stk;
	stk = st; 
	return;
	
}
            

//符号出栈,生成一条JUMP指令到当前符号          
void Interpreter::poplabel_jump(TStackList * & stk)
{
	TStackList * st;
	TCommand cmd;

	//符号出栈
	st = stk; 
	stk = stk->pNext;
	
	if ( st->pStackItem->type != stLABEL){
		throw TException("poplable_jump: not a label in stack");
	}

	//生成指令
	cmd.type = cJUMP;
	cmd.dbl_data = st->pStackItem->dbl_data;
	
	delete st->pStackItem;
	delete st;
	
	add_command ( cmd);
	return;
}


//交换栈顶两个元素
void Interpreter::stackswap(TStackList * & stk)
{
	TStackList * st1,*st2;
	
	st1 = stk;
	st2 = stk->pNext;
	
	st1->pNext = st2->pNext;
	st2->pNext = st1;
	
	stk = st2;

	return;
}                      



//生成一条指令
void Interpreter::add_command(TCommand &  cmd)
{
	cmd.line = ++cmd_count;
	
	TCmdList ** iter;
	
	for ( iter = &command_list; *iter; iter = &((**iter).pNext));
	
	*iter = new TCmdList;
	(*iter)->pNext =NULL;
	(**iter).pCmd = new TCommand;
	*((**iter).pCmd) = cmd;
	return;
}

//生成一条 "向堆栈压入一个符号" 的指令
void Interpreter::create_pushsym(char * symbol)
{
	TCommand cmd;
	
	cmd.type = cPUSHSYM;       //类型:符号压栈
	cmd.str_data = symbol;      //数据:符号
	add_command( cmd);
	return;
}

//执行一条 "向堆栈压入一个变量" 指令
void Interpreter::exec_pushsym(TCmdList *  curr_cmd)
{
	TStackList * st;
	
	st = new TStackList;
	st->pStackItem = new TStackentry;
	
	st->pStackItem->type    = stSYMBOL;
	strcpy ( st->pStackItem->str_data , curr_cmd->pCmd->str_data);
	TSymList * p=find_symbol(curr_cmd->pCmd->str_data);
	st->pStackItem->symbol_ptr = p->pSym; 
//	cout<<"symbol value:"<< (*st.symbol_ptr).dbl_data<<endl;
	
	st->pNext = runtime_stack;
	runtime_stack = st;
	
}

//生成一条 "向堆栈压入一个数值" 指令
void Interpreter::create_pushdbl(double value)
{
	TCommand cmd;
	cmd.type = cPUSHDBL;    //类型: 数值压栈
	cmd.dbl_data = value;   //数据: 数值
	add_command( cmd);
	return;
	
}

//执行一条 "向堆栈压入一个数值" 指令	
void Interpreter::exec_pushdbl(TCmdList *  curr_cmd)
{
	TStackList * st;
	
	st = new TStackList;
	st->pStackItem = new TStackentry;
	
	st->pStackItem->type    = stNUMBER;
	st->pStackItem->dbl_data = curr_cmd->pCmd->dbl_data;
	
//	cout<<"symbol value:"<< (*st.symbol_ptr).dbl_data<<endl;
	
	st->pNext = runtime_stack;
	runtime_stack = st;
}

//生成一条 "向堆栈压入一个字符串值" 指令
void Interpreter::create_pushstr(char * value)
{
	TCommand cmd;
	cmd.type = cPUSHSTR;   //类型:字符压栈
	cmd.str_data = value;  //数据:字符串
	add_command( cmd);
	return;
}

//执行一条 "向堆栈压入一个字符串值" 指令	
void Interpreter::exec_pushstr(TCmdList *  curr_cmd)
{
	TStackList * st;
	
	st = new TStackList;
	st->pStackItem = new TStackentry;
	
	st->pStackItem->type    = stSTRING;
	strcpy ( st->pStackItem->str_data , curr_cmd->pCmd->str_data);
	
//	cout<<"symbol value:"<< (*st.symbol_ptr).dbl_data<<endl;
	
	st->pNext = runtime_stack;
	runtime_stack = st;

}

//生成一条 "二元二进制运算" 指令 
void Interpreter::create_dblbin(char c)
{
	TCommand cmd;
	switch (c){
		case '*':
			cmd.type = cDBLMUL;
			break;
		case '/':
			cmd.type = cDBLDIV;
			break;
		case '+':
			cmd.type = cDBLADD;
			break;
		case '-':
			cmd.type = cDBLMIN;
			break;
		case '%':
			cmd.type = cDBLMOD;
			break;
	}
	add_command(cmd);
	return;
}

//执行一条 "二元二进制运算" 指令
void Interpreter::exec_dblbin(TCmdList *  curr_cmd)
{
	TStackentry  a, b,c;
	a = *(runtime_stack->pStackItem);
	b = *(runtime_stack->pNext->pStackItem);
  
	switch(curr_cmd->pCmd->type) {
		case cDBLADD:
		{
			if (b.is_number())
			{
				c.dbl_data = b.get_dbl()+a.get_dbl();
				c.type = stNUMBER;
			}
			else
			{
				strcpy ( c.str_data, b.get_str());
				strcat ( c.str_data, a.get_str());
				c.type = stSTRING;
			}
		}
		break;
		
		case cDBLMIN:
		c.dbl_data = b.get_dbl() - a.get_dbl();
		c.type =stNUMBER;
		break;
		
		case cDBLMUL:
		c.dbl_data =b.get_dbl() * a.get_dbl();
		c.type =stNUMBER;
		break;
		
		case cDBLDIV: 
		if (  a.get_dbl() !=0)
		    c.dbl_data = (long )b.get_dbl() / (long )a.get_dbl();
		else 
		    c.dbl_data = 0;
		    
		c.type =stNUMBER;
		break;
		
		case cDBLMOD:
		c.dbl_data = ((long)b.get_dbl()) % ((long)a.get_dbl());
		c.type =stNUMBER;
		break;
	}
	
	TStackList * pop;
	
	pop = runtime_stack;
	runtime_stack =runtime_stack->pNext;
	*(runtime_stack->pStackItem) = c;
	
	delete pop->pStackItem;
	delete pop;
}


//生成一条 "出栈" 指令
void Interpreter::create_pop()
{
	TCommand cmd;
	cmd.type = cPOP;
	add_command(cmd);
}

//执行一条 "出栈" 指令
void Interpreter::exec_pop(TCmdList *  curr_cmd)
{
	TStackList * pop;
	pop = runtime_stack;
	runtime_stack =runtime_stack->pNext;
	delete pop->pStackItem;
	delete pop;
	return;
}

//生成一条 "条件判断" 指令
void Interpreter::create_decide()
{
	TCommand cmd;
	cmd.type = cDECIDE;
	add_command(cmd);
}


//执行条件判断
void Interpreter::exec_decide(TCmdList * & curr_cmd)
{
	TStackList * st;
	
	//取出栈顶变量
	st= runtime_stack ;
	//判断变量值
	
	if ( st->pStackItem->get_dbl() !=0) curr_cmd = curr_cmd->pNext;
	
	//释放栈顶元素
	runtime_stack = runtime_stack->pNext;
	delete st->pStackItem;
	delete st;
		
	return;
}


//生成一条 "赋值" 指令
void Interpreter::create_assign()
{
	TCommand cmd;
	cmd.type = cASSIGN;
	add_command(cmd);
}

//执行赋值
void Interpreter::exec_assign(TCmdList *  curr_cmd)
{
	TStackentry *src,*dst;
	
	//取出赋值号右边的值
	src = runtime_stack->pStackItem;
	//赋值
	dst = runtime_stack->pNext->pStackItem; //runtime_stack.pop();
	
	switch ( dst->type) { //堆栈数据类型
		case stSYMBOL:
			{
				//cout<< "assign dest:"<<dst.str_data<<endl;
				
				if ((dst->symbol_ptr)->type == symDOUBLE && !src->is_number()){
					dst->symbol_ptr->type = symSTRING;
				}
				
				switch ( (* (dst->symbol_ptr)).type ){ //符号类型
					case symDOUBLE:        //符号左边是数值符号
						dst->symbol_ptr->dbl_data = src->get_dbl();
						break;
					case symDOUBLE_ARRAY:  //符号左边是数值数组符号
						dst->symbol_ptr->dbl_list = src->get_dblarray();
						break;
					case symSTRING:        //符号左边是字符串符号
						strcpy ( dst->symbol_ptr->str_data, src->get_str());
						break;
					case symSTRING_ARRAY:  //符号左边是字符串数组
						dst->symbol_ptr->str_list  = src->get_strarray();
						break;
					default:
						throw TException("exec_assign:赋值语句左边的值类型未知");
				};
			}
			break;
		case stARRAYMEMBER:
			{
				switch ((* (dst->symbol_ptr)).type){
					case symDOUBLE_ARRAY:
						dst->symbol_ptr->set_value((int)dst->dbl_data,src->get_dbl());
						break;
					case symSTRING_ARRAY:
						dst->symbol_ptr->set_value((int)dst->dbl_data,src->get_str());
						break;
					default:
						throw TException("exec_assign:堆栈和符号的类型不匹配");	
				};
			}
			break;
		default:
			throw TException("exec_assign:赋值语句左边的值类型不匹配");
			break;
	}
	
	TStackList * pop;
	
	pop = runtime_stack;
	runtime_stack = runtime_stack->pNext;
	delete pop->pStackItem;
	delete pop;

	return;
}

//生成 "一元运算" 指令
void Interpreter::create_unary_op(int op)
{
	TCommand cmd;
	
	switch (op){
		case '+':
			cmd.type = cPOS;
			break;
		case '-':
			cmd.type = cNEG;
			break;
		case '!':
			cmd.type = cNOT;
			break;
		case wwParser::INC_OP:
			cmd.type = cINC;
			break;
		case wwParser::DEC_OP:
			cmd.type = cDEC;
			break;
	};
	
	add_command(cmd);
}

//执行 "一元运算" 指令
void Interpreter::exec_unary(TCmdList *  curr_cmd)
{
	TStackentry * st;
	st = runtime_stack->pStackItem;
		
	double result = st->get_dbl();
	
	switch ( curr_cmd->pCmd->type){
		case cPOS:
			break;
		case cNEG:
			result = -result;
			break;
		case cNOT:
			result = !result;
			break;
		case cINC:
			result = result+1;
			break;
		case cDEC:
			result = result-1;
			break;
	}
	if ( curr_cmd->pCmd->type==cINC || curr_cmd->pCmd->type==cDEC) //需要改变原符号的值
	{
		
		if ( st->type != stSYMBOL && st->type != stARRAYMEMBER ){
			throw TException("exec_unary: INC DEC : 不是一个有效的符号");
		}
		else {
			
			st->set_value (result);
		}
	}
	st->dbl_data = result;
	st->type    = stNUMBER;
	
	return;
}

//生成一条 "右加一" 指令
void Interpreter::create_postinc()
{
	TCommand cmd;
	cmd.type = cPINC;
	add_command(cmd);
}

//执行一条 "右加一" 指令
void Interpreter::exec_postinc(TCmdList *  curr_cmd)
{
	TStackentry * st;
	
	st = runtime_stack->pStackItem;
	
	double result = st->get_dbl();
	
	result++;
	
	st->set_value( result); //改变符号的值
	
	st->type = stNUMBER;
	st->dbl_data = result;
	return;
}

//生成一条 "右减一" 指令
void Interpreter::create_postdec()
{
	TCommand cmd;
	cmd.type = cPDEC;
	add_command(cmd);
}

//执行一条 "右减一" 指令
void Interpreter::exec_postdec(TCmdList *  curr_cmd)
{
	TStackentry * st;
	
	st = runtime_stack->pStackItem;
	
	double result = st->get_dbl();
	
	result--;
	
	st->set_value(result);//改变符号的值

	st->type = stNUMBER;
	st->dbl_data = result;
	return;
}

//生成一条 "关系运算" 指令
void Interpreter::create_reop(int op)
{
	TCommand cmd;
	
	switch ( op)
	{
		case '>':
			cmd.type = cGT;
			break;
		case '<':
			cmd.type = cLT;
			break;
		case wwParser::LE_OP:
			cmd.type = cLE;
			break;
		case wwParser::GE_OP:
			cmd.type = cGE;
			break;
		case wwParser::EQ_OP:
			cmd.type = cEQ;
			break;
		case wwParser::NE_OP:
			cmd.type = cNE;
			break;
	}
	add_command(cmd);
	return;
}

//执行一条 "关系运算" 指令
void Interpreter::exec_reop(TCmdList *  curr_cmd)
{
	TStackentry * a, * b;
	double result;
	
	a = runtime_stack->pStackItem;
	b = runtime_stack->pNext->pStackItem;
	
	if ( b->is_number()){
		
		switch ( curr_cmd->pCmd->type){
			case cLT:
				result = b->get_dbl()<a->get_dbl();
				break;
			case cGT:
				result = b->get_dbl()>a->get_dbl();
				break;
			case cLE:
				result = b->get_dbl()<=a->get_dbl();
				break;
			case cGE:
				result = b->get_dbl()>=a->get_dbl();
				break;
			case cEQ: 
				result = b->get_dbl()==a->get_dbl();
				break;
			case cNE:
				result = b->get_dbl()!=a->get_dbl();
				break;
		}
	}
	else {
		switch ( curr_cmd->pCmd->type){
			case cLT:
				result = b->get_str()<a->get_str();
				break;
			case cGT:
				result = b->get_str()>a->get_str();
				break;
			case cLE:
				result = b->get_str()<=a->get_str();
				break;
			case cGE:
				result = b->get_str()>=a->get_str();
				break;
			case cEQ: 
				result = b->get_str()==a->get_str();
				break;
			case cNE:
				result = b->get_str()!=a->get_str();
				break;
		}
	}
	
	b->type = stNUMBER;
	b->dbl_data = result;
	
	TStackList * pop;
	
	pop = runtime_stack;
	runtime_stack = runtime_stack->pNext;
	delete pop->pStackItem;
	delete pop;
	
	return;
}

//生成一条 "逻辑运算" 指令
void Interpreter::create_logicalop(int op)
{
	TCommand cmd;
	switch (op)
	{
		case wwParser::AND_OP:
			cmd.type = cAND;
			break;
		case wwParser::OR_OP:
			cmd.type = cOR;
			break; 
	}
	add_command(cmd);
	return;
}

//执行一条 "逻辑运算" 指令
void Interpreter::exec_logicalop(TCmdList *  curr_cmd)
{
	TStackentry *a,*b;
	double va,vb,result;
	
	a = runtime_stack->pStackItem;
	b = runtime_stack->pNext->pStackItem;
	
	va = a->get_dbl();
	vb = b->get_dbl();

	switch ( curr_cmd->pCmd->type)
	{
		case cAND:
			result = va && vb ? 1:0;
			break;
		case cOR:
			result = va || vb ? 1:0;
	}
	
	b->type = stNUMBER;
	b->dbl_data = result;

	TStackList * pop;
	
	pop = runtime_stack;
	runtime_stack = runtime_stack->pNext;
	delete pop->pStackItem;
	delete pop;
	return;
}

//生成跳转指令,实现break
void Interpreter::create_break()
{
	TStackentry *st2;
	TCommand  cmd;
	
	//跳过栈顶元素
	st2 = iteration_stack->pNext->pStackItem;
	
	//生成 JUMP 指令
	cmd.type = cJUMP;
	cmd.dbl_data = st2->dbl_data;
	
	add_command(cmd);
}
                   
//生成跳转指令,实现continue
void Interpreter::create_continue()
{
	TStackentry * st2;
	TCommand  cmd;
	
	//跳转到栈顶的地址
	st2 = iteration_stack->pStackItem;
	
	cmd.type = cJUMP;
	cmd.dbl_data = st2->dbl_data;
	
	add_command(cmd);
}

//生成一条符号定义指令
void Interpreter::create_symbol_def(int type, char * name)
{
	TCommand cmd;
	
	cmd.type = cDEFSYMBOL;
	cmd.dbl_data = type;
	cmd.str_data = name;
	
	add_command ( cmd);
	return;
}

//执行一条符号定义指令
void Interpreter::exec_symbol_def(TCmdList *  curr_cmd)
{
	TSymList * sym;
	
	sym = new TSymList;
	sym->pSym = new TSymbol;
	strcpy ( sym->pSym->name, curr_cmd->pCmd->str_data);
	
	switch ( (int )(curr_cmd->pCmd->dbl_data)){
		
		case wwParser::TYPE_INT :
		case wwParser::TYPE_DOUBLE:
			sym->pSym->type = symDOUBLE;
			sym->pNext = symbol_list;
			symbol_list = sym;
			break;
		case wwParser::TYPE_STRING:
			sym->pSym->type = symSTRING;
			sym->pNext = symbol_list;
			symbol_list = sym;
			break;
		default:
			throw TException("exec_symbol_def:未定义的类型");
	}
	return;
}


//生成一条数组符号定义指令
void Interpreter::create_symbol_array_def(int type, char * name)
{
	TCommand cmd;
	
	cmd.type = cDEFSYMBOL_ARRAY;
	cmd.dbl_data = type;
	cmd.str_data = name;
	
	add_command ( cmd);
	return;
}

//执行一条数组符号定义指令   
void Interpreter::exec_symbol_array_def(TCmdList *  curr_cmd)
{
	TSymList * sym;
	
	sym = new TSymList;
	sym->pSym = new TSymbol;
	strcpy ( sym->pSym->name, curr_cmd->pCmd->str_data);
	
	switch ( (int )(curr_cmd->pCmd->dbl_data)){
		
		case wwParser::TYPE_INT :
		case wwParser::TYPE_DOUBLE:
			sym->pSym->type = symDOUBLE_ARRAY;
			sym->pNext = symbol_list;
			symbol_list = sym;
			break;

		case wwParser::TYPE_STRING:
			sym->pSym->type = symSTRING_ARRAY;
			sym->pNext = symbol_list;
			symbol_list = sym;
			break;
		default:
			cout<<"unknow type"<<endl;
	}
	return;
}

//生成一条 "数组引用" 指令
void Interpreter::create_array_ref()
{
	TCommand cmd;
	
	cmd.type = cARRAYREF;
	
	add_command (cmd);
	return;
}

//执行一条 "数组引用" 指令
void Interpreter::exec_array_ref(TCmdList *  curr_cmd)
{
	TStackentry * st1,* st2,st3;
	
	st1 = runtime_stack->pStackItem;
	st2 = runtime_stack->pNext->pStackItem;
	
	if ( st2->type != stSYMBOL || ( (*(st2->symbol_ptr)).type != symDOUBLE_ARRAY && (*(st2->symbol_ptr)).type != symSTRING_ARRAY ))
		throw TException ("exec_array_ref:无效的数组引用");
	
	st3.dbl_data = st1->get_dbl();
	strcpy ( st3.str_data , st2->str_data);
	
	st3.type = stARRAYMEMBER;
	st3.symbol_ptr = st2->symbol_ptr;
	
	TStackList * pop;
	pop  = runtime_stack;
	runtime_stack = runtime_stack->pNext;
	delete pop->pStackItem;
	delete pop;
	*(runtime_stack->pStackItem) = st3;
	return;
}

//生成一条 "输出" 指令
void Interpreter::create_output ( char * name)
{
	TCommand cmd;
	
	cmd.type = cOUTPUT;
	cmd.str_data = name;
	
	add_command (cmd);
	return;
}

//执行一条 "输出" 指令
void Interpreter::exec_output(TCmdList *  curr_cmd)
{
	TStackentry st;
	TSymbol os; //输出符号
	
	st =*(runtime_stack->pStackItem);// runtime_stack.pop();
	
	TStackList * pop;
	pop = runtime_stack;
	runtime_stack = runtime_stack->pNext;
	delete pop->pStackItem;
	delete pop;
	
	cout<<"output:\t"<<curr_cmd->pCmd->str_data<<"=";
	
	strcpy (os.name , curr_cmd->pCmd->str_data);
	
	switch (st.type) {
		case stNUMBER:
			os.dbl_data = st.dbl_data;
			os.type = symDOUBLE;
			cout<<st.dbl_data;
			break;
		case stSTRING:
			strcpy ( os.str_data , st.str_data);
			os.type = symSTRING;
			cout<<st.str_data;
			break;
		case stSYMBOL:
		case stARRAYMEMBER:
			switch ( (*st.symbol_ptr).type){
				case symDOUBLE:
				case symDOUBLE_ARRAY:
					os.dbl_data = st.get_dbl();
					os.type = symDOUBLE;
					cout<<st.get_dbl();
					break;
				case symSTRING:
				case symSTRING_ARRAY:
					strcpy ( os.str_data , st.get_str());
					os.type = symSTRING;
					cout<<st.get_str();
					break;
				default:
					throw TException("exec_output:无效的符号类型");
			}
			break;
		default:
			throw TException("exec_output:无效的堆栈数据类型");
	};
	
	TSymList * sym;
	
	sym = new TSymList;
	sym->pSym = new TSymbol;
	*(sym->pSym) = os;
	sym->pNext = output_data_list;
	output_data_list = sym;
	
	cout<<endl;
	return;
	
}

//生成一条 "增加输入参数" 指令
void Interpreter::create_input_param(char *name)
{
	TCommand cmd;
	
	cmd.type = cINPUTPARAM;
	cmd.str_data =name;
	
	add_command(cmd);
	return;
}

//执行一条 "增加输入参数" 指令
void Interpreter::exec_input_param(TCmdList *  curr_cmd)
{
	TStackentry st1;
	TSymbol param;
	
	st1 =*(runtime_stack->pStackItem);// runtime_stack.pop();
	
	TStackList * pop;
	pop = runtime_stack;
	runtime_stack = runtime_stack->pNext;
	delete pop->pStackItem;
	delete pop;
	
	strcpy ( param.name, curr_cmd->pCmd->str_data);
	if (st1.is_number())
	{
		//数值型
		param.type = symDOUBLE;
		param.dbl_data = st1.get_dbl();
	}
	else
	{
		//字符串类型
		param.type = symSTRING;
		strcpy ( param.str_data, st1.get_str());
	}
	
	TSymList * sym;
	
	sym = new TSymList;
	sym->pSym = new TSymbol;
	*(sym->pSym) = param;
	sym->pNext = input_param_list;
	input_param_list = sym;
	
	return;
}

//生成一条 "输入" 指令
void Interpreter::create_input()
{
	TCommand cmd;
	cmd.type = cINPUT;
	add_command(cmd);
	return;
}

//执行一条 "输入" 指令
void Interpreter::exec_input(TCmdList * curr_cmd)
{
	TStackList * st;
	
	/*if (! InputHandler()){
		throw TException("exec_input:输入函数返回取值失败");
	}
	*/
	st = new TStackList;
	st->pStackItem = new TStackentry;
	
	strcpy ( input_data.name, "");
	
	TSymList * sym;
	
	sym  = new TSymList;
	sym->pSym = new TSymbol;
	*(sym->pSym) = input_data;
	
	sym->pNext = symbol_list;
	symbol_list = sym; 

	st->pStackItem->type = stSYMBOL;
	st->pStackItem->symbol_ptr = sym->pSym;
	
	//runtime_stack.push(st);	
	st->pNext = runtime_stack;
	runtime_stack = st;
	
	return;
}

//生成一条 "准备输入" 指令
void Interpreter::create_prepare_input()
{
	TCommand cmd;
	cmd.type = cPREPAREINPUT;
	add_command(cmd);
	return;
}

void Interpreter::FreeSymList(TSymList * & head)
{
	TSymList * iter, * next;
	
	for ( iter = head; iter != NULL ; iter=next)
	{
		next = iter->pNext;
		delete iter->pSym;
		delete iter;
	}
	head = NULL;
};

void Interpreter::FreeCmdList(TCmdList * & head)
{
	TCmdList * iter, * next;
	
	for ( iter = head; iter != NULL ; iter=next)
	{
		next = iter->pNext;
		delete iter->pCmd;
		delete iter;
	}
	head = NULL;
};


void Interpreter::FreeStrList(TStrList * & head)
{
	TStrList * iter, * next;
	
	for ( iter = head; iter != NULL ; iter=next)
	{
		next = iter->pNext;
		delete iter;
	}
	head = NULL;
};

void Interpreter::FreeDblList(TDblList * & head)
{
	TDblList * iter, * next;
	
	for ( iter = head; iter != NULL ; iter=next)
	{
		next = iter->pNext;
		delete iter;
	}
	head = NULL;
};

void Interpreter::FreeStackList(TStackList * & head)
{
	TStackList * iter, * next;
	
	for ( iter = head; iter != NULL ; iter=next)
	{
		next = iter->pNext;
		delete iter->pStackItem;
		delete iter;
	}
	head = NULL;
};

//执行一条 "准备输入" 指令
void Interpreter::exec_prepare_input( TCmdList *  curr_cmd)
{
	FreeSymList (input_param_list);
	FreeStrList (input_data.str_list);
	FreeDblList(input_data.dbl_list);
	return;
}

//根据名字取得输入参数值(转换为字符串)
bool Interpreter::GetInputParam(char * name,TSymbol & sym)          
{
	TSymList * iter;

	for (iter = input_param_list; iter!= NULL; iter=iter->pNext){
		if ( strcmp ( (iter->pSym)->name, name)==0){
			sym = *(iter->pSym);
			return true;
		}
	}
	return false;
}

//设置数值(数值数组)类型输入数据
void Interpreter::SetInputData(int type, double value)
{
	input_data.type = type;

	if ( type == symDOUBLE_ARRAY){
		
		TDblList * new_item, * iter;
		
		new_item = new TDblList;
		new_item->dbl_value = value;
		new_item->pNext = NULL; 
		
		for ( iter = input_data.dbl_list; iter!=NULL ; iter = iter->pNext)
		{
			if ((iter->pNext) == NULL){
				iter->pNext = new_item;
				break;
			}
		}
		
		if (iter == NULL)
		{
			input_data.dbl_list = new_item;
		}
	}
	else {
		input_data.dbl_data = value;
	}
}
 
//设置字符串(字符串数组)类型输入数据
void Interpreter::SetInputData(int type, char * value)
{
	
	input_data.type = type;

	if ( type == symSTRING_ARRAY){
		TStrList * new_item, * iter;
		
		new_item = new TStrList;
		strcpy ( new_item->str , value);
		new_item->pNext = NULL; 
		
		for ( iter = input_data.str_list; iter!=NULL ; iter = iter->pNext)
		{
			if ((iter->pNext) == NULL){
				iter->pNext = new_item;
				break;
			}
		}
		
		if (iter == NULL)
		{
			input_data.str_list = new_item;
		}

	}
	else {
		strcpy ( input_data.str_data ,value);
	}
}
 
//取得输出数据
TSymbol * & Interpreter::GetOutput(char * name)
{
	TSymList * iter;

	for (iter = output_data_list; iter!= NULL; iter= iter->pNext){
		if ( strcmp ( iter->pSym->name,name)==0){
			return  iter->pSym;
		}
	}
	
	//如果找不到OUTPUT的输出,在symbol_list中找
	iter = find_symbol(name);

	return iter->pSym;
	
}

TSymbol * Interpreter::GetOutput(int index, char *name)
{
	TSymList * iter;
	int i=0;
	for (iter = output_data_list; iter!= NULL; iter= iter->pNext){
		if (i == index) {
			strcpy (name, iter->pSym->name);
			return iter->pSym;
		}

		i++;
	}

	return NULL;
}


//add 2003/1/21
void Interpreter::create_sysinput(char * name)
{
	TCommand cmd;
	cmd.type = cSYSINPUT;
	cmd.str_data = name;
	add_command(cmd);
}

void Interpreter::exec_sysinput(TCmdList *  curr_cmd)
{
	TStrList * iter;
	
	for ( iter = sysinput_list; iter!= NULL; iter = iter->pNext){
		if ( strcmp ( curr_cmd->pCmd->str_data, iter->str)==0 )
		{
			//如果系统变量已经被输入,则什么都不做返回
			return ;
		}
	}
	//系统变量未被输入
	
	//读取变量的值
	double dblValue = Sysinput(curr_cmd->pCmd->str_data);
	
	TSymList * sym;
	
	sym = find_symbol(curr_cmd->pCmd->str_data);
	
	sym->pSym->type = symDOUBLE;
	sym->pSym->dbl_data = dblValue;
	
	
	TStrList *  pStr;
	
	pStr = new TStrList;
	
	strcpy ( pStr->str, curr_cmd->pCmd->str_data);
	pStr->pNext = sysinput_list;
	sysinput_list = pStr;
	return;
	
}


void Interpreter::create_sysinputs(char * name)
{
	TCommand cmd;
	cmd.type = cSYSINPUTS;
	cmd.str_data = name;
	add_command(cmd);
}

void Interpreter::exec_sysinputs(TCmdList * curr_cmd)
{
	TStrList * iter;
	
	for ( iter = sysinput_list; iter!= NULL; iter = iter->pNext){
		if ( strcmp ( curr_cmd->pCmd->str_data, iter->str)==0 )
		{
			//如果系统变量已经被输入,则什么都不做返回
			return ;
		}
	}
	//系统变量未被输入
	
	//读取变量的值
	char * strValue = SysinputS(curr_cmd->pCmd->str_data);
	
	TSymList * sym;
	
	sym = find_symbol(curr_cmd->pCmd->str_data);
	
	sym->pSym->type = symSTRING;
	strcpy ( sym->pSym->str_data, strValue);
	
	TStrList *  pStr;
	
	pStr = new TStrList;
	
	strcpy ( pStr->str, curr_cmd->pCmd->str_data);
	pStr->pNext = sysinput_list;
	sysinput_list = pStr;
	return;
	
}

void Interpreter::Clear()
{
	
	FreeSymList (output_data_list);
	FreeCmdList (command_list);
	FreeSymList (symbol_list);
	FreeStackList ( runtime_stack);
	FreeStackList ( selection_stack);
	FreeStackList ( iteration_stack);
	labelcount = 0;
	cmd_count = 0;
	FreeSymList ( input_param_list);
	FreeStrList ( sysinput_list);
	
}

void Interpreter::ClearResult()
{
	FreeSymList (output_data_list);
	FreeSymList (symbol_list);

	FreeStackList ( runtime_stack);
	FreeStackList ( selection_stack);
	FreeStackList ( iteration_stack);

	FreeStrList ( sysinput_list);

	cmd_count = 0;
	FreeSymList ( input_param_list);
}

//end of add

Interpreter::Interpreter()
{
	labelcount=0;
	cmd_count=0; 
	command_list=NULL;
	symbol_list=NULL;
	sysinput_list =NULL;
	input_param_list =NULL;
	runtime_stack =NULL;
	selection_stack =NULL;
	iteration_stack =NULL;
	output_data_list=NULL;
};   //constructor
Interpreter::~Interpreter()
{
	Clear();
};  //destructor


void Interpreter::StartRet()
{
    cur_result =  symbol_list;
}


bool Interpreter::NextRet()
{
    if ( cur_result == NULL)
        return false;
    
    Ret = cur_result->pSym;
    cur_result = cur_result->pNext;
    return true;        
}

