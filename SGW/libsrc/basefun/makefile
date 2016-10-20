include	../../Make.defines

TARGETS=../../lib/libbasefun.a
CURENT_LIBS=../../lib/libbasefun.a

OBJS=basefun.o

all: $(TARGETS)

$(TARGETS):$(OBJS) 
	ar -rv $(CURENT_LIBS) $(OBJS)
	ranlib $(CURENT_LIBS)

clean: 	
	rm -f *.o $(TARGETS)
