include $(MAKE_DEFINES)

TARGETS= PowerMain PowerFile PowerTest POWER_CTL power_cl
GSOAP_OBJS = stdsoap2.o  soapC.o soapClient.o
POWER_OBJS= ParserXml.o PowerQuery.o PowerFile.o PowerXml.o common.o ../bill/Statement.o

POWER_MAIN_OBJS = $(GSOAP_OBJS) $(POWER_OBJS) PowerMain.o 
POWER_FILE_OBJS = $(GSOAP_OBJS) $(POWER_OBJS) PowerFileMain.o
POWER_TEST_OBJS = TestParser.o ParserXml.o common.o

POWER_CTL_OBJS=POWER_CTL.o $(POWER_OBJS) $(GSOAP_OBJS)
POWER_CTL_SVC=POWER_CTL_RUN
power_cl_OBJS=power_cl.o

all:$(TARGETS)

PowerMain: $(POWER_MAIN_OBJS)
	make -f ./Makefile OBJS="$(POWER_MAIN_OBJS) " EXE=$@ build_main

PowerFile: $(POWER_FILE_OBJS)
	make -f ./Makefile OBJS="$(POWER_FILE_OBJS)" EXE=$@ build_main

PowerTest:$(POWER_TEST_OBJS)
	make -f ./Makefile OBJS="$(POWER_TEST_OBJS)" EXE=$@ build_main

POWER_CTL: $(POWER_CTL_OBJS)
	make -f ./Makefile SVC="$(POWER_CTL_SVC)" OBJS="$(POWER_CTL_OBJS) " EXE=$@ build_server


power_cl: $(power_cl_OBJS)
	make -f ./Makefile OBJS="$(power_cl_OBJS)" EXE=$@ build_client

		
###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
