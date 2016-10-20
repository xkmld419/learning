include $(MAKE_DEFINES)

TARGETS =  abmfinderr

ALL_OBJS = ABMFindErrMain.o ABMFindErr.o
ALL_OBJS1 = AbmProcessMain.o AbmProcess.o 

all: $(CURENT_LIBS) $(TARGETS)

abmprocess : $(ALL_OBJS1)
	make -f ./Makefile OBJS="$(ALL_OBJS1) $(LD_BILL_CURSES_LIBS) " EXE=$@ build_main
	
abmfinderr : $(ALL_OBJS)
	make -f ./Makefile OBJS="$(ALL_OBJS) $(LD_BILL_CURSES_LIBS) " EXE=$@ build_main
###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
