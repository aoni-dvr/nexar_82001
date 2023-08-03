CFLAGS		+=	$(CPPFLAGS) -Wall  -Werror $(ARMBADFLAGS)

LIBS		+= 	-lpthread

LDFLAGS		+=	$(LIBS)

#for backtrace
CFLAGS		+=	-funwind-tables -rdynamic -g

CXXFLAGS	+=	-std=c++11 -I$(STAGING_DIR)/usr/include/opencv4

CXXLIBS		+=	-lpthread -lstdc++ \
			-lopencv_core -lopencv_imgcodecs -lopencv_imgproc

CXXLDFLAGS		+=	$(CXXLIBS)

STAGINGLIBPATH = $(abspath $(STAGING_DIR)/usr/lib)
STDLIB = libstdc++.so
OPENCVLIB = libopencv_core.so
OPENCVIMGPROCLIB = libopencv_imgproc.so
OPENCVIMGCODECSLIB = libopencv_imgcodecs.so

STATICLIBS	=	$(STAGINGLIBPATH)/$(STDLIB) \
				$(STAGINGLIBPATH)/$(OPENCVLIB) \
				$(STAGINGLIBPATH)/$(OPENCVIMGPROCLIB) \
				$(STAGINGLIBPATH)/$(OPENCVIMGCODECSLIB)

LIBOBJS		= 	example_osdutil.o

OBJS		=	example_osddraw.o

MP4OBJS		=	example_readmp4.o

TARGET_EXEC		= example_osddraw example_readmp4
TARGET_STATICLIB	= libexample_osdutil.a
TARGET_SHARELIB	= libexample_osdutil.so

VER_SO = 1

.PHONY: all clean install lib_install

all: $(TARGET_STATICLIB) $(TARGET_SHARELIB) $(TARGET_EXEC)

#This is an example to draw OSD with opencv APIs.
example_osddraw: $(OBJS) $(TARGET_STATICLIB)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(TARGET_STATICLIB)

#This is another example to read video file with opencv APIs.
example_readmp4: $(MP4OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(CXXLDFLAGS) -lopencv_videoio

libexample_osdutil.a: $(LIBOBJS)
	@$(RM) $@
	$(AR) crs $@ $(LIBOBJS) $(STATICLIBS)

libexample_osdutil.so: $(LIBOBJS)
	@$(RM) $@
	$(CXX) -fPIC -shared -Wl,-soname,$@ -o $@.$(VER_SO) $(LIBOBJS) $(CXXLDFLAGS) -lc
	ln -s $@.$(VER_SO) $@

install: $(TARGET_EXEC)
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $(TARGET_EXEC); do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

lib_install: $(TARGET_STATICLIB) $(TARGET_SHARELIB)
	mkdir -p $(DESTDIR)/usr/lib;
	install -D -m 644 libexample_osdutil.a $(DESTDIR)/usr/lib/libexample_osdutil.a
	$(RM) -rf $(DESTDIR)/usr/lib/libexample_osdutil.so*;
	install -D -m 755 libexample_osdutil.so.$(VER_SO) $(DESTDIR)/usr/lib/libexample_osdutil.so.$(VER_SO);
	ln -s libexample_osdutil.so.$(VER_SO) $(DESTDIR)/usr/lib/libexample_osdutil.so;
	install -D -m 644 example_osdutil.h $(DESTDIR)/usr/include/example_osdutil.h

lib_uninstall:
	$(RM) -rf $(DESTDIR)/usr/lib/libexample_osdutil.a
	$(RM) -rf $(DESTDIR)/usr/lib/libexample_osdutil.so.$(VER_SO)
	$(RM) -rf $(DESTDIR)/usr/lib/libexample_osdutil.so
	$(RM) -rf $(DESTDIR)/usr/include/example_osdutil.h

clean:
	$(RM) -rf $(TARGET_EXEC) $(OBJS) $(LIBOBJS) $(TARGET_STATICLIB) $(TARGET_SHARELIB)
