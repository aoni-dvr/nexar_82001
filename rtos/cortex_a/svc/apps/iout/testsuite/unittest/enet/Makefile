# all: AmbaENET
# AmbaENET: AmbaENET

SRCPATH := ../../../../../../

OUTPUT_AmbaENET			= main_AmbaENET
OUTPUT_AmbaRTSL_ENET	= main_AmbaRTSL_ENET
OUTPUT_AmbaCSL_ENET		= main_AmbaCSL_ENET

CHIP = cv22
CFLAGS = -fprofile-arcs -ftest-coverage -fPIC
CFLAGS += -I $(SRCPATH)/soc/io/src/common/inc/
CFLAGS += -I $(SRCPATH)/soc/io/src/common/inc/arm/
CFLAGS += -I $(SRCPATH)/soc/io/src/common/inc/rtsl/
CFLAGS += -I $(SRCPATH)/soc/io/src/common/inc/csl/
CFLAGS += -I $(SRCPATH)/soc/io/src/common/inc/reg/
CFLAGS += -I $(SRCPATH)/svc/comsvc/ambalink/inc/
CFLAGS += -I $(SRCPATH)/svc/comsvc/print/
CFLAGS += -I $(SRCPATH)/svc/comsvc/ambalink/core/inc/
CFLAGS += -I $(SRCPATH)/svc/comsvc/shell/inc/
CFLAGS += -I $(SRCPATH)/svc/comsvc/ambalink/inc/rpcprog/
CFLAGS += -I $(SRCPATH)/svc/comsvc/ambalink/core/
CFLAGS += -I $(SRCPATH)/vendors/ambarella/inc/io/$(CHIP)/
CFLAGS += -I $(SRCPATH)/vendors/ambarella/inc/
CFLAGS += -I $(SRCPATH)/vendors/ambarella/inc/io/
CFLAGS += -I $(SRCPATH)/soc/io/src/$(CHIP)/inc/

SRC_AmbaENET		= main_AmbaENET.c \
					  ../stub/AmbaEnet_stub.c ../stub/AmbaCache_stub.c ../stub/AmbaKAL_stub.c \
					  ../stub/AmbaWrap_stub.c ../stub/AmbaMisraFix_stub.c ../stub/AmbaRTSL_Enet_stub.c \
					  ../stub/AmbaCSL_Enet_stub.c ../stub/AmbaPrint_stub.c
SRC_AmbaRTSL_ENET	= main_AmbaRTSL_ENET.c \
					  ../stub/AmbaEnet_stub.c ../stub/AmbaCSL_Enet_stub.c ../stub/AmbaINT_stub.c
SRC_AmbaCSL_ENET	= main_AmbaCSL_ENET.c \
					  ../stub/AmbaEnet_stub.c ../stub/AmbaMisraFix_stub.c

CC = gcc -g
DFLAGS = -DGCOVR_ENABLE -DCONFIG_THREADX -DCPU_CORTEX_A53 -DCONFIG_SOC_CV22

default: all

all: $(OUTPUT_AmbaENET) $(OUTPUT_AmbaRTSL_ENET) $(OUTPUT_AmbaCSL_ENET)

$(OUTPUT_AmbaENET): $(SRC_AmbaENET)
	$(CC) $(CFLAGS) $(DFLAGS) -o $(OUTPUT_AmbaENET) $(SRC_AmbaENET) -lm

$(OUTPUT_AmbaRTSL_ENET): $(SRC_AmbaRTSL_ENET)
	$(CC) $(CFLAGS) $(DFLAGS) -o $(OUTPUT_AmbaRTSL_ENET) $(SRC_AmbaRTSL_ENET)

$(OUTPUT_AmbaCSL_ENET): $(SRC_AmbaCSL_ENET)
	$(CC) $(CFLAGS) $(DFLAGS) -o $(OUTPUT_AmbaCSL_ENET) $(SRC_AmbaCSL_ENET)

clean:
	rm -f *.o *.gcno *.gcda *.html *.txt *.css *.log $(OUTPUT_AmbaENET) $(OUTPUT_AmbaRTSL_ENET) $(OUTPUT_AmbaCSL_ENET)
