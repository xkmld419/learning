include ../../Make.defines

##### self define #####
#TARGETS=appite
ALL_OBJS= ../libite/sqlite3.o  TSQLIteQuery.o TSQLIteQuery_f.o
#EXE_OBJS= sqlite3.o shell.o

#CFLAGS= -g -O2 -w -fpermissive -MD -MP -MF
#DEF_SR = $(DEFINE)
#DEFS = -DPACKAGE_NAME=\"sqlite\" -DPACKAGE_TARNAME=\"sqlite\" -DPACKAGE_VERSION=\"3.7.5\" -DPACKAGE_STRING=\"sqlite\ 3.7.5\" -DPACKAGE_BUGREPORT=\"http://www.sqlite.org\" -DPACKAGE=\"sqlite\" -DVERSION=\"3.7.5\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DHAVE_FDATASYNC=1 -DHAVE_USLEEP=1 -DHAVE_LOCALTIME_R=1 -DHAVE_GMTIME_R=1 -DHAVE_READLINE=1
#DEFINE =-DSQLITE_THREADSAFE=1  -DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_RTREE $(DEFS)
SQLITE_LIBS= libsqlite.a

CURENT_LIBS=$(SQLITE_LIBS)
#EXE_LIBS=$(EXE_OBJS)

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	ranlib $(CURENT_LIBS)
	cp $(CURENT_LIBS) $(APP_HOME)/lib

all: $(CURENT_LIBS) $(TARGETS)


##### create executed files #####
#test: test1.o test2.o
#	make -f ./Makefile OBJS="test1.o test2.o" EXE=$@ build_bill


#appite: $(CURENT_LIBS)
#	@echo
#	make -f ./Makefile OBJS="$(EXE_LIBS) -ldl -lpthread" EXE=$@ build_self_main
#	@echo


#xlC -q64 -g  -o sqlite -I. libsqlite3.a  -lm -lpthread  -lcurses -L/oracle/OraHome/rdbms/lib -L/oracle/OraHome/lib -locci -lclntsh -lm `cat /oracle/OraHome/lib/sysliblist` -lpthread  -lc 
###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include ../../etc/install/Make.Clean

