CFLAGS		+=	$(CPPFLAGS) -Wall -fPIC
LDFLAGS		+=	$(LIBS)

LIBS		+=	-lpthread \
				-lambaipc


OBJS		=  example_camctrlserver.o
CLIENT_OBJS =  example_camctrlclient.o
TARGET		= camctrlserver example_camctrlclient amba_camctrl amba_camsvc

VER_SO = 1

.PHONY: all clean install

all: $(TARGET)

camctrlserver: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

example_camctrlclient: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

amba_camctrl: amba_camctrl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

amba_camsvc: amba_camsvc.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $(TARGET); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

clean:
	$(RM) -rf $(TARGET) $(OBJS)

