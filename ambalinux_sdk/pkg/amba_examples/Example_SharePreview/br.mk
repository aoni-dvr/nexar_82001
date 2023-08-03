CFLAGS		+=	$(CPPFLAGS) -Wall -fPIC

LDFLAGS		+=	$(LIBS)

LIBS		+=	-lpthread \
				-lambaipc

OBJS		=	example_sharepreview.o

TARGET		= example_sharepreview

.PHONY: all clean install

all: $(TARGET)

example_sharepreview: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $(TARGET); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

clean:
	$(RM) -rf $(TARGET) $(OBJS)
