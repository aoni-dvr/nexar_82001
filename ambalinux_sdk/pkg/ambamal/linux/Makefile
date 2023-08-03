PWD	:= $(shell pwd)

CFLAGS		+=	$(CPPFLAGS) -Wall  -Werror -fPIC
CFLAGS		+= -I./ -I./linux -I./inc
LDFLAGS		+=	$(LIBS)

LIB_OBJS	=  linux/AmbaMAL_Linux.o

ifeq ($(BR2_PACKAGE_BUILD_AMBAMAL_SHARED_LIB),y)
TARGET_STATICLIB	= 
TARGET_SHARELIB		= libambamal.so
else
TARGET_STATICLIB	= libambamal.a
TARGET_SHARELIB		= 
endif

VER_SO = 1

.PHONY: all clean install

all: $(TARGET_STATICLIB) $(TARGET_SHARELIB)

libambamal.a: $(LIB_OBJS)
	@$(RM) $@
	$(AR) crs $@ $(LIB_OBJS)

libambamal.so: $(LIB_OBJS)
	@$(RM) $@
	$(CC) -shared -Wl,-soname,$@ -o $@.1 $(LIB_OBJS) $(LDFLAGS) -lc
	ln -s $@.$(VER_SO) $@

%.o : %.c
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

install: $(TARGET_STATICLIB) $(TARGET_SHARELIB)
	mkdir -p $(DESTDIR)/usr/lib;
ifeq ($(BR2_PACKAGE_BUILD_AMBAMAL_SHARED_LIB),y)
	$(RM) -rf $(DESTDIR)/usr/lib/libambamal.so*;
	install -D -m 755 libambamal.so.$(VER_SO) $(DESTDIR)/usr/lib/libambamal.so.$(VER_SO);
	ln -s libambamal.so.$(VER_SO) $(DESTDIR)/usr/lib/libambamal.so;
endif

install_staging: $(TARGET_STATICLIB) $(TARGET_SHARELIB)
	mkdir -p $(DESTDIR)/usr/lib;
ifeq ($(BR2_PACKAGE_BUILD_AMBAMAL_SHARED_LIB),y)
	$(RM) -rf $(DESTDIR)/usr/lib/libambamal.so*;
	install -D -m 755 libambamal.so.$(VER_SO) $(DESTDIR)/usr/lib/libambamal.so.$(VER_SO);
	ln -s libambamal.so.$(VER_SO) $(DESTDIR)/usr/lib/libambamal.so;
else
	install -D -m 644 libambamal.a $(DESTDIR)/usr/lib/libambamal.a
endif
	install -D -m 644 ./inc/*.h $(DESTDIR)/usr/include/;

uninstall_staging:
ifeq ($(BR2_PACKAGE_BUILD_AMBAMAL_SHARED_LIB),y)
	$(RM) -rf $(DESTDIR)/usr/lib/libambamal.so.1
	$(RM) -rf $(DESTDIR)/usr/lib/libambamal.so.1.direct
	$(RM) -rf $(DESTDIR)/usr/lib/libambamal.so
else
	$(RM) -rf $(DESTDIR)/usr/lib/libambamal.a
endif
	$(RM) -rf $(DESTDIR)/usr/include/AmbaMAL*.h

clean:
	$(RM) -rf $(LIB_OBJS) $(TARGET_STATICLIB) $(TARGET_SHARELIB)

