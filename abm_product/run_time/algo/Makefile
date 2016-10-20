include $(MAKE_DEFINES)

TARGETS=
CURENT_LIBS=$(ALGO_LIBS)
OBJECTS =  Rsa.o CycleSHMIntHashIndex.o
all : $(CURENT_LIBS)
$(CURENT_LIBS): $(OBJECTS)
	$(AR) $(CURENT_LIBS) $(OBJECTS)
	$(RANLIB) $(CURENT_LIBS)





tssm: tssm.o
	make -f ./Makefile OBJS="tssm.o" EXE=$@ build_main_z

include $(BILLDIR)/etc/install/Make.Clean
