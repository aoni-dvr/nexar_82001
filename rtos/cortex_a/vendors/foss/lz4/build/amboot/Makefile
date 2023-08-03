
KBUILD_CFLAGS := $(subst -Werror ,,$(KBUILD_CFLAGS))
KBUILD_CFLAGS += -Wno-pedantic -Wno-switch-default

obj-y += libfoss_lz4.a

libfoss_lz4-objs += ../../lz4.o
libfoss_lz4-objs += ../../lz4hc.o
libfoss_lz4-objs += ../../AmbootLZ4.o

ccflags-y += -I$(srctree)/vendors/ambarella/inc -I$(srctree)/svc/comsvc/print

install-files := libfoss_lz4.a
