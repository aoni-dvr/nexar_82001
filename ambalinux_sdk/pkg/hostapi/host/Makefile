ifndef SRCDIR
SRCDIR = $(abspath ../)
endif
export SRCDIR

ifndef BUILDDIR
BUILDDIR = $(SRCDIR)/build
endif
export BUILDDIR

LIB_NAME = core network shell
LIBS = $(addprefix "$(BUILDDIR)/libhost_",$(addsuffix ".a",$(LIB_NAME)))


UNAME = $(strip $(shell uname))


MAJOR_VERSION := 1
MINOR_VERSION := 0  # Linux only

ifeq ($(UNAME),Darwin)
LINKNAME      := libhostapi.dylib
LIBFILE       := libhostapi.$(MAJOR_VERSION).dylib
LDFLAGS       := -dynamiclib -current_version $(MAJOR_VERSION) 
else
ifeq ($(UNAME),Linux)
LINKNAME      := libhostapi.so
SONAME        := libhostapi.so.$(MAJOR_VERSION)
LIBFILE       := $(SONAME).$(MINOR_VERSION)
LDFLAGS       := -shared -fPIC -Wl,-soname,$(SONAME)
else
$(error "Unrecognized uname $(UNAME)"
endif
endif


hostapilib: $(BUILDDIR)/$(LIBFILE)


subdirs:
	@echo $(LIBS)
	@$(MAKE) -C $(SRCDIR)/host/core
	@$(MAKE) -C $(SRCDIR)/host/shell/tcpip_linux
	@$(MAKE) -C $(SRCDIR)/host/network/tcpip_linux
#	@$(AR) -rcT $(BUILDDIR)/libhostapi.a $(LIBS)

$(BUILDDIR)/$(LIBFILE): subdirs
	$(CC) $(LDFLAGS) $(BUILDDIR)/output/lib/*.o -o $(BUILDDIR)/$(LIBFILE)


ifdef PREFIX

install: $(BUILDDIR)/$(LIBFILE)
	install -d -m 775 $(PREFIX)/include $(PREFIX)/lib 
	install -m 664 $(SRCDIR)/common/inc/AmbaHost_api.h $(PREFIX)/include
	install -m 775 $(BUILDDIR)/$(LIBFILE) $(PREFIX)/lib
ifeq ($(UNAME),Darwin)
	cd $(PREFIX)/lib ;				\
	if [ ! -f $(LINKNAME) ]; then			\
	  ln -s $(notdir $(LIBFILE)) $(LINKNAME);	\
	fi
else
ifeq ($(UNAME),Linux)
	cd $(PREFIX)/lib ;			 \
	if [ ! -f $(SONAME) ]; then		 \
	  ln -s $(notdir $(LIBFILE)) $(SONAME);	 \
	fi;					 \
	if [ ! -f $(LINKNAME) ]; then		 \
	  ln -s $(notdir $(SONAME)) $(LINKNAME); \
	fi
else
	$(error "Unrecognized uname $(UNAME)")
endif
endif

else

install:
	@echo "Need to set PREFIX to use install target!"
	false

endif


clean:
	@echo "Cleaning up build dir $(BUILDDIR) ..."
	@rm -rf $(BUILDDIR)
