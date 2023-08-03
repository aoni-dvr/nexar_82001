CFLAGS		+=	$(CPPFLAGS) -Wall -Werror -fPIC -I./

LDFLAGS		+=	$(TARGET_LDFLAGS) -L./ -lromfs

.PHONY: all clean install

all: libromfs.a romfs

libromfs.a: libromfs.o
	@$(RM) $@
	$(AR) crs $@ libromfs.o

romfs: romfs.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: romfs
	mkdir -p $(DESTDIR)/usr/bin;
	install -D -m 755 libromfs.h $(STAGING_DIR)/usr/include;
	for i in romfs; do install -D -m 755 $$i $(DESTDIR)/usr/bin/$$i; done

clean:
	$(RM) -rf romfs* libromfs*
