include ../../Make.defines

#INCLUDE=$(TUXEDO_INCLUDE)
#LDSTRING=-DTUXEDO -DCOMMON
#SELF_DEFINE=-DTUXEDO -DCOMMON
SELF_DEFINE=-DTUXEDO -DCOMMON -DNONXA

MAKEFILE=Makefile

all: lib lib_non lib_ww
	cp *.a $(TUXDIR)/lib
	cp *.a $(APP_HOME)/lib

STARGET=libmcci.so
TARGET =libmcci.a

SOURCE = mcci.cpp cslfml.cpp cslpublic.cpp
OBJECTS = $(SOURCE:.cpp=.o)

so: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(STARGET) $(LIBPATH) $(LIBS)  $(OBJECTS)    

lib: $(OBJECTS)
	$(AR) $(TARGET) $(OBJECTS)

STARGET_WW =libwwfml.so
TARGET_WW =libwwfml.a

SOURCE_WW = mcci.cpp wwfml.cpp cslpublic.cpp
OBJECTS_WW = $(SOURCE_WW:.cpp=.o)

so_ww: $(OBJECTS_WW)
	$(CC) $(CFLAGS) -o $(STARGET_WW) $(LIBPATH) $(LIBS)  $(OBJECTS_WW)

lib_ww: $(OBJECTS_WW)
	$(AR) $(TARGET_WW) $(OBJECTS_WW)

STARGET_NON=libnon_ws_mcci.so
TARGET_NON =libnon_ws_mcci.a

SOURCE_NON = non_ws_mcci.cpp cslpublic.cpp
OBJECTS_NON = $(SOURCE_NON:.cpp=.o)

so_non: $(OBJECTS_NON)
	$(CC) $(CFLAGS) -o $(STARGET_NON) $(LIBPATH) $(LIBS)  $(OBJECTS_NON)    

lib_non: $(OBJECTS_NON)
	$(AR) $(TARGET_NON) $(OBJECTS_NON)

clean:
	rm -f *.a *.o

