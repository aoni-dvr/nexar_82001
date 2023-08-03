# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.

include $(objtree)/include/config/auto.conf

ICAM_MAIN_SRC = $(CURDIR)/../../src
ICAM_CORE_SRC = $(CURDIR)/../../../core/src
CT_SYS_SRC    = $(CURDIR)/../../../ct_sys

EXTRA_SRCVPATH := $(ICAM_MAIN_SRC)/
EXTRA_SRCVPATH += $(ICAM_MAIN_SRC)/cmd/
EXTRA_SRCVPATH += $(ICAM_MAIN_SRC)/config/
EXTRA_SRCVPATH += $(ICAM_MAIN_SRC)/entry/qnx/
EXTRA_SRCVPATH += $(ICAM_MAIN_SRC)/warnicon/

# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
SRCS := SvcSysEntry.c

SRCS += SvcInitTask.c
SRCS += iCamBufPool.c
SRCS += SvcCmdApp.c
SRCS += SvcCmdSys.c
SRCS += SvcCmdDSP.c
SRCS += SvcSysStatTask.c
SRCS += SvcControlTask.c
SRCS += SvcVinCtrlTask.c
SRCS += SvcVinBufMonitorTask.c
SRCS += SvcVoutCtrlTask.c
SRCS += SvcLiveviewTask.c
SRCS += SvcDisplayTask.c
SRCS += SvcDspTask.c
SRCS += SvcDataLoader.c
SRCS += SvcFwUpdateTask.c
SRCS += SvcOsdTask.c
SRCS += SvcGuiTask.c
SRCS += SvcMenuTask.c
SRCS += SvcButtonCtrlTask.c
SRCS += SvcStgMonitorTask.c
SRCS += SvcCanSpeedTask.c
SRCS += SvcNvmTask.c
SRCS += SvcResCfgTask.c
SRCS += SvcBufMap.c
SRCS += SvcIKCfg.c
SRCS += SvcInfoPack.c
SRCS += SvcUserPref.c
SRCS += SvcLogoDraw.c
SRCS += SvcAmageTask.c
SRCS += SvcClkSwTask.c
SRCS += SvcSysInfoTask.c

##### project
    # adas
ifeq ($(CONFIG_ICAM_PROJECT_ADAS_DVR),y)
SRCS += SvcControlTask_AdasDvr.c
endif
    # emr
ifeq ($(CONFIG_ICAM_PROJECT_EMIRROR),y)
SRCS += SvcControlTask_Emr.c
endif
    # dms
ifeq ($(CONFIG_ICAM_PROJECT_DMS), y)
SRCS += SvcControlTask_Dms.c
endif
    # surr
ifeq ($(CONFIG_ICAM_PROJECT_SURROUND), y)
SRCS += SvcControlTask_Surround.c
SRCS += SvcAnim.c
SRCS += SvcAnimTask.c
SRCS += SvcAnimCalib.c
SRCS += SvcAnimMemPool.c
ifeq ($(CONFIG_BUILD_CV),y)
SRCS += SvcPsdTask.c
SRCS += SvcOwsTask.c
SRCS += SvcRctaTask.c
SRCS += SvcSurTask.c
SRCS += SvcSurDrawTask.c
endif
endif
    # cnn_testbed
ifeq ($(CONFIG_ICAM_PROJECT_CNNTESTBED), y)
SRCS += SvcControlTask_CnnTestbed.c
endif

#### image
    # aaa
ifeq ($(CONFIG_BUILD_IMGFRW_AAA),y)
SRCS += SvcImgTask.c
SRCS += SvcColorBalance.c
endif
    # rawcap
ifeq ($(CONFIG_ICAM_IMGRAWCAP_USED),y)
SRCS += SvcRawCapTask.c
endif
    # view control
ifeq ($(CONFIG_ICAM_VIEWCTRL_USED),y)
SRCS += SvcEmrTask.c
endif
    # cal
ifeq ($(CONFIG_ICAM_IMGCAL_USED),y)
SRCS += SvcCalibTask.c
SRCS += SvcCalibCfg.c
ifeq ($(CONFIG_ICAM_IMGCAL_STITCH_USED),y)
SRCS += SvcCalibStitchLdc.c
SRCS += SvcCalibStitchBlend.c
endif
ifeq ($(CONFIG_ICAM_EEPROM_USED),y)
SRCS += SvcCalibModuleTask.c
SRCS += SvcCalibModuleCfg.c
endif
endif

##### record
ifeq ($(CONFIG_ICAM_RECORD_USED),y)
SRCS += SvcRecTask.c
SRCS += SvcRecInfoPack.c
SRCS += SvcCmdRec.c
ifeq ($(CONFIG_ICAM_RAWENC_USED),y)
SRCS += SvcRawEncTask.c
endif
endif

##### audio
ifeq ($(CONFIG_ICAM_AUDIO_USED),y)
SRCS += SvcAudioTask.c
SRCS += SvcAudInfoPack.c
SRCS += SvcCmdAudio.c
endif

##### playback
ifeq ($(CONFIG_ICAM_PLAYBACK_USED),y)
SRCS += SvcPlaybackTask.c
SRCS += SvcPbkInfoPack.c
SRCS += SvcCmdPbk.c
endif

##### still capture
ifeq ($(CONFIG_ICAM_STLCAP_USED),y)
SRCS += SvcStillTask.c
endif

##### cv
ifeq ($(CONFIG_BUILD_CV), y)
SRCS += SvcCmdApp_CV.c
SRCS += SvcCvMainTask.c
SRCS += SvcCvCtrlTask.c
SRCS += SvcCvFileInTask.c
SRCS += SvcODDrawTask.c
SRCS += SvcSegDrawTask.c
SRCS += SvcVoutFrmCtrlTask.c
#SRCS += SvcWarningIconTask.c
SRCS += SvcExtOsdDrawTask.c
    # regression
ifeq ($(CONFIG_ICAM_ENABLE_REGRESSION_FLOW), y)
SRCS += SvcRegressionTask.c
endif
    # adas_dvr
ifeq ($(CONFIG_BUILD_AMBA_ADAS),y)
ifeq ($(CONFIG_ICAM_PROJECT_ADAS_DVR),y)
SRCS += SvcLdwsTask.c
SRCS += SvcFcwsFcmdTask.c
SRCS += SvcAdasNotify.c
endif
    # emr_bsd
ifeq ($(CONFIG_ICAM_PROJECT_EMIRROR),y)
SRCS += SvcBsdTask.c
endif
    # emr_ema
ifeq ($(CONFIG_ICAM_IMGCAL_STITCH_USED),y)
SRCS += SvcEmrAdaptiveTask.c
endif
endif
    # stereo
ifeq ($(CONFIG_ICAM_CV_STEREO),y)
SRCS += SvcStereoTask.c
SRCS += SvcCalibStereo.c
SRCS += SvcWarp.c
ifeq ($(CONFIG_ICAM_CV_STEREO_AUTO_CALIB),y)
SRCS += SvcStereoACTask.c
endif
endif
    # logger
ifeq ($(CONFIG_ICAM_CV_LOGGER),y)
SRCS += SvcCvLoggerTask.c
endif
endif

##### safety
ifeq ($(CONFIG_BUILD_MONFRW_GRAPH),y)
SRCS += SvcSafeTask.c
ifeq ($(CONFIG_SVC_VOUT_ERR_CTRL_USED),y)
SRCS += SvcVoutErrTask.c
endif
endif
ifeq ($(CONFIG_SVC_VIN_ERR_CTRL_USED),y)
SRCS += SvcVinErrTask.c
endif

##### other
ifeq ($(CONFIG_ICAM_ENABLE_VOUT_DEF_IMG), y)
SRCS += SvcVoutDefImgTask.c
endif
    # dirt detection
ifeq ($(CONFIG_ICAM_USE_AMBA_DIRTDETECT),y)
ifeq ($(CONFIG_BUILD_CV),y)
ifneq ($(wildcard $(srctree)/vendors/ambarella/inc/adas/AmbaDirtDetect.h),)
SRCS += SvcDirtDetectTask.c
endif
endif
endif
    # defog
ifeq ($(CONFIG_ICAM_USE_AMBA_DEFOG),y)
SRCS += SvcDefogTask.c
endif
    # ambalink
ifeq ($(CONFIG_AMBALINK_BOOT_OS),y)
SRCS += SvcRpmsgApp.c
endif


ifeq ($(CONFIG_ICAM_CVBS_ADC_FINE_TUNE),y)
SRCS += SvcNtscAdjust.c
endif


#
# inc path
#
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/vision
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/perif
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/qnx
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/usb
EXTRA_INCVPATH += $(srctree)/vendors/foss/libwrapc
EXTRA_INCVPATH += $(srctree)/vendors/foss/lvgl/lvgl/src
EXTRA_INCVPATH += $(srctree)/vendors/foss/lz4
EXTRA_INCVPATH += $(srctree)/soc/vision/cv_common/inc
EXTRA_INCVPATH += $(srctree)/soc/vision/cv_common/inc/cvapi
EXTRA_INCVPATH += $(srctree)/soc/io/src/common/inc/
EXTRA_INCVPATH += $(srctree)/soc/io/src/common/inc/arm
EXTRA_INCVPATH += $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
EXTRA_INCVPATH += $(srctree)/bsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/bridge/inc
EXTRA_INCVPATH += $(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/display/inc
EXTRA_INCVPATH += $(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/sensor/inc
EXTRA_INCVPATH += $(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/yuvinput/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/adas/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/adas/refflow/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/adas/refflow/inc/bsd
EXTRA_INCVPATH += $(srctree)/svc/comsvc/adas/refflow/inc/fc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/adas/refflow/inc/ldws
EXTRA_INCVPATH += $(srctree)/svc/comsvc/adas/refflow/inc/psd
EXTRA_INCVPATH += $(srctree)/svc/comsvc/adas/refflow/inc/rcta
ifneq ($(wildcard $(srctree)/svc/comsvc/adas/utils/inc),)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/adas/utils/inc
endif
EXTRA_INCVPATH += $(srctree)/svc/comsvc/ambalink/core/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/ambalink/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/ambalink/inc/rpcprog
EXTRA_INCVPATH += $(srctree)/svc/comsvc/anim/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/camera_module/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cfs/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/codec/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/cv_common/inc/cvapi
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/cv_common/inc/svccvalgo
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/cv_common/inc/svccvalgo/qnx
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/arm_framework/app/CtUtils/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/arm_framework/app/CtSingleFD/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/arm_framework/app/CtSSD/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/arm_framework/app/CtDetCls/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cv/arm_framework/app/CtSingleFDHeader/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/dcf/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/exif/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgcal
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgcal/arch
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgproc/imgaaa/inc/aaa
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgproc/imgaaa/inc/iqparam
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgproc/imgaaa/inc/wbcalib
EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/monfrw/main/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/net/app
EXTRA_INCVPATH += $(srctree)/svc/comsvc/net/app/avdecc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/net/app/avdecc/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/net/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/net/rtsp/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print
EXTRA_INCVPATH += $(srctree)/svc/comsvc/shell/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/stereoutility
EXTRA_INCVPATH += $(srctree)/svc/comsvc/tune/ituner
EXTRA_INCVPATH += $(srctree)/svc/comsvc/tune/ctuner
EXTRA_INCVPATH += $(srctree)/svc/comsvc/tune/ctuner/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/tune/ctuner/inc/common
EXTRA_INCVPATH += $(ICAM_MAIN_SRC)/inc
EXTRA_INCVPATH += $(ICAM_MAIN_SRC)/config/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/adas/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/cal/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/cal/module/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/config/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/cv/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/cv/cvflow/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/cv/transfer/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/driver/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/dsp/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/img/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/draw/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/playback/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/record/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/safe/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/sys/inc
EXTRA_INCVPATH += $(srctree)/output
ifeq ($(CONFIG_ICAM_ENABLE_REGRESSION_FLOW),y)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/ambalink/link_user
endif
ifeq ($(CONFIG_ICAM_PROJECT_CNNTESTBED), y)
EXTRA_INCVPATH += $(CT_SYS_SRC)/icam_cnntestbed/inc/entry
endif

ifeq ($(CONFIG_ICAM_CVBS_EXIST),y)
EXTRA_INCVPATH += $(srctree)/soc/io/src/common/inc/reg
endif


# lib path
EXTRA_LIBVPATH += $(srctree)/vendors/ambarella/lib64/$(AMBA_CHIP_ID)/qnx

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
