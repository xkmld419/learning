/*VER: 1*/ 
# include <stdio.h>
# include <stddef.h>
# include <locale.h>
# include <stdlib.h>
# define U(x) ((x)&0377)
# define NCH 256
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 2000

#ifdef __cplusplus
int yylook(void);
extern "C" int yywrap(void), yyless(int), yyreject(void);
#endif /* __cplusplus */

#if defined(__cplusplus) && defined(_CPP_IOSTREAMS)
# include <iostream.h>
# define output(c) (*yyout) << ((unsigned char) c)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):yyin->get())==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define ECHO (*yyout) << yytext
istream *yyin = &cin;
ostream *yyout = &cout;
#else
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define ECHO fprintf(yyout, "%S",yywtext)
FILE *yyin = NULL, *yyout = NULL;
#endif /* defined(__cplusplus) && defined(_CPP_IOSTREAMS) */
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define REJECT { yynstr = yyreject(); goto yyfussy;}
# define yysetlocale() setlocale(LC_ALL,"")
# define wreturn(r) return(yywreturn(r))
# define winput() yywinput()
# define wunput(c) yywunput(c)
# define woutput(c) yywoutput(c)
int yyleng;
extern char yytext[];
int yywleng; extern wchar_t yywtext[];
int yymorfg;
int yymbcurmax = -1;
int __once_yylex = 1;
extern unsigned char *yysptr, yysbuf[];
int yytchar;
extern int yylineno;
struct yywork;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
  Copyright by Wholewise  2002
  File Name: wwLex.h
  Created Date: 2002.08.01
  Author: xiaozc
  Version: 0.0.01
  Create Version: 0.0.01
  Last Version Date: 2002.08.01
  Create Version Date: 2002.08.01
 ***********************************************/
#include <stdlib.h>
#include <list>
#include "wwParser.h"
		for ( i1 = str_list.begin(); i1!= str_list.end(); i1++){\
			free(*i1);\
		};\
char yytext[YYLMAX];
int yyback(int *yyp, int yym);
# if YYHSIZE
int yyhlook(int yyc, int yyv);
int yymlook(int yyc);
# endif /*YYHSIZE*/
# if YYXSIZE
int yyxlook (int yyc, int yyv);
#endif /*YYXSIZE*/
int yywinput();
void yywoutput(int yyc);
void yywunput(int yyc);
int yywreturn(int yyr);
int yyinput();
void yyoutput(int yyc);
void yyunput(int yyc);
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
int yymbinput();
void yymboutput(int yyc);
void yymbunput(int yyc);
int yymbreturn(int yyx);
#ifdef __cplusplus
}
#endif /* __cplusplus */
# define YYNEWLINE 10
#ifdef __cplusplus
extern "C"
#endif /*__cplusplus */
int yylex(){
int yynstr; extern int yyprevious;
if (__once_yylex) {
      yysetlocale();
#if !(__cplusplus && _CPP_IOSTREAMS)
      if (yyin == NULL) yyin = stdin;
      if (yyout == NULL) yyout = stdout;
#endif /* !(__cplusplus && _CPP_IOSTREAMS) */
      __once_yylex = 0; }
if(yymbcurmax<=0) yymbcurmax=MB_CUR_MAX;
while((yynstr = yylook()) >= 0)
yyfussy: switch(yynstr){
case 0:
if(yywrap()) return(0); break;
case 1:
		{ comment(); }
/*NOTREACHED*/ break;
case 2:
		{ wwCount(); return(wwParser::BREAK); }
/*NOTREACHED*/ break;
case 3:
	{ wwCount(); return(wwParser::CONTINUE); }
/*NOTREACHED*/ break;
case 4:
		{ wwCount(); return(wwParser::ELSE); }
/*NOTREACHED*/ break;
case 5:
		{ wwCount(); return(wwParser::IF); }
/*NOTREACHED*/ break;
case 6:
		{ wwCount(); return(wwParser::FOR); }
/*NOTREACHED*/ break;
case 7:
		{ wwCount(); return(wwParser::WHILE); }
/*NOTREACHED*/ break;
case 8:
		{ wwCount(); return(wwParser::DO); }
/*NOTREACHED*/ break;
case 9:
		{ wwCount(); return(wwParser::TYPE_INT); }
/*NOTREACHED*/ break;
case 10:
	{ wwCount(); return(wwParser::TYPE_DOUBLE); }
/*NOTREACHED*/ break;
case 11:
	{ wwCount(); return(wwParser::TYPE_STRING); }
/*NOTREACHED*/ break;
case 12:
	{ wwCount(); return(wwParser::OUTPUT);}
/*NOTREACHED*/ break;
case 13:
		{ wwCount(); return(wwParser::INPUT);}
/*NOTREACHED*/ break;
case 14:
    { wwCount();char * s =strdup((char *)yytext);  val->s_val=s; str_list.push_back(s) ; return(wwParser::SYS_INPUT);}
/*NOTREACHED*/ break;
case 15:
    { wwCount();char * s =strdup((char *)yytext);  val->s_val=s; str_list.push_back(s) ; return(wwParser::SYS_INPUTS);}
/*NOTREACHED*/ break;
case 16:
	{ wwCount(); char * s =strdup((char *)yytext);  val->s_val=s; str_list.push_back(s) ;return(wwParser::IDENTIFIER); }
/*NOTREACHED*/ break;
case 17:
	{ wwCount(); val->f_val = atof((char *)yytext); return(wwParser::CONSTANT); }
/*NOTREACHED*/ break;
case 18:
	{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }
/*NOTREACHED*/ break;
case 19:
	{ wwCount();  val->f_val = atof((char *)yytext);   return(wwParser::CONSTANT); }
/*NOTREACHED*/ break;
case 20:
	{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }
/*NOTREACHED*/ break;
case 21:
	{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }
/*NOTREACHED*/ break;
case 22:
	{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }
/*NOTREACHED*/ break;
case 23:
	{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }
/*NOTREACHED*/ break;
case 24:
	{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }
/*NOTREACHED*/ break;
case 25:
{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }
/*NOTREACHED*/ break;
case 26:
{ wwCount(); val->f_val = atof((char *)yytext);return(wwParser::CONSTANT); }
/*NOTREACHED*/ break;
case 27:
{ wwCount(); yytext[ strlen( (char *)yytext)-1]='\0';yytext[0]='\0'; char * s =strdup((char *)yytext+1);  val->s_val=s; str_list.push_back(s) ;  return(wwParser::STRING_LITERAL); }
/*NOTREACHED*/ break;
case 28:
		{ wwCount(); return(wwParser::INC_OP); }
/*NOTREACHED*/ break;
case 29:
		{ wwCount(); return(wwParser::DEC_OP); }
/*NOTREACHED*/ break;
case 30:
		{ wwCount(); return(wwParser::AND_OP); }
/*NOTREACHED*/ break;
case 31:
		{ wwCount(); return(wwParser::OR_OP); }
/*NOTREACHED*/ break;
case 32:
		{ wwCount(); return(wwParser::LE_OP); }
/*NOTREACHED*/ break;
case 33:
		{ wwCount(); return(wwParser::GE_OP); }
/*NOTREACHED*/ break;
case 34:
		{ wwCount(); return(wwParser::EQ_OP); }
/*NOTREACHED*/ break;
case 35:
		{ wwCount(); return(wwParser::NE_OP); }
/*NOTREACHED*/ break;
case 36:
		{ wwCount(); return(';'); }
/*NOTREACHED*/ break;
case 37:
		{ wwCount(); return('{'); }
/*NOTREACHED*/ break;
case 38:
		{ wwCount(); return('}'); }
/*NOTREACHED*/ break;
case 39:
		{ wwCount(); return(','); }
/*NOTREACHED*/ break;
case 40:
		{ wwCount(); return(':'); }
/*NOTREACHED*/ break;
case 41:
		{ wwCount(); return('='); }
/*NOTREACHED*/ break;
case 42:
		{ wwCount(); return('('); }
/*NOTREACHED*/ break;
case 43:
		{ wwCount(); return(')'); }
/*NOTREACHED*/ break;
case 44:
		{ wwCount(); return('['); }
/*NOTREACHED*/ break;
case 45:
		{ wwCount(); return(']'); }
/*NOTREACHED*/ break;
case 46:
		{ wwCount(); return('!'); }
/*NOTREACHED*/ break;
case 47:
		{ wwCount(); return('-'); }
/*NOTREACHED*/ break;
case 48:
		{ wwCount(); return('+'); }
/*NOTREACHED*/ break;
case 49:
		{ wwCount(); return('*'); }
/*NOTREACHED*/ break;
case 50:
		{ wwCount(); return('/'); }
/*NOTREACHED*/ break;
case 51:
		{ wwCount(); return('%'); }
/*NOTREACHED*/ break;
case 52:
		{ wwCount(); return('<'); }
/*NOTREACHED*/ break;
case 53:
		{ wwCount(); return('>'); }
/*NOTREACHED*/ break;
case 54:
		{ wwCount(); return('.'); }
/*NOTREACHED*/ break;
case 55:
	{ wwCount(); }
/*NOTREACHED*/ break;
case 56:
		{ /* ignore bad characters */ }
/*NOTREACHED*/ break;
case -1:
break;
default:
#if defined(__cplusplus) && defined(_CPP_IOSTREAMS)
(*yyout) << "bad switch yylook " << yynstr;
#else
fprintf(yyout,"bad switch yylook %d",yynstr);
#endif /* defined(__cplusplus) && defined(_CPP_IOSTREAMS)*/
} return(0); }
/* end of yylex */

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

