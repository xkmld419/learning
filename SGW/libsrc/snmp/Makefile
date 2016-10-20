include ../../Make.defines

AM_CPPFLAGS = 
#INCLUDES = -I. 
AM_LDFLAGS = -L.
RM = rm -rf
AR = ar -rv
LIBPARSER=../lib/../libsnmp.a

#CXXFLAGS = -DMXML -w -g
CXXFLAGS = -DMXML -w -g
SELF_CFLAGS = $(CXXFLAGS)
#CFLAGS = $(CXXFLAGS)
LD    = $(LIBPARSER)
LDADD = 
CXX=g++



I_SYM=-I
HEAD_INCLUDE = ./include/snmp_pp
YHINCLUDE = ./include/
#INCLUDE=$(I_SYM)$(YHINCLUDE)
INCLUDE_HEADS=$(HEAD_INCLUDE)


USERHEADERS = $(INCLUDE_HEADS)/address.h $(INCLUDE_HEADS)/counter.h $(INCLUDE_HEADS)/ctr64.h \
	$(INCLUDE_HEADS)/gauge.h $(INCLUDE_HEADS)/octet.h $(INCLUDE_HEADS)/oid.h \
	$(INCLUDE_HEADS)/pdu.h $(INCLUDE_HEADS)/smi.h $(INCLUDE_HEADS)/target.h \
	$(INCLUDE_HEADS)/timetick.h $(INCLUDE_HEADS)/vb.h $(INCLUDE_HEADS)/snmp_pp.h \
	$(INCLUDE_HEADS)/integer.h $(INCLUDE_HEADS)/smival.h $(INCLUDE_HEADS)/snmperrs.h \
	$(INCLUDE_HEADS)/collect.h $(INCLUDE_HEADS)/oid_def.h $(INCLUDE_HEADS)/asn1.h \
	$(INCLUDE_HEADS)/snmpmsg.h $(INCLUDE_HEADS)/v3.h $(INCLUDE_HEADS)/sha.h \
	$(INCLUDE_HEADS)/usm_v3.h $(INCLUDE_HEADS)/mp_v3.h $(INCLUDE_HEADS)/idea.h \
	$(INCLUDE_HEADS)/config_snmp_pp.h $(INCLUDE_HEADS)/auth_priv.h  \
	$(INCLUDE_HEADS)/reentrant.h

#$(INCLUDE_HEADS)/userdefined.h
#$(INCLUDE_HEADS)/usertimeout.h

UXHEADERS = $(INCLUDE_HEADS)/msgqueue.h  \
		$(INCLUDE_HEADS)/eventlist.h \
	$(INCLUDE_HEADS)/msec.h $(INCLUDE_HEADS)/notifyqueue.h


HEADERS = $(USERHEADERS) $(UXHEADERS)

#auth_priv.o

ALL_OBJS = log.o integer.o idea.o gauge.o eventlistholder.o eventlist.o ctr64.o counter.o collect.o \
			auth_priv.o asn1.o address.o uxsnmp.o usm_v3.o timetick.o target.o snmpmsg.o sha.o reentrant.o \
					pdu.o oid.o octet.o notifyqueue.o msgqueue.o msec.o mp_v3.o md5c.o vb.o v3.o

ALL_SRC = log.cpp integer.cpp idea.cpp gauge.cpp eventlistholder.cpp eventlist.cpp ctr64.cpp counter.cpp collect.cpp \
					auth_priv.cpp asn1.cpp address.cpp uxsnmp.cpp usm_v3.cpp timetick.cpp target.cpp snmpmsg.cpp sha.cpp reentrant.cpp \
					pdu.cpp oid.cpp octet.cpp notifyqueue.cpp msgqueue.cpp msec.cpp mp_v3.cpp md5c.cpp vb.cpp v3.cpp



CPPCOMPILE = $(CXX) $(CFLAGS) $(AM_CPPFLAGS) $(INCLUDES) -D$(HOST_OS) 

.SUFFIXES:
.SUFFIXES: .cpp .o .h
CURRLIBS=../../lib/libsnmp.a


all: $(CURRLIBS)

$(CURRLIBS): $(ALL_OBJS) $(HEADERS)
	$(AR) $(CURRLIBS) $(ALL_OBJS)


#.cpp.o:$(HEADERS) $(ALL_SRC)
#	$(CPPCOMPILE) -c -o $@ $<

clean:
	$(RM) *.o *.exe *.dll $(CURRLIBS)

