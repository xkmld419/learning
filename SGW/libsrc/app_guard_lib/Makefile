include ../../Make.defines

##### self define #####
LIBS=
TARGETS=
ALL_OBJS=GuardMain.o Guard.o common.o ParamDefineMgr.o Server.o TException.o mntapi.o WfPublic.o IpcKey.o HBECodeParse.o SgwStatMem.o
CURENT_LIBS=../../lib/libappguardlib.a

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)

all: $(CURENT_LIBS)

##### create executed files #####


###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include ../../etc/install/Make.Clean
