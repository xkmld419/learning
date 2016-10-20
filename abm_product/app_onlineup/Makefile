include $(MAKE_DEFINES)

TARGETS=onlineUpdate cponlineup
ALL_OBJS= onlineUpdate.o

CURENT_LIBS=

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

all: $(ALL_OBJS) $(TARGETS)

##### create executed files #####
#test: test1.o test2.o
#	make -f ./Makefile OBJS="test1.o test2.o" EXE=$@ build_bill
onlineUpdate: $(ALL_OBJS)
	make -f ./Makefile OBJS="$(ALL_OBJS)" EXE=$@ build_main
	
cponlineup:
	cp  ONLINEUP ./onlineup
	chmod 755 onlineup
	cp  onlineup $(BILLDIR)/bin/
	
###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean

