ifneq ($(OBJ_NAME),)
# Rule for libcomsvc.a
gnu_make_dir := dramshmoo
dramshmoo_dir := dramshmoo

else
# Rule for libdramshmoo.a
OBJ_NAME = libdramshmoo

obj-y := $(OBJ_NAME).a
install-files += $(OBJ_NAME).a

gnu_make_dir := ../../src/linux
dramshmoo_dir := ../../..

endif

ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg
#ccflags-y += -I$(srctree)/soc/io/src/common/inc
#ccflags-y += -I$(srctree)/soc/io/src/common/inc/rtsl
ccflags-y += -I$(srctree)/svc/comsvc/dramshmoo/inc
ccflags-y += -I$(srctree)/svc/comsvc/dramshmoo/common
ifneq ($(findstring $(CONFIG_SOC_CV5)$(CONFIG_SOC_CV52), y y),)
ccflags-y += -I$(srctree)/svc/comsvc/dramshmoo/$(AMBA_CHIP_ID)
endif

#$(OBJ_NAME)-objs += $(gnu_make_dir)/AmbaDramShmoo.o
ifeq ($(CONFIG_DRAM_TYPE_LPDDR5),y)
$(OBJ_NAME)-objs += $(gnu_make_dir)/AmbaDramAdjust.o
endif
ifneq ($(findstring $(CONFIG_SOC_CV5)$(CONFIG_SOC_CV52), y y),)
$(OBJ_NAME)-objs += $(dramshmoo_dir)/common/AmbaDramCommon.o
$(OBJ_NAME)-objs += $(dramshmoo_dir)/$(AMBA_CHIP_ID)/AmbaDiag_CmdDDR.o
endif
