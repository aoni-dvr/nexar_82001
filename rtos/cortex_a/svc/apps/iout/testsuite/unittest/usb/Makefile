TARGET_001 = main_001.exe
TARGET_002 = main_002.exe
TARGET = $(TARGET_001) $(TARGET_002)
OBJDIR = obj

all: $(TARGET)

ROOT_PATH_REL = ../../../../../../
ROOT_PATH = $(realpath $(ROOT_PATH_REL))

OS = threadx
CHIP = cv22
INCLUDE_SOC  = -I ${ROOT_PATH}/soc/io/src/common/inc/
INCLUDE_SOC += -I ${ROOT_PATH}/soc/io/src/common/inc/arm/
INCLUDE_SOC += -I ${ROOT_PATH}/soc/io/src/common/inc/rtsl/
INCLUDE_SOC += -I ${ROOT_PATH}/soc/io/src/common/inc/csl/
INCLUDE_SOC += -I ${ROOT_PATH}/soc/io/src/common/inc/reg/
INCLUDE_SOC += -I ${ROOT_PATH}/soc/io/src/$(CHIP)/inc/
INCLUDE_SOC += -I ${ROOT_PATH}/soc/io/src/$(CHIP)/inc/reg/
INCLUDE_SOC += -I ${ROOT_PATH}/soc/io/src/$(CHIP)/inc/csl/
INCLUDE_SOC += -I ${ROOT_PATH}/soc/io/src/$(CHIP)/inc/rtsl/

INC  = -I ${ROOT_PATH}/vendors/ambarella/inc/
INC += -I ${ROOT_PATH}/vendors/ambarella/inc/io/
INC += -I ${ROOT_PATH}/vendors/ambarella/inc/io/$(CHIP)/
INC += -I ${ROOT_PATH}/vendors/ambarella/inc/$(OS)/
INC += -I ./stub/inc
INC += -I ./util/inc
INC += -I ./

#W_OBJS   = ""

# stub functions
W_OBJS  += $(OBJDIR)/wrapper_csl_gic.o
W_OBJS  += $(OBJDIR)/wrapper_generic.o
W_OBJS  += $(OBJDIR)/wrapper_io_utility.o
W_OBJS  += $(OBJDIR)/wrapper_kal.o
W_OBJS  += $(OBJDIR)/wrapper_mmu.o
W_OBJS  += $(OBJDIR)/wrapper_rtsl_gic.o
W_OBJS  += $(OBJDIR)/wrapper_stdc.o

# util functions
W_OBJS += $(OBJDIR)/wrapper_resource.o

# code to be tested
W_OBJS += $(OBJDIR)/AmbaRTSL_USB.o
W_OBJS += $(OBJDIR)/AmbaCSL_USB.o
W_OBJS += $(OBJDIR)/AmbaCSL_USB_PHY.o

# usbx code

# test code
W_OBJS += $(OBJDIR)/test_group_csl_usb_read_write.o
W_OBJS += $(OBJDIR)/test_group_rtsl_rct_port_state.o
W_OBJS += $(OBJDIR)/test_group_rtsl_usb_isr.o
W_OBJS += $(OBJDIR)/test_group_rtsl_usb_mutex.o
W_OBJS += $(OBJDIR)/test_group_rtsl_usb_read_address.o
W_OBJS += $(OBJDIR)/test_group_rtsl_usb_set_hw_info.o

CC = gcc -g

VPATH = ./
VPATH += ./stub
VPATH += ./util
VPATH += $(ROOT_PATH)/soc/io/src/common/rtsl
VPATH += $(ROOT_PATH)/soc/io/src/common/csl

AMBA_BUILD_DATE = \""2021201"\"
AMBA_BUILD_DATE_NUM = 20211201
AMBA_BUILD_SEC_NUM = 1
AMBA_BUILD_MACHINE = \""xx_machine"\"
AMBA_CI_COUNT = 0
AMBA_CI_ID = "NULL"

DFLAGS  = -DGCOVR_ENABLE -DCONFIG_THREADX -DCPU_CORTEX_A53 -DCONFIG_SOC_CV22 -DSIMULATION -DCONFIG_DEVICE_TREE_SUPPORT 
DFLAGS += -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)         \
        -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
        -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)   \
        -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)   \
        -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)             \
        -DAMBA_CI_ID=$(AMBA_CI_ID)
CFLAGS = -m32 -fprofile-arcs -ftest-coverage -fPIC -O0 $(INC) $(INCLUDE_SRC) $(INCLUDE_SOC) $(INCLUDE_USB) $(INCLUDE_FS) $(DFLAGS)

$(TARGET_001): ${W_OBJS} $(OBJDIR)/main_001.o
	@echo generating $@
	@$(CC) $(INC) $(CFLAGS) $(DFLAGS) -o $(TARGET_001) ${W_OBJS} $(OBJDIR)/main_001.o

$(TARGET_002): ${W_OBJS} $(OBJDIR)/main_002.o
	@echo generating $@
	@$(CC) $(INC) $(CFLAGS) $(DFLAGS) -o $(TARGET_002) ${W_OBJS} $(OBJDIR)/main_002.o


$(OBJDIR)/%.o: %.c
	@echo compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)/*.o $(OBJDIR)/*.gcno $(OBJDIR)/*.gcda report/*.html report/*.css report/*.json $(TARGET)
