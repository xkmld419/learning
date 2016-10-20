include $(MAKE_DEFINES)

##### self define #####
TARGETS=$(SERVER_NAME)stat

ALL_OBJS = StatFormBase.o StatBaseMgr.o OnlineTime.o LoggInfo.o\
	UserLogin.o CommandBase.o AbmHandlInfo.o ProcessSes.o  SHMInfoShow.o MsgQueue.o  
	

%.cpp:%.h
	touch $@

$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

all: $(CURENT_LIBS) $(TARGETS)

##### create executed files #####
$(SERVER_NAME)stat: $(ALL_OBJS)
	make -f ./Makefile OBJS="$(ALL_OBJS)" EXE=$@ build_bill


###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了a
include $(BILLDIR)/etc/install/Make.Clean
