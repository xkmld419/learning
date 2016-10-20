include $(MAKE_DEFINES)

##### self define #####
TARGETS= petri_commander petri_schd petri_syntsstate serverPetri serverPetriLocal 

PETRI_OBJS = petri.o  p_ParseBuff.o p_cmd.o p_exec.o  p_transition.o p_places.o encode.o ReadIni.o Date.o TOCIQuery.o exception.o

SERVER_OBJS =  serverPetri.o p_ParseBuff.o petri.o encode.o ReadIni.o Date.o TOCIQuery.o exception.o

SERVER_LOCAL_OBJS =  serverPetriLocal.o  p_ParseBuff.o petri.o encode.o ReadIni.o Date.o TOCIQuery.o exception.o

SCHD_OBJS = p_schd.o petri.o p_ParseBuff.o   p_transition.o p_places.o p_exec.o encode.o ReadIni.o Date.o TOCIQuery.o exception.o

SYN_OBJS = p_synTsState.o petri.o p_ParseBuff.o p_transition.o p_places.o p_exec.o encode.o ReadIni.o Date.o TOCIQuery.o exception.o

ALL_OBJS= $(PETRI_OBJS) $(SERVER_OBJS) $(SERVER_LOCAL_OBJS) $(SCHD_OBJS) $(SYN_OBJS)

##### create curent libs #####
#$(CURENT_LIBS): $(ALL_OBJS)
#	@echo "=======[ $(CURENT_LIBS) ]======="
#	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
#	$(RANLIB) $(CURENT_LIBS)


all: $(TARGETS)


##### create executed files #####
petri_commander: $(PETRI_OBJS)
	make -f ./Makefile OBJS="$(PETRI_OBJS)" EXE=$@ build_simple

petri_schd: $(SCHD_OBJS)
	make -f ./Makefile OBJS="$(SCHD_OBJS)" EXE=$@ build_simple

petri_syntsstate: $(SYN_OBJS)
	make -f ./Makefile OBJS="$(SYN_OBJS)" EXE=$@ build_simple
	
server_petri_svc=PETRI_FIRE,P_CHANGE_STATUS,P_GET_TRANSINFO
serverPetri: $(SERVER_OBJS)
	make -f ./Makefile SVC="$(server_petri_svc)" OBJS="$(SERVER_OBJS)" EXE=$@ build_server

server_petri_local_svc=P_F_LOCAL,P_C_S_LOCAL,P_G_T_LOCAL
serverPetriLocal: $(SERVER_LOCAL_OBJS)
	make -f ./Makefile SVC="$(server_petri_local_svc)" OBJS="$(SERVER_LOCAL_OBJS)" EXE=$@ build_server


###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
