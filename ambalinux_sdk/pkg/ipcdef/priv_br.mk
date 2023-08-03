CFLAGS	+= -Wall -O2 -g -fPIC
LDFLAGS += -L./ -lpthread

HEADERS := $(shell find private -name '*.h')

CRC_SH := $(shell ./crc.sh priv_crc.h _PRIV_CRC_H_ private/ priv_crc_result priv_rpc_prog_id priv_rpc_num)
RPC_NUM := $(shell ls -l private | grep "total" | awk -F 'l' '{print $$2}')

CRC_PRIV_OBJ = aipc_priv_crc.o
CRC_PRIV_SRC = aipc_priv_crc.c

TARGET_SHARELIB = libcrc_priv.so

all: $(TARGET_SHARELIB)

libcrc_priv.so: $(CRC_PRIV_OBJ) $(HEADERS)
ifneq ($(strip $(RPC_NUM)),0)
	@$(RM) $@
	$(CC) -shared -Wl,-soname,$@ -o $@.1 $< $(LDFLAGS) -lc
	ln -s $@.1 $@
endif

$(CRC_PRIV_OBJ): $(HEADERS) $(CRC_PRIV_SRC)
	@$(RM) $@
	$(CC) $(CFLAGS) -c $(CRC_PRIV_SRC)

clean:
ifneq ($(strip $(RPC_NUM)), 0)
	$(RM) -rf *.o $(TARGET_SHARELIB)
endif

install: 
	mkdir -p $(DESTDIR)/usr/lib
	for i in $(TARGET_SHARELIB); do install -D -m 644 $$i $(DESTDIR)/usr/lib/$$i; done

install_staging:
	mkdir -p $(DESTDIR)/usr/lib
	mkdir -p $(DESTDIR)/usr/include
	for i in $(TARGET_SHARELIB); do install -D -m 644 $$i $(DESTDIR)/usr/lib/$$i; done
	for i in $(HEADERS); do install -D -m 644 $$i $(DESTDIR)/usr/include; done

uninstall:
ifneq ($(strip $(RPC_NUM)), 0)
	for i in $(TARGET_SHARELIB); do rm -f $(DESTDIR)/usr/lib/$$i; done;
endif

