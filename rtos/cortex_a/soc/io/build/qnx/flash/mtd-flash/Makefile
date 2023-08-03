#EXTRA_SRCVPATH += $(CURDIR)/../../../../src/$(AMBA_CHIP_ID)/spinor/qnx/
EXTRA_SRCVPATH += $(CURDIR)/../../../../src/common/spinor/qnx/flash/mtd-flash/
EXTRA_SRCVPATH += $(CURDIR)/../../../../src/common/spinor/qnx/flash/mtd-flash/rom/
EXTRA_SRCVPATH += $(CURDIR)/../../../../src/common/spinor/qnx/flash/mtd-flash/sram/

#$(warning $(EXTRA_SRCVPATH))

# src
SRCS := flash_list.o
SRCS += flash_probe.o
SRCS += mem_access.o
SRCS += pad_value.o
SRCS += socket_option.o
SRCS += socket_syspage.o
SRCS += usage.o

#src rom
SRCS += rom_erase.c
SRCS += rom_ident.c
SRCS += rom_sync.c
SRCS += rom_write.c

#src sram
SRCS += sram_erase.c
SRCS += sram_ident.c
SRCS += sram_reset.c
SRCS += sram_sync.c
SRCS += sram_v2erase.c
SRCS += sram_v2islock.c
SRCS += sram_v2lock.c
SRCS += sram_v2sync.c
SRCS += sram_v2unlock.c
SRCS += sram_v2unlockall.c
SRCS += sram_v2write.c
SRCS += sram_write.c


LIST=CPU
include recurse.mk
