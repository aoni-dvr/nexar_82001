# For debug
#KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
#KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))
#

obj-y += libwrap_std_ut.a

libwrap_std_ut-objs := \
	test_libwrap_std.o

ccflags-y += -I$(srctree)/vendors/ambarella/inc

install-files := libwrap_std_ut.a


