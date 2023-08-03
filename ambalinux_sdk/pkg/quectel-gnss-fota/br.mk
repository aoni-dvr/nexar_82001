CFLAGS		+=	$(CPPFLAGS) -Wall  -Werror $(ARMBADFLAGS) -fPIC

LDFLAGS		+=	$(LIBS) -lambaipc -lrtos_api

#for backtrace
CFLAGS		+=	-funwind-tables -rdynamic -g

TARGET		= quectel-gnss-fota

LIBS = -pthread -lrt

.PHONY: all clean install

all: $(TARGET)

$(TARGET):
	rm -rf *.o
	$(CC) -o ${TARGET} main.c ota.c serial_port.c ${LDFLAGS}

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	install -D -m 755 $(TARGET) $(DESTDIR)/usr/bin/$(TARGET);

clean:
	rm -rf libs *.o $(TARGET) *~
