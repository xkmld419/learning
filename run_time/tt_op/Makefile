include ../../Make.defines

all : lib

TARGET = $(TT_OP_LIBS)
OBJECTS =  TimesTenInterface.o TimesTenAccess.o ../algo/ReadIni.o ../system/encode.o
lib : $(OBJECTS)
	$(AR) $(TARGET) $(OBJECTS)
	$(RANLIB) $(TARGET)

clean:
	rm -f *.o $(TARGET)
