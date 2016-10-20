include $(MAKE_DEFINES)

##### self define #####
LIBS=../app_dataio/FileMgr.o ../app_dataio/DataExportor.o \
		../app_dataio/SHMParamInfo.o ../app_dataio/SHMInfoCtl.o
TARGETS=sgwshmtools test
ALL_OBJS=SHMSGWInfoBase.o SGWSHMParamApi.o SHMDataMgr.o $(LIBS) SHMCmdSet.o
CURENT_LIBS=$(SHMAPI_LIBS)

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)

all: $(CURENT_LIBS) $(TARGETS)

##### create executed files #####

test: ShmApitest.o
	make -f ./Makefile OBJS="ShmApitest.o" EXE=$@ build_main


sgwshmtools:SGWSHMCmdMain.o
	make -f ./Makefile OBJS="SGWSHMCmdMain.o" EXE=$@ build_main
clean:
	rm -f *.o $(CLEANFILES) $(TARGETS)


