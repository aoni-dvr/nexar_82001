ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(objtree)/include/config/auto.conf

# Compiling options
CCFLAGS += -O2 \
	   -Werror \
	   -fomit-frame-pointer -fno-strength-reduce \
	   -D_FILE_OFFSET_BITS=64

# We link the stack with -E so a lot of the undefined
# references get resolved from the stack itself.  If
# you want them listed at link time, turn off
# --allow-shlib-undefined and replace with --warn-once
# if desired.
#LDFLAGS+=-Wl,--warn-once
#LDFLAGS+=-Wl,--allow-shlib-undefined
LDFLAGS+=-Wl,-T $(PROJECT_ROOT)/icam.lds
LDFLAGS+=-Map

ifneq ($(CONFIG_AMBALINK_RPMSG_G2),)
LIBS += c socket
LIBS += rpmsg_api
LIBPREF_rpmsg_api = -Bstatic
LIBPOST_rpmsg_api = -Bdynamic
endif

LIBS += svc_icamcore
LIBPREF_svc_icamcore = -Bstatic
LIBPOST_svc_icamcore = -Bdynamic

LIBS += wrap_std
LIBS += m
LIBS += bsp
LIBS += perif
LIBS += kal-ambarella
LIBS += nvm-ambarella
LIBS += fs-ambarella
LIBS += io-mw
LIBS += cache
LIBS += dsp_wrapper
LIBS += dsp_imgkernel
LIBS += dsp_imgkernelcore
LIBS += comsvc-ambarella-misc
LIBS += comsvc-ambarella-print
LIBS += comsvc-ambarella-shell
LIBS += comsvc_misc
LIBS += img_proc
LIBS += socket

ifeq ($(CONFIG_ENABLE_NAND_BOOT)$(CONFIG_ENABLE_SPINAND_BOOT),y)
LIBS += nand-ambarella
endif

ifeq ($(CONFIG_ENABLE_SPINOR_BOOT),y)
LIBS += spinor-ambarella
endif

ifeq ($(CONFIG_BUILD_MAL),y)
LIBS += mal
endif

ifeq ($(CONFIG_ICAM_AUDIO_USED),y)
LIBS += dsp_audio
LIBS += aacenc
LIBS += aacdec
LIBS += asound
endif

ifeq ($(CONFIG_BUILD_COMSVC_IMGFRW),y)
LIBS += imgfrw
ifeq ($(CONFIG_BUILD_IMGFRW_AAA),y)
LIBS += imgaaa
endif
ifeq ($(CONFIG_BUILD_COMMON_SERVICE_AF),y)
LIBS += comsvc_af
endif
endif

ifeq ($(CONFIG_BUILD_COMSVC_MONFRW),y)
LIBS += monfrw
endif

ifeq ($(CONFIG_BUILD_COMMON_TUNE_ITUNER),y)
LIBS += img_ituner
endif

ifeq ($(CONFIG_BUILD_COMMON_TUNE_CTUNER),y)
LIBS += img_tuner
endif

ifeq ($(CONFIG_ICAM_IMGCAL_USED),y)
LIBS += img_cal
endif

ifeq ($(CONFIG_BUILD_COMMON_SERVICE_DCF),y)
LIBS += comsvc_dcf
endif

ifeq ($(CONFIG_BUILD_COMMON_SERVICE_EXIF),y)
LIBS += comsvc_exif
endif

ifeq ($(CONFIG_BUILD_COMMON_SERVICE_CFS),y)
LIBS += comsvc_cfs
endif

ifeq ($(CONFIG_BUILD_COMMON_SERVICE_DRAW),y)
LIBS += comsvc_draw
endif

ifeq ($(CONFIG_ICAM_RECORD_USED),y)
LIBS += comsvc_fifo
LIBS += comsvc_wrapper
LIBS += comsvc_container
LIBS += comsvc_codec
endif

ifdef CONFIG_BUILD_CV
LIBS += ambacv cv_AmbaCV_Flexidag cv_schdr

ifeq ($(CONFIG_ICAM_CV_STEREO),y)
ifeq ($(CONFIG_SOC_CV2), y)
LIBS += cv_stereopriv_cv2
LIBS += cv_vopriv_cv2
LIBS += cv_mvacpriv_cv2
LIBS += img_stu img_stu_refflow
LDFLAGS += -Wl,--whole-archive -l cv_stereopriv_cv2 -lcv_vopriv_cv2 -lcv_mvacpriv_cv2 -limg_stu -limg_stu_refflow -Wl,--no-whole-archive -lang-c++
LIBS += cv_SCAAmbaStereo_cv2
endif

ifeq ($(CONFIG_SOC_CV2FS), y)
LIBS += cv_stereopriv_cv2a
LIBS += img_stu
endif
endif
endif

ifdef CONFIG_BUILD_COMMON_SERVICE_CV
LIBS += cv_vputils_$(AMBA_CHIP_ID) cv_protection cv_accelerator cv_SCAOpenOD cv_SCAOpenSeg SvcCvAlgoUtil
LIBS += cv_SCAAppTest
endif

ifdef CONFIG_CV_FLEXIDAG_UT
LIBS += cv_FlexidagUt
LDFLAGS += -Wl,--whole-archive -lcv_HLMobilenetSSD -Wl,--no-whole-archive
endif

ifeq ($(CONFIG_SVC_LVGL_USED),y)
LIBS += foss_lvgl
endif

ifeq ($(CONFIG_BUILD_FOSS_MBEDTLS),y)
LIBS += foss_mbedtls
endif

ifeq ($(CONFIG_BUILD_FOSS_LZ4),y)
LIBS += foss_lz4
endif

ifeq ($(CONFIG_ICAM_PROJECT_CNNTESTBED),y)
LIBS += icam_cnntestbed
LIBS += CtUtils CtSSD CtDetCls CtFrcnnProc CtSingleFD CtSingleFDHeader flexidagio
endif

EXCLUDE_OBJS =

# module description
NAME := amba_icam
USEFILE = $(PROJECT_ROOT)/Usemsg
define PINFO
PINFO DESCRIPTION="amba icam application"
endef
INSTALLDIR = bin

ifneq ($(CONFIG_AMBALINK_RPMSG_G2),)
LIBS += c socket
LIBS += rpmsg_api
LIBPREF_rpmsg_api = -Bstatic
LIBPOST_rpmsg_api = -Bdynamic
endif

# This macro tells the makefiles to search the INSTALL_ROOT_nto directory tree
# when the compiler and linker are seaching for headers and libraries.
USE_INSTALL_ROOT=1

include $(MKFILES_ROOT)/qtargets.mk
