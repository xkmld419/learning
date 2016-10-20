/************************************************
  Copyright by Wholewise  2002
  File Name: wwParser.y
  Created Date: 2002.08.01
  Author: xiaozc
  Version: 0.0.01
  Create Version: 0.0.01
  Last Version Date: 2002.08.01
  Create Version Date: 2002.08.01
 ***********************************************/

%name wwParser


%header{
#include <iostream>
#include <stdio.h>
#include <string>
#include "wwInterpreter.h"
%}

%union {
  double f_val;          /* float number */
  int    i_val;              /* number of newlines */
  char * s_val;
}

%type <f_val> CONSTANT 
%type <s_val> IDENTIFIER STRING_LITERAL 
%type <i_val> assignment_operator

%type <i_val> TYPE_STRING TYPE_INT TYPE_DOUBLE
%type <i_val> type_specifier declaration_specifiers
%type <s_val> declarator
%type <s_val> SYS_INPUT SYS_INPUTS
/*类继承成员定义*/
%define MEMBERS public:  int cur_type; \
~wwParser(){};


%define INHERIT :public Interpreter 

%define LSP_NEEDED
%define LEX_BODY=0
%define ERROR_BODY =0
%define CONSTRUCTOR_CODE  yylloc.first_line =1; yylloc.first_column =1; 


%token IDENTIFIER CONSTANT STRING_LITERAL
%token INC_OP DEC_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP type_name

%token IF ELSE SWITCH CASE BREAK DEFAULT CONTINUE

%token FOR WHILE DO

%token TYPE_INT TYPE_DOUBLE TYPE_STRING

%token OUTPUT INPUT

%token SYS_INPUT SYS_INPUTS

%start compound_statement

%%



primary_expr
	: IDENTIFIER  { create_pushsym($1);}
	| SYS_INPUT   { create_pushsym($1);  create_sysinput($1); }
	| SYS_INPUTS   { create_pushsym($1);  create_sysinputs($1); }
	| CONSTANT    { create_pushdbl($1);}
	| STRING_LITERAL { create_pushstr($1);}
	| '(' expr ')'
	;

postfix_expr
	: primary_expr
	| postfix_expr INC_OP  {create_postinc();}
	| postfix_expr DEC_OP  {create_postdec();}
	| postfix_expr '[' expr ']'  {create_array_ref();}
	| postfix_expr '(' argument_expr_list ')'
	;

argument_expr_list
	: assignment_expr
	| argument_expr_list ',' assignment_expr
	;

unary_expr
	: postfix_expr
	| INC_OP unary_expr {create_unary_op(INC_OP);}
	| DEC_OP unary_expr {create_unary_op(DEC_OP);}
	| '+' cast_expr {create_unary_op('+'); }
	| '-' cast_expr {create_unary_op('-'); }
	| '!' cast_expr {create_unary_op('!'); }
	;

cast_expr
	: unary_expr
	;


multiplicative_expr
	: cast_expr
	| multiplicative_expr '*' cast_expr {create_dblbin('*');}
	| multiplicative_expr '/' cast_expr {create_dblbin('/');}
	| multiplicative_expr '%' cast_expr {create_dblbin('%');}
	;

additive_expr
	: multiplicative_expr
	| additive_expr '+' multiplicative_expr {create_dblbin('+');}
	| additive_expr '-' multiplicative_expr {create_dblbin('-');}
	;

shift_expr
	: additive_expr
	;

relational_expr
	: shift_expr
	| relational_expr '<' shift_expr   {create_reop('<');}
	| relational_expr '>' shift_expr   {create_reop('>');}
	| relational_expr LE_OP shift_expr {create_reop(LE_OP);}
	| relational_expr GE_OP shift_expr {create_reop(GE_OP);}
	;

equality_expr
	: relational_expr
	| equality_expr EQ_OP relational_expr {create_reop(EQ_OP);}
	| equality_expr NE_OP relational_expr {create_reop(NE_OP);}
	;

and_expr
	: equality_expr
	;

exclusive_or_expr
	: and_expr
	;

inclusive_or_expr
	: exclusive_or_expr
	;

logical_and_expr
	: inclusive_or_expr
	| logical_and_expr AND_OP inclusive_or_expr {create_logicalop(AND_OP);}
	;

logical_or_expr
	: logical_and_expr
	| logical_or_expr OR_OP logical_and_expr {create_logicalop(OR_OP);}
	;

conditional_expr
	: logical_or_expr
	|input_statement
	;

assignment_expr
	: conditional_expr
	| unary_expr assignment_operator  assignment_expr {create_assign();}
	;

assignment_operator
	: '='  
	;

expr
	: assignment_expr 
	| expr ',' {create_pop();} assignment_expr 
	;


statement
	: compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	| output_statement
	;


compound_statement
	: '{' '}'
	| '{' statement_list '}'
	| '{' declaration_list '}'
	| '{' declaration_list statement_list '}'
	;
	

statement_list
	: statement
	| statement_list statement
	;

expression_statement
	: ';'
	| expr ';' {create_pop();}
	;

selection_statement
	: IF '(' expr ')' jump_op statement  {  poplabel(selection_stack);}
	| IF '(' expr ')' jump_op statement ELSE { pushlabel_jump(selection_stack); stackswap(selection_stack); poplabel(selection_stack);} statement { poplabel(selection_stack); }
	;

jump_op
	: {  create_decide(); 
		 pushlabel_jump(selection_stack);  /* --> 1 */ 
	  }
	;

iteration_statement
	: for_loop
	| while_loop
	;
	
for_loop
	: FOR '(' 
	  expr ';' {create_pop();}
	  /*判断部分*/
	  {pushlabel(iteration_stack);       /*    -->1 */ } 
	  expr 
	  { create_decide(); 
	    pushlabel_jump(iteration_stack); /*  -->2-1 */
	    stackswap(iteration_stack);      /*     1-2 */ 
	    pushlabel_jump(iteration_stack); /*-->3-1-2 */
	    stackswap(iteration_stack);      /*   1-3-2 */
	  } 
	  ';'
	  /*增加部分*/
	  {
	  	pushlabel(iteration_stack);   /* -->4-1-3-2 */
	  	stackswap(iteration_stack);   /*    1-4-3-2 */ 
	  } 
	  expr 
	  {create_pop();}
	  { 
	  	poplabel_jump(iteration_stack);/*1 <--4-3-2 */
	  } 
	  ')' 
	  /*执行语句部分*/ 
	  {
	  	stackswap(iteration_stack);   /*      3-4-2 */
	  	poplabel(iteration_stack);    /*   3 <--4-2 */
	  	                    /* 堆栈内剩下两个LABEL: top: 继续循环 top-1: 循环结束*/
	  }  
	  statement
	  /*语句结束部分*/
	  {
	  	poplabel_jump(iteration_stack);  /* 4 <-- 2*/      
	  	poplabel(iteration_stack);       /* 2 <--  */
	  }


while_loop
	: WHILE '(' 
		{ pushlabel(iteration_stack); }
	   expr  
	    { create_decide(); pushlabel_jump(iteration_stack); stackswap(iteration_stack);} 
	  ')' 
	  statement
	  {
	  	poplabel_jump(iteration_stack);        
	  	poplabel(iteration_stack);       
	  }
	| DO {
		pushlabel(iteration_stack); 
		pushnolabel(iteration_stack); 
		pushnolabel(iteration_stack); 
		}
	   statement 
	   WHILE '('
	    {
	    	/*continue入口*/
	    	poplabel(iteration_stack); 
	    } 
	   expr
	    {
	    	create_decide();
	    	nopop_jump(iteration_stack);
	    	stackswap(iteration_stack);
	    	poplabel_jump(iteration_stack);
	    }
	   ')' ';'
	    { poplabel(iteration_stack);}
	;

jump_statement
	: BREAK ';'    {create_break();}
	| CONTINUE ';' {create_continue();}
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

declaration
	: declaration_specifiers { cur_type = $1;} declarator_list ';'
	;

declaration_specifiers
	: type_specifier {$$=$1;}
	;

declarator_list
	: declarator
	| declarator_list ',' declarator
	;

declarator
	: IDENTIFIER         {create_symbol_def(cur_type, $1);}
	| IDENTIFIER '[' ']' {create_symbol_array_def(cur_type, $1);}
	;

type_specifier
	: TYPE_STRING  {$$=TYPE_STRING;}
	| TYPE_INT     {$$=TYPE_INT;}
	| TYPE_DOUBLE  {$$=TYPE_DOUBLE;}
	;

output_statement
	: OUTPUT  '(' STRING_LITERAL  ','  assignment_expr  ')'  ';'  {create_output($3);}
	| OUTPUT  '(' assignment_expr  ')'  ';'  {create_output("");}
	;

input_statement
	: INPUT {create_prepare_input(); } '(' input_argument_list ')' {create_input();} 
	;

input_argument_list
	: input_argument_pair 
	| input_argument_list ',' input_argument_pair
	;

input_argument_pair
	: STRING_LITERAL ',' assignment_expr {create_input_param($1);}
	;

%%



