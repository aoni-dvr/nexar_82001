SRCPATH := ../../../../../..
CC = gcc
CFLAGS  = -fprofile-arcs -ftest-coverage -fPIC -DCONFIG_SOC_CV2FS -DCONFIG_THREADX

AMBA_CHIP_ID = cv2fs
CONFIG_BSP_NAME = cv2fsbub_option_a_v300

OUTPUT	= main_adc

CFLAGS  += -Istub
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/io/
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)/
CFLAGS  += -I $(SRCPATH)/vendors/ambarella/inc/threadx/
CFLAGS  += -I $(SRCPATH)/soc/io/src/$(AMBA_CHIP_ID)/inc/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/arm/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/csl/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/rtsl/
CFLAGS  += -I $(SRCPATH)/soc/io/src/common/inc/reg/
CFLAGS  += -I $(SRCPATH)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl/
CFLAGS  += -I $(SRCPATH)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg/

SRC 	= main.c \
		  ../stub/AmbaKAL_stub.c	\
		  ../stub/AmbaWrap_stub.c	\
		  ../stub/AmbaMisraFix_stub.c	\
		  ../stub/AmbaCache_stub.c	\
		  ../stub/AmbaDMA_stub.c	\
		  ../stub/AmbaRTSL_PLL_stub.c	\
		  ../stub/AmbaRTSL_GIC_stub.c	\
		  $(SRCPATH)/soc/io/src/common/io-mw/threadx/AmbaADC.c \
		  $(SRCPATH)/soc/io/src/common/rtsl/AmbaRTSL_ADC.c \
		  $(SRCPATH)/soc/io/src/common/csl/AmbaCSL_ADC.c

default: all

all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC) -lm

clean:
	$(RM) $(OUTPUT)
	$(RM) *.gcda *.gcno *.html *.o *.css *.log bbb
