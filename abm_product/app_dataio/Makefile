include $(MAKE_DEFINES)
##### self define #####
TARGETS= abmimport abmexport abmcheckpoint

ABMIMPORT_OBJS=ImportMain.o DataImport.o 

ABMEXPORT_OBJS=ExportMain.o DataExportor.o 

ABMCHECKPOINT_OBJS=CheckPointMain.o DataCheckPoint.o 

#CURENT_LIBS=$(BILL_DATAIO_LIBS)

#ALL_OBJS=FileMgr.o SHMInfoCtl.o SHMSqlMgr.o 

###### create curent libs #####
#$(CURENT_LIBS): $(ALL_OBJS)
#	@echo "=======[ $(CURENT_LIBS) ]======="
#	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
#	$(RANLIB) $(CURENT_LIBS)


#lib: $(CURENT_LIBS)

all: $(CURENT_LIBS) $(TARGETS)

##### create executed files #####

abmimport: $(ABMIMPORT_OBJS) 
	make -f ./Makefile OBJS="$(ABMIMPORT_OBJS)" EXE=$@ build_main

abmexport: $(ABMEXPORT_OBJS)
	make -f ./Makefile OBJS="$(ABMEXPORT_OBJS)" EXE=$@ build_main
	
abmcheckpoint: $(ABMCHECKPOINT_OBJS)
	make -f ./Makefile OBJS="$(ABMCHECKPOINT_OBJS)" EXE=$@ build_main

###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
