include $(MAKE_DEFINES)

##### self define #####
TARGETS= makeform
ALL_OBJS= control.o edit.o button.o listbox.o readinfo.o
CURENT_LIBS=$(BILL_CURSES_LIBS)

##### create curent libs #####
$(CURENT_LIBS): $(ALL_OBJS)
	@echo "=======[ $(CURENT_LIBS) ]======="
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)

all: $(CURENT_LIBS) $(TARGETS)

##### create executed files #####
#test: test1.o test2.o
#	make -f ./Makefile OBJS="test1.o test2.o" EXE=$@ build_simple

makeform: makeform.o
	make -f ./Makefile OBJS="makeform.o" EXE=$@ build_simple

sample: sample.o login_f.o login_j.o
#	make -f ./Makefile OBJS="main.o login_f.o login_j.o $(ALL_OBJS)" build_simple
	g++ -o sample sample.o login_f.o login_j.o $(ALL_OBJS) -lcurses

###############################################################
## 这个脚本放在 $TIBS_HOME/etc/install 下面 
## 这个脚本只能包含在子目录 Makefile 的最后面
## 相当于 make clean
## 请不要再写 make clean ，包含这个脚本就可以了
include $(BILLDIR)/etc/install/Make.Clean
