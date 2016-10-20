include $(MAKE_DEFINES)
#include $(MAKE_DEFINES)

CURENT_LIBS=$(FRAMEWORK_LIBS)
OBJECTS = PerformanceStatistics.o  
all:$(CURENT_LIBS)
$(CURENT_LIBS) : $(OBJECTS) $(MAIN_OBJ)
	$(AR) $(CURENT_LIBS) $(OBJECTS)
	$(RANLIB) $(CURENT_LIBS)
include $(BILLDIR)/etc/install/Make.Clean
