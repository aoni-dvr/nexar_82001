ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else ifeq ($(CONFIG_SOC_CV52),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif

obj-y += libdsp_visualLL.a

DIR := ../../../src/LL

ccflags-y := \
             -I$(srctree)/vendors/ambarella/inc \
             -I$(srctree)/vendors/ambarella/inc/io \
             -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
             -I$(srctree)/vendors/ambarella/inc/dsp \
             -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP) \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
             -I$(srctree)/soc/dsp/visual/inc/LL/$(VIS_FAMILY_PATH) \
             -I$(srctree)/soc/dsp/visual/inc/LL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP) \
             -I$(srctree)/soc/dsp/visual/inc/HL/$(VIS_FAMILY_PATH) \
             -I$(srctree)/soc/dsp/visual/inc/HL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP) \
             -I$(srctree)/soc/dsp/visual/inc/common \
             -I$(srctree)/soc/dsp/visual/inc/common/$(VIS_OS_NAME) \
             -I$(srctree)/svc/comsvc/print \
             -I$(srctree)/svc/comsvc/misc

libdsp_visualLL-objs += \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_Log.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_ArmComm.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_EventCtrl.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_CommonAPI.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_CmdsSend.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_CmdMsgDump.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_MsgDispatcher.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_Buffers.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/AmbaDSP_Int.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/AmbaDSP_CmdMsgDump_Priv.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/AmbaDSP_ArmComm_Priv.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/AmbaDSP_Buffers_Priv.o \
                        $(DIR)/../common/$(VIS_OS_NAME)/dsp_osal.o
ifeq ($(or $(CONFIG_SOC_CV5),$(CONFIG_SOC_CV52)),y)
libdsp_visualLL-objs += $(DIR)/../common/$(VIS_OS_NAME)/dsp_aaa_stat.o
else ifeq ($(CONFIG_SOC_CV52),y)
libdsp_visualLL-objs += $(DIR)/../common/$(VIS_OS_NAME)/dsp_aaa_stat.o
endif

ifeq ($(VIS_FAMILY_PATH),fs)
libdsp_visualLL-objs += $(DIR)/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/ucode_error.o
else ifeq ($(VIS_FAMILY_PATH),cv5x)
libdsp_visualLL-objs += $(DIR)/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/ucode_error.o
endif

libdsp_visualLL-objs += $(DIR)/$(VIS_FAMILY_PATH)/AmbaVerSetDspVisualLL.o
$(obj)/AmbaVerSetDspVisualLL.o : $(call amba_ver_dep, AmbaVerSetDspVisualLL.o, $(libdsp_visual-objs))
CFLAGS_AmbaVerSetDspVisualLL.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)            \
                               -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)    \
                               -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)      \
                               -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)      \
                               -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)                \
                               -DAMBA_CI_ID=$(AMBA_CI_ID)


install-files := libdsp_visualLL.a

