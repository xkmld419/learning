include ../../Make.defines

all : lib

TARGET = $(ALGO_LIBS)
OBJECTS = Rsa.o ReadIni.o ReadCfg.o SHMAccess.o CycleSHMIntHashIndex.o 
lib : $(OBJECTS)
	$(AR) $(TARGET) $(OBJECTS)
	$(RANLIB) $(TARGET)

clean:
	rm -f *.o $(TARGET)
