ifneq ($(findstring $(CONFIG_SOC_CV2)$(CONFIG_SOC_CV2FS), y),)

obj-y += libimg_stu.a
libimg_stu-objs := ../../src/common/AmbaSTU_IF.o
libimg_stu-objs += ../../src/common/AmbaSTU_PGMPacker.o
libimg_stu-objs += ../../src/common/AmbaSTU_StringConverter.o
ccflags-y += -I$(srctree)/svc/comsvc/stereoutility/inc/common/
ccflags-y += -I$(srctree)/svc/comsvc/stereoutility
ccflags-y += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-y += -I$(srctree)/svc/comsvc/imgcal
ccflags-y += -I$(srctree)/svc/comsvc/print
#ccflags-y += -I$(srctree)/vendors/ambarella/inc/platform
ccflags-y += -I$(srctree)/vendors/ambarella/inc/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)

install-files := libimg_stu.a
endif
