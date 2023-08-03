CFLAGS	+= -Wall -O2 -g -fPIC
LDFLAGS += -L./ -lambafs_notify -lambaipc -lcrc

TARGET   = ambafs_notify_test

all: $(TARGET)

ambafs_notify_test : ambafs_notify_test.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LDFLAGS)

clean:
	$(RM) -rf core $(TARGET) *.o

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/lib;
	for i in $(TARGET); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

uninstall: $(TARGET)
	for i in $(TARGET); do rm -f $(DESTDIR)/usr/bin/$$i; done

