CFLAGS		+=	$(CPPFLAGS) -Wall -Werror -fPIC

LIBS		+=	-lambaipc

LDFLAGS		+=	$(LIBS) -L./

OBJS		= bbx_handler.o

OBJS_TMOD	= bbx_handler_test.o

TARGET_EXEC		= bbx_handler_test
TARGET_STATICLIB	= libbbxhandler.a
TARGET_SHARELIB	= libbbxhandler.so

VER_SO = 1

.PHONY: all clean install lib_install

all: $(TARGET_STATICLIB) $(TARGET_SHARELIB) $(TARGET_EXEC)

libbbxhandler.a: $(OBJS)
	@$(RM) $@
	$(AR) crs $@ $(OBJS)

libbbxhandler.so: $(OBJS)
	@$(RM) $@
	$(CC) -shared -Wl,-soname,$@ -o $@.1 $(OBJS) $(LDFLAGS) -lc
	ln -s $@.$(VER_SO) $@

bbx_handler_test: $(OBJS_TMOD) $(TARGET_SHARELIB)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lbbxhandler

install: $(TARGET_SHARELIB) $(TARGET_EXEC)
	mkdir -p $(DESTDIR)/usr/lib;
	$(RM) -rf $(DESTDIR)/usr/lib/libbbxhandler.so*;
	install -D -m 755 libbbxhandler.so.$(VER_SO) $(DESTDIR)/usr/lib/libbbxhandler.so.$(VER_SO);
	ln -s libbbxhandler.so.$(VER_SO) $(DESTDIR)/usr/lib/libbbxhandler.so;
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $(TARGET_EXEC); do $(RM) -rf $(DESTDIR)/usr/bin/$$i; done
	for i in $(TARGET_EXEC); do install -D -m 755 $$i $(DESTDIR)/usr/bin/$$i; done

lib_install: $(TARGET_STATICLIB) $(TARGET_SHARELIB)
	mkdir -p $(DESTDIR)/usr/lib;
	install -D -m 644 libbbxhandler.a $(DESTDIR)/usr/lib/libbbxhandler.a
	$(RM) -rf $(DESTDIR)/usr/lib/libbbxhandler.so*;
	install -D -m 755 libbbxhandler.so.$(VER_SO) $(DESTDIR)/usr/lib/libbbxhandler.so.$(VER_SO);
	ln -s libbbxhandler.so.$(VER_SO) $(DESTDIR)/usr/lib/libbbxhandler.so;
	install -D -m 644 bbx_handler.h $(DESTDIR)/usr/include/bbx_handler.h

lib_uninstall:
	$(RM) -rf $(DESTDIR)/usr/lib/libbbxhandler.a
	$(RM) -rf $(DESTDIR)/usr/lib/libbbxhandler.so.1
	$(RM) -rf $(DESTDIR)/usr/lib/libbbxhandler.so.1.direct
	$(RM) -rf $(DESTDIR)/usr/lib/libbbxhandler.so
	$(RM) -rf $(DESTDIR)/usr/include/bbxhandler.h

clean:
	$(RM) -rf $(TARGET_EXEC) $(OBJ_SVC) $(OBJ_CLNT) $(OBJ_TMOD) $(TARGET_STATICLIB) $(TARGET_SHARELIB)

