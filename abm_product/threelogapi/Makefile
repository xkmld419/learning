include $(MAKE_DEFINES)

CURENT_LIBS =$(BILL_LOGG_LIBS)

TARGETS=

all: $(CURENT_LIBS) $(TARGETS)

SOURCE =ThreeLogGroupBase.cpp ThreeLogGroupMgr.cpp ThreeLogGroup.cpp

ALL_OBJS = ThreeLogGroupBase.o ThreeLogGroupMgr.o ThreeLogGroup.o

%.cpp:%.h
	touch $@

$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

include $(BILLDIR)/etc/install/Make.Clean
