CFLAGS		+=	$(CPPFLAGS) -Wall  -Werror -fPIC

LDFLAGS		+=	$(LIBS)

LIBS		+=	-lambanetfifo

#for backtrace
CFLAGS		+=	#-funwind-tables -rdynamic -g

OBJS		=	example_streamin.o example_source.o

TARGET		= example_streamin

.PHONY: all clean install

all: $(TARGET)

example_streamin: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $(TARGET); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

clean:
	$(RM) -rf $(TARGET) $(OBJS)
