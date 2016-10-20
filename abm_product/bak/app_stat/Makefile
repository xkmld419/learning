include $(MAKE_DEFINES)
MAKEFILE=./Makefile
TARGET=billstat 

all:$(TARGET)

alllib:

STATDATAOBJ=ExtractFileName.o StatOneTwoGroup.o ../app_prep/interrupt.o

BILLSTAT_OBJS= BssOrgMgr.o list_index.o trie_list_index.o DynCreate.o stat_public.o TBL_CALL_EVENT_CLASS.o	TBL_SERV_CLASS.o  TBL_ACCT_ITEM_CLASS.o TBL_ACCT_ITEM_SOURCE_CLASS.o TBL_ACCT_BALANCE_CLASS.o TBL_BALANCE_PAYOUT_CLASS.o TBL_BALANCE_SOURCE_CLASS.o TBL_PAYMENT_CLASS.o TBL_STAFF_CLASS.o TBL_BILL_CLASS.o StatCallEvent.o StatServ.o StatAcctItem.o StatAcctBalance.o StatBalanceSource.o StatBalancePayout.o StatPayment.o StatBill.o TStatTask.o TBL_OTHER_EVENT_CLASS.o StatOtherEvent.o StatDataEvent.o TBL_DATA_EVENT_CLASS.o TBL_CYCLE_FEE_EVENT_CLASS.o StatCycleFeeEvent.o TBL_VALUE_ADDED_EVENT_CLASS.o StatValueAddedEvent.o TBL_ACCT_ITEM_AGGR_CLASS.o StatAcctItemAggr.o TBL_ERROR_EVENT_CLASS.o StatErrorEvent.o TBL_REDO_EVENT_CLASS.o StatRedoEvent.o StatARefundLog.o TBL_A_REFUND_LOG_CLASS.o TBL_SERV_ACCT_CLASS.o StatServAcct.o

billstat: $(BILLSTAT_OBJS) billstat.o
	make -f $(MAKEFILE) OBJS="billstat.o $(BILLSTAT_OBJS)" EXE=$@ build_bill

TestObj: TestObj.o $(BILLSTAT_OBJS)
	make -f $(MAKEFILE) OBJS="TestObj.o $(BILLSTAT_OBJS)" EXE=$@ build_main

TBL_STAFF_CLASS:TBL_STAFF_CLASS.o	
	make -f $(MAKEFILE) OBJS="TBL_STAFF_CLASS.o " EXE=$@ build_bill
test1: test1.o $(BILLSTAT_OBJS)
	make -f $(MAKEFILE) OBJS="test1.o " EXE=$@ build_bill
	
statdata:$(STATDATAOBJ)
	make -f $(MAKEFILE) OBJS="$(STATDATAOBJ)" EXE=$@ build_main
        
###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
