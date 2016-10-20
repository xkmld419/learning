EXT="`uname|tr ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz`" 
#inclue ./Makefile.hpux
all:
	@FILE_EXT=$(EXT);case $$FILE_EXT in \
	*hp-ux*) \
	$(MAKE) -f ./Makefile.all EXT=hpux $@ \
	;; \
	*aix*) \
	$(MAKE) -f ./Makefile.all EXT=aix $@ \
	;; \
	*cygwin*) \
	$(MAKE) -f ./Makefile.all EXT=cygwin $@ \
	;; \
	*) \
	$(MAKE) -f ./Makefile.all EXT=linux $@ \
	;; \
	esac

clean:
	@FILE_EXT=$(EXT);case $$FILE_EXT in \
	*hp-ux*) \
	$(MAKE) -f ./Makefile.all EXT=hpux $@ \
	;; \
	*aix*) \
	$(MAKE) -f ./Makefile.all EXT=aix $@ \
	;; \
	*cygwin*) \
	$(MAKE) -f ./Makefile.all EXT=cygwin $@ \
	;; \
	*) \
	$(MAKE) -f ./Makefile.all EXT=linux $@ \
	;; \
	esac

win32:
	$(MAKE) -f Makefile.all EXT="win32" clean all
	
