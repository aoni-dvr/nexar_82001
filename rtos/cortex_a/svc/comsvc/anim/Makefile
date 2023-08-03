ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else
ifeq ($(CONFIG_SOC_CV52),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif
endif

ccflags-y += \
		-I$(srctree)/vendors/foss/lz4 \
		-I$(srctree)/svc/comsvc/anim/inc \
		-I$(srctree)/svc/comsvc/misc \
		-I$(srctree)/svc/comsvc/print \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
        -I$(srctree)/vendors/ambarella/inc/ \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
        -I$(srctree)/vendors/ambarella/inc/dsp \
        -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)

ifneq ($(OBJ_NAME),)
# Rule for libcomsvc.a
gnu_make_dir := anim

else
# Rule for libcomsvc_anim.a
OBJ_NAME = libcomsvc_anim

obj-y := $(OBJ_NAME).a
install-files += $(OBJ_NAME).a

gnu_make_dir := .

endif

$(OBJ_NAME)-objs += $(gnu_make_dir)/AmbaAnimLib.o 
$(OBJ_NAME)-objs += $(gnu_make_dir)/AmbaCalibData.o 

