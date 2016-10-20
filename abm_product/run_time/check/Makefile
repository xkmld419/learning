include $(MAKE_DEFINES)

TARGETS=hssxx
ASNPARSER=checkFunc.o regular.o rule.o test.o
CURENT_LIBS=

all : $(TARGETS)

hssxx: $(ASNPARSER)
	make -f ./Makefile OBJS="$(ASNPARSER)" EXE=$@ build_test_hss


include $(BILLDIR)/etc/install/Make.Clean
