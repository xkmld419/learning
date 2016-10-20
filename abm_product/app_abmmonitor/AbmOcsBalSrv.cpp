include ../Make.defines

TARGETS = GuardMain \
          monitor \
          server_monitor \
          server_client \
          GuardRemote

MONITOR_OBJS = monitor.o login_f.o login_j.o password_f.o password_j.o \
           	   mainmenu_j.o mainmenu_f.o process_f.o process_j.o ticket_f.o ticket_j.o \
	             msgqueue_j.o msgqueue_f.o sharedmemory_f.o sharedmemory_j.o \
	             control.o button.o readinfo.o listbox.o Environment.o \
	             IpcKey.o Log.o edit.o  mntapi.o \
	             WfPublic.o socketapi.o 
MNT_OBJS= WfPublic.o \
           Guard.o Server.o GuardMain.o Environment.o \
           mntapi.o IpcKey.o Log.o
SERVER_MONITOR_OBJS = ServerMonitor.o Server.o 
SERVER_CLIENT_OBJS = ServerClient.o Server.o 
GUARD_REMOTE_OBJS = Guard.o Server.o GuardRemote.o 

all: $(TARGETS)

build_mon: $(INCS)
	@echo
	@echo "=======[ $(EXE) ]======="
	$(CC) $(CFLAGS) -o $(APP_EXE)/$(EXE) $(OBJS)  $(LD_USR_LIBS) $(LD_ORA_LIBS) -lcurses
	if [ -f $(ABMAPP_DEPLOY)/bin/$(EXE) ]; then rm $(ABMAPP_DEPLOY)/bin/$(EXE); fi;
	ln -s $(APP_EXE)/$(EXE) $(ABMAPP_DEPLOY)/bin/$(EXE)
	@echo

monitor: $(MONITOR_OBJS)
	make -f ./Makefile OBJS="$(MONITOR_OBJS)" ABM_MAIN_OBJ="" EXE=$@ build_mon
GuardMain: $(MNT_OBJS)
	make -f ./Makefile OBJS="$(MNT_OBJS)" EXE=$@ build_mon
	
#GuardMain: $(MNT_OBJS)
#	make -f ./Makefile OBJS="$(MNT_OBJS) $(TUXDIR)/lib/tpinit.o" EXE=$@ build_abm


GuardRemote: $(GUARD_REMOTE_OBJS)
	make -f ./Makefile OBJS="$(GUARD_REMOTE_OBJS) $(TUXDIR)/lib/tpinit.o" EXE=$@ build_abm

server_monitor_svc=MONITOR_LOGIN,MONITOR_LOGOUT,RECV_APPINFO,RECV_PASSINFO,SEND_COMMAND,CHANGE_STATUS,REFRESH_RUNPLAN
server_monitor: $(SERVER_MONITOR_OBJS)
	make -f ./Makefile SVC="$(server_monitor_svc)" OBJS="$(SERVER_MONITOR_OBJS)" EXE=$@ build_server

server_client_svc=CLIENT_LOGIN,CLIENT_LOGOUT,GET_COMMAND,ACK_COMMAND,SEND_REMOTEMSG
server_client: $(SERVER_CLIENT_OBJS)
	make -f ./Makefile SVC="$(server_client_svc)" OBJS="$(SERVER_CLIENT_OBJS)" EXE=$@ build_server

clean:
	rm -f *.o
