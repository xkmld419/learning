include ../Make.defines

all: parserxmlmsg packxmlmsg

TINYXML_OBJS = tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o
PARSE_OBJES = $(TINYXML_OBJS) ReadXmlConf.o parserxmlmsg.o
PACK_OBJES = $(TINYXML_OBJS) ReadXmlConf.o packxmlmsg.o
TEST_OBJES = $(TINYXML_OBJS) ReadXmlConf.o testxmlmsg.o

parserxmlmsg: $(PARSE_OBJES)
	make -f ./Makefile OBJS="$(PARSE_OBJES)" EXE=$@ build_main
packxmlmsg: $(PACK_OBJES)
	make -f ./Makefile OBJS="$(PACK_OBJES)" EXE=$@ build_main

testxmlmsg: $(TEST_OBJES)
	make -f ./Makefile OBJS="$(TEST_OBJES)" EXE=$@ build_main
	

clean:
	rm -f *.o
	
