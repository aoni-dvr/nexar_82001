ROOT_DIR := $(notdir $(CURDIR))
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(objtree)/include/config/auto.conf

HOST_MKIFS := mkifs

ifeq ($(CONFIG_ENABLE_EMMC_BOOT),y)
HOST_MKXFS := mkqnx6fsimg
else
HOST_MKXFS := mkefs
endif

SUFFIXES := .build .ifs .raw

.PHONY: all clean

all:  ifs-ambarella.raw

clean:
	$(RM_HOST) -f ifs-ambarella.raw
	$(RM_HOST) -f QNX-IFS.bin

BUILD_FILE = $(strip $(subst ",,$(CONFIG_BUILD_QNX_DOT_BUILD)))
FS_BUILD_FILE = $(strip $(subst ",,$(CONFIG_BUILD_QNX_FS_DOT_BUILD)))

ifeq ($(BUILD_FILE),)
ifeq ($(CONFIG_BUILD_QNX_CVFLOW_COMM),y)
BUILD_FILE =	$(CHIP_ID)_cnn_testbed.build
else ifeq ($(CONFIG_SVC_APPS_ICAM),y)
BUILD_FILE =	$(CHIP_ID)_icam.build
else ifeq ($(CONFIG_BUILD_CV),y)
BUILD_FILE =	$(CHIP_ID)_cv_ut.build
else
ifeq ($(CONFIG_DEVICE_TREE_SUPPORT),y)
ifeq ($(CONFIG_SVC_APPS_DIAGNOSIS),y)
BUILD_FILE =	$(CHIP_ID)_diagnosis_dtb.build
else
BUILD_FILE =	$(CHIP_ID)_ut_dtb.build
endif
else
BUILD_FILE =	$(CHIP_ID)_ut.build
endif
endif

# XEN needs different memory layout
ifeq ($(CONFIG_XEN_SUPPORT_QNX),y)
BUILD_FILE :=	xen-$(BUILD_FILE)
endif

ifeq ($(CONFIG_SVC_IOUT_QNX_IMG),y)
BUILD_FILE =	$(CHIP_ID)_iout.build
endif
endif # ($(BUILD_FILE),)

ifneq ($(CONFIG_DEVICE_TREE_SUPPORT),)
QNX_BASE_ADDR = $(shell printf "0x%x" $$(( $(CONFIG_DTB_LOADADDR) + $(CONFIG_DTB_SIZE) )))
endif

ifs-ambarella.raw: $(BUILD_FILE)
ifneq ($(CONFIG_DEVICE_TREE_SUPPORT),)
	sed -i 's|image=\(.*\)|image=$(QNX_BASE_ADDR)]|g' $(BUILD_FILE)
endif
	rm -f ./ifs-ambarella.raw
	$(HOST_MKIFS) -v -r$(INSTALL_ROOT_nto) $(MKIFSFLAGS) $^ $@
	mv ifs-ambarella.raw $(OUTPUT_FOLDER)/qnx.bin
ifneq ($(FS_BUILD_FILE),)
	$(HOST_MKXFS) -v -r$(INSTALL_ROOT_nto) $(FS_BUILD_FILE) qnx_fs.bin
	mv qnx_fs.bin $(OUTPUT_FOLDER)/qnx_fs.bin
endif
