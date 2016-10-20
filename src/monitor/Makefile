include ../../Make.defines

all : monitorFile



ABM_MONITOR= monitorFile.o PublicConndb.o  TTTns.o


monitorFile:$(ABM_MONITOR)
	make -f ./Makefile OBJS="$(ABM_MONITOR)" EXE=$@ build_dcc_test
	#make -f ./Makefile OBJS="$(ABM_MONITOR)" EXE=$@ build_main

clean:
	rm -f  *.o
