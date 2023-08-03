#### ThreadX
ccflags-y := \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
        -I$(srctree)/vendors/ambarella/inc/dsp \
        -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID) \
        -I$(srctree)/svc/comsvc/print \
		-I$(srctree)/svc/comsvc/misc \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl

ifneq ($(OBJ_NAME),)
# Rule for libcomsvc.a
gnu_make_dir := misc

else
# Rule for libmisc.a
OBJ_NAME = libmisc

obj-y := $(OBJ_NAME).a
install-files += $(OBJ_NAME).a

gnu_make_dir := .

endif

$(OBJ_NAME)-objs += $(gnu_make_dir)/../../AmbaUtility.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../AmbaUtility_Crc32Hw.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../AmbaDspInt.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../AmbaSvcWrap.o
