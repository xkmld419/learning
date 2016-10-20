include $(MAKE_DEFINES)
#include $(MAKE_DEFINES)

CURENT_LIBS=$(TT_OP_LIBS)

OBJECTS =  TTTns.o TimesTenInterface.o TimesTenAccess.o TTFlag.o
all : $(CURENT_LIBS)
$(CURENT_LIBS) : $(OBJECTS)
	$(AR) $(CURENT_LIBS) $(OBJECTS)
	$(RANLIB) $(CURENT_LIBS)
include $(BILLDIR)/etc/install/Make.Clean
