ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else ifeq ($(CONFIG_SOC_CV52),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif

ifeq ($(VIS_FAMILY_PATH),fs)
obj-y += similar/
endif

obj-y += libdsp_visualHL.a

DIR := ../../../src/HL

ccflags-y := \
             -I$(srctree)/vendors/ambarella/inc \
             -I$(srctree)/vendors/ambarella/inc/io \
             -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
             -I$(srctree)/vendors/ambarella/inc/dsp \
             -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP) \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
             -I$(srctree)/soc/io/src/common/inc/csl \
             -I$(srctree)/soc/io/src/common/inc/rtsl \
             -I$(srctree)/soc/io/src/common/inc/reg \
             -I$(srctree)/soc/dsp/visual/inc/LL/$(VIS_FAMILY_PATH) \
             -I$(srctree)/soc/dsp/visual/inc/LL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP) \
             -I$(srctree)/soc/dsp/visual/inc/HL/$(VIS_FAMILY_PATH) \
             -I$(srctree)/soc/dsp/visual/inc/HL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP) \
             -I$(srctree)/soc/dsp/visual/wrapper/inc/$(VIS_OS_NAME) \
             -I$(srctree)/soc/dsp/visual/inc/common \
             -I$(srctree)/soc/dsp/visual/inc/common/$(VIS_OS_NAME) \
             -I$(srctree)/svc/comsvc/print \
             -I$(srctree)/svc/comsvc/misc
             
ifeq ($(VIS_FAMILY_PATH),fs)
ccflags-y += -I$(srctree)/soc/dsp/visual/inc/similar/$(VIS_FAMILY_PATH)
endif

libdsp_visualHL-objs += \
                        $(DIR)/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/AmbaDSP_EncodeAPI_Priv.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/AmbaDSP_Main_Priv.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/AmbaDSP_Liveview_Priv.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_ApiDump.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_ApiCheck.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_ApiResource.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_ApiTiming.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_BaseCfg.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_Cmds.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_ContextUtility.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_DecodeUtility.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_EncodeAPI.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_EncodeUtility.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_Event.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_IntChk.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_Liveview.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_Main.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_StillDec.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_StillCapture.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_VOUT.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_VideoEnc.o \
                        $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_VideoDec.o \
                        $(DIR)/../../wrapper/src/$(VIS_OS_NAME)/dsp_api.o \
                        $(DIR)/../../wrapper/src/$(VIS_OS_NAME)/dsp_wrapper_osal.o

ifeq ($(VIS_FAMILY_PATH),auto)
libdsp_visualHL-objs += $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_TimeManager.o
else ifeq ($(VIS_FAMILY_PATH),fs)
libdsp_visualHL-objs += $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_TimeManager.o
else ifeq ($(VIS_FAMILY_PATH),cv5x)
libdsp_visualHL-objs += $(DIR)/$(VIS_FAMILY_PATH)/AmbaDSP_TimeManager.o
endif
                        
libdsp_visualHL-objs += $(DIR)/$(VIS_FAMILY_PATH)/AmbaVerSetDspVisualHL.o
$(obj)/AmbaVerSetDspVisualHL.o : $(call amba_ver_dep, AmbaVerSetDspVisualHL.o, $(libdsp_visual-objs))
CFLAGS_AmbaVerSetDspVisualHL.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)            \
                                 -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)    \
                                 -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)      \
                                 -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)      \
                                 -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)                \
                                 -DAMBA_CI_ID=$(AMBA_CI_ID)

install-files := libdsp_visualHL.a


