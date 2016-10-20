include ../../Make.defines

all : lib

TARGET = $(ORACLE_OP_LIBS)
OBJECTS = TException.o TOCIQuery.o OracleStaticConn.o OracleConn.o OracleAccess.o
lib : $(OBJECTS)
	$(AR) $(TARGET) $(OBJECTS)
	$(RANLIB) $(TARGET)

clean:
	rm -f *.o $(TARGET)
