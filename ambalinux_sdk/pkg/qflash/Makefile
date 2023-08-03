
export AMBALINK_CV25_DIR=${PWD}/../../output.oem/ambalink

CROSS_COMPILE=${AMBALINK_CV25_DIR}/host/bin/aarch64-none-linux-gnu-

#CFLAGS+=-Wall
#CFLAGS+=-O2
#CFLAGS+=-Wmissing-declarations
#CFLAGS+=-fno-strict-aliasing
#CFLAGS+=-Wno-deprecated-declarations
#CFLAGS+=-Wint-to-pointer-cast
#CFLAGS+=-Wfloat-equal
#CFLAGS+=-Wno-unused-parameter
#CFLAGS+=-Wno-sign-compare
#CFLAGS+=-Wunused-but-set-variable
#CFLAGS+=-Wundef
#CFLAGS+=-Wpointer-arith
#CFLAGS+=-Winit-self
#CFLAGS+=-Wshadow
#CFLAGS+=-Wmissing-include-dirs
#CFLAGS+=-Waggregate-return
#CFLAGS+=-Wformat-security
#CFLAGS+=-Wtype-limits
## CFLAGS+=-Werror
#CFLAGS+=-Wunreachable-code
#CFLAGS+=-pipe
#CFLAGS+=-fstack-check
#CFLAGS+=-Wredundant-decls
#CFLAGS+=-fstack-protector-all

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

LIBS+=-lrt
LIBS+=-lpthread

TARGET=QFlash

all:
	rm -rf *.o
	$(CROSS_COMPILE)gcc -g -c -DPROGRESS_FILE_FAETURE -DUSE_FASTBOOT $(FASTBOOT_FILES)
	$(CROSS_COMPILE)gcc -g -c -DPROGRESS_FILE_FAETURE -DFIREHOSE_ENABLE $(FIREHOSE_FILES)
	$(CROSS_COMPILE)g++ -g -c -DPROGRESS_FILE_FAETURE -DFIREHOSE_ENABLE $(QFLASH_FILES)
	$(CROSS_COMPILE)g++ *.o $(LIBS) -o $(TARGET)
debug:
	rm -rf *.o
	$(CROSS_COMPILE)gcc -DDEBUG -g -c -DPROGRESS_FILE_FAETURE -DUSE_FASTBOOT $(FASTBOOT_FILES)
	$(CROSS_COMPILE)gcc -DDEBUG -g -c -DPROGRESS_FILE_FAETURE -DFIREHOSE_ENABLE $(FIREHOSE_FILES)
	$(CROSS_COMPILE)g++ -DDEBUG -g -c -DPROGRESS_FILE_FAETURE -DFIREHOSE_ENABLE $(QFLASH_FILES)
	$(CROSS_COMPILE)g++ *.o $(LIBS) -o $(TARGET)
clean:
	rm -rf $(NDK_OUT) libs *.o QFlash QFlash *~
	cd firehose && make clean && cd ..
	cd fastboot && make clean && cd ..
	
