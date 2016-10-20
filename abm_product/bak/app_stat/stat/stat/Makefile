include $(MAKE_DEFINES)
MAKEFILE=./Makefile
TARGET=billstat 

all:$(TARGET)

alllib:

BILLSTAT_OBJS= BssOrgMgr.o list_index.o trie_list_index.o DynCreate.o stat_public.o TBL_CALL_EVENT_CLASS.o	TBL_SERV_CLASS.o  TBL_ACCT_ITEM_CLASS.o TBL_ACCT_ITEM_SOURCE_CLASS.o TBL_ACCT_BALANCE_CLASS.o TBL_BALANCE_PAYOUT_CLASS.o TBL_BALANCE_SOURCE_CLASS.o TBL_PAYMENT_CLASS.o TBL_STAFF_CLASS.o TBL_BILL_CLASS.o StatCallEvent.o StatServ.o StatAcctItem.o StatAcctBalance.o StatBalanceSource.o StatBalancePayout.o StatPayment.o StatBill.o TStatTask.o

billstat: $(BILLSTAT_OBJS) billstat.o
	make -f $(MAKEFILE) OBJS="billstat.o $(BILLSTAT_OBJS)" EXE=$@ build_main

TestObj: TestObj.o $(BILLSTAT_OBJS)
	make -f $(MAKEFILE) OBJS="TestObj.o $(BILLSTAT_OBJS)" EXE=$@ build_main

TBL_STAFF_CLASS:TBL_STAFF_CLASS.o	
	make -f $(MAKEFILE) OBJS="TBL_STAFF_CLASS.o " EXE=$@ build_bill
test1: test1.o $(BILLSTAT_OBJS)
	make -f $(MAKEFILE) OBJS="test1.o " EXE=$@ build_bill
clean:
	rm -f $(CLFUNC) *.o $(CLEANFILES) $(TARGET)
