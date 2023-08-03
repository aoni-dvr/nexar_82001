SRCPATH := ../../../../../../

OUTPUT_AmbaIPC_RPMSG	= main_AmbaIPC_RPMSG
OUTPUT_AmbaIPC_Vq		= main_AmbaIPC_Vq
OUTPUT_AmbaSafety_IPC	= main_AmbaSafety_IPC

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
CFLAGS += -I $(SRCPATH)/svc/comsvc/misc/
CFLAGS += -I $(SRCPATH)/vendors/ambarella/inc/
CFLAGS += -I $(SRCPATH)/vendors/ambarella/inc/$(OS)/
CFLAGS += -I $(SRCPATH)/vendors/ambarella/inc/io/
CFLAGS += -I $(SRCPATH)/vendors/ambarella/inc/io/$(CHIP)/

SRC_common			= ../stub/AmbaMisraFix_stub.c ../stub/AmbaKAL_stub.c ../stub/AmbaWrap_stub.c \
					  ../stub/Ipc_stub.c

SRC_AmbaIPC_RPMSG	= main_AmbaIPC_RPMSG.c \
					  $(SRC_common) ../stub/AmbaPrint_stub.c ../stub/AmbaINT_stub.c
SRC_AmbaIPC_Vq		= main_AmbaIPC_Vq.c \
					  $(SRC_common)
SRC_AmbaSafety_IPC	= main_AmbaSafety_IPC.c \
					  $(SRC_common) ../stub/AmbaPrint_stub.c

CC = gcc -g
DFLAGS = -DGCOVR_ENABLE -DCONFIG_THREADX -DCPU_CORTEX_A53 -DCONFIG_SOC_CV22
DFLAGS_Safety_IPC = -DCONFIG_AMBALINK_G2_RPMSG_NUM=2048

default: all

# all: $(OUTPUT_AmbaIPC_RPMSG) $(OUTPUT_AmbaIPC_Vq) $(OUTPUT_AmbaSafety_IPC)
all: $(OUTPUT_AmbaIPC_RPMSG)
# all: $(OUTPUT_AmbaSafety_IPC)

$(OUTPUT_AmbaIPC_RPMSG): $(SRC_AmbaIPC_RPMSG)
	$(CC) $(CFLAGS) $(DFLAGS) -o $(OUTPUT_AmbaIPC_RPMSG) $(SRC_AmbaIPC_RPMSG) -lm

$(OUTPUT_AmbaIPC_Vq): $(SRC_AmbaIPC_Vq)
	$(CC) $(CFLAGS) $(DFLAGS) -o $(OUTPUT_AmbaIPC_Vq) $(SRC_AmbaIPC_Vq) -lm

$(OUTPUT_AmbaSafety_IPC): $(SRC_AmbaSafety_IPC)
	$(CC) $(CFLAGS) $(DFLAGS) $(DFLAGS_Safety_IPC) -o $(OUTPUT_AmbaSafety_IPC) $(SRC_AmbaSafety_IPC) -lm

clean:
	rm -f *.o *.gcno *.gcda *.html *.txt *.css *.log $(OUTPUT_AmbaIPC_RPMSG) $(OUTPUT_AmbaIPC_Vq) $(OUTPUT_AmbaSafety_IPC)
