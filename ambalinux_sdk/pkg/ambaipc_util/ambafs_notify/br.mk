CFLAGS	+= -Wall -O2 -g -fPIC
LDFLAGS += -L./

LIB_SRCS = ambafs_notify.c ambafs_notify_clnt.c
LIB_OBJS = $(LIB_SRCS:.c=.o)
NOTIFYLIB = libambafs_notify.so

all: $(NOTIFYLIB)

libambafs_notify.so: $(LIB_OBJS)
	@$(RM) $@
	$(CC) -shared -Wl,-soname,$@ -Wl,--as-needed -Wl,--warn-common -o $@  $(LIB_OBJS) -lc

clean:
	$(RM) core $(NOTIFYLIB) *.o

install: $(NOTIFYLIB)
	mkdir -p $(DESTDIR)/usr/lib;
	for i in $(NOTIFYLIB); do install -D -m 644  $$i $(DESTDIR)/usr/lib/$$i; done

install_staging: $(NOTIFYLIB)
	mkdir -p $(DESTDIR)/usr/lib;
	for i in $(NOTIFYLIB); do install -D -m 644  $$i $(DESTDIR)/usr/lib/$$i; done
	install -D -m 644 ambafs_notify.h $(DESTDIR)/usr/include/;

uninstall: $(NOTIFYLIB)
	for i in $(NOTIFYLIB); do rm -f $(DESTDIR)/usr/lib/$$i; done
