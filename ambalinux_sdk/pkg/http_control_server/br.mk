#CFLAGS	+= -Wall -O2 -g -fPIC

LDFLAGS += -lpthread -lambaipc -lrtos_api -lsqlite3

TARGET   = nexar_httpd

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) -o $@ main.c cmd_handler.c cmd_response.c md5.c cJSON/cJSON.c ${LDFLAGS}
	${STRIP} $@

clean:
	$(RM) -rf $(TARGET) *.o

install: $(TARGET)
	for i in $(TARGET); do install -D -m 755 $$i $(DESTDIR)/usr/bin/$$i; done

uninstall: $(TARGET)
	for i in $(TARGET); do rm -f $(DESTDIR)/usr/bin/$$i; done

