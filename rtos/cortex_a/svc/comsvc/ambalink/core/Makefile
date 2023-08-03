ifeq ($(CONFIG_ENABLE_AMBALINK),y)

obj-y += core.o

#KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
#KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))


core-objs := \
	AmbaIPC.o		\
	AmbaLink.o		\
	AmbaIPC_Mutex.o		\
	AmbaIPC_SpinLock.o	\
	AmbaIPC_RPMSG.o		\
	AmbaIPC_Vq.o		\
	AmbaIPC_EventHandler.o


ifneq ($(CONFIG_AMBALINK_BOOT_OS),)
core-objs += \
	hyp/sw_hvc.o		\
	AmbaLink_Dtb.o		\
	fdt_boot.o		\
	fdt_cvshm.o

ifneq ($(CONFIG_AMBALINK_RPMSG_HIBER),)
core-objs += \
	AmbaIPC_Hiber.o
endif # CONFIG_AMBALINK_RPMSG_HIBER

ifneq ($(CONFIG_THREADX64),)
core-objs += AmbaLinkBoot_A64.o
else
core-objs += AmbaLinkBoot_A32.o
endif

endif # CONFIG_AMBALINK_BOOT_OS

ifneq ($(CONFIG_AMBALINK_RPMSG_ASIL),)
core-objs += AmbaSafety_IPC.o \
	     AmbaIPC_crc32.o
endif # CONFIG_AMBALINK_RPMSG_ASIL

ifneq ($(CONFIG_AMBALINK_BOOT_OS),)
ccflags-y += -I$(srctree)/vendors/foss/libwrapc
ccflags-y += \
	-I$(srctree)/vendors/foss/libfdt
endif # CONFIG_AMBALINK_BOOT_OS

endif # $(CONFIG_ENABLE_AMBALINK)
