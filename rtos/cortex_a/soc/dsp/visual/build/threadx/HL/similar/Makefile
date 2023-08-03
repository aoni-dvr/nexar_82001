obj-y += libdsp_visualSimilar.a

DIR := ../../../../src/similar/fs

ccflags-y := \
             -I$(srctree)/vendors/ambarella/inc \
             -I$(srctree)/vendors/ambarella/inc/io \
             -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
             -I$(srctree)/vendors/ambarella/inc/dsp \
             -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID) \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
             -I$(srctree)/soc/dsp/visual/inc/similar/$(VIS_FAMILY_PATH) \
             -I$(srctree)/soc/dsp/visual/wrapper/inc/$(VIS_OS_NAME) \
             -I$(srctree)/soc/dsp/visual/inc/common \
             -I$(srctree)/soc/dsp/visual/inc/common/$(VIS_OS_NAME) \
             -I$(srctree)/svc/comsvc/print \
             -I$(srctree)/svc/comsvc/misc \
             -I$(srctree)/soc/dsp/visual/inc/HL/$(VIS_FAMILY_PATH) \
             -I$(srctree)/soc/dsp/visual/inc/HL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID) \
             -I$(srctree)/soc/dsp/visual/inc/LL/$(VIS_FAMILY_PATH) \
             -I$(srctree)/soc/dsp/visual/inc/LL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID)

libdsp_visualSimilar-objs += \
                             $(DIR)/AmbaDSPSimilar_EncodeAPI.o \
                             $(DIR)/AmbaDSPSimilar_EncodeUtility.o \
                             $(DIR)/AmbaDSPSimilar_DecodeUtility.o \
                             $(DIR)/AmbaDSPSimilar_Context.o \
                             $(DIR)/$(AMBA_CHIP_ID)/AmbaDSPSimilar_EncodeAPI_Priv.o

install-files := libdsp_visualSimilar.a


