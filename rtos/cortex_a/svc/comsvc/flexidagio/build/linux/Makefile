ccflags-y := \
	-I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc \
        -I$(srctree)/svc/comsvc/misc \
        -I$(srctree)/svc/comsvc/flexidagio \
        -I$(srctree)/svc/comsvc/flexidagio/inc \

ifneq ($(OBJ_NAME),)
# Rule for libcomsvc.a
gnu_make_dir := flexidagio

else
# Rule for libflexidagio.a
OBJ_NAME = libflexidagio

obj-y := $(OBJ_NAME).a
install-files += $(OBJ_NAME).a

gnu_make_dir := .

endif

$(OBJ_NAME)-objs += $(gnu_make_dir)/../../AmbaIPC_FlexidagIO.o
