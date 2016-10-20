include $(MAKE_DEFINES)

PUBLIBOBJS	=	mxml-attr.o mxml-entity.o mxml-file.o mxml-index.o \
			mxml-node.o mxml-search.o mxml-set.o
XMLLIBOBJS		=	$(PUBLIBOBJS) mxml-private.o mxml-string.o
ALL_OBJS = $(XMLLIBOBJS) Time_Value.o Message_Block.o Free_List.o \
	ACE.o Auto_Ptr.o Node.o Unbounded_Set.o Local_Memory_Pool.o \
	Malloc.o Malloc_T.o Malloc_Allocator.o\
	avplist.o q_avplist.o comlist.o g_avplist.o \
	parser.o avp_container.o parser_avp.o avp_header.o \
	error_status.o parser_q_avplist.o xml.o xmldict.o \
	xml_errorreporter.o memory_manager.o parser_avpvalue.o DccMsgParser.o

TARGETS=  libdccparser.a
PUBLIB_SRC	=	mxml-attr.cpp mxml-entity.cpp mxml-file.cpp mxml-index.cpp \
			mxml-node.cpp mxml-search.cpp mxml-set.cpp
XMLLIB_SRC		=	$(PUBLIB_SRC) mxml-private.cpp mxml-string.cpp
ALL_SRC = $(XMLLIB_SRC) avplist.cpp q_avplist.cpp comlist.cpp g_avplist.cpp \
	parser.cpp avp_container.cpp parser_avp.cpp avp_header.cpp \
	error_status.cpp parser_q_avplist.cpp xml.cpp xmldict.cpp \
	xml_errorreporter.cpp memory_manager.cpp parser_avpvalue.cpp \
	Time_Value.cpp Message_Block.cpp Free_List.cpp \
	ACE.cpp Auto_Ptr.cpp Node.cpp Unbounded_Set.cpp Local_Memory_Pool.cpp \
	Malloc.cpp Malloc_T.cpp Malloc_Allocator.cpp
XMLLIB_H = config.h mxml.h mxml-private.h
ALL_H   = $(XMLLIB_H) parser_avp.h parser_avpvalue.h \
				parser.h xmldict.h \
				avp_header.h \
				comlist.h \
				error_status.h \
				parser_q_avplist.h \
				xml_errorreporter.h \
				avplist.h \
				g_avplist.h \
				q_avplist.h \
        memory_manager.h \
        xml.h \
        Malloc_Base.h \
        macros.h \
        Time_Value.h \
        Message_Block.h \
        Free_List.h \
        Default_Constants.h \
        ACE.h Auto_Ptr.h Node.h Unbounded_Set.h Local_Memory_Pool.h \
        Malloc.h Malloc_T.h Malloc_Allocator.h \
        diameter_parser_api.h

TARGETS=
CURENT_LIBS=libdccparser.a
all: $(CURENT_LIBS) 

$(CURENT_LIBS): $(ALL_OBJS)
	$(AR) $(CURENT_LIBS) $(ALL_OBJS)
	$(RANLIB) $(CURENT_LIBS)
	
include $(BILLDIR)/etc/install/Make.Clean
