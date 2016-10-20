#!/bin/sh -x

#create the share library

INCLUDE=" -I. -I${SETTLE_HOME}/include "
LIBFLAGS=

SRC="basefun.cc"

SOFILENAME="libbasefun.so"
SONAME=

SO_FLAG=
PIC_FLAG=
COMPILE_FLAG=" -O "

rm *.o *.a *.so *.sl

#HPUX && aCC
if [ $CXX = "aCC" ] ; then
	SO_FLAG=" -b -o "
	PIC_FLAG="  +z "
	SOFILENAME="libbasefun.sl"

#AIX && xlC
elif [ $CXX = "xlC" ] ; then
	SO_FLAG=" -qmkshrobj  -o "
	PIC_FLAG=
	SONAME=
	SOFILENAME="libbasefun.a"

#SOLARIS && CC
elif [ $CXX = "CC" ] ; then
	SO_FLAG=" -G -o "
	PIC_FLAG=" -Kpic "

#g++
elif [ $CXX = "g++" ] ; then
	SO_FLAG=" -Wall -shared -o"
	PIC_FLAG=" -fPIC "

#gcc
elif [ $CXX = "gcc" ] ; then
	SO_FLAG=" -Wall -shared -o"
	PIC_FLAG=" -fPIC "

else
	echo "Warning:	Please set the 'CXX' var!!! "
	return
fi

${CXX}  ${COMPILE_FLAG} -D_POSIX_PTHREAD_SEMANTICS  -D_PARAM_SHM ${INCLUDE} ${LIBFLAGS} ${SO_FLAG} ${SOFILENAME} ${SONAME} ${PIC_FLAG} ${SRC}
cp -f $SOFILENAME ${SETTLE_HOME}/libso

