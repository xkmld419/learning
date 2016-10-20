/*VER: 1*/ 
#ifndef YY_wwParser_h_included
#define YY_wwParser_h_included
/* before anything */
#ifdef c_plusplus
#ifndef __cplusplus
#define __cplusplus
#endif
#endif
#ifdef __cplusplus
#ifndef YY_USE_CLASS
#define YY_USE_CLASS
#endif
#else
#endif
#include <stdio.h>

#include <iostream>
#include <stdio.h>
#include <string>
#include "wwInterpreter.h"

typedef union {
  double f_val;          /* float number */
  int    i_val;              /* number of newlines */
  char * s_val;
} yy_wwParser_stype;
#define YY_wwParser_STYPE yy_wwParser_stype
#define YY_wwParser_MEMBERS  public:  int cur_type; \
~wwParser(){};
#define YY_wwParser_INHERIT  :public Interpreter 
#define YY_wwParser_LSP_NEEDED 
#define YY_wwParser_LEX_BODY =0
#define YY_wwParser_ERROR_BODY  =0
#define YY_wwParser_CONSTRUCTOR_CODE   yylloc.first_line =1; yylloc.first_column =1; 
 /* %{ and %header{ and %union, during decl */
#ifndef YY_wwParser_COMPATIBILITY
#ifndef YY_USE_CLASS
#define  YY_wwParser_COMPATIBILITY 1
#else
#define  YY_wwParser_COMPATIBILITY 0
#endif
#endif

#if YY_wwParser_COMPATIBILITY != 0
/* backward compatibility */
#ifdef YYLTYPE
#ifndef YY_wwParser_LTYPE
#define YY_wwParser_LTYPE YYLTYPE
/* WARNING obsolete !!! user defined YYLTYPE not reported into generated header */
/* use %define LTYPE */
#endif
#endif
#ifdef YYSTYPE
#ifndef YY_wwParser_STYPE 
#define YY_wwParser_STYPE YYSTYPE
/* WARNING obsolete !!! user defined YYSTYPE not reported into generated header */
/* use %define STYPE */
#endif
#endif
#ifdef YYDEBUG
#ifndef YY_wwParser_DEBUG
#define  YY_wwParser_DEBUG YYDEBUG
/* WARNING obsolete !!! user defined YYDEBUG not reported into generated header */
/* use %define DEBUG */
#endif
#endif
#ifdef YY_wwParser_STYPE
#ifndef yystype
#define yystype YY_wwParser_STYPE
#endif
#endif
/* use goto to be compatible */
#ifndef YY_wwParser_USE_GOTO
#define YY_wwParser_USE_GOTO 1
#endif
#endif

/* use no goto to be clean in C++ */
#ifndef YY_wwParser_USE_GOTO
#define YY_wwParser_USE_GOTO 0
#endif

#ifndef YY_wwParser_PURE
/* YY_wwParser_PURE */
#endif
/* prefix */
#ifndef YY_wwParser_DEBUG
/* YY_wwParser_DEBUG */
#endif
#ifndef YY_wwParser_LSP_NEEDED
 /* YY_wwParser_LSP_NEEDED*/
#endif
/* DEFAULT LTYPE*/
#ifdef YY_wwParser_LSP_NEEDED
#ifndef YY_wwParser_LTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YY_wwParser_LTYPE yyltype
#endif
#endif
/* DEFAULT STYPE*/
#ifndef YY_wwParser_STYPE
#define YY_wwParser_STYPE int
#endif
/* DEFAULT MISCELANEOUS */
#ifndef YY_wwParser_PARSE
#define YY_wwParser_PARSE yyparse
#endif
#ifndef YY_wwParser_LEX
#define YY_wwParser_LEX yylex
#endif
#ifndef YY_wwParser_LVAL
#define YY_wwParser_LVAL yylval
#endif
#ifndef YY_wwParser_LLOC
#define YY_wwParser_LLOC yylloc
#endif
#ifndef YY_wwParser_CHAR
#define YY_wwParser_CHAR yychar
#endif
#ifndef YY_wwParser_NERRS
#define YY_wwParser_NERRS yynerrs
#endif
#ifndef YY_wwParser_DEBUG_FLAG
#define YY_wwParser_DEBUG_FLAG yydebug
#endif
#ifndef YY_wwParser_ERROR
#define YY_wwParser_ERROR yyerror
#endif

#ifndef YY_wwParser_PARSE_PARAM
#ifndef __STDC__
#ifndef __cplusplus
#ifndef YY_USE_CLASS
#define YY_wwParser_PARSE_PARAM
#ifndef YY_wwParser_PARSE_PARAM_DEF
#define YY_wwParser_PARSE_PARAM_DEF
#endif
#endif
#endif
#endif
#ifndef YY_wwParser_PARSE_PARAM
#define YY_wwParser_PARSE_PARAM void
#endif
#endif

/* TOKEN C */
#ifndef YY_USE_CLASS

#ifndef YY_wwParser_PURE
extern YY_wwParser_STYPE YY_wwParser_LVAL;
#endif

#define	CONSTANT	258
#define	IDENTIFIER	259
#define	STRING_LITERAL	260
#define	TYPE_STRING	261
#define	TYPE_INT	262
#define	TYPE_DOUBLE	263
#define	SYS_INPUT	264
#define	SYS_INPUTS	265
#define	INC_OP	266
#define	DEC_OP	267
#define	LE_OP	268
#define	GE_OP	269
#define	EQ_OP	270
#define	NE_OP	271
#define	AND_OP	272
#define	OR_OP	273
#define	type_name	274
#define	IF	275
#define	ELSE	276
#define	SWITCH	277
#define	CASE	278
#define	BREAK	279
#define	DEFAULT	280
#define	CONTINUE	281
#define	FOR	282
#define	WHILE	283
#define	DO	284
#define	OUTPUT	285
#define	INPUT	286

 /* #defines token */
/* after #define tokens, before const tokens S5*/
#else
#ifndef YY_wwParser_CLASS
#define YY_wwParser_CLASS wwParser
#endif

#ifndef YY_wwParser_INHERIT
#define YY_wwParser_INHERIT
#endif
#ifndef YY_wwParser_MEMBERS
#define YY_wwParser_MEMBERS 
#endif
#ifndef YY_wwParser_LEX_BODY
#define YY_wwParser_LEX_BODY  
#endif
#ifndef YY_wwParser_ERROR_BODY
#define YY_wwParser_ERROR_BODY  
#endif
#ifndef YY_wwParser_CONSTRUCTOR_PARAM
#define YY_wwParser_CONSTRUCTOR_PARAM
#endif
/* choose between enum and const */
#ifndef YY_wwParser_USE_CONST_TOKEN
#define YY_wwParser_USE_CONST_TOKEN 0
/* yes enum is more compatible with flex,  */
/* so by default we use it */
#endif
#if YY_wwParser_USE_CONST_TOKEN != 0
#ifndef YY_wwParser_ENUM_TOKEN
#define YY_wwParser_ENUM_TOKEN yy_wwParser_enum_token
#endif
#endif

class YY_wwParser_CLASS YY_wwParser_INHERIT
{
public:
#if YY_wwParser_USE_CONST_TOKEN != 0
/* static const int token ... */
static const int CONSTANT;
static const int IDENTIFIER;
static const int STRING_LITERAL;
static const int TYPE_STRING;
static const int TYPE_INT;
static const int TYPE_DOUBLE;
static const int SYS_INPUT;
static const int SYS_INPUTS;
static const int INC_OP;
static const int DEC_OP;
static const int LE_OP;
static const int GE_OP;
static const int EQ_OP;
static const int NE_OP;
static const int AND_OP;
static const int OR_OP;
static const int type_name;
static const int IF;
static const int ELSE;
static const int SWITCH;
static const int CASE;
static const int BREAK;
static const int DEFAULT;
static const int CONTINUE;
static const int FOR;
static const int WHILE;
static const int DO;
static const int OUTPUT;
static const int INPUT;

 /* decl const */
#else
enum YY_wwParser_ENUM_TOKEN { YY_wwParser_NULL_TOKEN=0
	,CONSTANT=258
	,IDENTIFIER=259
	,STRING_LITERAL=260
	,TYPE_STRING=261
	,TYPE_INT=262
	,TYPE_DOUBLE=263
	,SYS_INPUT=264
	,SYS_INPUTS=265
	,INC_OP=266
	,DEC_OP=267
	,LE_OP=268
	,GE_OP=269
	,EQ_OP=270
	,NE_OP=271
	,AND_OP=272
	,OR_OP=273
	,type_name=274
	,IF=275
	,ELSE=276
	,SWITCH=277
	,CASE=278
	,BREAK=279
	,DEFAULT=280
	,CONTINUE=281
	,FOR=282
	,WHILE=283
	,DO=284
	,OUTPUT=285
	,INPUT=286

 /* enum token */
     }; /* end of enum declaration */
#endif
public:
 int YY_wwParser_PARSE(YY_wwParser_PARSE_PARAM);
 virtual void YY_wwParser_ERROR(char *msg) YY_wwParser_ERROR_BODY;
#ifdef YY_wwParser_PURE
#ifdef YY_wwParser_LSP_NEEDED
 virtual int  YY_wwParser_LEX(YY_wwParser_STYPE *YY_wwParser_LVAL,YY_wwParser_LTYPE *YY_wwParser_LLOC) YY_wwParser_LEX_BODY;
#else
 virtual int  YY_wwParser_LEX(YY_wwParser_STYPE *YY_wwParser_LVAL) YY_wwParser_LEX_BODY;
#endif
#else
 virtual int YY_wwParser_LEX() YY_wwParser_LEX_BODY;
 YY_wwParser_STYPE YY_wwParser_LVAL;
#ifdef YY_wwParser_LSP_NEEDED
 YY_wwParser_LTYPE YY_wwParser_LLOC;
#endif
 int YY_wwParser_NERRS;
 int YY_wwParser_CHAR;
#endif
#if YY_wwParser_DEBUG != 0
public:
 int YY_wwParser_DEBUG_FLAG;	/*  nonzero means print parse trace	*/
#endif
public:
 YY_wwParser_CLASS(YY_wwParser_CONSTRUCTOR_PARAM);
public:
 YY_wwParser_MEMBERS 
};
/* other declare folow */
#endif


#if YY_wwParser_COMPATIBILITY != 0
/* backward compatibility */
#ifndef YYSTYPE
#define YYSTYPE YY_wwParser_STYPE
#endif

#ifndef YYLTYPE
#define YYLTYPE YY_wwParser_LTYPE
#endif
#ifndef YYDEBUG
#ifdef YY_wwParser_DEBUG 
#define YYDEBUG YY_wwParser_DEBUG
#endif
#endif

#endif
/* END */
#endif
