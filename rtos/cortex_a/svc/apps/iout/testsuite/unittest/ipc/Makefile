# all: AmbaMutex AmbaIPC
# AmbaMutex: AmbaMutex
# AmbaIPC: AmbaIPC

SRCPATH := ../../../../../../

OUTPUT_AmbaIPC			= main_AmbaIPC
OUTPUT_AmbaIPC_Mutex	= main_AmbaIPC_Mutex
OUTPUT_AmbaIPC_SpinLock	= main_AmbaIPC_SpinLock

OS = threadx
CHIP = cv22
CFLAGS = -fprofile-arcs -ftest-coverage -fPIC

CFLAGS += -I $(SRCPATH)/soc/io/src/common/inc/
CFLAGS += -I $(SRCPATH)/soc/io/src/common/inc/arm/
CFLAGS += -I $(SRCPATH)/soc/io/src/common/inc/rtsl/
CFLAGS += -I $(SRCPATH)/soc/io/src/common/inc/csl/
CFLAGS += -I $(SRCPATH)/soc/io/src/common/inc/reg/
CFLAGS += -I $(SRCPATH)/soc/io/src/$(CHIP)/inc/
CFLAGS += -I $(SRCPATH)/svc/comsvc/ambalink/inc/
CFLAGS += -I $(SRCPATH)/svc/comsvc/ambalink/core/inc/
CFLAGS += -I $(SRCPATH)/svc/comsvc/ambalink/inc/rpcprog/
CFLAGS += -I $(SRCPATH)/svc/comsvc/ambalink/core/
CFLAGS += -I $(SRCPATH)/svc/comsvc/print/
CFLAGS += -I $(SRCPATH)/svc/comsvc/shell/inc/
CFLAGS += -I $(SRCPATH)/vendors/ambarella/inc/
CFLAGS += -I $(SRCPATH)/vendors/ambarella/inc/$(OS)/
CFLAGS += -I $(SRCPATH)/vendors/ambarella/inc/io/
CFLAGS += -I $(SRCPATH)/vendors/ambarella/inc/io/$(CHIP)/

SRC_AmbaIPC				= main_AmbaIPC.c \
						  ../stub/AmbaMisraFix_stub.c
SRC_AmbaIPC_Mutex		= main_AmbaIPC_Mutex.c \
						  ../stub/AmbaMisraFix_stub.c ../stub/AmbaWrap_stub.c ../stub/AmbaPrint_stub.c \
						  ../stub/AmbaKAL_stub.c ../stub/Ipc_stub.c
SRC_AmbaIPC_SpinLock	= main_AmbaIPC_SpinLock.c \
						  ../stub/AmbaMisraFix_stub.c ../stub/AmbaWrap_stub.c ../stub/AmbaPrint_stub.c \
						  ../stub/Ipc_stub.c

CC = gcc -g
DFLAGS = -DGCOVR_ENABLE -DCONFIG_THREADX -DCPU_CORTEX_A53 -DCONFIG_SOC_CV22
DFLAGS_SpinLock = -DAMBA_CORTEX_A53_SCRATCHPAD_NS_BASE_ADDR -DAMBA_CORTEX_A53_AXI_CONFIG_PHYS_BASE_ADDR -DCONFIG_ARM32

default: all

all: $(OUTPUT_AmbaIPC) $(OUTPUT_AmbaIPC_Mutex) $(OUTPUT_AmbaIPC_SpinLock)

$(OUTPUT_AmbaIPC): $(SRC_AmbaIPC)
	$(CC) $(CFLAGS) $(DFLAGS) -o $(OUTPUT_AmbaIPC) $(SRC_AmbaIPC)

$(OUTPUT_AmbaIPC_Mutex): $(SRC_AmbaIPC_Mutex)
	$(CC) $(CFLAGS) $(DFLAGS) -o $(OUTPUT_AmbaIPC_Mutex) $(SRC_AmbaIPC_Mutex) -lm

$(OUTPUT_AmbaIPC_SpinLock): $(SRC_AmbaIPC_SpinLock)
	$(CC) $(CFLAGS) $(DFLAGS) $(DFLAGS_SpinLock) -o $(OUTPUT_AmbaIPC_SpinLock) $(SRC_AmbaIPC_SpinLock) -lm

clean:
	rm -f *.o *.gcno *.gcda *.html *.txt *.css *.log $(OUTPUT_AmbaIPC) $(OUTPUT_AmbaIPC_Mutex) $(OUTPUT_AmbaIPC_SpinLock)
