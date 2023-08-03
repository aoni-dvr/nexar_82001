TARGET_001 = main_001.exe
TARGET_002 = main_002.exe
TARGET_003 = main_003.exe
TARGET_004 = main_004.exe
TARGET_005 = main_005.exe
TARGET = $(TARGET_001) $(TARGET_002) $(TARGET_003) $(TARGET_004) $(TARGET_005)
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
#W_OBJS  += $(OBJDIR)/wrapper_csl_gic.o
#W_OBJS  += $(OBJDIR)/wrapper_generic.o
#W_OBJS  += $(OBJDIR)/wrapper_io_utility.o
#W_OBJS  += $(OBJDIR)/wrapper_kal.o
#W_OBJS  += $(OBJDIR)/wrapper_mmu.o
#W_OBJS  += $(OBJDIR)/wrapper_rtsl_gic.o
W_OBJS  += $(OBJDIR)/wrapper_crc32_hw.o
W_OBJS  += $(OBJDIR)/wrapper_fdt.o
W_OBJS  += $(OBJDIR)/wrapper_stdc.o

# util functions
#W_OBJS += $(OBJDIR)/wrapper_resource.o

# code to be tested
W_OBJS += $(OBJDIR)/AmbaIOUtility.o

# test code
W_OBJS += $(OBJDIR)/test_group_crc32.o
W_OBJS += $(OBJDIR)/test_group_fdt_handle_offset.o
W_OBJS += $(OBJDIR)/test_group_fdt_node_offset_by_cid.o
W_OBJS += $(OBJDIR)/test_group_fdt_property_u32.o
W_OBJS += $(OBJDIR)/test_group_fdt_property_u32_quick.o
W_OBJS += $(OBJDIR)/test_group_fdt_property_u64_quick.o
W_OBJS += $(OBJDIR)/test_group_ptr_to_u32_addr.o
W_OBJS += $(OBJDIR)/test_group_ptr_to_u64_addr.o
W_OBJS += $(OBJDIR)/test_group_register_mask_read32.o
W_OBJS += $(OBJDIR)/test_group_register_mask_write32.o
W_OBJS += $(OBJDIR)/test_group_register_read32.o
W_OBJS += $(OBJDIR)/test_group_register_write32.o
W_OBJS += $(OBJDIR)/test_group_string_append.o
W_OBJS += $(OBJDIR)/test_group_string_compare.o
W_OBJS += $(OBJDIR)/test_group_string_copy.o
W_OBJS += $(OBJDIR)/test_group_string_length.o
W_OBJS += $(OBJDIR)/test_group_string_print_str.o
W_OBJS += $(OBJDIR)/test_group_string_print_uint32.o
W_OBJS += $(OBJDIR)/test_group_string_print_uint64.o
#W_OBJS += $(OBJDIR)/test_group_string_to_u32.o
W_OBJS += $(OBJDIR)/test_group_u32_addr_to_ptr.o
W_OBJS += $(OBJDIR)/test_group_uint32_to_str.o
W_OBJS += $(OBJDIR)/test_group_uint64_to_str.o
W_OBJS += $(OBJDIR)/test_util.o

CC = gcc -g

VPATH = ./
VPATH += ./stub
VPATH += ./util
VPATH += $(ROOT_PATH)/soc/io/src/common/

AMBA_BUILD_DATE = \""2021201"\"
AMBA_BUILD_DATE_NUM = 20211201
AMBA_BUILD_SEC_NUM = 1
AMBA_BUILD_MACHINE = \""xx_machine"\"
AMBA_CI_COUNT = 0
AMBA_CI_ID = "NULL"

DFLAGS  = -DGCOVR_ENABLE -DCONFIG_THREADX -DCPU_CORTEX_A53 -DCONFIG_SOC_CV22 -DSIMULATION -DCONFIG_DEVICE_TREE_SUPPORT 
DFLAGS += -DCONFIG_DTB_LOADADDR=100
DFLAGS += -DCONFIG_ARM32
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

$(TARGET_003): ${W_OBJS} $(OBJDIR)/main_003.o
	@echo generating $@
	@$(CC) $(INC) $(CFLAGS) $(DFLAGS) -o $(TARGET_003) ${W_OBJS} $(OBJDIR)/main_003.o

$(TARGET_004): ${W_OBJS} $(OBJDIR)/main_004.o
	@echo generating $@
	@$(CC) $(INC) $(CFLAGS) $(DFLAGS) -o $(TARGET_004) ${W_OBJS} $(OBJDIR)/main_004.o

$(TARGET_005): ${W_OBJS} $(OBJDIR)/main_005.o
	@echo generating $@
	@$(CC) $(INC) $(CFLAGS) $(DFLAGS) -o $(TARGET_005) ${W_OBJS} $(OBJDIR)/main_005.o


$(OBJDIR)/%.o: %.c
	@echo compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)/*.o $(OBJDIR)/*.gcno $(OBJDIR)/*.gcda report/*.html report/*.css report/*.json $(TARGET)
