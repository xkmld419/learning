include $(MAKE_DEFINES)

##### self define #####
TARGETS= $(SERVER_NAME)log testlog seeklog logremain

OTHER_OBJS=
ALL_OBJS= LoggDis.o Logg.o testlog.o  seeklog.o LogGroupRummage.o $(OTHER_OBJS)

CURENT_LIBS=

##APPLOGG_OBJS用于非前台版本
APPLOGG_OBJS= LoggDis.o Logg.o $(OTHER_OBJS)
##APPLOGG_WEB_OBJS用于前后台通用版本,LoggWeb.cpp是对Logg.cpp的改造
APPLOGG_WEB_OBJS= LoggDis.o  $(OTHER_OBJS) 
TEST_OBJS = testlog.o  $(OTHER_OBJS)
SEEK_OBJS = seeklog.o  $(OTHER_OBJS)
REMAIN_OBJS = LogGroupRummage.o interrupt.o $(OTHER_OBJS)


##### create curent libs #####
#$(CURENT_LIBS): $(ALL_OBJS)
#	@echo "=======[ $(CURENT_LIBS) ]======="
#	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
#	$(RANLIB) $(CURENT_LIBS)

all: $(ALL_OBJS) $(TARGETS)

##### create executed files #####
#test: test1.o test2.o
#	make -f ./Makefile OBJS="test1.o test2.o" EXE=$@ build_bill

$(SERVER_NAME)log: $(APPLOGG_OBJS)
	@echo
	make -f ./Makefile OBJS="$(APPLOGG_OBJS)" EXE=$@ build_main
	@echo


testlog:$(TEST_OBJS)
	@echo
	make -f ./Makefile OBJS="$(TEST_OBJS)" EXE=$@ build_main
	@echo

seeklog:$(SEEK_OBJS)
	@echo
	make -f ./Makefile OBJS="$(SEEK_OBJS)" EXE=$@ build_main
	@echo

logremain:$(REMAIN_OBJS)
	@echo
	make -f ./Makefile OBJS="$(REMAIN_OBJS)" EXE=$@ build_main
	@echo

###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
