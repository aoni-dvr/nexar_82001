CFLAGS		+=	$(CPPFLAGS) -Wall  -Werror $(ARMBADFLAGS) -fPIC

LDFLAGS		+=	$(LIBS)

LIBS		+=	-lpthread \
			-lambanetfifo

#for backtrace
CFLAGS		+=	-funwind-tables -rdynamic -g

OBJS		=	example_framer.o

TARGET		= example_framer

.PHONY: all clean install

all: $(TARGET)

example_framer: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $(TARGET); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

clean:
	$(RM) -rf $(TARGET) $(OBJS)
