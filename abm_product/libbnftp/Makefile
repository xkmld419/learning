include $(MAKE_DEFINES)


##### self define #####
TARGETS=
ALL_OBJS= ctpbuf.o ctplib.o ctpnet.o

CURENT_LIBS=libbnftp.a

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

all: $(CURENT_LIBS) $(TARGETS)

##### create executed files #####
#test: test1.o test2.o
#	make -f ./Makefile OBJS="test1.o test2.o" EXE=$@ build_simple

#testsema: testsemaphore.o
#	make -f ./Makefile OBJS="testsemaphore.o" EXE=$@ build_main


	
###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
##include ../../etc/install/Make.Clean
include $(BILLDIR)/etc/install/Make.Clean
