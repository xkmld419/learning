include $(MAKE_DEFINES)
#include $(MAKE_DEFINES)

CURENT_LIBS=$(ORACLE_OP_LIBS)

OBJECTS = OracleStaticConn.o OracleConn.o OracleAccess.o OracleConn.o 

all:$(CURENT_LIBS)
$(CURENT_LIBS) : $(OBJECTS)
	$(AR) $(CURENT_LIBS) $(OBJECTS)
	$(RANLIB) $(CURENT_LIBS)

include $(BILLDIR)/etc/install/Make.Clean
