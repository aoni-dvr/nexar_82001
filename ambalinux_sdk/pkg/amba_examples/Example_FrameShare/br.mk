CFLAGS		+=	$(CPPFLAGS) -Wall -fPIC

LDFLAGS		+=	$(LIBS)

LIBS		+=	-lpthread \
				-lambaipc

OBJS		=	example_frameshare.o

TARGET		= example_frameshare

.PHONY: all clean install

all: $(TARGET)

example_frameshare: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $(TARGET); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

clean:
	$(RM) -rf $(TARGET) $(OBJS)
