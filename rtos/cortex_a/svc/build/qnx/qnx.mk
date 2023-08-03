ROOT_DIR := $(notdir $(CURDIR))
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)
unexport ROOT_DIR

# Macro to add newline for dependency
define NEWLINE

endef

SUFFIXES := .mk

include $(objtree)/include/config/auto.conf

ifeq ($(CONFIG_SVC_APPS_UT)$(CONFIG_SVC_APPS_DIAGNOSIS)$(CONFIG_SVC_APPS_ICAM),y)
    CFG_UT_PREBUILT := $(srctree)/vendors/ambarella/lib64/$(AMBA_CHIP_ID)/qnx
endif

# Config that will pass to QNX building system
CFG_QNX_JLEVEL := $(CONFIG_QNX_JLEVEL)
CFG_PREBUILT := $(srctree)/vendors/qnx/prebuilt
CFG_INSTALL   := $(srctree)/$(if $(O),$(O)/)install.qnx
CFG_IMAGE   := $(srctree)/$(if $(O),$(O)/)image.qnx
#CFG_MAKEFILE := qnx.mk
CFG_MAKEFILE := Makefile

# Compiling flags that will pass to QNX building system
QNX_FLAGS :=
QNX_FLAGS += -include $(objtree)/include/generated/autoconf.h
QNX_FLAGS += -include $(objtree)/include/AmbaVer_define.h
QNX_FLAGS += -Wall -Wextra
# For SIMD/NEON
QNX_FLAGS += -march=armv8-a+simd
# For A53 erratum
QNX_FLAGS += -mfix-cortex-a53-835769
QNX_FLAGS += -mfix-cortex-a53-843419

QNX_CFLAGS := $(QNX_FLAGS)
QNX_AFLAGS := $(QNX_FLAGS)
ifneq ($(CONFIG_QNX_DEBUG),)
QNX_CFLAGS += -g -gdwarf-3
QNX_AFLAGS += -g -Wa,-gdwarf-2
endif
# For CRC32
QNX_AFLAGS += -Wa,-march=armv8-a -Wa,-mcpu=generic+crc

# Global extra inclding path to search. (Recommend to add this at your moduls.)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc
export EXTRA_INCVPATH

# Global extra library path to search. (Recommend to add this at your moduls.)
#EXTRA_LIBVPATH := $(CFG_INSTALL)/aarch64le/lib/dll/
#export EXTRA_LIBVPATH

# ---------------------------------------------------------------------------
# Dirs for targets
# ---------------------------------------------------------------------------
if_exist = $(if $(wildcard $(srctree)/$(1)), $(1))
if_mk_exist = $(if $(wildcard $(srctree)/$(1)/$(CFG_MAKEFILE)), $(1))
# $(1) depends on $(2)
dep_add = $(srctree)/$(1): $(srctree)/$(2) FORCE $(NEWLINE)

# Note: sequnce is important if here needs some libraries
pre_mod_dir :=
mod_dir :=

# Libraries needed by other modules
pre_mod_dir += $(call if_exist,vendors/ambarella/wrapper/std/lib/build/qnx)
pre_mod_dir += $(call if_exist,vendors/ambarella/wrapper/kal/qnx/)

#QNX startup, don't touch following lines
pre_mod_dir += $(call if_exist,vendors/qnx/startup/lib)
pre_mod_dir += $(call if_exist,vendors/qnx/startup/boards/ambarella/$(AMBA_CHIP_ID))
#QNX startup, don't touch above lines

#QNX IPL, don't touch following lines
pre_mod_dir += $(call if_exist,vendors/qnx/ipl/lib)
mod_dir += $(call if_exist,vendors/qnx/ipl/boards/$(AMBA_CHIP_ID))
#QNX IPL, don't touch above lines

#------------------------
# SVC CommonService
#------------------------
ifneq ($(CONFIG_BUILD_SVC_COMMON_SERVICE),)
mod_dir += $(call if_exist,svc/comsvc/print/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/shell/build/qnx)
pre_mod_dir += $(call if_exist,svc/comsvc/misc/build/qnx)
pre_mod_dir += $(call if_exist,svc/comsvc/misc/build/qnx/utility_base)
ifeq ($(CONFIG_BUILD_COMSVC_IMGAAA),y)
pre_mod_dir += $(call if_exist,svc/comsvc/imgproc/imgaaa/build/qnx)
endif
ifeq ($(CONFIG_BUILD_COMMON_SERVICE_AF),y)
pre_mod_dir += $(call if_exist,svc/comsvc/af/build/qnx)
endif
ifeq ($(CONFIG_BUILD_COMSVC_IMGFRW),y)
pre_mod_dir += $(call if_exist,svc/comsvc/imgfrw/build/qnx)
endif
ifeq ($(CONFIG_BUILD_COMSVC_MONFRW),y)
mod_dir += $(call if_exist,svc/comsvc/monfrw/build/qnx)
endif
ifneq ($(CONFIG_BUILD_COMMON_SERVICE_CODEC),)
mod_dir += $(call if_exist,svc/comsvc/codec/lib/build/qnx)
endif
ifneq ($(CONFIG_BUILD_COMMON_SERVICE_DCF),)
mod_dir += $(call if_exist,svc/comsvc/dcf/lib/build/qnx)
endif
ifneq ($(CONFIG_BUILD_COMMON_SERVICE_EXIF),)
mod_dir += $(call if_exist,svc/comsvc/exif/lib/build/qnx)
endif
#ifneq ($(CONFIG_BUILD_COMMON_SERVICE_USB),)
#mod_dir += $(call if_exist,svc/comsvc/usb/lib/build/qnx)
#endif
#ifneq ($(CONFIG_AMBA_RTSP),)
#mod_dir += $(call if_exist,svc/comsvc/net/rtsp/lib/build/qnx)
#endif
endif
ifeq ($(CONFIG_BUILD_COMMON_SERVICE_DRAW),y)
mod_dir += $(call if_exist,svc/comsvc/draw/build/qnx)
endif
ifeq ($(CONFIG_BUILD_COMMON_SERVICE_CFS),y)
mod_dir += $(call if_exist,svc/comsvc/cfs/build/qnx)
endif
ifeq ($(CONFIG_BUILD_COMSVC_FIFO),y)
mod_dir += $(call if_exist,svc/comsvc/codec/lib/src/common/dataflow/fifo/build/qnx)
endif
ifeq ($(CONFIG_BUILD_COMSVC_CONTAINER),y)
mod_dir += $(call if_exist,svc/comsvc/codec/lib/src/common/dataflow/container/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/codec/lib/src/common/dataflow/wrapper/build/qnx)
endif
ifeq ($(CONFIG_AMBA_AVDECC),y)
mod_dir += $(call if_exist,svc/comsvc/net/avdecc/build/qnx)
endif

#Basic I/O driver, don't touch following lines
mod_dir += $(call if_exist,soc/io/build/qnx/io-utility)
ifeq ($(CONFIG_SOC_CV2FS),)
mod_dir += $(call if_exist,soc/io/build/qnx/adc)
endif
ifeq ($(CONFIG_SOC_CV2FS),y)
mod_dir += $(call if_exist,soc/io/build/qnx/cehu)
endif
mod_dir += $(call if_exist,soc/io/build/qnx/clock)
mod_dir += $(call if_exist,soc/io/build/qnx/devc)
mod_dir += $(call if_exist,soc/io/build/qnx/dma)
mod_dir += $(call if_exist,soc/io/build/qnx/gpio)
mod_dir += $(call if_exist,soc/io/build/qnx/gdma)
mod_dir += $(call if_exist,soc/io/build/qnx/i2c)
mod_dir += $(call if_exist,soc/io/build/qnx/spi)
mod_dir += $(call if_exist,soc/io/build/qnx/rng)
mod_dir += $(call if_exist,soc/io/build/qnx/can)
mod_dir += $(call if_exist,soc/io/build/qnx/rtc)
mod_dir += $(call if_exist,soc/io/build/qnx/timer)
ifeq ($(CONFIG_SOC_CV2FS),)
mod_dir += $(call if_exist,soc/io/build/qnx/dramc)
endif
mod_dir += $(call if_exist,soc/io/build/qnx/wdt)
mod_dir += $(call if_exist,soc/io/build/qnx/otp)
mod_dir += $(call if_exist,soc/io/build/qnx/misc)
mod_dir += $(call if_exist,soc/io/build/qnx/io-mw)
mod_dir += $(call if_exist,soc/io/build/qnx/fs)
mod_dir += $(call if_exist,soc/io/build/qnx/nvm)
ifeq ($(CONFIG_ENABLE_SPINOR_BOOT),y)
mod_dir += $(call if_exist,soc/io/build/qnx/nor)
mod_dir += $(call if_exist,soc/io/build/qnx/spinor)
mod_dir += $(call if_exist,soc/io/build/qnx/flash/ambarella)
mod_dir += $(call if_exist,soc/io/build/qnx/flash/mtd-flash)
endif
mod_dir += $(call if_exist,soc/io/build/qnx/sd)
mod_dir += $(call if_exist,soc/io/build/qnx/deva/ctrl)
mod_dir += $(call if_exist,soc/io/build/qnx/deva/mixer)
mod_dir += $(call if_exist,soc/io/build/qnx/devb)
mod_dir += $(call if_exist,soc/io/build/qnx/diag)
mod_dir += $(call if_exist,soc/io/build/qnx/pwm)

ifeq ($(CONFIG_ENABLE_NAND_BOOT)$(CONFIG_ENABLE_SPINAND_BOOT),y)
mod_dir += $(call if_exist,soc/io/build/qnx/nand)
mod_dir += $(call if_exist,soc/io/build/qnx/fio)
#mod_dir += $(call if_exist,soc/io/build/qnx/etfs)
endif
#Basic I/O driver, don't touch above lines


ifneq ($(CONFIG_BUILD_SSP_ENET),)
mod_dir += $(call if_exist,soc/io/build/qnx/eth)
endif

ifeq ($(CONFIG_BUILD_DSP_VISUAL),y)
mod_dir += $(call if_exist,soc/dsp/visual/wrapper/build/qnx)
mod_dir += $(call if_exist,soc/dsp/visual/build/qnx)
endif

ifeq ($(CONFIG_BUILD_DSP_AUDIO),y)
mod_dir += $(call if_exist,soc/dsp/audio/qnx/build/qnx)
mod_dir += $(call if_exist,soc/dsp/audio/aac/enc/build/qnx)
mod_dir += $(call if_exist,soc/dsp/audio/aac/dec/build/qnx)
endif

ifeq ($(CONFIG_BUILD_IK),y)
ifeq ($(CONFIG_SOC_CV2FS),y)
pre_mod_dir += $(call if_exist,soc/dsp/imgkernel/imgkernel/cv2xfs/build/qnx)
else
pre_mod_dir += $(call if_exist,soc/dsp/imgkernel/imgkernel/cv2x/build/qnx)
endif
endif

ifeq ($(CONFIG_BUILD_COMMON_TUNE_ITUNER),y)
mod_dir += $(call if_exist,svc/comsvc/tune/ituner/build/qnx)
endif

ifeq ($(CONFIG_BUILD_COMMON_WARP_UTILITY),y)
mod_dir += $(call if_exist,svc/comsvc/warputility/build/qnx)
endif

ifeq ($(CONFIG_BUILD_COMSVC_IMGALGO),y)
ifeq ($(CONFIG_SOC_CV2FS),y)
mod_dir += $(call if_exist,svc/comsvc/imgproc/imgalgo/cv2fs/build/qnx)
else
mod_dir += $(call if_exist,svc/comsvc/imgproc/imgalgo/cv2x/build/qnx)
endif
endif

ifeq ($(CONFIG_BUILD_CTFW),y)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/Ctfw/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/CtfwTask/build/qnx)
endif

ifeq ($(CONFIG_BUILD_COMMON_STEREO_UTILITY),y)
ifeq ($(CONFIG_SOC_CV2FS),)
mod_dir += $(call if_exist,svc/comsvc/stereoutility/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/stereoutility/refflow/build/qnx)
endif
endif

ifeq ($(CONFIG_BUILD_COMSVC_IQPARAM),y)
ifeq ($(CONFIG_SOC_CV2FS),y)
mod_dir += $(call if_exist,svc/comsvc/iqparams/cv2fs/build/qnx)
else
mod_dir += $(call if_exist,svc/comsvc/iqparams/cv2x/build/qnx)
endif
endif

ifneq ($(CONFIG_BUILD_CV),)
mod_dir += $(call if_exist,soc/vision/build/qnx)
mod_dir += $(call if_exist,soc/vision/build/qnx_kernel)
mod_dir += $(call if_exist,soc/vision/arm_framework/app/AmbaCV_Flexidag/build/qnx)
mod_dir += $(call if_exist,soc/vision/arm_framework/app/flexidag_user/build/qnx)
ifeq ($(CONFIG_SOC_CV2),y)
mod_dir += $(call if_exist,soc/vision/arm_framework/app/stereopriv_cv2/build/qnx)
mod_dir += $(call if_exist,soc/vision/arm_framework/app/vopriv_cv2/build/qnx)
mod_dir += $(call if_exist,soc/vision/arm_framework/app/mvacpriv_cv2/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/SCAAmbaStereo_cv2/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/SCAStixel/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/SCAStixelV2/build/qnx)
endif
ifeq ($(CONFIG_SOC_CV2FS),y)
mod_dir += $(call if_exist,soc/vision/arm_framework/app/stereopriv_cv2a/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/SCAStixel/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/SCAStixelV2/build/qnx)
endif
endif

ifneq ($(CONFIG_BUILD_COMMON_SERVICE_CV),)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/FlexidagAmbaFetchChipInfo/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/protection/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/vputils_$(AMBA_CHIP_ID)/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/FlexidagNetTest/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/accelerator/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/cvtask/HLMobilenetSSD/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/SCAOpenOD/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/SCAOpenSeg/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/SCAAppTest/build/qnx)
ifeq ($(CONFIG_SOC_CV2),y)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/SCAAmbaStereo_cv2/build/qnx)
endif
endif

ifneq ($(CONFIG_CV_FLEXIDAG_UT),)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/FlexidagUt/build/qnx)
endif

ifneq ($(CONFIG_BUILD_COMMON_SERVICE_CV),)
mod_dir += $(call if_exist,svc/comsvc/flexidagio/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/SvcCvAlgoUtil/build/qnx)
endif

ifeq ($(CONFIG_BUILD_QNX_CVFLOW_COMM),y)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/CtDetCls/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/CtFrcnnProc/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/CtSingleFD/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/CtSSD/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/CtSingleFDHeader/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/CtUtils/build/qnx)
mod_dir += $(call if_exist,svc/apps/icam/ct_sys/cvflow_comm/build/qnx)
mod_dir += $(call if_exist,svc/apps/icam/ct_sys/amba_vision_flexi/build/qnx)
mod_dir += $(call if_exist,svc/apps/icam/ct_sys/avf_util/build/qnx)
endif

ifeq ($(CONFIG_ICAM_PROJECT_CNNTESTBED),y)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/CtDetCls/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/CtFrcnnProc/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/CtSingleFD/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/CtSSD/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/CtSingleFDHeader/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/cv/arm_framework/app/CtUtils/build/qnx)
mod_dir += $(call if_exist,svc/apps/icam/ct_sys/icam_cnntestbed/build/qnx)
endif

ifeq ($(CONFIG_BUILD_IK),y)
mod_dir += $(call if_exist,soc/dsp/imgkernel/imgcal/build/qnx)
endif

ifeq ($(CONFIG_BUILD_COMMON_TUNE_CTUNER),y)
mod_dir += $(call if_exist,svc/comsvc/tune/ctuner/build/qnx)
endif

ifeq ($(CONFIG_BUILD_IK),y)
ifeq ($(CONFIG_SOC_CV2),)
mod_dir += $(call if_exist,soc/dsp/imgkernel/private_code/featurematching/build/qnx)
endif
endif

#BSP
mod_dir += $(call if_exist,bsp/$(AMBA_CHIP_ID))
mod_dir += $(call if_exist,bsp/$(AMBA_CHIP_ID))/peripherals

ifneq ($(CONFIG_AMBALINK_RPMSG_G2),)
mod_dir += $(call if_exist,svc/comsvc/ambalink/build/qnx/rpmsg/rpmsg_core)
mod_dir += $(call if_exist,svc/comsvc/ambalink/build/qnx/rpmsg/rpmsg_api)
mod_dir += $(call if_exist,svc/comsvc/ambalink/build/qnx/rpmsg/rpmsg_app_echo)
endif  # ($(CONFIG_AMBALINK_RPMSG_G2),)

ifneq ($(CONFIG_BUILD_SVC_COMMON_SERVICE),)
mod_dir += $(call if_exist,svc/comsvc/qnx/test)
endif

ifeq ($(CONFIG_SVC_APPS_IOUT),y)
mod_dir += $(call if_exist,svc/apps/iout/build/qnx)
endif

ifeq ($(CONFIG_SVC_APPS_UT),y)
mod_dir += $(call if_exist,svc/apps/ut/build/qnx)
endif

ifeq ($(CONFIG_SVC_APPS_DIAGNOSIS),y)
mod_dir += $(call if_exist,svc/apps/diagnosis/build/qnx)
endif

# icam
ifeq ($(CONFIG_SVC_APPS_ICAM),y)
mod_dir += $(call if_exist,svc/apps/icam/main/build/qnx)
mod_dir += $(call if_exist,svc/apps/icam/core/build/qnx)
endif

ifneq ($(CONFIG_BUILD_AMBA_ADAS),)
mod_dir += $(call if_exist,svc/comsvc/adas/refflow/build/qnx)
mod_dir += $(call if_exist,svc/comsvc/adas/src/build/qnx)
endif

# foss
ifeq ($(CONFIG_BUILD_FOSS_LVGL),y)
mod_dir += $(call if_exist, vendors/foss/lvgl/build/qnx)
endif

# foss mbedtls
ifeq ($(CONFIG_BUILD_FOSS_MBEDTLS),y)
mod_dir += $(call if_exist, vendors/foss/mbedtls/build/qnx)
endif

# foss lz4
ifeq ($(CONFIG_BUILD_FOSS_LZ4),y)
mod_dir += $(call if_exist, vendors/foss/lz4/build/qnx)
endif

# 3rd party
ifneq ($(CONFIG_BUILD_COMMON_SERVICE_CV),)
mod_dir += $(call if_exist,svc/apps/qnx_apps/flexidag_schdr/build/qnx)
mod_dir += $(call if_exist,svc/apps/qnx_apps/visdmsg/build/qnx)
mod_dir += $(call if_exist,svc/apps/qnx_apps/vpstatus/build/qnx)
mod_dir += $(call if_exist,svc/apps/qnx_apps/appcv_bisenet/build/qnx)
mod_dir += $(call if_exist,svc/apps/qnx_apps/appcv_mnetssd/build/qnx)
mod_dir += $(call if_exist,svc/apps/qnx_apps/appcv_feedpicinfo/build/qnx)
endif

# Unit-test
#mod_dir += $(call if_exist,vendors/ambarella/wrapper/std/ut/build/qnx)

pre_mod_alldirs := $(foreach d,$(pre_mod_dir),$(srctree)/$(d))
mod_alldirs := $(foreach d,$(mod_dir),$(srctree)/$(d))
qnx_alldirs := $(pre_mod_alldirs) $(mod_alldirs)
PHONY += $(qnx_alldirs)

$(mod_alldirs): $(pre_mod_alldirs)

# Add individual dependence here, $(1) depends on $(2)
#  e.g. $(eval $(call dep_add,$(1),$(2)))
$(eval $(call dep_add,vendors/qnx/startup/boards/ambarella/$(AMBA_CHIP_ID),vendors/qnx/startup/lib))
$(eval $(call dep_add,vendors/qnx/ipl/boards/$(AMBA_CHIP_ID),vendors/qnx/ipl/lib))
$(eval $(call dep_add,soc/io/$(AMBA_CHIP_ID)/qnx/diag,soc/io/$(AMBA_CHIP_ID)/qnx/gpio))
$(eval $(call dep_add,soc/vision/build/qnx_kernel,soc/io/build/qnx/io-mw))
# Note: $(srctree)/build/maintenance/qnx_getDep.sh will parse
#	'LIBS' defined in 'common.mk' to generate dependency-rules
$(if $(findstring clean,$(MAKECMDGOALS)),,$(eval -include $(objtree)/dep_example.mk))


# ---------------------------------------------------------------------------
# targets
# ---------------------------------------------------------------------------
PHONY += all clean images prebuilt binaries

all: images
	@echo done


clean:
	@for dir in $(qnx_alldirs); do \
		$(if $(Q),echo "  [Clean]      $${dir}";) $(MAKE) $(if $(Q),-s) \
		-C $${dir} \
		-f $${dir}/$(CFG_MAKEFILE) \
		INSTALL_ROOT_nto=$(CFG_INSTALL) \
		MAKEFILE=$(CFG_MAKEFILE) \
		JLEVEL=$(CFG_QNX_JLEVEL) \
		clean; \
	done
	$(Q)rm -rf $(CFG_INSTALL)
	$(Q)rm -f $(srctree)/vendors/qnx/images/ifs*
	$(Q)rm -f $(objtree)/QnxModDep.pdf $(objtree)/dep_example.mk
	$(Q)rm -rf $(srctree)/$(O).64/fwprog
	$(Q)rm -rf $(srctree)/$(O).64/out

%-clean:
	$(Q)dir=$(subst -clean,,$@) && \
		$(MAKE) \
		-C $${dir} \
		-f $${dir}/$(CFG_MAKEFILE) \
		INSTALL_ROOT_nto=$(CFG_INSTALL) \
		MAKEFILE=$(CFG_MAKEFILE) \
		JLEVEL=$(CFG_QNX_JLEVEL) \
		clean

%-qmk:
	$(Q)dir=$(subst -qmk,,$@) && \
		$(MAKE) \
		-C $${dir} \
		-f $${dir}/$(CFG_MAKEFILE) \
		INSTALL_ROOT_nto=$(CFG_INSTALL) \
		MAKEFILE=$(CFG_MAKEFILE) \
		JLEVEL=$(CFG_QNX_JLEVEL) \
		CCOPTS="$(QNX_CFLAGS)" \
		ASOPTS="$(QNX_AFLAGS)" \
		install

# For Building information. enable FORCE will make file to be generated every time.
$(objtree)/include/AmbaVer_define.h: FORCE
	$(Q)rm -f $@
	$(Q)echo "#ifndef AMBAVER_DEFINE_H" >> $@
	$(Q)echo "#define AMBAVER_DEFINE_H" >> $@
	$(Q)echo "#define AMBA_BUILD_DATE \"`date`\"" >> $@
	$(Q)echo "#define AMBA_BUILD_DATE_NUM (`date '+%Y%m%d'`u)" >> $@
	$(Q)echo "#define AMBA_BUILD_SEC_NUM (`date '+%s'`u)" >> $@
	$(Q)echo "#define AMBA_BUILD_MACHINE \"`whoami`@`hostname`:[gcc `$(CC) -dumpversion`]\"" >> $@
	$(Q)echo "#endif" >> $@

PHONY += hinstall
hinstall: prebuilt
	@for dir in $(qnx_alldirs); do \
		$(if $(Q),echo "  [HInstall]   $${dir}";) $(MAKE) $(if $(Q),-s) \
		-C $${dir} \
		-f $${dir}/$(CFG_MAKEFILE) \
		INSTALL_ROOT_nto=$(CFG_INSTALL) \
		MAKEFILE=$(CFG_MAKEFILE) \
		JLEVEL=$(CFG_QNX_JLEVEL) \
		hinstall; \
	done

# Force to compile one-by-one
PHONY += install
install: hinstall
	$(Q)for dir in $(qnx_alldirs); do \
		$(MAKE) \
		-C $${dir} \
		-f $${dir}/$(CFG_MAKEFILE) \
		INSTALL_ROOT_nto=$(CFG_INSTALL) \
		MAKEFILE=$(CFG_MAKEFILE) \
		JLEVEL=$(CFG_QNX_JLEVEL) \
		CCOPTS="$(QNX_CFLAGS)" \
		ASOPTS="$(QNX_AFLAGS)" \
		install; \
		if [ "$$?" != "0" ]; then exit 1; fi; \
	done

# Parallel compiling if have good dependence.
PHONY += $(qnx_alldirs)
$(qnx_alldirs): hinstall
	@$(if $(Q),echo "  [Build]      $@";) $(MAKE) $(if $(Q),-s) \
		-C $@ \
		-f $@/$(CFG_MAKEFILE) \
		INSTALL_ROOT_nto=$(CFG_INSTALL) \
		MAKEFILE=$(CFG_MAKEFILE) \
		JLEVEL=$(CFG_QNX_JLEVEL) \
		CCOPTS="$(QNX_CFLAGS)" \
		ASOPTS="$(QNX_AFLAGS)" \
		install


#images: install
images: $(qnx_alldirs)
	$(Q)mkdir -p $(CFG_IMAGE)
	$(Q)$(MAKE) -C $(srctree)/vendors/qnx/images/ INSTALL_ROOT_nto=$(CFG_INSTALL) OUTPUT_FOLDER=$(CFG_IMAGE) CHIP_ID=$(AMBA_CHIP_ID)
ifeq ($(CONFIG_XEN_SUPPORT_QNX),y)
	$(Q)$(MAKE) -C $(srctree)/vendors/xen/boot/qnx
endif

prebuilt: $(objtree)/include/AmbaVer_define.h $(objtree)/dep_example.mk
	@echo "  [Install]    prebuilt"
	$(Q)mkdir -p $(CFG_INSTALL)
	$(Q)cp -rf $(CFG_PREBUILT)/* $(CFG_INSTALL)
ifeq ($(CONFIG_SVC_APPS_UT)$(CONFIG_SVC_APPS_DIAGNOSIS)$(CONFIG_SVC_APPS_ICAM),y)
	$(Q)mkdir -p $(CFG_INSTALL)/aarch64le/lib
	$(Q)cp -rf $(CFG_UT_PREBUILT)/* $(CFG_INSTALL)/aarch64le/lib/
endif


PHONY += qnx-draw-dep
$(objtree)/dep_example.mk qnx-draw-dep:
	$(Q)$(srctree)/build/maintenance/qnx_getDep.sh $(qnx_alldirs)
	@echo
	@echo "Plz check dep_example.mk (and QnxModDep.pdf) @ ${objtree} for dependency-graph of modules."
	@echo


PHONY += FORCE
FORCE:

.PHONY: $(PHONY)

