CFLAGS	+= -Wall -O2 -g -fPIC
LDFLAGS += -L./ -lpthread -lambaipc -laipc_cfg

TARGET   = util_svc amba_pt

all: $(TARGET)

util_svc: util_svc.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LDFLAGS)

amba_pt: amba_pt.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LDFLAGS)

clean:
	$(RM) core $(TARGET) *.o amba_Camctrl/*.o

install: $(IPCLIB) $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $(TARGET); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

uninstall: $(IPCLIB) $(TARGET)
	for i in $(TARGET); do rm -f $(DESTDIR)/usr/bin/$$i; done

