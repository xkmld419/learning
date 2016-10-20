include $(MAKE_DEFINES)
CURENT_LIBS=$(SHM_OP_LIBS)
OBJECTS =SHMParamInfo.o SHMHSSMess.o SrvSHMMgr.o ReadParamConf.o SHMInFile.o
all : $(CURENT_LIBS)
$(CURENT_LIBS) : $(OBJECTS)
	$(AR) $(CURENT_LIBS) $(OBJECTS)
	$(RANLIB) $(CURENT_LIBS)

include $(BILLDIR)/etc/install/Make.Clean
