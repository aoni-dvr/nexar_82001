SRCPATH := ../../../../../..
CC = gcc
CFLAGS  = -fprofile-arcs -ftest-coverage -fPIC -DCONFIG_SOC_CV2FS

AMBA_CHIP_ID = cv2fs
CONFIG_BSP_NAME = cv2fsbub_option_a_v300

OUTPUT	= main_int

CFLAGS  += -Istub
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/io/
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)/
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/threadx/
CFLAGS  += -I $(SRCPATH)/vendors/foss/xen/include/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/arm/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/csl/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/rtsl/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/reg/
CFLAGS  += -I $(SRCPATH)/soc/io/src/$(AMBA_CHIP_ID)/inc/
CFLAGS  += -I $(SRCPATH)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl/
CFLAGS  += -I $(SRCPATH)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl/
CFLAGS  += -I $(SRCPATH)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/

SRC 	= main.c \
		  ../stub/AmbaKAL_stub.c	\
		  ../stub/AmbaWrap_stub.c	\
		  ../stub/AmbaMisraFix_stub.c	\
		  ../stub/AmbaRTSL_PLL_stub.c	\
		  ../stub/AmbaRTSL_UART_stub.c	\
		  ../stub/AmbaIOUtility_stub.c	\
		  $(SRCPATH)/soc/io/src/common/AmbaINT.c \
		  $(SRCPATH)/soc/io/src/common/arm/AmbaRTSL_GIC.c \
		  $(SRCPATH)/soc/io/src/common/csl/AmbaCSL_GIC.c

default: all

all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC) -lm

clean:
	$(RM) $(OUTPUT)
	$(RM) *.gcda *.gcno *.html *.o *.css *.log bbb
