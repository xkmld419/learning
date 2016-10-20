include $(MAKE_DEFINES)


##### self define #####
#TARGETS=testjson
ALL_OBJS= JsonMgr.o json_reader.o json_value.o json_writer.o  
CURENT_LIBS=libjson.a


all: $(ALL_OBJS) $(TARGETS) $(CURENT_LIBS)

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)


##### create executed files #####
#test: test1.o test2.o
#	make -f ./Makefile OBJS="test1.o test2.o" EXE=$@ build_bill


ls:
	@echo
	make clean
	make all
	@echo

###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
