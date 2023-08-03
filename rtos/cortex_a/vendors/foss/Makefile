
KBUILD_CFLAGS += -Wno-pedantic -Wno-switch-default

ifeq ($(CONFIG_BUILD_FOSS_LWIP),y)
obj-y += lwip/
endif

ifeq ($(CONFIG_BUILD_FOSS_MBEDTLS),y)
obj-y += mbedtls/build/threadx/
endif


ifeq ($(CONFIG_BUILD_FOSS_LVGL),y)
obj-y += lvgl/build/threadx/
endif


ifeq ($(CONFIG_BUILD_FOSS_LIB),y)

obj-y += libfoss.a
libfoss-objs :=

ccflags-y := \
    -I$(srctree)/vendors/ambarella/inc

ifeq ($(CONFIG_BUILD_FOSS_LZ4),y)
include $(srctree)/vendors/foss/lz4/Makefile
endif

ifeq ($(CONFIG_BUILD_FOSS_JSMN),y)
include $(srctree)/vendors/foss/jsmn/Makefile
endif

install-files := libfoss.a

endif # ifeq ($(CONFIG_BUILD_FOSS_LIB),y)

subdir-y += libwrapc
subdir-y += libfdt
subdir-y += libmetal open-amp
ifeq ($(CONFIG_BUILD_SSP_PCIE_LIB),y)
subdir-y += libnvme
endif

# uniq subdir-y
#subdir-y := $(shell echo $(subdir-y) | tr ' ' '\n' | awk '!a[$$0]++')

