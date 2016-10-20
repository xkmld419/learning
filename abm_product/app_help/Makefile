include $(MAKE_DEFINES)

##CFLAGS= -AA -g +DA2.0W -w -lstd_v2 -lCsup_v2 -mt

##### self define #####
TARGETS= abmhelp modify
ALL_OBJS=

CURENT_LIBS=NULL

APPHELP_OBJS= help.o Xml.o Markup.o

APPMODIFY_OGJS= modify.o Xml.o Markup.o

##### create curent libs #####
#$(CURENT_LIBS): $(ALL_OBJS)
#	@echo "=======[ $(CURENT_LIBS) ]======="
#	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
#	$(RANLIB) $(CURENT_LIBS)

all: $(ALL_OBJS) $(TARGETS)

##### create executed files #####
#test: test1.o test2.o
#	make -f ./Makefile OBJS="test1.o test2.o" EXE=$@ build_bill

abmhelp: $(APPHELP_OBJS)
	@echo
	make -f ./Makefile OBJS="$(APPHELP_OBJS)" EXE=$@ build_main
	@echo

modify: $(APPMODIFY_OGJS)
	@echo
	make -f ./Makefile OBJS="$(APPMODIFY_OGJS)" EXE=$@ build_main
	@echo
	
###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
