include	../../Make.defines

TARGETS=../../lib/librw_config.a
CURENT_LIBS=../../lib/librw_config.a

OBJS=rw_config.o

all: $(TARGETS)

$(TARGETS):$(OBJS) 
	ar -rv $(CURENT_LIBS) $(OBJS)
	ranlib $(CURENT_LIBS)

clean: 	
	rm -f *.o $(TARGETS)
