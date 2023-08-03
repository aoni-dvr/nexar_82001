CFLAGS		+=	$(CPPFLAGS) -Wall  -Werror $(ARMBADFLAGS) -fPIC

LDFLAGS		+=	$(LIBS)

#for backtrace
CFLAGS		+=	-funwind-tables -rdynamic -g

TARGET		= qflash

FASTBOOT_FILES+=fastboot/protocol.c
FASTBOOT_FILES+=fastboot/engine.c
FASTBOOT_FILES+=fastboot/fastboot.c
FASTBOOT_FILES+=fastboot/usb_linux_fastboot.c
FASTBOOT_FILES+=fastboot/util_linux.c

FIREHOSE_FILES+=firehose/qfirehose.c
FIREHOSE_FILES+=firehose/sahara_protocol.c
FIREHOSE_FILES+=firehose/firehose_protocol.c
FIREHOSE_FILES+=firehose/usb_linux.c

QFLASH_FILES+=tinystr.cpp
QFLASH_FILES+=tinyxml.cpp
QFLASH_FILES+=tinyxmlerror.cpp
QFLASH_FILES+=tinyxmlparser.cpp
QFLASH_FILES+=md5.cpp
QFLASH_FILES+=at_tok.cpp
QFLASH_FILES+=atchannel.cpp
QFLASH_FILES+=ril-daemon.cpp
QFLASH_FILES+=download.cpp
QFLASH_FILES+=file.cpp
QFLASH_FILES+=os_linux.cpp
QFLASH_FILES+=serialif.cpp
QFLASH_FILES+=quectel_log.cpp
QFLASH_FILES+=quectel_common.cpp
QFLASH_FILES+=quectel_crc.cpp
QFLASH_FILES+=check_env.cpp

LIBS = -pthread -lrt

.PHONY: all clean install

all: $(TARGET)

$(TARGET):
	rm -rf *.o
	$(CC) -g -c -DPROGRESS_FILE_FAETURE -DUSE_FASTBOOT $(FASTBOOT_FILES)
	$(CC) -g -c -DPROGRESS_FILE_FAETURE -DFIREHOSE_ENABLE $(FIREHOSE_FILES)
	$(CXX) -g -c -DPROGRESS_FILE_FAETURE -DFIREHOSE_ENABLE $(QFLASH_FILES)
	$(CXX) *.o $(LIBS) -o $(TARGET)

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	install -D -m 755 $(TARGET) $(DESTDIR)/usr/bin/$(TARGET);

clean:
	rm -rf libs *.o $(TARGET) *~
