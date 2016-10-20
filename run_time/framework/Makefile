include ../../Make.defines

all : lib $(MAIN_OBJ)

TARGET = $(FRAMEWORK_LIBS)
OBJECTS = PerformanceStatistics.o Application.o 
lib : $(OBJECTS)
	$(AR) $(TARGET) $(OBJECTS)
	$(RANLIB) $(TARGET)

clean:
	rm -f *.o $(TARGET) $(MAIN_OBJ)
