CFLAGS		+=	$(CPPFLAGS) -Wall -fPIC

LDFLAGS		+=	$(LIBS) -L./

LIBS		+=	-lrt \
			-lutil

OBJS		=	ambasocksvc.o

TMOD_OBJS	=	libambasocksvc_svc_test.o

TMD_CLNT_OBJS	=	libambasocksvc_clnt_test.o

TARGET_STATICLIB	=	libambasocksvc.a
TARGET_SHARELIB	= libambasocksvc.so
TARGET		= libambasocksvc_svc_test libambasocksvc_clnt_test

.PHONY: all clean install

all: $(TARGET_LIB) $(TARGET_SHARELIB) $(TARGET)

libambasocksvc.a: $(OBJS)
	@$(RM) $@
	$(AR) crs $@ $(OBJS)

libambasocksvc.so: $(OBJS)
	@$(RM) $@
	$(CC) -shared -Wl,-soname,$@ -o $@.1 $(OBJS) $(LDFLAGS) -lc
	ln -s $@.1 $@

libambasocksvc_svc_test: $(TMOD_OBJS) libambasocksvc.so
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lambasocksvc

libambasocksvc_clnt_test: $(TMD_CLNT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(TARGET_SHARELIB) $(TARGET)
	mkdir -p $(DESTDIR)/usr/lib;
	$(RM) -rf $(DESTDIR)/usr/lib/libambasocksvc.so*;
	install -D -m 755 libambasocksvc.so.1 $(DESTDIR)/usr/lib/libambasocksvc.so.1;
	ln -s libambasocksvc.so.1 $(DESTDIR)/usr/lib/libambasocksvc.so;
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $(TARGET); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

lib_install: $(TARGET_STATICLIB) $(TARGET_SHARELIB)
	mkdir -p $(DESTDIR)/usr/lib;
	$(RM) -rf $(DESTDIR)/usr/lib/libambasocksvc.so*;
	install -D -m 644 libambasocksvc.a $(DESTDIR)/usr/lib/libambasocksvc.a
	install -D -m 755 libambasocksvc.so.1 $(DESTDIR)/usr/lib/libambasocksvc.so.1
	ln -s libambasocksvc.so.1 $(DESTDIR)/usr/lib/libambasocksvc.so;
	install -D -m 644 ambasocksvc.h $(DESTDIR)/usr/include/ambasocksvc.h

lib_uninstall:
	$(RM) -rf $(DESTDIR)/usr/lib/libambasocksvc.a
	$(RM) -rf $(DESTDIR)/usr/lib/libambasocksvc.so.1
	$(RM) -rf $(DESTDIR)/usr/lib/libambasocksvc.so
	$(RM) -rf $(DESTDIR)/usr/include/ambasocksvc.h

clean:
	$(RM) -rf $(TARGET) $(OBJS) $(TMOD_OBJS) $(TARGET_STATICLIB) $(TARGET_SHARELIB)

