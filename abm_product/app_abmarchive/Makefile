#include ../Make.defines
include $(MAKE_DEFINES)

TARGET = abmarchive abmclear
all: $(TARGET)

ABMARCHIVE_OBJS= abmarchive.o AutoClearABMTT.o
ABMCLEAR_OBJS= abmclear.o AutoClearABMTT.o


abmarchive:$(ABMARCHIVE_OBJS)
	make -f ./Makefile OBJS="$(ABMARCHIVE_OBJS)" EXE=$@ build_main
	
abmclear:$(ABMCLEAR_OBJS)
	make -f ./Makefile OBJS="$(ABMCLEAR_OBJS)" EXE=$@ build_main

clean:
	rm -f *.o $(TARGET)
