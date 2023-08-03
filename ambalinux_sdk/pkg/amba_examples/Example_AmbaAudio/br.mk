CFLAGS		+=	$(CPPFLAGS) -Wall  -Werror -fPIC
LDFLAGS		+=	$(LIBS)

LIBS		+=	-lpthread \
				-lrt -lasound


OBJS		=  main.o
TARGET		= amba_audio

VER_SO = 1

.PHONY: all clean install

all: $(TARGET)

amba_audio: main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $(TARGET); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

clean:
	$(RM) -rf $(TARGET) $(OBJS)

