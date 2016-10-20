include $(MAKE_DEFINES)

ALL_OBJS = 	Log_Msg.o \
		Log_Msg_Backend.o \
		Log_Msg_Callback.o \
		Log_Msg_IPC.o \
		Log_Msg_NT_Event_Log.o \
		Log_Msg_UNIX_Syslog.o \
		Log_Record.o \
		Trace.o	\
		Time_Value.o \
		IPC_SAP.o \
		ACE.o \
		Auto_Ptr.o \
		Free_List.o \
		Addr.o \
		INET_Addr.o \
		Handle_Set.o \
		Unbounded_Set.o \
		Local_Memory_Pool.o \
		Malloc.o \
		Malloc_Allocator.o \
		Malloc_Instantiations.o \
		Malloc_T.o \
		Message_Block.o \
		Node.o \
		OS_Memory.o \
		SOCK.o \
		SOCK_Acceptor.o \
		SOCK_Connector.o \
		SOCK_IO.o \
		SOCK_Stream.o \
		Sock_Connect.o \
		OS_NS_string.o

ALL_SRC = 	Log_Msg.cpp \
		Log_Msg_Backend.cpp \
		Log_Msg_Callback.cpp \
		Log_Msg_IPC.cpp \
		Log_Msg_NT_Event_Log.cpp \
		Log_Msg_UNIX_Syslog.cpp \
		Log_Record.cpp \
		Trace.cpp \
		ACE.cpp \
		Time_Value.cpp \
		IPC_SAP.cpp \
		Auto_Ptr.cpp \
		Free_List.cpp \
		Addr.cpp \
		INET_Addr.cpp \
		Unbounded_Set.cpp \
		Handle_Set.cpp \
		Local_Memory_Pool.cpp \
		Malloc.cpp \
		Malloc_Allocator.cpp \
		Malloc_Instantiations.cpp \
		Malloc_T.cpp \
		Message_Block.cpp \
		Node.cpp \
		OS_Memory.cpp \
		SOCK.cpp \
		SOCK_Acceptor.cpp \
		SOCK_Connector.cpp \
		SOCK_IO.cpp \
		SOCK_Stream.cpp \
		Sock_Connect.cpp 

ALL_H   = Log_Msg.h \
		Log_Msg_Backend.h \
		Log_Msg_Callback.h \
		Log_Msg_IPC.h \
		Log_Msg_NT_Event_Log.h \
		Log_Msg_UNIX_Syslog.h \
		Log_Priority.h \
		Log_Record.h \
		Trace.h \
		ACE.h \
		IPC_SAP.h \
		Time_Value.h \
		Auto_Ptr.h \
		Basic_Types.h \
		Default_Constants.h \
		Free_List.h \
		Addr.h \
		INET_Addr.h \
		Handle_Set.h \
		Unbounded_Set.h \
		Local_Memory_Pool.h \
		Malloc.h \
		Malloc_Allocator.h \
		Malloc_Base.h \
		Malloc_T.h \
		Message_Block.h \
		Node.h \
		OS_Memory.h \
		OS_NS_sys_select.h \
		OS_NS_sys_socket.h \
		SOCK.h \
		SOCK_Acceptor.h \
		SOCK_Connector.h \
		SOCK_IO.h \
		SOCK_Stream.h \
		Sock_Connect.h \
		Time_Value.h \
		macros.h
		
#DEFAULT_INCLUDES = -I. -I$(ABMAPP_SRC)/component/newace

#CXXCOMPILE = $(CXX) $(CFLAGS) $(DEFAULT_INCLUDES) \
#        $(CPPFLAGS) -D$(HOST_OS)

#CXXLINK = $(CXX) -o $@ $(ALL_OBJS) $(AM_CXXFLAGS) \
#        $(CXXFLAGS) $(LDFLAGS) -D$(HOST_OS)

#.SUFFIXES:
#.SUFFIXES: .cxx .h .cpp .lo .o .obj

CURENT_LIBS=$(BILL_ACE_LIBS)
all: $(CURENT_LIBS)

$(CURENT_LIBS):$(ALL_OBJS) $(ALL_H)
	$(AR) libace.a $(ALL_OBJS)
	$(RANLIB) libace.a

#.cpp.o:$(ALL_H) $(ALL_SRC)
#	$(CXXCOMPILE) -c -o $@ $<

###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
