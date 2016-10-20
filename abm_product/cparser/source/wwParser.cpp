/*VER: 1*/ 
#define YY_wwParser_h_included

/*  A Bison++ parser, made from wwParser.y  */

 /* with Bison++ version bison++ Version 1.21-7, adapted from GNU bison by coetmeur@icdc.fr
  */

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Bob Corbett and Richard Stallman

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* HEADER SECTION */
#ifndef _MSDOS
#ifdef MSDOS
#define _MSDOS
#endif
#endif
/* turboc */
#ifdef __MSDOS__
#ifndef _MSDOS
#define _MSDOS
#endif
#endif

#ifndef alloca
#if defined( __GNUC__)
#define alloca __builtin_alloca

#elif (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc)  || defined (__sgi)
#include <alloca.h>

#elif defined (_MSDOS)
#include <malloc.h>
#ifndef __TURBOC__
/* MS C runtime lib */
#define alloca _alloca
#endif

#elif defined(_AIX)
#include <malloc.h>
#pragma alloca

#elif defined(__hpux)
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */

#endif /* not _AIX  not MSDOS, or __TURBOC__ or _AIX, not sparc.  */
#endif /* alloca not defined.  */
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
#ifndef __STDC__
#define const
#endif
#endif
#include <stdio.h>
#define YYBISON 1  

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
#define YY_wwParser_BISON 1
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
#endif
#endif
#ifdef YYSTYPE
#ifndef YY_wwParser_STYPE 
#define YY_wwParser_STYPE YYSTYPE
#endif
#endif
#ifdef YYDEBUG
#ifndef YY_wwParser_DEBUG
#define  YY_wwParser_DEBUG YYDEBUG
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
/*  YY_wwParser_PURE */
#endif

/* section apres lecture def, avant lecture grammaire S2 */
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
      /* We used to use `unsigned long' as YY_wwParser_STYPE on MSDOS,
	 but it seems better to be consistent.
	 Most programs should declare their own type anyway.  */

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
#if YY_wwParser_COMPATIBILITY != 0
/* backward compatibility */
#ifdef YY_wwParser_LTYPE
#ifndef YYLTYPE
#define YYLTYPE YY_wwParser_LTYPE
#else
/* WARNING obsolete !!! user defined YYLTYPE not reported into generated header */
#endif
#endif
#ifndef YYSTYPE
#define YYSTYPE YY_wwParser_STYPE
#else
/* WARNING obsolete !!! user defined YYSTYPE not reported into generated header */
#endif
#ifdef YY_wwParser_PURE
#ifndef YYPURE
#define YYPURE YY_wwParser_PURE
#endif
#endif
#ifdef YY_wwParser_DEBUG
#ifndef YYDEBUG
#define YYDEBUG YY_wwParser_DEBUG 
#endif
#endif
#ifndef YY_wwParser_ERROR_VERBOSE
#ifdef YYERROR_VERBOSE
#define YY_wwParser_ERROR_VERBOSE YYERROR_VERBOSE
#endif
#endif
#ifndef YY_wwParser_LSP_NEEDED
#ifdef YYLSP_NEEDED
#define YY_wwParser_LSP_NEEDED YYLSP_NEEDED
#endif
#endif
#endif
#ifndef YY_USE_CLASS
/* TOKEN C */
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

 /* #defines tokens */
#else
/* CLASS */
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
#ifndef YY_wwParser_CONSTRUCTOR_CODE
#define YY_wwParser_CONSTRUCTOR_CODE
#endif
#ifndef YY_wwParser_CONSTRUCTOR_INIT
#define YY_wwParser_CONSTRUCTOR_INIT
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
 int YY_wwParser_PARSE (YY_wwParser_PARSE_PARAM);
 virtual void YY_wwParser_ERROR(char *msg) YY_wwParser_ERROR_BODY;
#ifdef YY_wwParser_PURE
#ifdef YY_wwParser_LSP_NEEDED
 virtual int  YY_wwParser_LEX (YY_wwParser_STYPE *YY_wwParser_LVAL,YY_wwParser_LTYPE *YY_wwParser_LLOC) YY_wwParser_LEX_BODY;
#else
 virtual int  YY_wwParser_LEX (YY_wwParser_STYPE *YY_wwParser_LVAL) YY_wwParser_LEX_BODY;
#endif
#else
 virtual int YY_wwParser_LEX() YY_wwParser_LEX_BODY;
 YY_wwParser_STYPE YY_wwParser_LVAL;
#ifdef YY_wwParser_LSP_NEEDED
 YY_wwParser_LTYPE YY_wwParser_LLOC;
#endif
 int   YY_wwParser_NERRS;
 int    YY_wwParser_CHAR;
#endif
#if YY_wwParser_DEBUG != 0
 int YY_wwParser_DEBUG_FLAG;   /*  nonzero means print parse trace     */
#endif
public:
 YY_wwParser_CLASS(YY_wwParser_CONSTRUCTOR_PARAM);
public:
 YY_wwParser_MEMBERS 
};
/* other declare folow */
#if YY_wwParser_USE_CONST_TOKEN != 0
const int YY_wwParser_CLASS::CONSTANT=258;
const int YY_wwParser_CLASS::IDENTIFIER=259;
const int YY_wwParser_CLASS::STRING_LITERAL=260;
const int YY_wwParser_CLASS::TYPE_STRING=261;
const int YY_wwParser_CLASS::TYPE_INT=262;
const int YY_wwParser_CLASS::TYPE_DOUBLE=263;
const int YY_wwParser_CLASS::SYS_INPUT=264;
const int YY_wwParser_CLASS::SYS_INPUTS=265;
const int YY_wwParser_CLASS::INC_OP=266;
const int YY_wwParser_CLASS::DEC_OP=267;
const int YY_wwParser_CLASS::LE_OP=268;
const int YY_wwParser_CLASS::GE_OP=269;
const int YY_wwParser_CLASS::EQ_OP=270;
const int YY_wwParser_CLASS::NE_OP=271;
const int YY_wwParser_CLASS::AND_OP=272;
const int YY_wwParser_CLASS::OR_OP=273;
const int YY_wwParser_CLASS::type_name=274;
const int YY_wwParser_CLASS::IF=275;
const int YY_wwParser_CLASS::ELSE=276;
const int YY_wwParser_CLASS::SWITCH=277;
const int YY_wwParser_CLASS::CASE=278;
const int YY_wwParser_CLASS::BREAK=279;
const int YY_wwParser_CLASS::DEFAULT=280;
const int YY_wwParser_CLASS::CONTINUE=281;
const int YY_wwParser_CLASS::FOR=282;
const int YY_wwParser_CLASS::WHILE=283;
const int YY_wwParser_CLASS::DO=284;
const int YY_wwParser_CLASS::OUTPUT=285;
const int YY_wwParser_CLASS::INPUT=286;

 /* const YY_wwParser_CLASS::token */
#endif
/*apres const  */
YY_wwParser_CLASS::YY_wwParser_CLASS(YY_wwParser_CONSTRUCTOR_PARAM) YY_wwParser_CONSTRUCTOR_INIT
{
#if YY_wwParser_DEBUG != 0
YY_wwParser_DEBUG_FLAG=0;
#endif
YY_wwParser_CONSTRUCTOR_CODE;
};
#endif


#define	YYFINAL		186
#define	YYFLAG		-32768
#define	YYNTBASE	49

#define YYTRANSLATE(x) ((unsigned)(x) <= 286 ? yytranslate[x] : 104)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    39,     2,     2,     2,    42,     2,     2,    32,
    33,    40,    37,    36,    38,     2,    41,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    48,    43,
    45,    44,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    34,     2,    35,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    46,     2,    47,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31
};

#if YY_wwParser_DEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     6,     8,    10,    14,    16,    19,    22,
    27,    32,    34,    38,    40,    43,    46,    49,    52,    55,
    57,    59,    63,    67,    71,    73,    77,    81,    83,    85,
    89,    93,    97,   101,   103,   107,   111,   113,   115,   117,
   119,   123,   125,   129,   131,   133,   135,   139,   141,   143,
   144,   149,   151,   153,   155,   157,   159,   161,   164,   168,
   172,   177,   179,   182,   184,   187,   194,   195,   205,   206,
   208,   210,   211,   212,   213,   214,   215,   216,   217,   234,
   235,   236,   244,   245,   246,   247,   258,   261,   264,   266,
   269,   270,   275,   277,   279,   283,   285,   289,   291,   293,
   295,   303,   309,   310,   316,   318,   322
};

static const short yyrhs[] = {     4,
     0,     9,     0,    10,     0,     3,     0,     5,     0,    32,
    67,    33,     0,    49,     0,    50,    11,     0,    50,    12,
     0,    50,    34,    67,    35,     0,    50,    32,    51,    33,
     0,    65,     0,    51,    36,    65,     0,    50,     0,    11,
    52,     0,    12,    52,     0,    37,    53,     0,    38,    53,
     0,    39,    53,     0,    52,     0,    53,     0,    54,    40,
    53,     0,    54,    41,    53,     0,    54,    42,    53,     0,
    54,     0,    55,    37,    54,     0,    55,    38,    54,     0,
    55,     0,    56,     0,    57,    43,    56,     0,    57,    44,
    56,     0,    57,    13,    56,     0,    57,    14,    56,     0,
    57,     0,    58,    15,    57,     0,    58,    16,    57,     0,
    58,     0,    59,     0,    60,     0,    61,     0,    62,    17,
    61,     0,    62,     0,    63,    18,    62,     0,    63,     0,
   100,     0,    64,     0,    52,    66,    65,     0,    45,     0,
    65,     0,     0,    67,    36,    68,    65,     0,    70,     0,
    72,     0,    73,     0,    76,     0,    91,     0,    99,     0,
    46,    47,     0,    46,    71,    47,     0,    46,    92,    47,
     0,    46,    92,    71,    47,     0,    69,     0,    71,    69,
     0,    48,     0,    67,    48,     0,    20,    32,    67,    33,
    75,    69,     0,     0,    20,    32,    67,    33,    75,    69,
    21,    74,    69,     0,     0,    77,     0,    85,     0,     0,
     0,     0,     0,     0,     0,     0,    27,    32,    67,    48,
    78,    79,    67,    80,    48,    81,    67,    82,    83,    33,
    84,    69,     0,     0,     0,    28,    32,    86,    67,    87,
    33,    69,     0,     0,     0,     0,    29,    88,    69,    28,
    32,    89,    67,    90,    33,    48,     0,    24,    48,     0,
    26,    48,     0,    93,     0,    92,    93,     0,     0,    95,
    94,    96,    48,     0,    98,     0,    97,     0,    96,    36,
    97,     0,     4,     0,     4,    34,    35,     0,     6,     0,
     7,     0,     8,     0,    30,    32,     5,    36,    65,    33,
    48,     0,    30,    32,    65,    33,    48,     0,     0,    31,
   101,    32,   102,    33,     0,   103,     0,   102,    36,   103,
     0,     5,    36,    65,     0
};

#endif

#if YY_wwParser_DEBUG != 0
static const short yyrline[] = { 0,
    70,    71,    72,    73,    74,    75,    79,    80,    81,    82,
    83,    87,    88,    92,    93,    94,    95,    96,    97,   101,
   106,   107,   108,   109,   113,   114,   115,   119,   123,   124,
   125,   126,   127,   131,   132,   133,   137,   141,   145,   149,
   150,   154,   155,   159,   160,   164,   165,   169,   173,   174,
   175,   179,   180,   181,   182,   183,   184,   189,   190,   191,
   192,   197,   198,   202,   203,   207,   208,   208,   212,   218,
   219,   223,   226,   228,   236,   241,   242,   247,   254,   261,
   264,   266,   271,   277,   283,   289,   294,   295,   299,   300,
   304,   304,   308,   312,   313,   317,   318,   322,   323,   324,
   328,   329,   333,   333,   337,   338,   342
};

static const char * const yytname[] = {   "$","error","$illegal.","CONSTANT",
"IDENTIFIER","STRING_LITERAL","TYPE_STRING","TYPE_INT","TYPE_DOUBLE","SYS_INPUT",
"SYS_INPUTS","INC_OP","DEC_OP","LE_OP","GE_OP","EQ_OP","NE_OP","AND_OP","OR_OP",
"type_name","IF","ELSE","SWITCH","CASE","BREAK","DEFAULT","CONTINUE","FOR","WHILE",
"DO","OUTPUT","INPUT","'('","')'","'['","']'","','","'+'","'-'","'!'","'*'",
"'/'","'%'","'<'","'>'","'='","'{'","'}'","';'","primary_expr","postfix_expr",
"argument_expr_list","unary_expr","cast_expr","multiplicative_expr","additive_expr",
"shift_expr","relational_expr","equality_expr","and_expr","exclusive_or_expr",
"inclusive_or_expr","logical_and_expr","logical_or_expr","conditional_expr",
"assignment_expr","assignment_operator","expr","@1","statement","compound_statement",
"statement_list","expression_statement","selection_statement","@2","jump_op",
"iteration_statement","for_loop","@3","@4","@5","@6","@7","@8","@9","while_loop",
"@10","@11","@12","@13","@14","jump_statement","declaration_list","declaration",
"@15","declaration_specifiers","declarator_list","declarator","type_specifier",
"output_statement","input_statement","@16","input_argument_list","input_argument_pair",
""
};
#endif

static const short yyr1[] = {     0,
    49,    49,    49,    49,    49,    49,    50,    50,    50,    50,
    50,    51,    51,    52,    52,    52,    52,    52,    52,    53,
    54,    54,    54,    54,    55,    55,    55,    56,    57,    57,
    57,    57,    57,    58,    58,    58,    59,    60,    61,    62,
    62,    63,    63,    64,    64,    65,    65,    66,    67,    68,
    67,    69,    69,    69,    69,    69,    69,    70,    70,    70,
    70,    71,    71,    72,    72,    73,    74,    73,    75,    76,
    76,    78,    79,    80,    81,    82,    83,    84,    77,    86,
    87,    85,    88,    89,    90,    85,    91,    91,    92,    92,
    94,    93,    95,    96,    96,    97,    97,    98,    98,    98,
    99,    99,   101,   100,   102,   102,   103
};

static const short yyr2[] = {     0,
     1,     1,     1,     1,     1,     3,     1,     2,     2,     4,
     4,     1,     3,     1,     2,     2,     2,     2,     2,     1,
     1,     3,     3,     3,     1,     3,     3,     1,     1,     3,
     3,     3,     3,     1,     3,     3,     1,     1,     1,     1,
     3,     1,     3,     1,     1,     1,     3,     1,     1,     0,
     4,     1,     1,     1,     1,     1,     1,     2,     3,     3,
     4,     1,     2,     1,     2,     6,     0,     9,     0,     1,
     1,     0,     0,     0,     0,     0,     0,     0,    16,     0,
     0,     7,     0,     0,     0,    10,     2,     2,     1,     2,
     0,     4,     1,     1,     3,     1,     3,     1,     1,     1,
     7,     5,     0,     5,     1,     3,     3
};

static const short yydefact[] = {     0,
     0,     4,     1,     5,    98,    99,   100,     2,     3,     0,
     0,     0,     0,     0,     0,     0,    83,     0,   103,     0,
     0,     0,     0,    58,    64,     7,    14,    20,    21,    25,
    28,    29,    34,    37,    38,    39,    40,    42,    44,    46,
    49,     0,    62,    52,     0,    53,    54,    55,    70,    71,
    56,     0,    89,    91,    93,    57,    45,    15,    16,     0,
    87,    88,     0,    80,     0,     0,     0,     0,    20,    17,
    18,    19,     8,     9,     0,     0,    48,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    50,    65,    59,    63,    60,     0,    90,     0,     0,
     0,     0,     0,     5,     0,     0,     6,     0,    12,     0,
    47,    22,    23,    24,    26,    27,    32,    33,    30,    31,
    35,    36,    41,    43,     0,    61,    96,     0,    94,    69,
    72,    81,     0,     0,     0,     0,     0,   105,    11,     0,
    10,    51,     0,     0,    92,     0,    73,     0,    84,     0,
   102,     0,   104,     0,    13,    97,    95,    66,     0,     0,
     0,     0,   107,   106,    67,    74,    82,    85,   101,     0,
     0,     0,    68,    75,     0,     0,    86,    76,    77,     0,
    78,     0,    79,     0,     0,     0
};

static const short yydefgoto[] = {    26,
    27,   108,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    78,    42,   125,    43,
    44,    45,    46,    47,   170,   146,    48,    49,   147,   159,
   171,   176,   179,   180,   182,    50,   102,   148,    65,   161,
   172,    51,    52,    53,    99,    54,   128,   129,    55,    56,
    57,    67,   137,   138
};

static const short yypact[] = {   -30,
   139,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    86,
    86,   -19,   -15,    -4,    10,    28,-32768,    43,-32768,    20,
    86,    86,    86,-32768,-32768,-32768,     7,    42,-32768,   -14,
    12,-32768,    -6,    39,-32768,-32768,-32768,    68,    74,-32768,
-32768,   -33,-32768,-32768,   216,-32768,-32768,-32768,-32768,-32768,
-32768,   185,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    20,
-32768,-32768,    20,-32768,   278,    62,    85,   -24,-32768,-32768,
-32768,-32768,-32768,-32768,    20,    20,-32768,    20,    86,    86,
    86,    86,    86,    86,    86,    86,    86,    86,    86,    86,
    86,-32768,-32768,-32768,-32768,-32768,   247,-32768,   117,   -16,
   -26,    20,    94,    91,    97,   126,-32768,    48,-32768,    33,
-32768,-32768,-32768,-32768,   -14,   -14,-32768,-32768,-32768,-32768,
    -6,    -6,-32768,    68,    20,-32768,    98,     0,-32768,-32768,
-32768,    99,   101,    20,    88,   104,    50,-32768,-32768,    20,
-32768,-32768,   102,   117,-32768,   278,-32768,   119,-32768,   120,
-32768,    20,-32768,   126,-32768,-32768,-32768,   113,    20,   278,
    20,   106,-32768,-32768,-32768,    99,-32768,    99,-32768,   278,
   107,   124,-32768,-32768,   110,    20,-32768,    99,-32768,   127,
-32768,   278,-32768,   161,   162,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,    24,   -17,    37,-32768,    -7,    40,-32768,-32768,
-32768,    82,    73,-32768,-32768,   -64,-32768,   -20,-32768,   -44,
   173,   122,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   123,-32768,-32768,-32768,    35,-32768,-32768,
-32768,-32768,-32768,    26
};


#define	YYLAST		326


static const short yytable[] = {    68,
    95,   105,    92,    70,    71,    72,    84,    85,   107,    92,
   109,    92,    60,   111,    93,     1,   130,    73,    74,    92,
   103,   131,     2,     3,     4,    79,    80,    81,     8,     9,
    10,    11,    61,    58,    59,   144,    86,    87,    75,   100,
    76,    63,   101,    62,    69,    69,    69,   145,    82,    83,
    19,    20,    95,    88,    89,   110,    21,    22,    23,    64,
   142,   112,   113,   114,     2,     3,   104,   141,    92,   150,
     8,     9,    10,    11,    66,   155,   117,   118,   119,   120,
   139,   132,   153,   140,    90,   154,    77,   163,     2,     3,
     4,    91,    19,    20,     8,     9,    10,    11,    21,    22,
    23,   158,    69,    69,    69,    69,    69,    69,    69,    69,
    69,    69,    69,    69,    69,   167,   106,    20,   115,   116,
   127,   133,    21,    22,    23,   173,   134,   121,   122,   135,
   136,   143,   149,   165,    92,   151,   156,   183,   166,   152,
   168,     2,     3,     4,     5,     6,     7,     8,     9,    10,
    11,   160,   162,   169,   174,   178,   175,   177,    12,   181,
   185,   186,    13,   124,    14,    15,    16,    17,    18,    19,
    20,   123,   184,    97,    98,    21,    22,    23,   157,   164,
     0,     0,     0,     0,     1,    24,    25,     2,     3,     4,
     5,     6,     7,     8,     9,    10,    11,     0,     0,     0,
     0,     0,     0,     0,    12,     0,     0,     0,    13,     0,
    14,    15,    16,    17,    18,    19,    20,     0,     2,     3,
     4,    21,    22,    23,     8,     9,    10,    11,     0,     0,
     1,    96,    25,     0,     0,    12,     0,     0,     0,    13,
     0,    14,    15,    16,    17,    18,    19,    20,     0,     2,
     3,     4,    21,    22,    23,     8,     9,    10,    11,     0,
     0,     1,    94,    25,     0,     0,    12,     0,     0,     0,
    13,     0,    14,    15,    16,    17,    18,    19,    20,     0,
     2,     3,     4,    21,    22,    23,     8,     9,    10,    11,
     0,     0,     1,   126,    25,     0,     0,    12,     0,     0,
     0,    13,     0,    14,    15,    16,    17,    18,    19,    20,
     0,     0,     0,     0,    21,    22,    23,     0,     0,     0,
     0,     0,     0,     1,     0,    25
};

static const short yycheck[] = {    20,
    45,    66,    36,    21,    22,    23,    13,    14,    33,    36,
    75,    36,    32,    78,    48,    46,    33,    11,    12,    36,
    65,    48,     3,     4,     5,    40,    41,    42,     9,    10,
    11,    12,    48,    10,    11,    36,    43,    44,    32,    60,
    34,    32,    63,    48,    21,    22,    23,    48,    37,    38,
    31,    32,    97,    15,    16,    76,    37,    38,    39,    32,
   125,    79,    80,    81,     3,     4,     5,    35,    36,   134,
     9,    10,    11,    12,    32,   140,    84,    85,    86,    87,
    33,   102,    33,    36,    17,    36,    45,   152,     3,     4,
     5,    18,    31,    32,     9,    10,    11,    12,    37,    38,
    39,   146,    79,    80,    81,    82,    83,    84,    85,    86,
    87,    88,    89,    90,    91,   160,    32,    32,    82,    83,
     4,    28,    37,    38,    39,   170,    36,    88,    89,    33,
     5,    34,    32,    21,    36,    48,    35,   182,   159,    36,
   161,     3,     4,     5,     6,     7,     8,     9,    10,    11,
    12,    33,    33,    48,    48,   176,    33,    48,    20,    33,
     0,     0,    24,    91,    26,    27,    28,    29,    30,    31,
    32,    90,     0,    52,    52,    37,    38,    39,   144,   154,
    -1,    -1,    -1,    -1,    46,    47,    48,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    20,    -1,    -1,    -1,    24,    -1,
    26,    27,    28,    29,    30,    31,    32,    -1,     3,     4,
     5,    37,    38,    39,     9,    10,    11,    12,    -1,    -1,
    46,    47,    48,    -1,    -1,    20,    -1,    -1,    -1,    24,
    -1,    26,    27,    28,    29,    30,    31,    32,    -1,     3,
     4,     5,    37,    38,    39,     9,    10,    11,    12,    -1,
    -1,    46,    47,    48,    -1,    -1,    20,    -1,    -1,    -1,
    24,    -1,    26,    27,    28,    29,    30,    31,    32,    -1,
     3,     4,     5,    37,    38,    39,     9,    10,    11,    12,
    -1,    -1,    46,    47,    48,    -1,    -1,    20,    -1,    -1,
    -1,    24,    -1,    26,    27,    28,    29,    30,    31,    32,
    -1,    -1,    -1,    -1,    37,    38,    39,    -1,    -1,    -1,
    -1,    -1,    -1,    46,    -1,    48
};
 /* fattrs + tables */

/* parser code folow  */


/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: dollar marks section change
   the next  is replaced by the list of actions, each action
   as one case of the switch.  */ 

#if YY_wwParser_USE_GOTO != 0
/*
 SUPRESSION OF GOTO : on some C++ compiler (sun c++)
  the goto is strictly forbidden if any constructor/destructor
  is used in the whole function (very stupid isn't it ?)
 so goto are to be replaced with a 'while/switch/case construct'
 here are the macro to keep some apparent compatibility
*/
#define YYGOTO(lb) {yy_gotostate=lb;continue;}
#define YYBEGINGOTO  enum yy_labels yy_gotostate=yygotostart; \
                     for(;;) switch(yy_gotostate) { case yygotostart: {
#define YYLABEL(lb) } case lb: {
#define YYENDGOTO } }
#define YYBEGINDECLARELABEL enum yy_labels {yygotostart
#define YYDECLARELABEL(lb) ,lb
#define YYENDDECLARELABEL  };
#else
/* macro to keep goto */
#define YYGOTO(lb) goto lb
#define YYBEGINGOTO
#define YYLABEL(lb) lb:
#define YYENDGOTO
#define YYBEGINDECLARELABEL
#define YYDECLARELABEL(lb)
#define YYENDDECLARELABEL
#endif
/* LABEL DECLARATION */
YYBEGINDECLARELABEL
  YYDECLARELABEL(yynewstate)
  YYDECLARELABEL(yybackup)
/* YYDECLARELABEL(yyresume) */
  YYDECLARELABEL(yydefault)
  YYDECLARELABEL(yyreduce)
  YYDECLARELABEL(yyerrlab)   /* here on detecting error */
  YYDECLARELABEL(yyerrlab1)   /* here on error raised explicitly by an action */
  YYDECLARELABEL(yyerrdefault)  /* current state does not do anything special for the error token. */
  YYDECLARELABEL(yyerrpop)   /* pop the current state because it cannot handle the error token */
  YYDECLARELABEL(yyerrhandle)
YYENDDECLARELABEL

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (YY_wwParser_CHAR = YYEMPTY)
#define YYEMPTY         -2
#define YYEOF           0
#define YYACCEPT        return(0)
#define YYABORT         return(1)
#define YYERROR         YYGOTO(yyerrlab1)
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL          YYGOTO(yyerrlab)
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do                                                              \
  if (YY_wwParser_CHAR == YYEMPTY && yylen == 1)                               \
    { YY_wwParser_CHAR = (token), YY_wwParser_LVAL = (value);                 \
      yychar1 = YYTRANSLATE (YY_wwParser_CHAR);                                \
      YYPOPSTACK;                                               \
      YYGOTO(yybackup);                                            \
    }                                                           \
  else                                                          \
    { YY_wwParser_ERROR ("syntax error: cannot back up"); YYERROR; }   \
while (0)

#define YYTERROR        1
#define YYERRCODE       256

#ifndef YY_wwParser_PURE
/* UNPURE */
#define YYLEX           YY_wwParser_LEX()
#ifndef YY_USE_CLASS
/* If nonreentrant, and not class , generate the variables here */
int     YY_wwParser_CHAR;                      /*  the lookahead symbol        */
YY_wwParser_STYPE      YY_wwParser_LVAL;              /*  the semantic value of the */
				/*  lookahead symbol    */
int YY_wwParser_NERRS;                 /*  number of parse errors so far */
#ifdef YY_wwParser_LSP_NEEDED
YY_wwParser_LTYPE YY_wwParser_LLOC;   /*  location data for the lookahead     */
			/*  symbol                              */
#endif
#endif


#else
/* PURE */
#ifdef YY_wwParser_LSP_NEEDED
#define YYLEX           YY_wwParser_LEX(&YY_wwParser_LVAL, &YY_wwParser_LLOC)
#else
#define YYLEX           YY_wwParser_LEX(&YY_wwParser_LVAL)
#endif
#endif
#ifndef YY_USE_CLASS
#if YY_wwParser_DEBUG != 0
int YY_wwParser_DEBUG_FLAG;                    /*  nonzero means print parse trace     */
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif
#endif



/*  YYINITDEPTH indicates the initial size of the parser's stacks       */

#ifndef YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif


#if __GNUC__ > 1                /* GNU C and GNU C++ define this.  */
#define __yy_bcopy(FROM,TO,COUNT)       __builtin_memcpy(TO,FROM,COUNT)
#else                           /* not GNU C or C++ */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */

#ifdef __cplusplus
static void __yy_bcopy (char *from, char *to, int count)
#else
#ifdef __STDC__
static void __yy_bcopy (char *from, char *to, int count)
#else
static void __yy_bcopy (from, to, count)
     char *from;
     char *to;
     int count;
#endif
#endif
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}
#endif

int
#ifdef YY_USE_CLASS
 YY_wwParser_CLASS::
#endif
     YY_wwParser_PARSE(YY_wwParser_PARSE_PARAM)
#ifndef __STDC__
#ifndef __cplusplus
#ifndef YY_USE_CLASS
/* parameter definition without protypes */
YY_wwParser_PARSE_PARAM_DEF
#endif
#endif
#endif
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YY_wwParser_STYPE *yyvsp;
  int yyerrstatus;      /*  number of tokens to shift before error messages enabled */
  int yychar1=0;          /*  lookahead token as an internal (translated) token number */

  short yyssa[YYINITDEPTH];     /*  the state stack                     */
  YY_wwParser_STYPE yyvsa[YYINITDEPTH];        /*  the semantic value stack            */

  short *yyss = yyssa;          /*  refer to the stacks thru separate pointers */
  YY_wwParser_STYPE *yyvs = yyvsa;     /*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YY_wwParser_LSP_NEEDED
  YY_wwParser_LTYPE yylsa[YYINITDEPTH];        /*  the location stack                  */
  YY_wwParser_LTYPE *yyls = yylsa;
  YY_wwParser_LTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YY_wwParser_PURE
  int YY_wwParser_CHAR;
  YY_wwParser_STYPE YY_wwParser_LVAL;
  int YY_wwParser_NERRS;
#ifdef YY_wwParser_LSP_NEEDED
  YY_wwParser_LTYPE YY_wwParser_LLOC;
#endif
#endif

  YY_wwParser_STYPE yyval;             /*  the variable used to return         */
				/*  semantic values from the action     */
				/*  routines                            */

  int yylen;
/* start loop, in which YYGOTO may be used. */
YYBEGINGOTO

#if YY_wwParser_DEBUG != 0
  if (YY_wwParser_DEBUG_FLAG)
    fprintf(stderr, "Starting parse\n");
#endif
  yystate = 0;
  yyerrstatus = 0;
  YY_wwParser_NERRS = 0;
  YY_wwParser_CHAR = YYEMPTY;          /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YY_wwParser_LSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
YYLABEL(yynewstate)

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YY_wwParser_STYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YY_wwParser_LSP_NEEDED
      YY_wwParser_LTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YY_wwParser_LSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YY_wwParser_LSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  YY_wwParser_ERROR("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_bcopy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YY_wwParser_STYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_bcopy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YY_wwParser_LSP_NEEDED
      yyls = (YY_wwParser_LTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_bcopy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YY_wwParser_LSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YY_wwParser_DEBUG != 0
      if (YY_wwParser_DEBUG_FLAG)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YY_wwParser_DEBUG != 0
  if (YY_wwParser_DEBUG_FLAG)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  YYGOTO(yybackup);
YYLABEL(yybackup)

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* YYLABEL(yyresume) */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    YYGOTO(yydefault);

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (YY_wwParser_CHAR == YYEMPTY)
    {
#if YY_wwParser_DEBUG != 0
      if (YY_wwParser_DEBUG_FLAG)
	fprintf(stderr, "Reading a token: ");
#endif
      YY_wwParser_CHAR = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (YY_wwParser_CHAR <= 0)           /* This means end of input. */
    {
      yychar1 = 0;
      YY_wwParser_CHAR = YYEOF;                /* Don't call YYLEX any more */

#if YY_wwParser_DEBUG != 0
      if (YY_wwParser_DEBUG_FLAG)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(YY_wwParser_CHAR);

#if YY_wwParser_DEBUG != 0
      if (YY_wwParser_DEBUG_FLAG)
	{
	  fprintf (stderr, "Next token is %d (%s", YY_wwParser_CHAR, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, YY_wwParser_CHAR, YY_wwParser_LVAL);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    YYGOTO(yydefault);

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	YYGOTO(yyerrlab);
      yyn = -yyn;
      YYGOTO(yyreduce);
    }
  else if (yyn == 0)
    YYGOTO(yyerrlab);

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YY_wwParser_DEBUG != 0
  if (YY_wwParser_DEBUG_FLAG)
    fprintf(stderr, "Shifting token %d (%s), ", YY_wwParser_CHAR, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (YY_wwParser_CHAR != YYEOF)
    YY_wwParser_CHAR = YYEMPTY;

  *++yyvsp = YY_wwParser_LVAL;
#ifdef YY_wwParser_LSP_NEEDED
  *++yylsp = YY_wwParser_LLOC;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  YYGOTO(yynewstate);

/* Do the default action for the current state.  */
YYLABEL(yydefault)

  yyn = yydefact[yystate];
  if (yyn == 0)
    YYGOTO(yyerrlab);

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
YYLABEL(yyreduce)
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YY_wwParser_DEBUG != 0
  if (YY_wwParser_DEBUG_FLAG)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
{ create_pushsym(yyvsp[0].s_val);;
    break;}
case 2:
{ create_pushsym(yyvsp[0].s_val);  create_sysinput(yyvsp[0].s_val); ;
    break;}
case 3:
{ create_pushsym(yyvsp[0].s_val);  create_sysinputs(yyvsp[0].s_val); ;
    break;}
case 4:
{ create_pushdbl(yyvsp[0].f_val);;
    break;}
case 5:
{ create_pushstr(yyvsp[0].s_val);;
    break;}
case 8:
{create_postinc();;
    break;}
case 9:
{create_postdec();;
    break;}
case 10:
{create_array_ref();;
    break;}
case 15:
{create_unary_op(INC_OP);;
    break;}
case 16:
{create_unary_op(DEC_OP);;
    break;}
case 17:
{create_unary_op('+'); ;
    break;}
case 18:
{create_unary_op('-'); ;
    break;}
case 19:
{create_unary_op('!'); ;
    break;}
case 22:
{create_dblbin('*');;
    break;}
case 23:
{create_dblbin('/');;
    break;}
case 24:
{create_dblbin('%');;
    break;}
case 26:
{create_dblbin('+');;
    break;}
case 27:
{create_dblbin('-');;
    break;}
case 30:
{create_reop('<');;
    break;}
case 31:
{create_reop('>');;
    break;}
case 32:
{create_reop(LE_OP);;
    break;}
case 33:
{create_reop(GE_OP);;
    break;}
case 35:
{create_reop(EQ_OP);;
    break;}
case 36:
{create_reop(NE_OP);;
    break;}
case 41:
{create_logicalop(AND_OP);;
    break;}
case 43:
{create_logicalop(OR_OP);;
    break;}
case 47:
{create_assign();;
    break;}
case 50:
{create_pop();;
    break;}
case 65:
{create_pop();;
    break;}
case 66:
{  poplabel(selection_stack);;
    break;}
case 67:
{ pushlabel_jump(selection_stack); stackswap(selection_stack); poplabel(selection_stack);;
    break;}
case 68:
{ poplabel(selection_stack); ;
    break;}
case 69:
{  create_decide(); 
		 pushlabel_jump(selection_stack);  /* --> 1 */ 
	  ;
    break;}
case 72:
{create_pop();;
    break;}
case 73:
{pushlabel(iteration_stack);       /*    -->1 */ ;
    break;}
case 74:
{ create_decide(); 
	    pushlabel_jump(iteration_stack); /*  -->2-1 */
	    stackswap(iteration_stack);      /*     1-2 */ 
	    pushlabel_jump(iteration_stack); /*-->3-1-2 */
	    stackswap(iteration_stack);      /*   1-3-2 */
	  ;
    break;}
case 75:
{
	  	pushlabel(iteration_stack);   /* -->4-1-3-2 */
	  	stackswap(iteration_stack);   /*    1-4-3-2 */ 
	  ;
    break;}
case 76:
{create_pop();;
    break;}
case 77:
{ 
	  	poplabel_jump(iteration_stack);/*1 <--4-3-2 */
	  ;
    break;}
case 78:
{
	  	stackswap(iteration_stack);   /*      3-4-2 */
	  	poplabel(iteration_stack);    /*   3 <--4-2 */
	  	                    /* 堆栈内剩下两个LABEL: top: 继续循环 top-1: 循环结束*/
	  ;
    break;}
case 79:
{
	  	poplabel_jump(iteration_stack);  /* 4 <-- 2*/      
	  	poplabel(iteration_stack);       /* 2 <--  */
	  ;
    break;}
case 80:
{ pushlabel(iteration_stack); ;
    break;}
case 81:
{ create_decide(); pushlabel_jump(iteration_stack); stackswap(iteration_stack);;
    break;}
case 82:
{
	  	poplabel_jump(iteration_stack);        
	  	poplabel(iteration_stack);       
	  ;
    break;}
case 83:
{
		pushlabel(iteration_stack); 
		pushnolabel(iteration_stack); 
		pushnolabel(iteration_stack); 
		;
    break;}
case 84:
{
	    	/*continue入口*/
	    	poplabel(iteration_stack); 
	    ;
    break;}
case 85:
{
	    	create_decide();
	    	nopop_jump(iteration_stack);
	    	stackswap(iteration_stack);
	    	poplabel_jump(iteration_stack);
	    ;
    break;}
case 86:
{ poplabel(iteration_stack);;
    break;}
case 87:
{create_break();;
    break;}
case 88:
{create_continue();;
    break;}
case 91:
{ cur_type = yyvsp[0].i_val;;
    break;}
case 93:
{yyval.i_val=yyvsp[0].i_val;;
    break;}
case 96:
{create_symbol_def(cur_type, yyvsp[0].s_val);;
    break;}
case 97:
{create_symbol_array_def(cur_type, yyvsp[-2].s_val);;
    break;}
case 98:
{yyval.i_val=TYPE_STRING;;
    break;}
case 99:
{yyval.i_val=TYPE_INT;;
    break;}
case 100:
{yyval.i_val=TYPE_DOUBLE;;
    break;}
case 101:
{create_output(yyvsp[-4].s_val);;
    break;}
case 102:
{create_output("");;
    break;}
case 103:
{create_prepare_input(); ;
    break;}
case 104:
{create_input();;
    break;}
case 107:
{create_input_param(yyvsp[-2].s_val);;
    break;}
}
   /* the action file gets copied in in place of this dollarsign  */
  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YY_wwParser_LSP_NEEDED
  yylsp -= yylen;
#endif

#if YY_wwParser_DEBUG != 0
  if (YY_wwParser_DEBUG_FLAG)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YY_wwParser_LSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = YY_wwParser_LLOC.first_line;
      yylsp->first_column = YY_wwParser_LLOC.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  YYGOTO(yynewstate);

YYLABEL(yyerrlab)   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++YY_wwParser_NERRS;

#ifdef YY_wwParser_ERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      YY_wwParser_ERROR(msg);
	      free(msg);
	    }
	  else
	    YY_wwParser_ERROR ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YY_wwParser_ERROR_VERBOSE */
	YY_wwParser_ERROR("parse error");
    }

  YYGOTO(yyerrlab1);
YYLABEL(yyerrlab1)   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (YY_wwParser_CHAR == YYEOF)
	YYABORT;

#if YY_wwParser_DEBUG != 0
      if (YY_wwParser_DEBUG_FLAG)
	fprintf(stderr, "Discarding token %d (%s).\n", YY_wwParser_CHAR, yytname[yychar1]);
#endif

      YY_wwParser_CHAR = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;              /* Each real token shifted decrements this */

  YYGOTO(yyerrhandle);

YYLABEL(yyerrdefault)  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) YYGOTO(yydefault);
#endif

YYLABEL(yyerrpop)   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YY_wwParser_LSP_NEEDED
  yylsp--;
#endif

#if YY_wwParser_DEBUG != 0
  if (YY_wwParser_DEBUG_FLAG)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

YYLABEL(yyerrhandle)

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    YYGOTO(yyerrdefault);

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    YYGOTO(yyerrdefault);

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	YYGOTO(yyerrpop);
      yyn = -yyn;
      YYGOTO(yyreduce);
    }
  else if (yyn == 0)
    YYGOTO(yyerrpop);

  if (yyn == YYFINAL)
    YYACCEPT;

#if YY_wwParser_DEBUG != 0
  if (YY_wwParser_DEBUG_FLAG)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = YY_wwParser_LVAL;
#ifdef YY_wwParser_LSP_NEEDED
  *++yylsp = YY_wwParser_LLOC;
#endif

  yystate = yyn;
  YYGOTO(yynewstate);
/* end loop, in which YYGOTO may be used. */
  YYENDGOTO
}

/* END */




