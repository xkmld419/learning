include $(MAKE_DEFINES)


##### self define #####
TARGETS=
ALL_OBJS= Compress.o Date.o DateTime.o Directory.o Encrypt.o Exception.o File.o \
	FtpClient_sys.o \
	MessageQueue.o SimpleSHM.o Socket.o SimpleMatrix.o Precision.o Log.o h_list.o \
	hash_list.o fee_shm.o trie.o ReadIni.o Calculator.o CSemaphore.o CRWLock.o FTPClient.o HBECodeParse.o

CURENT_LIBS=$(BILL_LIBS)

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

all: $(CURENT_LIBS) $(TARGETS)

##### create executed files #####
#test: test1.o test2.o
#	make -f ./Makefile OBJS="test1.o test2.o" EXE=$@ build_simple

testsema: testsemaphore.o
	make -f ./Makefile OBJS="testsemaphore.o" EXE=$@ build_main

testkey: testkeytree.o
	make -f ./Makefile OBJS="testkeytree.o" EXE=$@ build_main

testhashkey: testhashkey.o $(ALL_OBJ) Exception.o
	make -f ./Makefile OBJS="testhashkey.o Exception.o" EXE=$@ build_simple

testdate: testdate.o Date.o Exception.o Precision.o
	make -f ./Makefile OBJS="testdate.o Exception.o Date.o Precision.o" EXE=$@ build_simple

testp: testp.o 
	make -f ./Makefile OBJS="testp.o Exception.o Precision.o Calculator.o" EXE=$@ build_simple

testqueue: testqueue.o 
	make -f ./Makefile OBJS="testqueue.o Exception.o" EXE=$@ build_simple

testshm: testshm.o SimpleSHM.o
	make -f ./Makefile OBJS="testshm.o Exception.o SimpleSHM.o" EXE=$@ build_simple

testshm1: testshm1.o SimpleSHM.o
	make -f ./Makefile OBJS="testshm1.o Exception.o SimpleSHM.o" EXE=$@ build_simple

testshm2: testshm2.o SimpleSHM.o
	make -f ./Makefile OBJS="testshm2.o Exception.o SimpleSHM.o" EXE=$@ build_simple

testmq: testmq.o MessageQueue.o
	make -f ./Makefile OBJS="testmq.o Exception.o MessageQueue.o" EXE=$@ build_simple
	
	
###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
##include ../../etc/install/Make.Clean
include $(BILLDIR)/etc/install/Make.Clean
