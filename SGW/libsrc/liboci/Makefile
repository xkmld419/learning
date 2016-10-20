include ../../Make.defines

##### self define #####
#TARGETS= ocitest   
ALL_OBJS= TOCIDirPath.o TOCIQuery.o TBatchInsert.o Oci_Env.o exception.o
CURENT_LIBS=$(LIBOCI_LIBS)

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)

all: $(CURENT_LIBS)
#all: $(CURENT_LIBS) $(TARGETS)

##### create executed files #####
fetch_test: fetch_test.o $(ALL_OBJS)
	make -f ./Makefile OBJS="fetch_test.o $(ALL_OBJS)" EXE=$@ build_main
	
#ocitest: ocitest.o $(ALL_OBJS)
#	make -f ./Makefile OBJS="ocitest.o $(ALL_OBJS)" EXE=$@ build_main
	
tmp: tmp.o $(ALL_OBJS)
	make -f ./Makefile OBJS="tmp.o $(ALL_OBJS)" EXE=$@ build_main

clean:
	rm -f *.o $(CURENT_LIBS) $(CLEANFILES) $(TARGETS)


