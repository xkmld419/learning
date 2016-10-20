include $(MAKE_DEFINES)

TARGETS=infomonitor


INFOMONITOR_SOURCES = InfoMonitor.cpp interrupt.cpp *.h
INFOMONITOR_OBJS = InfoMonitor.o interrupt.o

all: $(INFOMONITOR_OBJS)  $(TARGETS)

infomonitor: $(INFOMONITOR_OBJS)
	@echo
	make -f ./Makefile OBJS="$(INFOMONITOR_OBJS)" EXE=$@ build_main
	@echo

clean:
	rm -f *.o $(INFOMONITOR_OBJS) $(TARGETS)
	
ls:
	@echo
	make clean
	make all
	@echo

#bin_PROGRAMS = infomonitor
#bindir = $(prefix)/bin
#infomonitor_SOURCES = InfoMonitor.cpp interrupt.cpp *.h
#infomonitor_LDFLAGS =$(INCLUDES) $(LD_ORA_LIBS)
#infomonitor_LDADD = $(BUILD_MAIN)
