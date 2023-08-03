#ifeq ($(CONFIG_BUILD_FOSS_LIBFDT),y)

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

obj-y += libnvme.a

# Nvme module
libnvme-objs := nvme.o                       \
                nvme_admin.o                \
                nvme_ctrlr.o                \
                nvme_ns.o                   \
                nvme_qpair.o                \
				nvme_request.o                \
				wrap.o               \
				wrap_pciaccess.o               \
                libnvme.o
#                nvme_quirks.o               \

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
		-I$(srctree)/vendors/arm/arm-trusted-firmware/src/arm-trusted-firmware/include/lib/libc \
		-I$(srctree)/vendors/arm/arm-trusted-firmware/src/arm-trusted-firmware/include/lib/libc/aarch64 \
		-I$(srctree)/vendors/arm/arm-trusted-firmware/src/arm-trusted-firmware/include/lib/libc/aarch32 \
        -I$(srctree)/$(IO_FOLDER)/common/inc/ \
        -I$(srctree)/$(IO_FOLDER)/common/inc/arm/  \
        -I$(srctree)/svc/comsvc/print/ \
        -I$(srctree)/svc/comsvc/misc/ \
        -I$(srctree)/svc/comsvc/shell/inc \
		-I$(srctree)/vendors/ambarella/inc/threadx \
        -I$(srctree)/vendors/foss/libwrapc

ccflags-y += -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -Wno-unused-value -Wno-uninitialized -Wno-int-conversion -Wno-incompatible-pointer-types 

install-files := libnvme.a

#endif # $(CONFIG_BUILD_FOSS_LIBFDT)
