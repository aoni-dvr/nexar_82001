CFLAGS		+=	$(CPPFLAGS) -Wall  -Werror -fPIC
LDFLAGS		+=	$(LIBS)

LIBS		+=	-lpthread
ifeq ($(BR2_PACKAGE_AMBAIPC),y)
LIBS            +=      -lambaipc
CFLAGS          +=      -DAMBAIPC
endif


LIB_OBJS	=  AmbaFlexidagIO.o

TARGET_STATICLIB	= libambaflexidagio.a
TARGET_SHARELIB	= libambaflexidagio.so

VER_SO = 1

.PHONY: all clean install

all: $(TARGET_STATICLIB) $(TARGET_SHARELIB)

libambaflexidagio.a: $(LIB_OBJS)
	@$(RM) $@
	$(AR) crs $@ $(LIB_OBJS)

libambaflexidagio.so: $(LIB_OBJS)
	@$(RM) $@
	$(CC) -shared -Wl,-soname,$@ -o $@.1 $(LIB_OBJS) $(LDFLAGS) -lc
	ln -s $@.$(VER_SO) $@

install: $(TARGET_STATICLIB) $(TARGET_SHARELIB)
	mkdir -p $(DESTDIR)/usr/lib;
	$(RM) -rf $(DESTDIR)/usr/lib/libambaflexidagio.so*;
	install -D -m 755 libambaflexidagio.so.$(VER_SO) $(DESTDIR)/usr/lib/libambaflexidagio.so.$(VER_SO);
	ln -s libambaflexidagio.so.$(VER_SO) $(DESTDIR)/usr/lib/libambaflexidagio.so;

lib_install: $(TARGET_STATICLIB) $(TARGET_SHARELIB)
	mkdir -p $(DESTDIR)/usr/lib;
	install -D -m 644 libambaflexidagio.a $(DESTDIR)/usr/lib/libambaflexidagio.a
	$(RM) -rf $(DESTDIR)/usr/lib/libambaflexidagio.so*;
	install -D -m 755 libambaflexidagio.so.$(VER_SO) $(DESTDIR)/usr/lib/libambaflexidagio.so.$(VER_SO);
	ln -s libambaflexidagio.so.$(VER_SO) $(DESTDIR)/usr/lib/libambaflexidagio.so;
	install -D -m 644 *.h $(DESTDIR)/usr/include/;

lib_uninstall:
	$(RM) -rf $(DESTDIR)/usr/lib/libambaflexidagio.a
	$(RM) -rf $(DESTDIR)/usr/lib/libambaflexidagio.so.1
	$(RM) -rf $(DESTDIR)/usr/lib/libambaflexidagio.so.1.direct
	$(RM) -rf $(DESTDIR)/usr/lib/libambaflexidagio.so
	$(RM) -rf $(DESTDIR)/usr/include/AmbaFlexidagIO*.h

clean:
	$(RM) -rf $(LIB_OBJS) $(TARGET_STATICLIB) $(TARGET_SHARELIB)

