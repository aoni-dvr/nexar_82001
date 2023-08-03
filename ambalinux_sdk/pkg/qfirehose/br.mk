SRC=firehose_protocol.c  qfirehose.c  sahara_protocol.c usb_linux.c stream_download_protocol.c md5.c usb2tcp.c 

CFLAGS += -Wall -Wextra -Werror -O1 #-s
LDFLAGS += -lpthread -ldl
TARGET=QFirehose

all: clean
	${CC} ${CFLAGS} ${SRC} -o QFirehose ${LDFLAGS}

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	install -D -m 755 $(TARGET) $(DESTDIR)/usr/bin/$(TARGET);

clean:
	rm -rf QFirehose obj libs usb2tcp *~
