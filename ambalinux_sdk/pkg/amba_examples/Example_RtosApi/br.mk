CFLAGS		+=	$(CPPFLAGS) -Wall  -Werror -Wno-error=unused-result $(ARMBADFLAGS) -fPIC

LDFLAGS		+=	$(LIBS)

#for backtrace
CFLAGS		+=	-funwind-tables -rdynamic -g

TARGET		= rtos_api hwid burn_info gpio_level linux_time_service stream_demo rtos_log camera_info metadata_reader factory_tool imu_kalman

LIB_OBJS	=  rtos_api_lib.c lock.c

FACTORY_OBJS = factoryd/factoryd.c factoryd/cJSON.c factoryd/cmd_handler.c factoryd/cmd_response.c 

.PHONY: all clean install

VER_SO=1

all: librtos_api.so

librtos_api.so: ${LIB_OBJS}
	@$(CC) -shared -Wl,-soname,$@ -o $@.$(VER_SO) $(LIB_OBJS) -lambaipc -lpthread $(LDFLAGS) -fPIC
	ln -s $@.$(VER_SO) $@

rtos_api: rtos_api.o imu_madgwick.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lrtos_api -lpthread -lm -L.

hwid: hwid.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lrtos_api -L.

burn_info: burn_info.o serial_port.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lrtos_api -L.

gpio_level: gpio_level.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lrtos_api -L.
	
linux_time_service: linux_time_service.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lambaipc -lpthread

stream_demo: stream_demo.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lrtos_api -lambaipc -lpthread

rtos_log: rtos_log.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lrtos_api -lambaipc -lpthread

camera_info: camera_info.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lrtos_api -lambaipc -lpthread

metadata_reader: metadata_reader.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

factory_tool: ${FACTORY_OBJS}
	$(CC) $(CFLAGS) -o $@ ${FACTORY_OBJS} $(LDFLAGS) -lrtos_api -lambaipc -lpthread

imu_kalman: imu_kalman.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lrtos_api -lambaipc -lpthread -lm

lib_install: librtos_api.so
	mkdir -p $(DESTDIR)/usr/lib;
	$(RM) -rf $(DESTDIR)/usr/lib/librtos_api.so*;
	$(RM) -rf $(DESTDIR)/usr/lib/libcamera.so*;
	install -D -m 755 librtos_api.so.$(VER_SO) $(DESTDIR)/usr/lib/librtos_api.so.$(VER_SO);
	ln -s librtos_api.so.$(VER_SO) $(DESTDIR)/usr/lib/librtos_api.so;
	ln -s librtos_api.so.$(VER_SO) $(DESTDIR)/usr/lib/libcamera.so;
	install -D -m 644 rtos_api_lib.h $(DESTDIR)/usr/include/;

lib_uninstall: librtos_api.so
	$(RM) -rf $(DESTDIR)/usr/lib/librtos_api.so*
	$(RM) -rf $(DESTDIR)/usr/lib/libcamera.so*
	$(RM) -rf $(DESTDIR)/usr/include/rtos_api_lib.h

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	install -D -m 755 rtos_api $(DESTDIR)/usr/bin/rtos_api;
	install -D -m 755 hwid $(DESTDIR)/usr/bin/hwid;
	install -D -m 755 burn_info $(DESTDIR)/usr/bin/burn_info;
	install -D -m 755 gpio_level $(DESTDIR)/usr/bin/gpio_level;
	install -D -m 755 linux_time_service $(DESTDIR)/usr/bin/linux_time_service;
	install -D -m 755 stream_demo $(DESTDIR)/usr/bin/stream_demo;
	install -D -m 755 rtos_log $(DESTDIR)/usr/bin/rtos_log;
	install -D -m 755 camera_info $(DESTDIR)/usr/bin/camera_info;
	install -D -m 755 metadata_reader $(DESTDIR)/usr/bin/metadata_reader;
	install -D -m 755 factory_tool $(DESTDIR)/usr/bin/factoryd;
	install -D -m 755 imu_kalman $(DESTDIR)/usr/bin/imu_kalman;
	mkdir -p $(DESTDIR)/usr/lib;
	$(RM) -rf $(DESTDIR)/usr/lib/librtos_api.so*;
	$(RM) -rf $(DESTDIR)/usr/lib/libcamera.so*;
	install -D -m 755 librtos_api.so.$(VER_SO) $(DESTDIR)/usr/lib/librtos_api.so.$(VER_SO);
	ln -s librtos_api.so.$(VER_SO) $(DESTDIR)/usr/lib/librtos_api.so;
	ln -s librtos_api.so.$(VER_SO) $(DESTDIR)/usr/lib/libcamera.so;

clean:
	$(RM) -rf $(TARGET) librtos_api.so* libcamera.so rtos_api.o hwid.o burn_info.o imu_kalman.o gpio_level.o linux_time_service.o stream_demo.o rtos_log.o camera_info.o metadata_reader.o factoryd/*.o
