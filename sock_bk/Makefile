include ../Make.defines

all : socksrv sockclnt

SOCK_SRV= SockSrv.o TcpSock.o SockSrvConf.o
SOCK_CLNT= SockClnt.o TcpSock.o SockSrvConf.o dccapi.o 

socksrv:$(SOCK_SRV)
	make -f ./Makefile OBJS="$(SOCK_SRV)" EXE=$@ build_main

sockclnt:$(SOCK_CLNT)
	make -f ./Makefile OBJS="$(SOCK_CLNT)" EXE=$@ build_main

clean:
	rm -f *.o
