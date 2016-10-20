include $(MAKE_DEFINES)

##### self define #####
TARGETS= InfoCheckStat testinfo nmpInfo NmpSysParam HssNmpInfoCollect
ALL_OBJS= ./../app_init/IniFileMgr.o ./../app_init/WriteIni.o ./../app_init/CmdMsg.o ./../app_init/CmdCommonFactory.o ./../app_init/CmdMsgQueue.o ./../app_init/CmdReceiver.o \
		 ./../app_init/CmdSender.o ./../app_init/SysParamDefine.o \
				  ./../app_init/Variant.o ./../app_init/CommandCom.o \
			./../app_init/MessageQueueEx.o ./../app_init/BaseFunc.o ./../app_init/BaseRuleMgr.o 

CURENT_LIBS=


INFOCHECKSTAT_OBJS= CheckStatInterface.o  CheckStatPlan.o   \
                    CheckInformation.o  CheckStatInfo.o  CheckStat.o ABMException.o

TESTINFO_OBJS= CheckStatInterface.o  CheckStatPlan.o  \
					     CheckInformation.o CheckStatInfo.o  TestInfo.o ABMException.o

NMPINFO_OBJS= FileLog.o LogManage.o MutexLock.o nmpInfo.o PubFun.o 

NMPSYSPARAM_OBJS= NmpSysParam.o interrupt.o 

HSSNMPINFOCOLLECT_OBJS = HssNmpInfoCollect.o   
 
##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

all: $(ALL_OBJS) $(TARGETS)

##### create executed files #####

InfoCheckStat: $(INFOCHECKSTAT_OBJS)
	@echo
	make -f ./Makefile OBJS="$(INFOCHECKSTAT_OBJS)" EXE=$@ build_main
	@echo

testinfo: $(TESTINFO_OBJS)
	@echo
	make -f ./Makefile OBJS="$(TESTINFO_OBJS)" EXE=$@ build_main
	@echo
	
nmpInfo: $(NMPINFO_OBJS)
	@echo
	make -f ./Makefile OBJS="$(NMPINFO_OBJS)" EXE=$@ build_bill
	@echo
	
NmpSysParam: $(NMPSYSPARAM_OBJS)
	@echo
	make -f ./Makefile OBJS="$(ALL_OBJS)  $(NMPSYSPARAM_OBJS)" EXE=$@ build_bill
	@echo

##Added by liyr	
HssNmpInfoCollect:$(HSSNMPINFOCOLLECT_OBJS)
	@echo
	make -f ./Makefile OBJS="$(HSSNMPINFOCOLLECT_OBJS)" EXE=$@ build_main
	@echo
		
clean:
	rm -f *.o $(CURENT_LIBS) $(CLEANFILES) $(TARGETS)

