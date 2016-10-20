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

	THIS IS UNPUBLISHED PROPRIETARY
	SOURCE CODE OF USL
	The copyright notice above does not
	evidence any actual or intended
	publication of such source code.
*/
#ifndef TMENV_H
#define TMENV_H 1

/* #ident	"@(#) gp/libgp/mach/linux_ev.h	$Revision: 1.9 $" */

/*  The linux files were copied and modified from the i386 files  */
#if defined(__STDC__) || defined(__cplusplus)
#define _TMPROTOTYPES 1
#endif

#if defined(_TMPROTOTYPES)
#if !defined(_TMCONST)
#define _TMCONST	const
#endif
#else
#define _TMCONST
#endif

#ifdef _TMPROTOTYPES
#define _(a) a
#else
#define _(a) ()
#endif

#ifndef NOWHAT
static	char	h_tmenv[] = "@(#) gp/libgp/mach/linux_ev.h	$Revision: 1.9 $";
#endif

#define O_BINARY	0

/* This is only used in Windows NT for thread instance data */
#define	_TM_THREADVAR

#define	_TM_FAR
#define	_TM_NEAR
#define _TMDLLENTRY
#define _TM_CDECL

#if defined(__cplusplus)
#include <unistd.h>
#include <sys/types.h>
#if defined(_CLOCK_T) && !defined(__cpp_stdc)
#define __cpp_stdc
#endif

#define _TMEDG 1	 /* latest C++ compiler - comment out if not needed */
#endif

#ifdef _TMPROTOTYPES
#include <sys/types.h>
typedef size_t		isize_t;
typedef uid_t           iuid_t;
typedef gid_t           igid_t;
#else
typedef unsigned short  uid_t;
typedef unsigned short  gid_t;
typedef long		off_t;
typedef int             isize_t; /* used by mem*, str*, stdio functions */
typedef int             mode_t;
typedef int             pid_t;
typedef int             iuid_t;
typedef int             igid_t;
#endif

#if !defined(__s390__)
#define _TML_ENDIAN 1
#else
#ifndef OBB_ANSI_CPP
#define OBB_ANSI_CPP
#endif
#endif

#define GCC_VERSION  (__GNUC__ * 10000 \
                        + __GNUC_MINOR__ * 100 \
                        + __GNUC_PATCHLEVEL__)

#if GCC_VERSION >= 40100
#ifndef OBB_ANSI_CPP
#define OBB_ANSI_CPP
#endif
#endif

#define _TMPAGESIZE 	512L

typedef	int		_TMXDRINT;
typedef	unsigned int	_TMXDRUINT;
typedef size_t TMNUMPTR;
#ifdef __LP64__
#include<stdlib.h>
#define _TMLONG64
typedef int		TM32I;
typedef unsigned int	TM32U;
#else
typedef	long		TM32I;
typedef	unsigned long	TM32U;
#endif

#if !defined(__STDC__) && !defined(__cpp_stdc)
#define _TMDEF_UINT 1
#endif

#define _TMIGW
#define _TMIGWT
#define _TMIBUFT
#define _TMIDNW
#define _TMIFML
#define _TMIFML32
#define _TMIFS
#define _TMIGP
#define _TMINWI
#define _TMINWS
#define _TMIQM
#define _TMIRMS
#define _TMISQL
#define _TMITMIB
#define _TMITUX
#define _TMITUX2
#define _TMITUX2WSC
#define _TMITUXWSC
#define _TMIUSORT
#define _TMIWSC
#define _TMITRPC

#define _TMNOCRYPTHDR 1

#endif
