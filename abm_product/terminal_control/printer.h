/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident	"@(#)printer.h	1.0	2001/02/01 ruanyongjiang" */

#ifndef    _PRINTER_H
#define    _PRINTER_H

#define    OPENLP         { system("stty ixon ixany");\
                             printf("\033[5i"); \
                           }
#define    CLOSELP         { printf("\033[4i");  }
#define    SETLINE(n)      printf("\033J%c",n)   /*设定行间距*/
#define    OUTPAPER        printf("");         /* 退纸 */
#define    HIGHFONT        printf("\033c")       /* 高字*/
#define    UHIGHFONT       printf("\033d")       /* 解 除*/
#define    BIGFONT         printf("\033h")       /* 大字*/
#define    UBIGFONT        printf("\033d")       /* 解 除*/

#endif

