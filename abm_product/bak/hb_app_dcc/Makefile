include $(MAKE_DEFINES)


##### self define #####
TARGETS=dccTran dccUnPack dccPack servSimulator dccCheckRecord msgQueTest
ALL_OBJS=
TRAN_OBJS=DccTran.o interrupt.o OcpMsgParser.o dcc_ParamsMgr.o
UNPACK_OBJS=DccUnPack.o DccLog.o interrupt.o OcpMsgParser.o dcc_ParamsMgr.o dcc_FileMgr.o dcc_FileInfo.o
PACK_OBJS=DccPack.o interrupt.o DccLog.o OcpMsgParser.o dcc_ParamsMgr.o
SERVSIMULATOR_OBJS=servSimulator.o OcpMsgParser.o
DCCCHECKRECORD_OBJS=DccCheckRecord.o interrupt.o 
SENDFROMFILE_OBJS = interrupt.o dcc_ParamsMgr.o dcc_FileInfo.o dcc_FileMgr.o sendDcrFromFile.o
MSGQUETEST_OBJS=MsgQueTest.o dcc_ParamsMgr.o dcc_FileInfo.o dcc_FileMgr.o


all: $(ALL_OBJS) $(TARGETS)

##### create executed files #####
dccTran: $(TRAN_OBJS)
	make -f ./Makefile OBJS="$(TRAN_OBJS)" EXE=$@ build_main

dccUnPack: $(UNPACK_OBJS)
	make -f ./Makefile OBJS="$(UNPACK_OBJS)" EXE=$@ build_bill

dccPack: $(PACK_OBJS)
	make -f ./Makefile OBJS="$(PACK_OBJS)" EXE=$@ build_bill

servSimulator: $(SERVSIMULATOR_OBJS)
	make -f ./Makefile OBJS="$(SERVSIMULATOR_OBJS)" EXE=$@ build_main

dccCheckRecord: $(DCCCHECKRECORD_OBJS)
	make -f ./Makefile OBJS="$(DCCCHECKRECORD_OBJS)" EXE=$@ build_bill

msgQueTest: $(MSGQUETEST_OBJS)
	make -f ./Makefile OBJS="$(MSGQUETEST_OBJS)" EXE=$@ build_bill

###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
