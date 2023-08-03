CFLAGS		+=	$(CPPFLAGS) -Wall -Werror -fPIC

LDFLAGS		+=	$(LIBS)

LIBS		+=	-lrt \
			-lutil \
			-lambaeventnotifier

SEND_OBJS	=	send_to_rtos.o

RECV_OBJS	=	recv_from_rtos.o

TARGET		= SendToRTOS RecvFromRTOS

.PHONY: all clean install

all: $(TARGET)

SendToRTOS: $(SEND_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

RecvFromRTOS: $(RECV_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $(TARGET); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

clean:
	$(RM) -rf $(TARGET) $(SEND_OBJS) $(RECV_OBJS)

