
# Do not use O2
KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))

obj-y += libcrt.a
ifneq ($(CONFIG_BUT_VCAST),)
obj-y += .stamp_options
endif

#libcrt-objs := startup.o
ifneq ($(CONFIG_ARM32),)
libcrt-objs := startup_v8_32b.o
#if 0
	ifneq ($(CONFIG_CPU_CORTEX_R52),)
	# If starting from EL2, need to switch to EL1
	libcrt-objs := startup_r52.o
	endif
#endif
else ifneq ($(CONFIG_ARM64),)
libcrt-objs := startup_v8_64b.o
endif
libcrt-objs += semihost.o
libcrt-objs += ambacrt0.o

ifneq ($(CONFIG_BUT_VCAST),)
libcrt-objs += nosys.o
endif
ifdef CONFIG_CC_CXX_SUPPORT
# libnosys.a does not support semihosting
libcrt-objs += nosys.o
endif

ifneq ($(CONFIG_BUT_VCAST),)
$(obj)/.stamp_options:
	@echo $(KBUILD_CFLAGS) > $@
endif

ccflags-y += -I$(srctree)/vendors/ambarella/inc

install-files := libcrt.a
clean-files += $(obj)/.stamp_options
clean-files += .stamp_options

