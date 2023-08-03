ifeq ($(CONFIG_BUILD_FOSS_LIBFDT),y)

KBUILD_CFLAGS += -Wno-sign-compare
KBUILD_CFLAGS += -Wno-switch-default

# For debug
#KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
#KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))

#ifeq ($(CONFIG_CC_USESTD),)
#NOSTDINC_FLAGS += -nostdinc -isystem $(shell $(CC) -print-file-name=include)
#KBUILD_CFLAGS += $(NOSTDINC_FLAGS)
#KBUILD_CFLAGS += -include $(srctree)/vendors/foss/libwrapc/AmbaStdWrap.h
#KBUILD_CFLAGS += -I$(srctree)/vendors/foss/libwrapc/inc_wrap
#endif

obj-y += libfdt.a

# Fdt module
libfdt-objs := fdt.o                            \
                fdt_empty_tree.o                \
                fdt_ro.o                        \
                fdt_rw.o                        \
                fdt_strerror.o                  \
                fdt_sw.o                        \
                fdt_wip.o                       \
                fdt_addresses.o                 \
                fdt_overlay.o                   \
                fdt_check.o
ifeq ($(CONFIG_ARM64),y)
libfdt-objs += AmbaLink_BldDtb.o
endif
IO_FOLDER = soc/io/src

ccflags-y += \
        -I$(srctree)/$(IO_FOLDER)/$(AMBA_CHIP_ID)/inc/ \
        -I$(srctree)/$(IO_FOLDER)/$(AMBA_CHIP_ID)/inc/csl/ \
        -I$(srctree)/$(IO_FOLDER)/$(AMBA_CHIP_ID)/inc/reg/ \
        -I$(srctree)/$(IO_FOLDER)/$(AMBA_CHIP_ID)/inc/rtsl/ \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)/

ccflags-y += \
        -I$(srctree)/vendors/ambarella/inc/ \
        -I$(srctree)/vendors/ambarella/inc/io/ \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)/ \
        -I$(srctree)/$(IO_FOLDER)/common/inc/ \
        -I$(srctree)/$(IO_FOLDER)/common/inc/arm/  \
        -I$(srctree)/$(IO_FOLDER)/common/inc/rtsl/  \
        -I$(srctree)/svc/comsvc/print/ \
        -I$(srctree)/svc/comsvc/misc/ \
        -I$(srctree)/svc/comsvc/shell/inc \
        -I$(srctree)/$(src) \
        -I$(srctree)/vendors/foss/libwrapc


install-files := libfdt.a

endif # $(CONFIG_BUILD_FOSS_LIBFDT)
