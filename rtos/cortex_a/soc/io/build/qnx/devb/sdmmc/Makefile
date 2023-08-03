EXTRA_SRCVPATH := $(CURDIR)/../../../../src/common/devb/qnx/sdmmc/
EXTRA_SRCVPATH += $(CURDIR)/../../../../src/common/devb/qnx/sdmmc/ambarella/
EXTRA_SRCVPATH += $(CURDIR)/../../../../src/common/devb/qnx/sdmmc/sdiodi/
EXTRA_SRCVPATH += $(CURDIR)/../../../../src/common/devb/qnx/sdmmc/sdiodi/hc/

EXTRA_SRCVPATH += $(CURDIR)/../../../../src/common/devb/qnx/include/
EXTRA_SRCVPATH += $(CURDIR)/../../../../src/common/devb/qnx/sdmmc/include/
EXTRA_SRCVPATH += $(CURDIR)/../../../../src/common/devb/qnx/sdmmc/sdiodi/include/

# src
SRCS := sim_sdmmc.c
SRCS += sim_assd.c

#ambarella
SRCS += sim_bs.c
SRCS += bs.c

#sdiodi
SRCS += soc.c
SRCS += sd.c
SRCS += pci.c
SRCS += mmc.c
SRCS += card.c
SRCS += base.c

#sdiodi/hc
SRCS += sdhci.c

LIST=CPU
include recurse.mk
