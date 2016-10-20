include $(MAKE_DEFINES)

##### self define #####
TARGETS=$(SERVER_NAME)param initserver  $(SERVER_NAME)init 
ALL_OBJS=IniFileMgr.o  SysParamDefine.o WriteIni.o \
		 CommandCom.o \
		 interrupt.o BaseFunc.o BaseRuleMgr.o BaseStationMgr.o \
		 ParamAccessor.o CDBAccessIF.o

INITSERVER_OBJS= InitServer.o 
PARAM_OBJS= SysParamClient.o  GroupDroitMgr.o ParamMgr.o 
HBCHECKPOINT_OBJS = CheckPoint.o 
INIT_OBJS=   sgwInit.o CheckPointMgr.o CheckPointInfo.o
CURENT_LIBS=$(BILL_INIT_LIBS)

%.cpp:%.h
	touch $@

$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

all: $(CURENT_LIBS) $(TARGETS)

##### create executed files #####
initserver: $(INITSERVER_OBJS)
	make -f ./Makefile OBJS="$(INITSERVER_OBJS)" EXE=$@ build_bill

$(SERVER_NAME)init: $(INIT_OBJS)
	make -f ./Makefile OBJS="$(INIT_OBJS)" EXE=$@ build_bill

$(SERVER_NAME)param: $(PARAM_OBJS)
	make -f ./Makefile OBJS="$(PARAM_OBJS)" EXE=$@ build_bill

###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了a
include $(BILLDIR)/etc/install/Make.Clean
