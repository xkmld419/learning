include ../../Make.defines

all : abmmgr_reverse abmmgr_pay abmmgr_qry abmsyn_send 

ABM_MGR=ABMMgr.o ABMSvcSupport.o UserSelfService.o UserSelfServiceSql.o ReadAppConf.o PaymentInfoSql.o PaymentInfo.o DepositInfo.o AllocateBalance.o AllocateBalanceSql.o PublicConndb.o  TTTns.o ReverseDeductBalance.o QryPasswordSend.o QryPasswordsendSql.o QryServInfosyn.o QryServInfosynSql.o QueryAll.o QueryAllSql.o HssQuery.o HssQuerySql.o ActiveAllocateBalance.o ReverseDepositInfo.o ReversePay.o DccOperation.o DccOperationSql.o ActiveAllocateBalanceBiz.o PasswdIdentify.o PasswdIdentifyBiz.o PasswdChange.o PasswdChangeBiz.o ABMSynRcv.o QryServSynSql.o 

ABM_ALL=PublicConndb.o ReadAppConf.o QryServSynSql.o TTTns.o
ABM_RCV=$(ABM_ALL) ABMSynRcv.o QryServSynRcv.o
ABM_SEND=$(ABM_ALL) ABMSynSend.o QryServSynSend.o DccOperation.o DccOperationSql.o
ABM_REVERSE_CCR=PublicConndb.o ReadAppConf.o TTTns.o ActiveSendReverse.o ActiveSendReverseBiz.o ActiveSendReverseBizSql.o DccOperation.o DccOperationSql.o

abmmgr_pay:$(ABM_MGR)
	make -f ./Makefile OBJS="$(ABM_MGR)" EXE=$@ build_main
abmmgr_qry:$(ABM_MGR)
	make -f ./Makefile OBJS="$(ABM_MGR)" EXE=$@ build_main
#abmsyn_rcv:$(ABM_RCV)
#	make -f ./Makefile OBJS="$(ABM_RCV)" EXE=$@ build_main
abmsyn_send:$(ABM_SEND)
	make -f ./Makefile OBJS="$(ABM_SEND)" EXE=$@ build_main
abmmgr_reverse:$(ABM_REVERSE_CCR)
	make -f ./Makefile OBJS="$(ABM_REVERSE_CCR)" EXE=$@ build_main
clean:
	rm -f  *.o
