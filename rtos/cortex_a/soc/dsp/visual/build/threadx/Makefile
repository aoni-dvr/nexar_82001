ifeq ($(CONFIG_SOC_CV2FS),y)
VIS_FAMILY_PATH:=fs
else ifeq ($(CONFIG_SOC_CV5),y)
VIS_FAMILY_PATH:=cv5x
else ifeq ($(CONFIG_SOC_CV52),y)
VIS_FAMILY_PATH:=cv5x
else
VIS_FAMILY_PATH:=auto
endif
export VIS_FAMILY_PATH

VIS_OS_NAME := threadx
export VIS_OS_NAME

obj-y += LL/
obj-y += HL/

ifeq ($(CONFIG_SOC_CV2FS),y)
ifeq ($(CONFIG_ENABLE_DSP_MONITOR),y)
    obj-y += monitor/
endif
endif

ifeq ($(CONFIG_ENABLE_DSP_DIAG),y)
    obj-y += diag/
endif