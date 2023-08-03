obj-y += libdsp_visualmonitor.a

DIR := ../../../monitor

ccflags-y := \
             -I$(srctree)/vendors/ambarella/inc \
             -I$(srctree)/vendors/ambarella/inc/io \
             -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
             -I$(srctree)/vendors/ambarella/inc/dsp \
             -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID) \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg \
             -I$(srctree)/soc/dsp/visual/wrapper/inc/$(VIS_OS_NAME) \
             -I$(srctree)/soc/dsp/visual/inc/common \
             -I$(srctree)/soc/dsp/visual/inc/common/$(VIS_OS_NAME) \
             -I$(srctree)/soc/dsp/visual/inc/HL/$(VIS_FAMILY_PATH) \
             -I$(srctree)/soc/dsp/visual/inc/HL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID) \
             -I$(srctree)/soc/dsp/visual/inc/LL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID) \
             -I$(srctree)/soc/dsp/visual/inc/LL/$(VIS_FAMILY_PATH) \
             -I$(srctree)/svc/comsvc/print \
             -I$(srctree)/svc/comsvc/misc
             
libdsp_visualmonitor-objs += \
                             $(DIR)/AmbaDSP_Monitor.o

install-files := libdsp_visualmonitor.a


