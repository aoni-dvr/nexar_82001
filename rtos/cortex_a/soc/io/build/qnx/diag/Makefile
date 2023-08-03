include $(objtree)/include/config/auto.conf

EXTRA_SRCVPATH := $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/diag/qnx/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/diag/qnx/

# src
ifeq ($(CONFIG_SOC_CV2),y)
SRCS := main.c
SRCS += DiagAdc.c
SRCS += DiagGpio.c
SRCS += DiagHdmi.c
SRCS += DiagI2c.c
SRCS += DiagOTP.c
SRCS += DiagRng.c
SRCS += DiagRtc.c
SRCS += DiagSPI.c
SRCS += DiagTimer.c
SRCS += DiagMPrint.c
SRCS += DiagPwm.c
SRCS += DiagDma.c
SRCS += DiagDram.c
SRCS += DiagGdma.c
SRCS += DiagCvbs.c
SRCS += DiagClk.c
SRCS += DiagWdt.c
SRCS += DiagCrc.c
SRCS += DiagCan.c
else ifeq ($(CONFIG_SOC_CV2FS),y)
SRCS := main.c
SRCS += DiagGpio.c
SRCS += DiagOTP.c
SRCS += DiagRng.c
SRCS += DiagRtc.c
SRCS += DiagSPI.c
SRCS += DiagMPrint.c
SRCS += DiagDma.c
SRCS += DiagGdma.c
SRCS += DiagWdt.c
SRCS += DiagCrc.c
SRCS += DiagClk.c
SRCS += DiagNvm.c
SRCS += DiagPwm.c
SRCS += DiagCan.c
SRCS += DiagTimer.c
SRCS += DiagI2c.c
else
$(error chip not supported for diag!)
endif

LIST=CPU
include recurse.mk
