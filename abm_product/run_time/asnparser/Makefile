include $(MAKE_DEFINES)

TARGETS=hssxx
ASNPARSER=test.o ASNCheck.o ASNObject.o ASNTree.o MoreNodeTree.o ASNDecode.o
CURENT_LIBS=

all : $(TARGETS)

hssxx: $(ASNPARSER)
	make -f ./Makefile OBJS="$(ASNPARSER)" EXE=$@ build_test_hss


include $(BILLDIR)/etc/install/Make.Clean
