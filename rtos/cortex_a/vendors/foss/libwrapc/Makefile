# Debug
#KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
#KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))

obj-y += libwrapc.a

libwrapc-objs := \
	wrapc.o \
	AmbaStdWrap.o

IO_FOLDER = soc/io/src

ccflags-y += \
	-DCONFIG_ENABLE_AMBALINK \
	-I$(srctree)/$(src)

ccflags-y += \
	-I$(srctree)/vendors/ambarella/inc \
	-I$(srctree)/$(IO_FOLDER)/common/inc \
	-I$(srctree)/svc/comsvc/misc

install-files := libwrapc.a

