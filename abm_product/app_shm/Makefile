include $(MAKE_DEFINES)

##### self define #####
TARGETS= abmshm shmmgr lrufile lrutool abmdb abmerrcode 
TARGETS_LIBS=ShowMemData.o AppAbmShm.o
SHMOBJS=CSemMgr.o
LRUTOOL=lrutool.o
LRUFILE=lrutofiles.o
ABMDB=abmdb.o
ABMERROR=abmerrcode.o

all:$(TARGETS_LIBS) $(TARGETS)

##### create executed files #####
abmshm: $(TARGETS_LIBS)
		@echo
		make -f ./Makefile OBJS="$(TARGETS_LIBS)"  EXE=$@ build_main
		@echo	
	
shmmgr: $(SHMOBJS)
		@echo
		make -f ./Makefile OBJS="$(SHMOBJS)"  EXE=$@ build_main
		@echo	
		
lrufile: $(LRUFILE)
		@echo
		make -f ./Makefile OBJS="$(LRUFILE)"  EXE=$@ build_main
		@echo	
		
lrutool: $(LRUTOOL)
		@echo
		make -f ./Makefile OBJS="$(LRUTOOL)"  EXE=$@ build_main
		@echo		
		
abmdb: $(ABMDB)
		@echo
		make -f ./Makefile OBJS="$(ABMDB)"  EXE=$@ build_main
		@echo	
		
abmerrcode: $(ABMERROR)
		@echo
		make -f ./Makefile OBJS="$(ABMERROR)"  EXE=$@ build_main
		@echo								
###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
