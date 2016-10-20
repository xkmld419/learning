/************************************************
  Copyright by Wholewise  2002
  File Name: wwLex.h
  Created Date: 2002.08.01
  Author: xiaozc
  Version: 0.0.01
  Create Version: 0.0.01
  Last Version Date: 2002.08.01
  Create Version Date: 2002.08.01

  Modify Date: 2005.09.21
  ruanyj
  version: 0.0.02
  content: T123_45_234
 ***********************************************/

/*头部定义*/
/*词法解析器类名*/
%name wwLex
%{
#include <stdlib.h>
#include <list>
#include "wwParser.h"
%}

/*新增方法定义*/
%define LEX_PARAM YY_wwParser_STYPE *val,YY_wwParser_LTYPE *loc
/*%define INHERIT*/
%define INPUT  Input
%define INPUT_NOCODE
%define MEMBERS \
public : \
char input_buf[INPUT_BUFFER_SIZE];\
int  input_len;\
int  input_cur_pos;\
int  cur_line,cur_column;\
list<char *> str_list;\
void comment();\
void wwCount();\
virtual bool SetBuf(char * buf,int size);

%define DESTRUCTOR_CODE 		list <char * >::iterator i1;\
		for ( i1 = str_list.begin(); i1!= str_list.end(); i1++){\
			free(*i1);\
		};\


/*包含wwParser.h,取得TOKEN定义*/
%header{
#define INPUT_BUFFER_SIZE 20000 
#include "wwParser.h"
#include <iostream>
%}


/*词法规则定义*/

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*
TVAR			[T][0-9_]+

%%
"/*"			{ comment(); }

"break"			{ wwCount(); return(wwParser::BREAK); }
"continue"		{ wwCount(); return(wwParser::CONTINUE); }
"else"			{ wwCount(); return(wwParser::ELSE); }
"if"			{ wwCount(); return(wwParser::IF); }
"for"			{ wwCount(); return(wwParser::FOR); }
"while"			{ wwCount(); return(wwParser::WHILE); }
"do"			{ wwCount(); return(wwParser::DO); }

"int"			{ wwCount(); return(wwParser::TYPE_INT); }
"double"		{ wwCount(); return(wwParser::TYPE_DOUBLE); }
"string"		{ wwCount(); return(wwParser::TYPE_STRING); }
"output"		{ wwCount(); return(wwParser::OUTPUT);}
"input"			{ wwCount(); return(wwParser::INPUT);}

{TVAR}     { wwCount();char * s =strdup((char *)yytext);  val->s_val=s; str_list.push_back(s) ; return(wwParser::SYS_INPUT);}
(SS){D}*     { wwCount();char * s =strdup((char *)yytext);  val->s_val=s; str_list.push_back(s) ; return(wwParser::SYS_INPUTS);}

{L}({L}|{D})*		{ wwCount(); char * s =strdup((char *)yytext);  val->s_val=s; str_list.push_back(s) ;return(wwParser::IDENTIFIER); }

0[xX]{H}+{IS}?		{ wwCount(); val->f_val = atof((char *)yytext); return(wwParser::CONSTANT); }
0[xX]{H}+{IS}?		{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }
0{D}+{IS}?		{ wwCount();  val->f_val = atof((char *)yytext);   return(wwParser::CONSTANT); }
0{D}+{IS}?		{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }
{D}+{IS}?		{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }
{D}+{IS}?		{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }
'(\\.|[^\\'])+'		{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }

{D}+{E}{FS}?		{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }
{D}*"."{D}+({E})?{FS}?	{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }
{D}+"."{D}*({E})?{FS}?	{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }

\"(\\.|[^\\"])*\"	{ wwCount(); yytext[ strlen( (char *)yytext)-1]='\0';yytext[0]='\0'; char * s =strdup((char *)yytext+1);  val->s_val=s; str_list.push_back(s) ;  return(wwParser::STRING_LITERAL); }

"++"			{ wwCount(); return(wwParser::INC_OP); }
"--"			{ wwCount(); return(wwParser::DEC_OP); }
"&&"			{ wwCount(); return(wwParser::AND_OP); }
"||"			{ wwCount(); return(wwParser::OR_OP); }
"<="			{ wwCount(); return(wwParser::LE_OP); }
">="			{ wwCount(); return(wwParser::GE_OP); }
"=="			{ wwCount(); return(wwParser::EQ_OP); }
"!="			{ wwCount(); return(wwParser::NE_OP); }
";"			{ wwCount(); return(';'); }
"{"			{ wwCount(); return('{'); }
"}"			{ wwCount(); return('}'); }
","			{ wwCount(); return(','); }
":"			{ wwCount(); return(':'); }
"="			{ wwCount(); return('='); }
"("			{ wwCount(); return('('); }
")"			{ wwCount(); return(')'); }
"["			{ wwCount(); return('['); }
"]"			{ wwCount(); return(']'); }
"!"			{ wwCount(); return('!'); }
"-"			{ wwCount(); return('-'); }
"+"			{ wwCount(); return('+'); }
"*"			{ wwCount(); return('*'); }
"/"			{ wwCount(); return('/'); }
"%"			{ wwCount(); return('%'); }
"<"			{ wwCount(); return('<'); }
">"			{ wwCount(); return('>'); }
"."			{ wwCount(); return('.'); }

[ \t\v\n\f]		{ wwCount(); }
.			{ /* ignore bad characters */ }


%%

void wwLex::comment()
{
	unsigned char c, c1;

loop:
	while ((c = yyinput()) != '*' && c > 0)
		cout<<c;

	if ((c1 = yyinput()) != '/' && c > 0)
	{
		yyunput(c1,yy_c_buf_p);
		goto loop;
	}

	if (c > 0)
		cout<<c;
	return;	
}

void wwLex::wwCount()
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
			{cur_column = 0; cur_line++;}
		else if (yytext[i] == '\t')
			{cur_column += 8 - (cur_column % 8);}
		else
			cur_column++;

	return;
}

/*重定义输入函数*/
int wwLex::Input(char * buf, int & result, int max_size)
{
	if ( input_cur_pos == input_len){
		result=0;
		return 0;
	};
	
	for ( result=0;input_cur_pos<input_len && result<=max_size; result++){
		buf[result] = input_buf[input_cur_pos++];
	}
	return 0;
}


bool wwLex::SetBuf(char * buf,int size)
{
	input_len = 0;
	input_cur_pos = 0;
	cur_line = 1;
	cur_column = 0;
	
	if ( size > INPUT_BUFFER_SIZE)
		return false;
	
	for ( input_len=0; input_len<size; input_len++){
		input_buf[input_len] = buf[input_len];
	}
	return true;
}

