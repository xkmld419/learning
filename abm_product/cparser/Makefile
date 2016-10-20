include $(MAKE_DEFINES)

##### self define #####
TARGETS=
ALL_OBJS= TScriptParser.o wwInterpreter.o wwLex.o wwParser.o exception.o
CURENT_LIBS=$(BILL_CPARSER_LIBS)

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

all: $(CURENT_LIBS) $(TARGETS)

##### create executed files #####
#test: test1.o test2.o
#	make -f ./Makefile OBJS="test1.o test2.o" EXE=$@ build_simple

wwC: wwC.o $(ALL_OBJS)
	make -f ./Makefile OBJS="wwC.o $(ALL_OBJS)" EXE=$@ build_simple

###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
