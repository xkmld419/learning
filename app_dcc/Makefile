include ../Make.defines

all : parserdccmsg packdccmsg

DCC_PARSER_OBJS = DccSql.o ReadDccConf.o dccpublic.o parserDccMsg.o
DCC_PACK_OBJS   = ReadDccConf.o dccpublic.o packDccMsg.o DccSql.o 

parserdccmsg: $(DCC_PARSER_OBJS) 
	make -f ./Makefile OBJS="$(DCC_PARSER_OBJS)" EXE=$@ build_main
packdccmsg: $(DCC_PACK_OBJS) 
	make -f ./Makefile OBJS="$(DCC_PACK_OBJS)" EXE=$@ build_main
parserdccmsgHSS: $(DCC_PARSER_OBJS) 
	make -f ./Makefile OBJS="$(DCC_PARSER_OBJS)" EXE=$@ build_main
packdccmsgHSS: $(DCC_PACK_OBJS) 
	make -f ./Makefile OBJS="$(DCC_PACK_OBJS)" EXE=$@ build_main

clean:
	rm -f *.o
