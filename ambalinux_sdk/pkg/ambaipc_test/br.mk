CFLAGS	+= -Wall -O2 -g -fPIC
LDFLAGS += -L./ -lpthread -lambaipc -lcrc

#IPCLIB   = ../ambaipc-1.0/libambaipc.so
TARGET   = svc_test clnt_test

all: $(IPCLIB) $(TARGET)

svc_test : $(IPCLIB) svc_test.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LDFLAGS)

clnt_test: $(IPCLIB) clnt_test.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LDFLAGS)

clean:
	$(RM) -rf core $(TARGET) *.o

install: $(IPCLIB) $(TARGET)
	mkdir -p $(DESTDIR)/usr/lib;
	for i in $(TARGET); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

uninstall: $(IPCLIB) $(TARGET)
	for i in $(TARGET); do rm -f $(DESTDIR)/usr/bin/$$i; done

