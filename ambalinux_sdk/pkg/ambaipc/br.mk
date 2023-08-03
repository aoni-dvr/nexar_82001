CFLAGS	+= -Wall -O2 -g -fPIC
LDFLAGS += -L./ -lpthread -lambaipc

LIB_SRCS = svc.c clnt.c util.c
LIB_OBJS = $(LIB_SRCS:.c=.o)
IPCLIB   = libambaipc.so
TARGET   = ipcbind ipcinfo

all: $(IPCLIB) $(TARGET)

libambaipc.so: $(LIB_OBJS)
	@$(RM) $@
	$(CC) -shared -Wl,-soname,$@ -Wl,--as-needed -Wl,--warn-common -o $@  $(LIB_OBJS) -lcrc -lcrc_priv -lc -laipc_cfg

ipcbind : ipcbind.o util.o
	$(CC) $(CFLAGS) -o $@ ipcbind.o util.o -lcrc -lcrc_priv -laipc_cfg

ipcinfo : ipcinfo.o util.o
	$(CC) $(CFLAGS) -o $@ ipcinfo.o util.o -laipc_cfg

clean:
	$(RM) core $(IPCLIB) $(TARGET) *.o

install: $(IPCLIB) $(TARGET)
	mkdir -p $(DESTDIR)/usr/lib;
	for i in $(IPCLIB); do install -D -m 644  $$i $(DESTDIR)/usr/lib/$$i; done
	for i in $(TARGET); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

install_staging: $(IPCLIB) $(TARGET)
	mkdir -p $(DESTDIR)/usr/lib;
	for i in $(IPCLIB); do install -D -m 644  $$i $(DESTDIR)/usr/lib/$$i; done
	install -D -m 644 aipc_msg.h $(DESTDIR)/usr/include/;
	install -D -m 644 aipc_user.h $(DESTDIR)/usr/include/;
	install -D -m 644 AmbaIPC_Rpc_Def.h $(DESTDIR)/usr/include/;

uninstall: $(IPCLIB) $(TARGET)
	for i in $(IPCLIB); do rm -f $(DESTDIR)/usr/lib/$$i; done
	for i in $(TARGET); do rm -f $(DESTDIR)/usr/bin/$$i; done

