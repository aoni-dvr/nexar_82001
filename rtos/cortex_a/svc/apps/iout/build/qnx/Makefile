################################################################
#                             QNX                              #
################################################################
include $(CURDIR)/../../../../../output/include/config/auto.conf

IOUT_OS_PATH:=qnx
export IOUT_OS_PATH

IOUT_MOD_ROOT := $(CURDIR)/../..
export IOUT_MOD_ROOT

EXTRA_SRCVPATH := $(IOUT_MOD_ROOT)/src/qnx/

SRCS := main.o

ifneq ($(findstring $(CONFIG_SOC_CV2)$(CONFIG_SOC_CV2FS), y),)
# EXTRA_INCVPATH - a spec-separated list of directories to search for include files.
EXTRA_INCVPATH += $(IOUT_MOD_ROOT)/inc/
EXTRA_INCVPATH += $(IOUT_MOD_ROOT)/inc/$(AMBA_CHIP_ID)/
EXTRA_INCVPATH += $(IOUT_MOD_ROOT)/src/peripherals/$(AMBA_CHIP_ID)/bridge/inc/
EXTRA_INCVPATH += $(IOUT_MOD_ROOT)/src/peripherals/$(AMBA_CHIP_ID)/sensor/inc/
EXTRA_INCVPATH += $(IOUT_MOD_ROOT)/src/peripherals/$(AMBA_CHIP_ID)/yuvinput/inc/
EXTRA_INCVPATH += $(IOUT_MOD_ROOT)/../../../vendors/ambarella/inc/perif/
EXTRA_INCVPATH += $(IOUT_MOD_ROOT)/../../../bsp/$(AMBA_CHIP_ID)/peripherals/bridge/inc/
EXTRA_INCVPATH += $(IOUT_MOD_ROOT)/../../../bsp/$(AMBA_CHIP_ID)/peripherals/sensor/inc/
EXTRA_INCVPATH += $(IOUT_MOD_ROOT)/../../../bsp/$(AMBA_CHIP_ID)/peripherals/yuvinput/inc/
EXTRA_INCVPATH += $(IOUT_MOD_ROOT)/../../../bsp/$(AMBA_CHIP_ID)/$(AMBA_BSP_NAME)/

# EXTRA_SRCVPATH - a spec-separated list of directories to search for source files.
EXTRA_SRCVPATH += $(IOUT_MOD_ROOT)/src/peripherals/$(AMBA_CHIP_ID)/bridge/
EXTRA_SRCVPATH += $(IOUT_MOD_ROOT)/src/peripherals/$(AMBA_CHIP_ID)/sensor/
EXTRA_SRCVPATH += $(IOUT_MOD_ROOT)/src/peripherals/$(AMBA_CHIP_ID)/yuvinput/
EXTRA_SRCVPATH += $(IOUT_MOD_ROOT)/src/diag/$(AMBA_CHIP_ID)/
EXTRA_SRCVPATH += $(IOUT_MOD_ROOT)/src/diag/common/

SRCS += AmbaIOUTDiag_CmdDiag.c

SRCS += AmbaIOUTDiag_CmdI2C.c
SRCS += AmbaIOUTDiag_CmdGPIO.c
SRCS += AmbaIOUTDiag_CmdVIN.c

ifeq ($(CONFIG_SOC_CV2),y)
SRCS += AmbaSensor_IMX317.c
SRCS += AmbaSensor_IMX317Table.c
SRCS += AmbaSensor_AR0238_PARALLEL.c
SRCS += AmbaSensor_AR0238_PARALLELTable.c
endif

ifeq ($(CONFIG_SOC_CV2FS),y)
SRCS += AmbaSensor_IMX317.c
SRCS += AmbaSensor_IMX317Table.c
SRCS += AmbaSensor_AR0238_PARALLEL.c
SRCS += AmbaSensor_AR0238_PARALLELTable.c
SRCS += AmbaSensor_IMX334.c
SRCS += AmbaSensor_IMX334Table.c
SRCS += AmbaSensor_MAX9295_96712_IMX390_2VC.c
SRCS += AmbaSensor_MAX9295_96712_IMX390_VC_DR.c
SRCS += AmbaSbrg_Max9295_96712_IOUT.c
SRCS += AmbaYuv_MAX9295_96712_AmbaVout.c
SRCS += AmbaYuv_MAX9295_96712_AmbaVoutTable.c
endif
endif

LIST=CPU
include recurse.mk
