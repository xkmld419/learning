all: libparser.a

#include	$(ORACLE_HOME)/precomp/lib/env_precomp.mk

CC = g++
CD=cd
MV=mv
CFLAGS= -w -ggdb
PUBLIC_INC=-I../liboci

INCLUDE=-I. -I$(ORACLE_HOME)/precomp/public  $(PUBLIC_INC) \
	-I$(ORACLE_HOME)/rdbms/demo/ -I$(ORACLE_HOME)/rdbms/public/ -I$(ORACLE_HOME)/plsql/public/ \
	-I$(ORACLE_HOME)/network/public/ 


.SUFFIXES : .cc .y .l .cpp $(SUFFIXES)

.cc.o :
	g++ -ggdb  -I$(CENTERCCLIBDIR)/incl -c  $*.cc

.cpp.o:
	$(CC) -I$(INCLUDE) -c $(CFLAGS) $*.cpp -o $*.o

.y.cpp :
	bison++ -d -v --no-lines -o $*.cpp -h $*.h  $*.y
.l.cpp :
	/usr/local/lib/flex++  -8  -h$*.h -o$*.cpp $*.l
.y.h :
	bison++ -d -v --no-lines -o $*.cpp -h $*.h  $*.y
.l.h :
	/usr/local/lib/flex++ -8   -h$*.h -o$*.cpp $*.l

all : wwC

TScriptParser.o : TScriptParser.cpp TScriptParser.h

wwInterpreter.o : wwInterpreter.cpp wwInterpreter.h

wwC.o : wwC.cpp wwLex.h wwParser.h 

wwParser.o : wwParser.cpp wwParser.h

wwLex.o : wwLex.cpp wwLex.h wwParser.h

wwParser.cpp : wwParser.y

wwLex.cpp : wwLex.l

wwParser.h : wwParser.y

wwLex.h : wwLex.l

wwC : wwC.o wwLex.o wwParser.o wwInterpreter.o TScriptParser.o
	g++  -o $@ wwC.o TScriptParser.o wwLex.o wwParser.o wwInterpreter.o -L./ -lparser -L../liboci -ldp

clean:
	rm -f wwC wwC.o TScriptParser.o wwLex.o wwParser.o  wwInterpreter.o wwParser.output libparser.a



libparser.a:  wwLex.o wwParser.o wwInterpreter.o TScriptParser.o
	ar -rv libparser.a  wwLex.o wwParser.o wwInterpreter.o TScriptParser.o

install: libparser.a 
	cp -f libparser.a $(PUB_LIB_PATH)/.
	cp -f wwLex.h  $(PUB_INC_PATH)/.
	cp -f wwParser.h $(PUB_INC_PATH)/.
	cp -f wwInterpreter.h $(PUB_INC_PATH)/.
	cp -f TScriptParser.h $(PUB_INC_PATH)/.

