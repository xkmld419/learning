include $(MAKE_DEFINES)
##### self define #####
TARGETS=

ALL_OBJS=   UserInfoMap.o SHMFileInfo.o UserInfoSHMFileCtl.o SHMStatLog.o  
CURENT_LIBS=$(BILL_MMDB_LIBS)

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

lib: $(CURENT_LIBS)
all: $(CURENT_LIBS) $(TARGETS)

##### create executed files #####
#test: test1.o test2.o
#	make -f ./Makefile OBJS="test1.o test2.o" EXE=$@ build_bill
testprodoffer:testProdOffer.o 
	make -f ./Makefile OBJS="testProdOffer.o" EXE=$@ build_main

testfee: AcctFeeGetter.o testfee.o
	make -f ./Makefile OBJS="AcctFeeGetter.o testfee.o" EXE=$@ build_main

LoadAggr: LoadAggr.o 
	make -f ./Makefile OBJS="LoadAggr.o" EXE=$@ build_bill

testsize: testsize.o
	make -f ./Makefile OBJS="testsize.o" EXE=$@ build_main

testAggrMgr_client: testAggrMgr_client.o AggrMgr.o
	make -f ./Makefile OBJS="testAggrMgr_client.o AggrMgr.o" EXE=$@ build_bill

testAggrMgr_server: testAggrMgr_server.o AggrMgr.o
	make -f ./Makefile OBJS="testAggrMgr_server.o AggrMgr.o" EXE=$@ build_bill

testEventIntableMgr: testEventIntableMgr.o EventIntableMgr.o
	make -f ./Makefile OBJS="testEventIntableMgr.o EventIntableMgr.o" EXE=$@ build_bill

testEventIntableMgr2: testEventIntableMgr2.o EventIntableMgr.o
	make -f ./Makefile OBJS="testEventIntableMgr2.o EventIntableMgr.o" EXE=$@ build_bill

testEndFileMgr: testEndFileMgr.o EndFileMgr.o
	make -f ./Makefile OBJS="testEndFileMgr.o EndFileMgr.o" EXE=$@ build_bill

testTransactionMgr_client: testTransactionMgr_client.o TransactionMgr.o
	make -f ./Makefile OBJS="testTransactionMgr_client.o TransactionMgr.o" EXE=$@ build_bill

testTransactionMgr_server: testTransactionMgr_server.o TransactionMgr.o
	make -f ./Makefile OBJS="testTransactionMgr_server.o TransactionMgr.o" EXE=$@ build_bill

RollbackFile: RollbackFile.o TransactionMgr.o ../public/InstTableListMgr.o ../app_dupchecker/CheckDup.o ../app_dupchecker/ascdate.o ../app_dupchecker/balance.o
	make -f ./Makefile OBJS="RollbackFile.o TransactionMgr.o ../public/InstTableListMgr.o ../app_dupchecker/CheckDup.o ../app_dupchecker/ascdate.o ../app_dupchecker/balance.o" EXE=$@ build_bill

filtrateStopUser: filtrateStopUser.o TransactionMgr.o ../public/InstTableListMgr.o
	make -f ./Makefile OBJS="filtrateStopUser.o TransactionMgr.o ../public/InstTableListMgr.o" EXE=$@ build_bill

testfilter:FilterSimple.o ../framework/Environment.o
	make -f ./Makefile OBJS="FilterSimple.o ../framework/Environment.o" EXE=$@ build_main

###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
