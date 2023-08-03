################################################################
#                             QNX                              #
################################################################
include $(objtree)/include/config/auto.conf

AUD_OS_PATH := qnx
export AUD_OS_PATH

AUD_MOD_ROOT := $(CURDIR)/../..
export AUD_MOD_ROOT

#$(warning dsp audio root $(AUD_MOD_ROOT))

EXTRA_SRCVPATH += $(AUD_MOD_ROOT)/
EXTRA_SRCVPATH += $(AUD_MOD_ROOT)/$(AMBA_CHIP_ID)

#$(warning dsp audio src path $(EXTRA_SRCVPATH))

SRCS := AmbaAudio_ADEC.c
SRCS += AmbaAudio_AENC.c
SRCS += AmbaAudio_AIN.c
SRCS += AmbaAudio_AOUT.c
#SRCS += AmbaVerSetDspAudio.c

EXTRA_INCVPATH += $(AUD_MOD_ROOT)/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(AUD_MOD_ROOT)/../../../../vendors/ambarella/inc/dsp/
EXTRA_INCVPATH += $(AUD_MOD_ROOT)/../../../../vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(AUD_MOD_ROOT)/../../../../vendors/ambarella/inc/io/
EXTRA_INCVPATH += $(AUD_MOD_ROOT)/../../../../vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(AUD_MOD_ROOT)/../../../../svc/comsvc/print/
EXTRA_INCVPATH += $(AUD_MOD_ROOT)/../../../../svc/comsvc/misc/

LIST=CPU
include recurse.mk

