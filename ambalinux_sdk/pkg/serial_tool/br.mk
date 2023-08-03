CFLAGS		+=	$(CPPFLAGS) -Wall  -Werror $(ARMBADFLAGS) -fPIC

LDFLAGS		+=	$(LIBS)

#for backtrace
CFLAGS		+=	-funwind-tables -rdynamic -g

TARGET		= gps_test_tool

.PHONY: all clean install

all:
	$(CC) $(CFLAGS) -o gps_test_tool *.c ${LDFLAGS} -lambaipc -lpthread

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	install -D -m 755 gps_test_tool $(DESTDIR)/usr/bin/gps_test_tool;

clean:
	$(RM) -rf $(TARGET) *.o
