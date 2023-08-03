CFLAGS		+=	$(CPPFLAGS) -Wall -fPIC

LDFLAGS		+=	$(LIBS) -L./

LIBS		+=	-lrt \
			-lutil

OBJS		=	libambamem.o

TARGET_STATICLIB	=	libambamem.a
TARGET_SHARELIB	= libambamem.so
TARGET		= t_mmap loadbin

.PHONY: all clean install

all: $(TARGET_LIB) $(TARGET_SHARELIB) $(TARGET)

libambamem.a: $(OBJS)
	@$(RM) $@
	$(AR) crs $@ $(OBJS)

libambamem.so: $(OBJS)
	@$(RM) $@
	$(CC) -shared -Wl,-soname,$@ -o $@.1 $(OBJS) $(LDFLAGS) -lc
	ln -s $@.1 $@

t_mmap: t_mmap.o libambamem.so
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lambamem

loadbin: loadbin.o libambamem.so
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lambamem

install: $(TARGET_SHARELIB) $(TARGET)
	mkdir -p $(DESTDIR)/usr/lib;
	$(RM) -rf $(DESTDIR)/usr/lib/libambamem.so*;
	install -D -m 755 libambamem.so.1 $(DESTDIR)/usr/lib/libambamem.so.1;
	ln -s libambamem.so.1 $(DESTDIR)/usr/lib/libambamem.so;
	mkdir -p $(DESTDIR)/usr/bin;
	ln -sf loadbin $(DESTDIR)/usr/bin/savebin;
	ln -sf loadbin $(DESTDIR)/usr/bin/readl;
	ln -sf loadbin $(DESTDIR)/usr/bin/writel;
	for i in $(TARGET); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

lib_install: $(TARGET_STATICLIB) $(TARGET_SHARELIB)
	mkdir -p $(DESTDIR)/usr/lib;
	$(RM) -rf $(DESTDIR)/usr/lib/libambamem.so*;
	install -D -m 644 libambamem.a $(DESTDIR)/usr/lib/libambamem.a
	install -D -m 755 libambamem.so.1 $(DESTDIR)/usr/lib/libambamem.so.1
	ln -s libambamem.so.1 $(DESTDIR)/usr/lib/libambamem.so
	install -D -m 644 libambamem.h $(DESTDIR)/usr/include/libambamem.h
	install -D -m 644 local_mem_util.h $(DESTDIR)/usr/include/local_mem_util.h
	install -D -m 644 ppm.h $(DESTDIR)/usr/include/ppm.h

lib_uninstall:
	$(RM) -rf $(DESTDIR)/usr/lib/libambamem.a
	$(RM) -rf $(DESTDIR)/usr/lib/libambamem.so.1
	$(RM) -rf $(DESTDIR)/usr/lib/libambamem.so
	$(RM) -rf $(DESTDIR)/usr/include/libambamem.h
	$(RM) -rf $(DESTDIR)/usr/include/local_mem_util.h
	$(RM) -rf $(DESTDIR)/usr/include/ppm.h

clean:
	$(RM) -rf $(TARGET) $(OBJS) $(TARGET_STATICLIB) $(TARGET_SHARELIB)

