CFLAGS		+=	$(CPPFLAGS) -Wall  -Werror $(ARMBADFLAGS) -fPIC

LDFLAGS		+=	$(LIBS)
LDFLAGS		+= -lambamal

LIBS		+=	-lpthread

#for backtrace
CFLAGS		+=	-funwind-tables -rdynamic -g

OBJS		=	example_socket_client.o mem_util.o

TARGET		= example_socket_client

.PHONY: all clean install

all: $(TARGET)

example_socket_client: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $(TARGET); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

clean:
	$(RM) -rf $(TARGET) $(OBJS)
