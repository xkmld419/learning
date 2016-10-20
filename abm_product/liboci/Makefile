include $(MAKE_DEFINES)

##### self define #####
TARGETS=
ALL_OBJS= TOCIDirPath.o exception.o TOCIQuery.o TBatchInsert.o
CURENT_LIBS=$(BILL_LIBOCI_LIBS)

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

all: $(CURENT_LIBS) $(TARGETS)

##### create executed files #####
ocitest: ocitest.o $(ALL_OBJS)
	make -f ./Makefile OBJS="ocitest.o $(ALL_OBJS)" EXE=$@ build_simple

###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
