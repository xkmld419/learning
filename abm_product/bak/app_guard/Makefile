include $(MAKE_DEFINES)

##### self define #####
#TARGETS=GuardMain hssmonitor cpmnt server_monitor server_client GuardRemote 
TARGETS=GuardMain hssmonitor cpmnt

MONITOR_OBJS= monitor.o login_f.o login_j.o password_f.o password_j.o \
	mainmenu_j.o mainmenu_f.o process_f.o process_j.o ticket_f.o ticket_j.o \
	msgqueue_j.o msgqueue_f.o \
	../app_init/SysParamClient.o ../app_init/HssParamMonitor.o paramset_j.o paramset_f.o \
	../app_init/BaseRuleMgr.o ../app_init/BaseFunc.o sysinfo_f.o sysinfo_j.o \
	../app_mode/mode.o hbmodectl_f.o hbmodectl_j.o \
	checkpointctl_f.o checkpointctl_j.o \
	LogFileMgr.o logctl_f.o logctl_j.o \
        ability_f.o ability_j.o


MNT_OBJS= Guard.o Server.o GuardMain.o 

CURENT_LIBS=$(BILL_WORKFLOW_LIBS)

SERVER_MONITOR_OBJS = ServerMonitor.o Server.o 
SERVER_CLIENT_OBJS = ServerClient.o Server.o 

#GUARD_MAIN_OBJS = Guard.o Server.o GuardMain.o
GUARD_REMOTE_OBJS = Guard.o Server.o GuardRemote.o
WF_STAT_LOG_OBJS = WfStatLogMgr.o

ALL_OBJS=mntapi.o WfPublic.o

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

lib: $(CURENT_LIBS)

all: $(CURENT_LIBS) $(TARGETS)


##### create executed files #####
hssmonitor: $(MONITOR_OBJS)
	make -f ./Makefile OBJS="$(MONITOR_OBJS) $(LD_BILL_CURSES_LIBS)" BILL_MAIN_OBJ="" EXE=$@ build_bill
	cp -p *.rc $(BILLDIR)/bin/
	
GuardMain: $(MNT_OBJS)
	make -f ./Makefile OBJS="$(MNT_OBJS)" EXE=$@ build_main


GuardRemote: $(GUARD_REMOTE_OBJS)
	make -f ./Makefile OBJS="$(GUARD_REMOTE_OBJS)" EXE=$@ build_client


server_monitor_svc=MONITOR_LOGIN,MONITOR_LOGOUT,RECV_APPINFO,SEND_COMMAND,CHANGE_STATUS,REFRESH_RUNPLAN
server_monitor: $(SERVER_MONITOR_OBJS)
	make -f ./Makefile SVC="$(server_monitor_svc)" OBJS="$(SERVER_MONITOR_OBJS)" EXE=$@ build_server
	@if [ ! -d "../../tuxsvc" ];then \
	 mkdir  ../../tuxsvc/ ; \
	 fi;
	cp -p sample_ubbcfg.h ../../tuxsvc/
	

server_client_svc=CLIENT_LOGIN,CLIENT_LOGOUT,GET_COMMAND,ACK_COMMAND,SEND_REMOTEMSG
server_client: $(SERVER_CLIENT_OBJS)
	make -f ./Makefile SVC="$(server_client_svc)" OBJS="$(SERVER_CLIENT_OBJS)" EXE=$@ build_server
wfstatlog: $(WF_STAT_LOG_OBJS)
	make -f ./Makefile OBJS="$(WF_STAT_LOG_OBJS)" EXE=$@ build_bill	

cpmnt:
	chmod 755 mnt
	cp  mnt $(BILLDIR)/bin/


###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
