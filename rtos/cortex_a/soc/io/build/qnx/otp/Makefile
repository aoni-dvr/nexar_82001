include $(objtree)/include/config/auto.conf

EXTRA_SRCVPATH := $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/otp/qnx/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/rtsl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/csl
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/csl

ifneq ($(or $(CONFIG_SOC_CV2),$(CONFIG_SOC_CV22),$(CONFIG_SOC_CV25),$(CONFIG_SOC_CV28),$(CONFIG_SOC_H32)),)
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/rtsl/cv2x
endif

# src
SRCS := main.c
SRCS += lib.c
SRCS += lib.c
SRCS += AmbaRTSL_OTP.c
SRCS += AmbaCSL_OTP.c

LIST=CPU
include recurse.mk

