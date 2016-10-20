include	../../Make.defines

TARGETS=../../lib/libbasesock.a
CURENT_LIBS=../../lib/libbasesock.a

OBJS=basesock.o

all: $(TARGETS)

$(TARGETS):$(OBJS) 
	ar -rv $(CURENT_LIBS) $(OBJS)
	ranlib $(CURENT_LIBS)

clean: 	
	rm -f *.o $(TARGETS)
