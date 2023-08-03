CFLAGS	+= -Wall -O2 -g -fPIC
LDFLAGS += -L./ -lpthread

HEADERS := $(shell find . -name '*.h')

CRC_SH := $(shell ./crc.sh crc.h _CRC_H_ ./ crc_result rpc_prog_id rpc_num)
CRC_SRC = aipc_crc.c
CRC_OBJ = aipc_crc.o

TARGET_SHARELIB = libcrc.so libaipc_cfg.so

all: $(TARGET_SHARELIB)

libcrc.so: $(CRC_OBJ)
	@$(RM) $@
	$(CC) -shared -Wl,-soname,$@ -o $@.1 $< $(LDFLAGS) -lc
	ln -s $@.1 $@

libaipc_cfg.so: aipc_cfg.o
	@$(RM) $@
	$(CC) -shared -Wl,-soname,$@ -o $@.1 $< $(LDFLAGS) -lc
	ln -s $@.1 $@

$(CRC_OBJ): $(HEADERS) $(CRC_SRC)
	@$(RM) $@
	$(CC) $(CFLAGS) -c $(CRC_SRC)

clean:
	$(RM) -rf *.o $(TARGET_SHARELIB)

install: $(TARGET_SHARELIB)
	mkdir -p $(DESTDIR)/usr/lib;
	for i in $(TARGET_SHARELIB); do install -D -m 644 $$i $(DESTDIR)/usr/lib/$$i; done

install_staging:
	mkdir -p $(DESTDIR)/usr/lib;
	mkdir -p $(DESTDIR)/usr/include;
	for i in $(TARGET_SHARELIB); do install -D -m 644 $$i $(DESTDIR)/usr/lib/$$i; done
	for i in $(HEADERS); do install -D -m 644 $$i $(DESTDIR)/usr/include; done

uninstall:
	for i in $(TARGET_SHARELIB); do rm -f $(DESTDIR)/usr/lib/$$i; done

