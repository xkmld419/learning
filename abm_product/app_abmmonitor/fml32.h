/*
	Copyright (c) 2005 BEA Systems, Inc.
	All rights reserved

	THIS IS UNPUBLISHED PROPRIETARY
	SOURCE CODE OF BEA Systems, Inc.
	The copyright notice above does not
	evidence any actual or intended
	publication of such source code.
*/

/*	Copyright (c) 1993 USL
	All rights reserved

*/
#ifndef FML32_H
#define FML32_H

/* #ident	"@(#) fml/libfml/fml32.h	$Revision: 1.14 $" */
#ifndef TMENV_H
#include <tmenv.h>
#endif
#ifndef NOWHAT
static	char	h_fml32[] = "@(#) fml/libfml/fml32.h	$Revision: 1.14 $";
#endif

/*
 *	DEFINITIONS NEEDED BY USER APPLICATION PROGRAMS.
 *
 *	Warning: This header file should not be changed in any
 *	way; doing so will destroy the compatibility with TUXEDO programs
 *	and libraries.
 */

/*
 *	---------- Definitions and Macros ----------
 */

#define MAXFBLEN32	0x7ffffffe	/* Maximum FBFR3232 length */
#ifndef FML_H
#define FSTDXINT	16		/* Default indexing interval */
#define FMAXNULLSIZE	2660
#define FVIEWCACHESIZE	128
#define FVIEWNAMESIZE	33

/* operations presented to _Fmodidx function */
#define FADD	1
#define FMLMOD	2
#define FDEL	3

/* Flag options used in Fvstof() */
#define F_OFFSET	1
#define F_SIZE		2
#define F_PROP		4			/* P */
#define F_FTOS		8			/* S */
#define F_STOF		16			/* F */
#define F_BOTH		(F_STOF | F_FTOS)	/* S,F */
#define F_OFF		0			/* Z */
#define F_LENGTH        32                      /* L */
#define F_COUNT         64                      /* C */
#define F_NONE          128                     /* NONE flag for null value */

/* These are used in Fstof() */
#define FUPDATE		1
#define FCONCAT		2
#define FJOIN		3
#define FOJOIN		4

/* Flags used in Fmbpack32(),Fmbupack32() */
#define FBUFENC		1
#define FTERMENC	2

/* field types */
#define FLD_SHORT	0	/* short int */
#define FLD_LONG	1	/* long int */
#define FLD_CHAR	2	/* character */
#define FLD_FLOAT	3	/* single-precision float */
#define FLD_DOUBLE	4	/* double-precision float */
#define FLD_STRING	5	/* string - null terminated */
#define FLD_CARRAY	6	/* character array */
				/* 7 reserved for FLD_INT (see viewmap.h) */
				/* 8 reserved for FLD_DECIMAL (see viewmap.h) */
/* field types for FML32 only */
#define	FLD_PTR		9	/* pointer */
#define	FLD_FML32	10	/* embedded FML32 buffer */
#define FLD_VIEW32	11	/* embedded VIEW32 */
#define FLD_MBSTRING	12	/* embedded multibyte codeset info */
#define FLD_FML		13 /* for embedded ptr to FML buffer */

/* invalid field id - returned from functions where field id not found */
#define BADFLDID (FLDID32)0
/* define an invalid field id used for first call to Fnext */
#define FIRSTFLDID (FLDID32)0
#endif	/* FML_H */

/* Field Error Codes - these correspond to the error messages in
 *			F_error.c - make sure to update the error
 *			message list if a new error is added
 */
#ifndef FML_H
#define FMINVAL 0		/* bottom of error message codes */
#define FALIGNERR 1		/* fielded buffer not aligned */
#define FNOTFLD 2		/* buffer not fielded */
#define FNOSPACE 3		/* no space in fielded buffer */
#define FNOTPRES 4		/* field not present */
#define FBADFLD 5		/* unknown field number or type */
#define FTYPERR 6               /* illegal field type */
#define FEUNIX 7		/* unix system call error */
#define FBADNAME 8		/* unknown field name */
#define FMALLOC 9		/* malloc failed */
#define FSYNTAX 10		/* bad syntax in boolean expression */
#define FFTOPEN 11		/* cannot find or open field table */
#define FFTSYNTAX 12		/* syntax error in field table */
#define FEINVAL 13		/* invalid argument to function */
#define FBADTBL 14      	/* destructive concurrent access to field
				   table */
#define FBADVIEW 15		/* cannot find or get view */
#define FVFSYNTAX 16		/* bad viewfile */
#define FVFOPEN 17		/* cannot find or open viewfile */
#define FBADACM 18              /* ACM contains negative value */
#define FNOCNAME 19             /* cname not found */
#define FEBADOP 20		/* operation invalid for field type */
#define FMAXVAL  21 		/* top of error message codes */

/* Defines for parser flags */
#define TPXPARSNEVER			0x0001
#define TPXPARSALWAYS			0x0002
#define TPXPARSSCHFULL			0x0004
#define TPXPARSCONFATAL			0x0008
#define TPXPARSNSPACE			0x0010
#define TPXPARSDOSCH			0x0020
#define TPXPARSEREFN			0x0040
#define TPXPARSNOEXIT			0x0080
#define TPXPARSNOINCWS			0x0100
#define TPXPARSCACHESET			0x0200
#define TPXPARSCACHERESET		0x0400
#define TPXPARSSTDURI			0x0800		/* TPXPARSFILE use only */
#define TPXPARSUSECACHEDGRAM		0x1000		/* TPXPARSFILE use only */
#define TPXMLALLOWEMPTY			0x2000		/* None parser flag, ATMI use only */
#define MINBUFSIZE			256
#define BUFSIZE				512
#define BLOCKBUFSIZE			4096
struct parser_option{
	long	flags;				/* parser flags the same with it in ATMI functions */
	short	scanner;			/* UseScanner */
	short	refresh;			/* need refresh? 0:not need; 1: need */
	char 	extsche[MINBUFSIZE];	/* ExternalSchemaLocation */
	char	nonssche[MINBUFSIZE];	/* NoNamespaceSchemaLocation */
	long 	datalen;			/* store the xml buf len in auto conversion */
};
#endif /* FML_H */



/*
 *	---------- Type Definitions ----------
 */

/* another typedef of FLDID/FLDLEN exists in tuxedo/include/tmbase.h */
#ifndef _FLDID32
#define _FLDID32
typedef TM32U FLDID32;
typedef TM32U FLDLEN32;
typedef TM32I FLDOCC32;
#endif
typedef struct Fbfr32 FBFR32;

typedef struct {
	TM32U vflags;			/* flags - currently unused */
	char vname[FVIEWNAMESIZE+1];	/* name of view */
	char *data;						/* pointer to view structure */
			
} FVIEWFLD;

#define FVIEWFLDOVHD FVIEWNAMESIZE

/*
 *	---------- External declarations ----------
 */

#if defined(__cplusplus)
extern "C" {
#endif

_TMIFML32 extern	char	*Femsgs32[];

#if defined(_TMDOWN) && !(defined(WIN32) && !defined(_TM_OLDNTWS))
/* handle case-insensitive link conflict for globals and functions */
#define Fread32(A,B) FMLread32(A,B)
#define Fwrite32(A,B) FMLwrite32(A,B)
#endif

#define Ferror32	(*_Fget_Ferror_addr32())
extern int _TM_FAR * _TMDLLENTRY _Fget_Ferror_addr32 _((void));

#if (defined(_TM_WIN) || defined(_TM_OS2) || (defined(WIN32) && defined(_TM_OLDNTWS))) && !defined(_TMDLL)

extern int _TMDLLENTRY getFerror32(void);

#endif

#if defined(__cplusplus)
}
#endif

#ifdef _TMPROTOTYPES
#include <stdio.h>
#endif



#if defined(__cplusplus)
extern "C" {
#endif

#if defined(_TMDOWN) && !(defined(WIN32) && !defined(_TM_OLDNTWS))
/* handle case-insensitive link conflict for and functions */
extern int _TMDLLENTRY FMLread32 _((FBFR32 _TM_FAR *, FILE _TM_FAR *));
extern int _TMDLLENTRY FMLwrite32 _((FBFR32 _TM_FAR *, FILE _TM_FAR *));
#else
extern int _TMDLLENTRY Fread32 _((FBFR32 *, FILE *));
extern int _TMDLLENTRY Fwrite32 _((FBFR32 *, FILE *));
#endif

extern int _TMDLLENTRY CFadd32 _((FBFR32 _TM_FAR *, FLDID32, char _TM_FAR*,
	FLDLEN32, int));
extern int _TMDLLENTRY CFchg32 _((FBFR32 _TM_FAR *, FLDID32, FLDOCC32,
	char _TM_FAR *, FLDLEN32, int ));
extern char _TM_FAR * _TMDLLENTRY CFfind32 _((FBFR32 _TM_FAR *, FLDID32,
	FLDOCC32, FLDLEN32 _TM_FAR *, int));
extern FLDOCC32 _TMDLLENTRY CFfindocc32 _((FBFR32 _TM_FAR *, FLDID32,
	char _TM_FAR *, FLDLEN32, int ));
extern int _TMDLLENTRY CFget32 _((FBFR32 _TM_FAR *, FLDID32, FLDOCC32,
	char _TM_FAR *, FLDLEN32 _TM_FAR *, int ));
extern char _TM_FAR * _TMDLLENTRY CFgetalloc32 _((FBFR32 _TM_FAR *, FLDID32,
	FLDOCC32, int, FLDLEN32 _TM_FAR *));
extern void _TMDLLENTRY F_error32 _((char *));
extern int _TMDLLENTRY Fadd32 _((FBFR32 _TM_FAR *, FLDID32, char _TM_FAR *,
	FLDLEN32));
extern FBFR32 _TM_FAR * _TMDLLENTRY Falloc32 _((FLDOCC32, FLDLEN32));
extern int _TMDLLENTRY Fappend32 _((FBFR32 _TM_FAR *, FLDID32, char _TM_FAR *,
	FLDLEN32));
extern FBFR32 _TM_FAR * _TMDLLENTRY Frealloc32 _((FBFR32 _TM_FAR *, FLDOCC32,
	FLDLEN32));
extern int _TMDLLENTRY Ffree32 _((FBFR32 _TM_FAR *));
extern int _TMDLLENTRY Fboolev32 _((FBFR32 _TM_FAR *, char _TM_FAR *));
extern int _TMDLLENTRY Fvboolev32 _((char _TM_FAR *, char _TM_FAR *, char _TM_FAR *));
extern double _TMDLLENTRY Ffloatev32 _((FBFR32 _TM_FAR *, char _TM_FAR *));
extern double _TMDLLENTRY Fvfloatev32 _((char _TM_FAR *, char _TM_FAR *, char _TM_FAR *));
extern void _TMDLLENTRY Fboolpr32 _((char _TM_FAR *, FILE _TM_FAR *));
extern int _TMDLLENTRY Fvboolpr32 _((char _TM_FAR *, FILE _TM_FAR *, char _TM_FAR *));
extern int _TMDLLENTRY Fchg32 _((FBFR32 _TM_FAR *, FLDID32, FLDOCC32,
	char _TM_FAR *, FLDLEN32));
extern long _TMDLLENTRY Fchksum32 _((FBFR32 _TM_FAR *));
extern int _TMDLLENTRY Fcmp32 _((FBFR32 _TM_FAR *, FBFR32 _TM_FAR *));
extern int _TMDLLENTRY Fconcat32 _((FBFR32 _TM_FAR *, FBFR32 _TM_FAR *));
extern int _TMDLLENTRY Fcpy32 _((FBFR32 _TM_FAR *,FBFR32 _TM_FAR *));
extern int _TMDLLENTRY Fdelall32 _((FBFR32 _TM_FAR *, FLDID32));
extern int _TMDLLENTRY Fdelete32 _((FBFR32 _TM_FAR *, FLDID32 _TM_FAR *));
extern int _TMDLLENTRY Fextread32 _((FBFR32 _TM_FAR *, FILE _TM_FAR *));
extern char _TM_FAR * _TMDLLENTRY Ffind32 _((FBFR32 _TM_FAR *, FLDID32,
	FLDOCC32, FLDLEN32 _TM_FAR *));
extern char _TM_FAR * _TMDLLENTRY Fvals32 _((FBFR32 _TM_FAR *, FLDID32,
	FLDOCC32 ));
extern long _TMDLLENTRY Fvall32 _((FBFR32 _TM_FAR *, FLDID32, FLDOCC32 ));
extern FLDOCC32 _TMDLLENTRY Ffindocc32 _((FBFR32 _TM_FAR *, FLDID32,
	char _TM_FAR *, FLDLEN32 ));
extern int _TMDLLENTRY Fget32 _((FBFR32 _TM_FAR *, FLDID32, FLDOCC32,
	char _TM_FAR *, FLDLEN32 _TM_FAR *));
extern char _TM_FAR * _TMDLLENTRY Fgetalloc32 _((FBFR32 _TM_FAR *, FLDID32,
	FLDOCC32, FLDLEN32 _TM_FAR *));
extern int _TMDLLENTRY Fldtype32 _((FLDID32));
extern FLDOCC32 _TMDLLENTRY Fldno32 _((FLDID32));
extern int _TMDLLENTRY Fielded32 _((FBFR32 _TM_FAR *));
extern long _TMDLLENTRY Fneeded32 _((FLDOCC32, FLDLEN32));
extern long _TMDLLENTRY Fused32 _((FBFR32 _TM_FAR *));
extern long _TMDLLENTRY Fidxused32 _((FBFR32 _TM_FAR *));
extern long _TMDLLENTRY Funused32 _((FBFR32 _TM_FAR *));
extern long _TMDLLENTRY Fsizeof32 _((FBFR32 _TM_FAR *));
extern FLDID32 _TMDLLENTRY Fmkfldid32 _((int, FLDID32 ));
extern FLDLEN32 _TMDLLENTRY Fieldlen32 _((char _TM_FAR *, FLDLEN32 _TM_FAR *,
	FLDLEN32 _TM_FAR *));
extern FLDOCC32 _TMDLLENTRY Funindex32 _((FBFR32 _TM_FAR *));
extern int _TMDLLENTRY Frstrindex32 _((FBFR32 _TM_FAR *, FLDOCC32 ));
extern int _TMDLLENTRY Findex32 _((FBFR32 _TM_FAR *, FLDOCC32 ));
extern int _TMDLLENTRY Finit32 _((FBFR32 _TM_FAR *, FLDLEN32));
extern int _TMDLLENTRY Fjoin32 _((FBFR32 _TM_FAR *, FBFR32 _TM_FAR *));
extern int _TMDLLENTRY Fojoin32 _((FBFR32 _TM_FAR *, FBFR32 _TM_FAR *));
extern char _TM_FAR * _TMDLLENTRY Ffindlast32 _((FBFR32 _TM_FAR *, FLDID32,
	FLDOCC32 _TM_FAR *, FLDLEN32 _TM_FAR *));
extern int _TMDLLENTRY Fgetlast32 _((FBFR32 _TM_FAR *, FLDID32,
	FLDOCC32 _TM_FAR *, char _TM_FAR *, FLDLEN32 _TM_FAR *));
extern long _TMDLLENTRY Flen32 _((FBFR32 _TM_FAR *, FLDID32, FLDOCC32 ));
extern int _TMDLLENTRY Fmbpack32 _((char _TM_FAR *, void _TM_FAR *,
	FLDLEN32, void _TM_FAR *, FLDLEN32 _TM_FAR *, long));
extern int _TMDLLENTRY Fmbunpack32 _((void _TM_FAR *, FLDLEN32,
	char _TM_FAR *, void _TM_FAR *, FLDLEN32 _TM_FAR *, long));
extern int _TMDLLENTRY Fmove32 _((char _TM_FAR *, FBFR32 _TM_FAR *));
extern int _TMDLLENTRY Fnext32 _((FBFR32 _TM_FAR *, FLDID32 _TM_FAR *,
	FLDOCC32 _TM_FAR *, char _TM_FAR *, FLDLEN32 _TM_FAR *));
extern FLDID32 _TMDLLENTRY Fldid32 _((char _TM_FAR *));
extern char _TM_FAR * _TMDLLENTRY Fname32 _((FLDID32));
extern char _TM_FAR * _TMDLLENTRY Ftype32 _((FLDID32));
extern void _TMDLLENTRY Fnmid_unload32 _((void));
extern void _TMDLLENTRY Fidnm_unload32 _((void));
extern FLDOCC32 _TMDLLENTRY Fnum32 _((FBFR32 _TM_FAR *));
extern FLDOCC32 _TMDLLENTRY Foccur32 _((FBFR32 _TM_FAR *, FLDID32 ));
extern int _TMDLLENTRY Fprint32 _((FBFR32 *));
extern int _TMDLLENTRY Ffprint32 _((FBFR32 _TM_FAR *, FILE _TM_FAR *));
extern int _TMDLLENTRY Fproj32 _((FBFR32 _TM_FAR *, FLDID32 _TM_FAR *));
extern int _TMDLLENTRY Fprojcpy32 _((FBFR32 _TM_FAR *, FBFR32 _TM_FAR *,
	FLDID32 _TM_FAR *));
extern char _TM_FAR * _TMDLLENTRY Ftypcvt32 _((FLDLEN32 _TM_FAR *, int,
	char _TM_FAR *, int, FLDLEN32 ));
extern int _TMDLLENTRY Fupdate32 _((FBFR32 _TM_FAR *, FBFR32 _TM_FAR *));
extern int _TMDLLENTRY Fvneeded32 _((char _TM_FAR *));
extern int _TMDLLENTRY Fvopt32 _((char _TM_FAR *, int, char _TM_FAR *));
extern int _TMDLLENTRY Fvsinit32 _((char _TM_FAR *, char _TM_FAR *));
extern int _TMDLLENTRY Fvnull32 _((char _TM_FAR *, char _TM_FAR *, FLDOCC32,
	char _TM_FAR *));
extern int _TMDLLENTRY Fvselinit32 _((char _TM_FAR *, char _TM_FAR *,
	char _TM_FAR *));
extern int _TMDLLENTRY Fvftos32 _((FBFR32 _TM_FAR *, char _TM_FAR *,
	char _TM_FAR *));
extern void _TMDLLENTRY Fvrefresh32 _((void));
extern int _TMDLLENTRY Fvstof32 _((FBFR32 _TM_FAR *, char _TM_FAR *, int,
	char _TM_FAR *));
extern char _TM_FAR * _TMDLLENTRY Fboolco32 _((char _TM_FAR *));
extern char _TM_FAR * _TMDLLENTRY Fvboolco32 _((char _TM_FAR *, char _TM_FAR *));
extern char _TM_FAR * _TMDLLENTRY Fstrerror32 _((int));

long _TMDLLENTRY Fvttos32 _((char _TM_FAR *cstruct, char _TM_FAR *trecord, char _TM_FAR *viewname));
long _TMDLLENTRY Fvstot32 _((char _TM_FAR *cstruct, char _TM_FAR *trecord, long treclen, char _TM_FAR *viewname));
extern int _TMDLLENTRY Fcodeset32 _((unsigned char _TM_FAR *codeset));
extern int _TMDLLENTRY Flevels32 _((int));

extern int  _TMDLLENTRY maskprt32 _((FBFR32 _TM_FAR *));

#ifdef FML_H
extern int _TMDLLENTRY F16to32 _((FBFR32 _TM_FAR *dbfr, FBFR _TM_FAR *sbfr));
extern int _TMDLLENTRY F32to16 _((FBFR _TM_FAR *dbfr, FBFR32 _TM_FAR *sbfr));
#endif

/* former macro definitions */
extern int _TMDLLENTRY Fdel32 _((FBFR32 _TM_FAR *, FLDID32, FLDOCC32));
extern int _TMDLLENTRY Fpres32 _((FBFR32 _TM_FAR *, FLDID32, FLDOCC32));
extern int _TMDLLENTRY Fadds32 _((FBFR32 _TM_FAR *, FLDID32, char _TM_FAR *));
extern int _TMDLLENTRY Fchgs32 _((FBFR32 _TM_FAR *, FLDID32, FLDOCC32, char _TM_FAR *));
extern int _TMDLLENTRY Fgets32 _((FBFR32 _TM_FAR *, FLDID32, FLDOCC32, char _TM_FAR *));
extern char _TM_FAR * _TMDLLENTRY Fgetsa32 _((FBFR32 _TM_FAR *, FLDID32, FLDOCC32, FLDLEN32 _TM_FAR *));
extern char _TM_FAR * _TMDLLENTRY Ffinds32 _((FBFR32 _TM_FAR *, FLDID32, FLDOCC32));
extern int _TMDLLENTRY _Finitx32 _((FBFR32 *, FLDLEN32, FLDOCC32));

extern int _TMDLLENTRY tpxmltofml32 _((char *, char *, FBFR32 **, char **, long));
extern int _TMDLLENTRY tpfml32toxml _((FBFR32 *, char *, char *, char **, long));

#if defined(__cplusplus)
}
#endif
/* typed-buffer types */
#define FMLTYPE32 "FML32"
#define VIEWTYPE32 "VIEW32"

#endif
