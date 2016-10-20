include ../../Make.defines

all : lib

TARGET = $(SYSTEM_LIBS)
OBJECTS = CSemaphore.o Date.o encode.o Exception.o FtpClient.o MessageQueue.o PthreadMutex.o SimpleSHM.o gettime.o \
		CFTPClient.o ABMException.o LogV2.o MmapFile.o FileV2.o
lib : $(OBJECTS)
	$(AR) $(TARGET) $(OBJECTS)
	$(RANLIB) $(TARGET)

clean:
	rm -f *.o $(TARGET)

#SHMMutex.o 
#SHMMutex.o 
