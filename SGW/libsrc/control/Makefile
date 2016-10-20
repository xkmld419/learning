include	$(APP_HOME)/Make.defines

###TARGET =../lib/libcontrols.a
TARGET =../../lib/libcontrols.a
all: lib

SOURCE = control.cpp edit.cpp button.cpp listbox.cpp readinfo.cpp

OBJECTS = $(SOURCE:.cpp=.o)

lib: $(OBJECTS) 
	$(AR) $(TARGET) $(OBJECTS) 
	$(RANLIB) $(TARGET)

clean: 	
	rm -f *.o $(TARGET)


