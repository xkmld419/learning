include ../../Make.defines
SELF_DEFINE=-DOCS_SYSTEM

##### self define #####
#TARGETS= $(SGW_MAIN_OBJ)
TARGETS= main.o
ALL_OBJS= EventReceiver.o EventSender.o OcpMsgParser.o Process.o GlobalTools.o Environment.o

#ShareNode.o Register.o ShareNodeMgr.o
CURENT_LIBS=$(FRAMEWORK_LIBS)

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

all: $(CURENT_LIBS) $(TARGETS)


##### create executed files #####
#test: test1.o test2.o
#	make -f ./Makefile OBJS="test1.o test2.o" EXE=$@ build_usedb
#tt_test : tt_test.o
#	make -f ./Makefile OBJS="tt_test.o" EXE=$@ build_main
#test1	: test1.o
#	make -f ./Makefile OBJS="test1.o" EXE=$@ build_main


#.cpp.o:$(ALL_H) $(ALL_SRC) test2.cpp DccMsgParser.cpp
#        $(CPPCOMPILE) -c -o $(OFilePath)$@ $<


###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include ../../etc/install/Make.Clean
