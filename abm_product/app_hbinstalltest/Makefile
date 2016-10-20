include $(MAKE_DEFINES)

##### self define #####
TARGETS=hbinstalltest hbcleantest 

INSTALL_TEST_OBJS=Hbinstalltest.o HbinstalltestMain.o
CLEAR_TEST_OBJS=Hbcleantest.o HbcleantestMain.o

CURENT_LIBS= 

##### create curent libs #####
#$(CURENT_LIBS): $(ALL_OBJS)
#	@echo "=======[ $(CURENT_LIBS) ]======="
#	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
#	$(RANLIB) $(CURENT_LIBS)

all: $(ALL_OBJS) $(TARGETS)

##### create executed files #####

hbinstalltest: $(INSTALL_TEST_OBJS)
	@echo
	make -f ./Makefile OBJS="$(INSTALL_TEST_OBJS)" EXE=$@ build_main
	@echo

hbcleantest: $(CLEAR_TEST_OBJS)
	@echo
	make -f ./Makefile OBJS="$(CLEAR_TEST_OBJS)" EXE=$@ build_main
	@echo
	
cpshell:
	@if [ ! -d "$(HSSDIR)/bin" ];then 		mkdir $(HSSDIR)/bin ; 	 fi;
	cp -R ./shell $(HSSDIR)/bin/ 
	chmod 755 $(HSSDIR)/bin/shell/* 
	
cpsql:
	@if [ ! -d "$(HSSDIR)/bin" ];then       mkdir $(HSSDIR)/bin ;    fi;
	cp -R ./sql $(HSSDIR)/bin/ 
	chmod 755 $(HSSDIR)/bin/sql/* 

###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
