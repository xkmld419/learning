include $(MAKE_DEFINES)

##### self define #####
TARGETS=abmaccess

ALL_OBJS=  checkUserInfo.o  abmaccessMain.o abmaccessLog.o abmaccessAdmin.o abmlogin_f.o abmlogin_j.o abmtablesmenu_f.o  \
                 abmtablesmenu_j.o abmtablescatalog_f.o abmtablescatalog_j.o password_f.o password_j.o \
                 abmmainmenu_j.o  abmmainmenu_f.o abmsqlfunc_f.o abmsqlfunc_j.o \
                 abmappcommand_f.o abmappcommand_j.o abmtablesdata_f.o abmtablesdata_j.o\
                 abmclearfile.o abmclear_f.o abmclear_j.o  control.o button.o edit.o  listbox.o \
                 test1_j.o  test1_f.o

USER_OBJS=  ../app_init/GroupDroitMgr.o
all: $(CURENT_LIBS) $(TARGETS)

abmaccess: $(ALL_OBJS) $(USER_OBJS)
	@echo
	make -f ./Makefile OBJS="$(ALL_OBJS) $(USER_OBJS) " BILL_MAIN_OBJ="" EXE=$@ build_main
	cp -p *.rc $(BILLDIR)/bin/
	@echo
	
###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
