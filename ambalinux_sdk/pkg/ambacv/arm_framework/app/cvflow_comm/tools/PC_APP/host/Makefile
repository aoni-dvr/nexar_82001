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

hostapilib:
	@echo $(LIBS)
	@$(MAKE) -C $(SRCDIR)/host/core
	@$(MAKE) -C $(SRCDIR)/host/shell/tcpip_linux
	@$(MAKE) -C $(SRCDIR)/host/network/tcpip_linux
#	@$(AR) -rcT $(BUILDDIR)/libhostapi.a $(LIBS)
	$(CC) -shared -fPIC -Wl,-soname,libhostapi.so.1 \
		-o $(BUILDDIR)/libhostapi.so.1.0 $(BUILDDIR)/output/lib/*.o
	@ln -snf $(BUILDDIR)/libhostapi.so.1.0 $(BUILDDIR)/libhostapi.so.1
	@ln -snf $(BUILDDIR)/libhostapi.so.1.0 $(BUILDDIR)/libhostapi.so

clean:
	@echo "Cleaning up build dir $(BUILDDIR) ..."
	@rm -rf $(BUILDDIR)
