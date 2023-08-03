CFLAGS		+=	$(CPPFLAGS) -Wall  -Werror -fPIC
LDFLAGS		+=	$(LIBS)

LIBS		+=	-lpthread \
				-lambaipc


LIB_OBJS	=  AmbaTransfer.o \
			   AmbaRpcProg_LU_Memio.o \
			   Transfer_impl_Camctrl.o \
			   Transfer_impl_DsiRes_Socket.o \
			   Transfer_impl_MemIO_ambalink.o \
			   Transfer_impl_MemIO.o

TARGET_STATICLIB	= libambatransfer.a
TARGET_SHARELIB	= libambatransfer.so

VER_SO = 1

.PHONY: all clean install

all: $(TARGET_STATICLIB) $(TARGET_SHARELIB)

libambatransfer.a: $(LIB_OBJS)
	@$(RM) $@
	$(AR) crs $@ $(LIB_OBJS)

libambatransfer.so: $(LIB_OBJS)
	@$(RM) $@
	$(CC) -shared -Wl,-soname,$@ -o $@.1 $(LIB_OBJS) $(LDFLAGS) -lc
	ln -s $@.$(VER_SO) $@

install: $(TARGET_STATICLIB) $(TARGET_SHARELIB)
	mkdir -p $(DESTDIR)/usr/lib;
	$(RM) -rf $(DESTDIR)/usr/lib/libambatransfer.so*;
	install -D -m 755 libambatransfer.so.$(VER_SO) $(DESTDIR)/usr/lib/libambatransfer.so.$(VER_SO);
	ln -s libambatransfer.so.$(VER_SO) $(DESTDIR)/usr/lib/libambatransfer.so;

lib_install: $(TARGET_STATICLIB) $(TARGET_SHARELIB)
	mkdir -p $(DESTDIR)/usr/lib;
	install -D -m 644 libambatransfer.a $(DESTDIR)/usr/lib/libambatransfer.a
	$(RM) -rf $(DESTDIR)/usr/lib/libambatransfer.so*;
	install -D -m 755 libambatransfer.so.$(VER_SO) $(DESTDIR)/usr/lib/libambatransfer.so.$(VER_SO);
	ln -s libambatransfer.so.$(VER_SO) $(DESTDIR)/usr/lib/libambatransfer.so;
	install -D -m 644 *.h $(DESTDIR)/usr/include/;

lib_uninstall:
	$(RM) -rf $(DESTDIR)/usr/lib/libambatransfer.a
	$(RM) -rf $(DESTDIR)/usr/lib/libambatransfer.so.1
	$(RM) -rf $(DESTDIR)/usr/lib/libambatransfer.so.1.direct
	$(RM) -rf $(DESTDIR)/usr/lib/libambatransfer.so
	$(RM) -rf $(DESTDIR)/usr/include/AmbaTransfer*.h

clean:
	$(RM) -rf $(LIB_OBJS) $(TARGET_STATICLIB) $(TARGET_SHARELIB)

