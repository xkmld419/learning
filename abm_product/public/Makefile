include $(MAKE_DEFINES)

##### self define #####
TARGETS= 
ALL_OBJS=ParamDefineMgr.o \
	SHMAccess.o SHMIntHashIndex.o SHMStringTreeIndex.o SHMStringTreeIndexEx.o SHMIntHashIndex_KR.o SHMMixHashIndex.o\
    SHMIntHashIndex_A.o SHMStringTreeIndex_A.o  \
	  BaseStationMgr.o \
	VirtualFileMaker.o   CRMControl.o \
	  MEMControl.o  \
    Environment_DB.o encode.o  IpcKey.o  \
    ThresholdAlarm.o\
    EventToolKits.o\
    QueueInDb.o  DBTableCheck.o DataOfferMgr.o MonitorFlow.o \
    TicketTlv.o  DBTableCheck.o LogDB.o LogDBBuf.o  
    
CURENT_LIBS=$(BILL_PUBLIC_LIBS)

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

all: $(CURENT_LIBS) $(TARGETS)

##### create executed files #####
#test: test1.o test2.o
#	make -f ./Makefile OBJS="test1.o test2.o" EXE=$@ build_usedb

###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
