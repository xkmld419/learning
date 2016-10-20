#include ../Make.defines
include $(MAKE_DEFINES)

TARGET = $(MONITOR_LIBS)

TARGETS = GuardMain \
          monitor \
          server_monitor \
          server_client \
          GuardRemote   \
          acctsendmsg \
          acctrecmsg

MONITOR_OBJS = monitor.o login_f.o login_j.o password_f.o password_j.o \
                mainmenu_j.o mainmenu_f.o process_f.o process_j.o ticket_f.o ticket_j.o \
	             msgqueue_j.o msgqueue_f.o sharedmemory_f.o sharedmemory_j.o \
	             control.o button.o readinfo.o listbox.o   Environment.o\
	              Log.o edit.o  mntapi.o \
	             WfPublic.o socketapi.o 
MNT_OBJS= WfPublic.o \
           Guard.o Server.o GuardMain.o  Environment.o\
           mntapi.o  Log.o
MONITOR_LIB = mntapi.o Log.o WfPublic.o Environment.o
libMonitor : $(MONITOR_LIB)
	$(AR) $(TARGET) $(MONITOR_LIB)
	$(RANLIB) $(TARGET)
	
ALL_OBJS=mntapi.o WfPublic.o	

CURENT_LIBS=$(BILL_WORKFLOW_LIBS)
##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

lib: $(CURENT_LIBS)


all: $(CURENT_LIBS) $(TARGETS)

build_mon: $(INCS)
	@echo
	@echo "=======[ $(EXE) ]======="
	$(CC) $(CFLAGS) -o $(APP_EXE)/$(EXE) $(OBJS)  $(LD_USR_LIBS) $(LD_ORA_LIBS) -lcurses
	if [ -f $(ABMAPP_DEPLOY)/bin/$(EXE) ]; then rm $(ABMAPP_DEPLOY)/bin/$(EXE); fi;
	ln -s $(APP_EXE)/$(EXE) $(ABMAPP_DEPLOY)/bin/$(EXE)
	@echo


monitor: $(MONITOR_OBJS) 
	@echo
	make -f ./Makefile OBJS="$(MONITOR_OBJS) " EXE=$@ build_main
	cp -p *.rc $(BILLDIR)/bin/
	@echo

GuardMain: $(MNT_OBJS) 
	@echo
	make -f ./Makefile OBJS="$(MNT_OBJS) " EXE=$@ build_main
	cp -p *.rc $(BILLDIR)/bin/
	@echo	

#monitor: $(MONITOR_OBJS)
#	make -f ./Makefile OBJS="$(MONITOR_OBJS)" ABM_MAIN_OBJ="" EXE=$@ build_mon
#GuardMain: $(MNT_OBJS)
#	make -f ./Makefile OBJS="$(MNT_OBJS)" EXE=$@ build_mon
	
	
AUDIT_SMSG_OBJ=AcctSendMsg.o mntapi.o Environment.o Log.o \
               WfPublic.o 
acctsendmsg: $(AUDIT_SMSG_OBJ)
	make -f ./Makefile OBJS="$(AUDIT_SMSG_OBJ)" EXE=$@ build_main

AUDIT_SMSG_OB= AcctRecMsg.o mntapi.o Environment.o Log.o \
               WfPublic.o 	
acctrecmsg: $(AUDIT_SMSG_OB)
	make -f ./Makefile OBJS="$(AUDIT_SMSG_OB)" EXE=$@ build_main

AUDIT_SMSG_OBJJ=AcctSendMsgg.o mntapi.o Environment.o Log.o \
                WfPublic.o control.o button.o readinfo.o listbox.o   Environment.o\
	              Log.o edit.o  mntapi.o \
	             WfPublic.o socketapi.o 
acctsendmsgg: $(AUDIT_SMSG_OBJJ)
	make -f ./Makefile OBJS="$(AUDIT_SMSG_OBJJ)" EXE=$@ build_mon




clean:
	rm -f *.o $(TARGET)
