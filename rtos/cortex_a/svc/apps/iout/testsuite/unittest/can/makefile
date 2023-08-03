# CAN gcovr makefile
SRCPATH := ../../../../../../
SRCDIR += $(SRCPATH)/soc/io/src/common/
CC = gcc
OUTPUT	= main_can

AMBA_CHIP_ID := cv2fs
#AMBA_CHIP_ID := cv5x

LDFLAGS += -m32 -lm
LDFLAGS += -DGCOVR_CAN_ENABLE
LDFLAGS += -DCONFIG_SOC_CV2FS
#LDFLAGS += -DCONFIG_SOC_CV5 -D__aarch64__


CFLAGS  = -fprofile-arcs -ftest-coverage -fPIC 
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/ 
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/io/
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)/
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/threadx/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/rtsl/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/csl/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/reg/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/arm/
CFLAGS  += -I $(SRCPATH)/soc/io/src/$(AMBA_CHIP_ID)/inc/
CFLAGS  += -I $(SRCPATH)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl/

SRC     += main.c
SRC     += can_ctest_stub.c
SRC     += can_ctest_enable.c
SRC     += can_ctest_read.c
SRC     += can_ctest_write.c
SRC     += can_ctest_writefd.c
SRC     += can_ctest_txcallback.c
SRC     += can_ctest_getinfo.c
SRC     += can_ctest_disable.c
SRC     += can_ctest_isr.c
#cv2fs only
SRC    += can_ctest_getsafestate.c
#cv2fs only
SRC    += can_ctest_setsafestate.c
SRC 	+= $(SRCPATH)/soc/io/src/common/io-mw/threadx/AmbaCAN.c
SRC 	+= $(SRCPATH)/soc/io/src/common/rtsl/AmbaRTSL_CAN.c
SRC 	+= $(SRCPATH)/soc/io/src/common/csl/AmbaCSL_CAN.c


#default: all
.PHONY: clean

all: $(OUTPUT) run coverage

$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC) $(LDFLAGS)

run:
	echo "Running CAN..."
	./main_can

coverage:
	@echo "Running gcovr..."	
	gcovr $(SRCPATH) -r $(SRCDIR) --html --html-details -o can-html-details.html;	
	echo "See coverage.html in can-html-details.html"

clean:
	$(RM) $(OUTPUT)
	$(RM) *.gcda *.gcno *.html *.o *.txt *.css *.log bbb
	

