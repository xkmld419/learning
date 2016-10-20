include $(MAKE_DEFINES)
#include $(MAKE_DEFINES)

CURENT_LIBS=$(XML_OP_LIBS)

OBJECTS =  tinyxml.o tinystr.o tinyxmlparser.o tinyxmlerror.o
all : $(CURENT_LIBS)
$(CURENT_LIBS) : $(OBJECTS)
	$(AR) $(CURENT_LIBS) $(OBJECTS)
	$(RANLIB) $(CURENT_LIBS)

include $(BILLDIR)/etc/install/Make.Clean
