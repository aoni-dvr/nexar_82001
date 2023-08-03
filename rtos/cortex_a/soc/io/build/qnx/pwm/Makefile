EXTRA_SRCVPATH := $(CURDIR)/../../../src/common/pwm/qnx/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/rtsl/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/csl/
				  
# src
SRCS := main.c
ifneq ($(AMBA_CHIP_ID), cv2fs)
SRCS += AmbaCSL_Stepper.c
SRCS += AmbaRTSL_Stepper.c
endif
SRCS += AmbaRTSL_PWM.c
SRCS += lib.c
SRCS += Amba_pwm.c

LIST=CPU
include recurse.mk

