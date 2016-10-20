include $(MAKE_DEFINES)

##### self define #####
TARGETS=
ALL_OBJS= TracePricing.o SeekUserInfo.o SetBillingCycle.o ctloldfiledb.o printlastevent.o ReadDupFile.o UpdateAccu.o DBManager.o PrepLog.o interrupt.o RunProcessTask.o GetAcctAggrForYear.o QueueCtl.o LogGroup.o LogGroupProcess.o LogGroupRun.o LastEventClear.o
ABMINTF_OBJS= interrupt.o ../app_filedb/DataIncrementMgr.o
CURENT_LIBS=

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

all:  $(TARGETS)

##### create executed files #####
#hsscheckpoint:$(ALL_OBJS) 
hsscheckpoint:  CheckPoint.o CheckPointRemote.o
	@echo
	make -f ./Makefile OBJS=" CheckPoint.o CheckPointRemote.o" EXE=$@ build_main
	@echo

qacctitem: qacctitem.o
	@echo
	make -f ./Makefile OBJS="qacctitem.o" EXE=$@ build_main
	@echo
cleanlastevent: LastEventClear.o
	@echo
	make -f ./Makefile OBJS="LastEventClear.o" EXE=$@ build_test
	@echo

endbillingcycle: EndBillingCycle.o
	@echo
	make -f ./Makefile OBJS="EndBillingCycle.o" EXE=$@ build_bill
	@echo

readdupfile: ReadDupFile.o
	@echo
	make -f ./Makefile OBJS="ReadDupFile.o" EXE=$@ build_main
	@echo

commitaggr: CommitAggr.o
	@echo
	make -f ./Makefile OBJS="CommitAggr.o" EXE=$@ build_bill
	@echo

seekacct: SeekAcctInfo.o
	@echo
	make -f ./Makefile OBJS="SeekAcctInfo.o" EXE=$@ build_bill
	@echo

trace_pricing: TracePricing.o
	@echo
	make -f ./Makefile OBJS="TracePricing.o" EXE=$@ build_bill
	@echo

forcecommit: ForceCommit.o
	@echo
	make -f ./Makefile OBJS="ForceCommit.o" EXE=$@ build_bill
	@echo

seekvalue: SeekValue.o
	@echo
	make -f ./Makefile OBJS="SeekValue.o" EXE=$@ build_bill
	@echo

applog: applog.o
	@echo
	make -f ./Makefile OBJS="applog.o" EXE=$@ build_bill
	@echo

setbillingcycle: SetBillingCycle.o
	@echo
	make -f ./Makefile OBJS="SetBillingCycle.o" EXE=$@ build_main
	@echo

reloaduserinfo: ReloadUserInfo.o
	@echo
	make -f ./Makefile OBJS="ReloadUserInfo.o" EXE=$@ build_bill
	@echo

rentmaker: RentMaker.o
	@echo
	make -f ./Makefile OBJS="RentMaker.o" EXE=$@ build_bill
	@echo

seekuser: SeekUserInfo.o
	@echo
	make -f ./Makefile OBJS="SeekUserInfo.o" EXE=$@ build_main
	@echo

redo_event_maker: RedoEventMaker.o
	make -f ./Makefile OBJS="RedoEventMaker.o" EXE=$@ build_bill

call_event_maker: CallEventMaker.o
	make -f ./Makefile OBJS="CallEventMaker.o" EXE=$@ build_bill

getevent: GetSHMEvent.o
	make -f ./Makefile OBJS="GetSHMEvent.o" EXE=$@ build_bill

testcycle_objs= testcycle.o

testcycle: $(testcycle_objs)
	make -f ./Makefile OBJS="$(testcycle_objs)" EXE=$@ build_main

ctloldfiledb: ctloldfiledb.o
	@echo
	make -f ./Makefile OBJS="ctloldfiledb.o" EXE=$@ build_main
	@echo

seekaccu: SeekAccu.o ../bill/Statement.o
	@echo
	make -f ./Makefile OBJS="SeekAccu.o ../bill/Statement.o" EXE=$@ build_main
	@echo

updateaccu: UpdateAccu.o ../mmdb/UserInfoReader.o
	@echo
	make -f ./Makefile OBJS="UpdateAccu.o ../mmdb/UserInfoReader.o" EXE=$@ build_main
	@echo
	
seekofferaggr: SeekProdOfferAggr.o
	@echo
	make -f ./Makefile OBJS="SeekProdOfferAggr.o" EXE=$@ build_main
	@echo

printlastevent: printlastevent.o
	@echo
	make -f ./Makefile OBJS="printlastevent.o" EXE=$@ build_bill
	@echo

dbmanager: DBManager.o
	@echo
	make -f ./Makefile OBJS="DBManager.o" EXE=$@ build_main
	@echo
	
preplog: PrepLog.o interrupt.o
	@echo
	make -f ./Makefile OBJS="PrepLog.o interrupt.o" EXE=$@ build_main
	@echo	

runOneTask:RunProcessTask.o
	@echo
	make -f ./Makefile OBJS="RunProcessTask.o" EXE=$@ build_main
	@echo

getacctforyear: GetAcctAggrForYear.o
	@echo
	make -f ./Makefile OBJS="GetAcctAggrForYear.o" EXE=$@ build_main
	@echo

ctlqueue: QueueCtl.o
	@echo
	make -f ./Makefile OBJS="QueueCtl.o" EXE=$@ build_main
	@echo
	
logdbmgr: LogDBMgr.o
	@echo
	make -f ./Makefile OBJS="LogDBMgr.o" EXE=$@ build_main
	@echo

ctlacculock: AccuLockCtl.o
	@echo
	make -f ./Makefile OBJS="AccuLockCtl.o" EXE=$@ build_main
	@echo
	
synaggr: $(ABMINTF_OBJS) ABMIntfSynMgr.o
	make -f ./Makefile OBJS="$(ABMINTF_OBJS) ABMIntfSynMgr.o" EXE=$@ build_bill

asynaggr: $(ABMINTF_OBJS) ABMIntfMgr.o
	make -f ./Makefile OBJS="$(ABMINTF_OBJS) ABMIntfMgr.o" EXE=$@ build_bill


accusender: $(ABMINTF_OBJS) AccuSender.o
	make -f ./Makefile OBJS="$(ABMINTF_OBJS) AccuSender.o" EXE=$@ build_bill

accureceiver: $(ABMINTF_OBJS) AccuReceiver.o
	make -f ./Makefile OBJS="$(ABMINTF_OBJS) AccuReceiver.o" EXE=$@ build_bill

overdue: $(ABMINTF_OBJS) OverTimeMgr.o 
	make -f ./Makefile OBJS="$(ABMINTF_OBJS) OverTimeMgr.o" EXE=$@ build_bill

hblogg: LogGroupProcess.o LogGroup.o 
	@echo
	make -f ./Makefile OBJS="LogGroupProcess.o LogGroup.o " EXE=$@ build_main
	@echo
hbloggrun: LogGroupRun.o LogGroup.o 
	@echo
	make -f ./Makefile OBJS="LogGroupRun.o LogGroup.o " EXE=$@ build_bill
	@echo
	

#ctlacculock:AccuLockCtl.o 
#	@echo
#	make -f ./Makefile OBJS="AccuLockCtl.o" EXE=$@ build_main
#	@echo

ctlcacheexport:LocalCacheCtl.o
	@echo
	make -f ./Makefile OBJS="LocalCacheCtl.o" EXE=$@ build_main
	@echo

###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
