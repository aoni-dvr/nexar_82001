# icam core
obj-y += ../../../core/build/linux/

# icam app
obj-y += libsvc_icam.a
obj-y += amba_svc.lds

#
MAIN_O_PREFIX  = ../../src
ENTRY_O_PREFIX = $(MAIN_O_PREFIX)/entry/linux
CFG_O_PREFIX   = $(MAIN_O_PREFIX)/config
CMD_O_PREFIX   = $(MAIN_O_PREFIX)/cmd

MAIN_INC_PREFIX  = $(src)/$(MAIN_O_PREFIX)
ENTRY_INC_PREFIX = $(src)/$(ENTRY_O_PREFIX)
CFG_INC_PREFIX   = $(src)/$(CFG_O_PREFIX)
CORE_INC_PREFIX  = $(src)/../../../core/src
CTSYS_INC_PREFIX = $(src)/../../../ct_sys

SOC_CVFLOW_PREFIX = $(srctree)/soc/vision
SVC_CVFLOW_PREFIX = $(srctree)/svc/comsvc/cv

# include path
ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/vision
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/perif
ccflags-y += -I$(srctree)/vendors/ambarella/inc/linux
ccflags-y += -I$(srctree)/vendors/ambarella/inc/usb
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/display/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/sensor/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/yuvinput/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/$(AMBA_BSP_NAME)
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/
ccflags-y += -I$(srctree)/svc/comsvc/adas/inc
ccflags-y += -I$(srctree)/svc/comsvc/cfs/inc
ccflags-y += -I$(srctree)/svc/comsvc/codec/inc
ccflags-y += -I$(srctree)/svc/comsvc/exif/inc
ccflags-y += -I$(srctree)/svc/comsvc/flexidagio/inc
ccflags-y += -I$(srctree)/svc/comsvc/cv/cv_common/inc/cvapi
ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/CtUtils/inc
ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/CtSingleFD/inc
ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/CtSSD/inc
ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/CtDetCls/inc
ccflags-y += -I$(srctree)/svc/comsvc/dcf/inc
ccflags-y += -I$(srctree)/svc/comsvc/imgcal
ccflags-y += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/aaa
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/iqparam
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/wbcalib
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/eis/inc
ccflags-y += -I$(srctree)/svc/comsvc/net/app
ccflags-y += -I$(srctree)/svc/comsvc/net/inc
ccflags-y += -I$(srctree)/svc/comsvc/net/rtsp/inc
ccflags-y += -I$(srctree)/soc/vision/cv_common/inc
ccflags-y += -I$(srctree)/soc/vision/cv_common/inc/cvapi
ccflags-y += -I$(srctree)/soc/io/src/common/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg
ccflags-y += -I$(O)
ccflags-y += -I$(CORE_INC_PREFIX)/config/inc
ccflags-y += -I$(CORE_INC_PREFIX)/cv/inc
ccflags-y += -I$(CORE_INC_PREFIX)/cv/cvflow/inc
ccflags-y += -I$(CORE_INC_PREFIX)/cv/transfer/inc
ccflags-y += -I$(CORE_INC_PREFIX)/driver/inc
ccflags-y += -I$(CORE_INC_PREFIX)/dsp/inc
ccflags-y += -I$(CORE_INC_PREFIX)/img/inc
ccflags-y += -I$(CORE_INC_PREFIX)/draw/inc
ccflags-y += -I$(CORE_INC_PREFIX)/playback/inc
ccflags-y += -I$(CORE_INC_PREFIX)/record/inc
ccflags-y += -I$(CORE_INC_PREFIX)/sys/inc
ccflags-y += -I$(CFG_INC_PREFIX)/inc
ccflags-y += -I$(MAIN_INC_PREFIX)/inc

#ccflags-$(CONFIG_ICAM_IMGCAL_USED) += -I$(srctree)/svc/comsvc/imgcal
#ccflags-$(CONFIG_ICAM_IMGCAL_USED) += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-$(CONFIG_ICAM_IMGCAL_USED) += -I$(CORE_INC_PREFIX)/cal/inc
ccflags-$(CONFIG_ICAM_EEPROM_USED) += -I$(CORE_INC_PREFIX)/cal/module/inc
ccflags-$(CONFIG_ICAM_EEPROM_USED) += -I$(srctree)/svc/comsvc/camera_module/inc
ccflags-$(CONFIG_BUILD_COMMON_TUNE_CTUNER) += -I$(srctree)/svc/comsvc/imgcal
ccflags-$(CONFIG_BUILD_COMMON_TUNE_CTUNER) += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-$(CONFIG_BUILD_COMMON_TUNE_CTUNER) += -I$(srctree)/svc/comsvc/tune/ctuner
ccflags-$(CONFIG_BUILD_COMMON_TUNE_CTUNER) += -I$(srctree)/svc/comsvc/tune/ctuner/inc/common
ccflags-$(CONFIG_ICAM_IMGITUNER_USED) += -I$(srctree)/svc/comsvc/tune/ituner
ccflags-$(CONFIG_ICAM_AUDIO_USED) += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/audio/inc
ccflags-$(CONFIG_BUILD_CV) += -I$(SVC_CVFLOW_PREFIX)/cv_common/inc/cvapi
ccflags-$(CONFIG_BUILD_CV) += -I$(SVC_CVFLOW_PREFIX)/cv_common/inc/svccvalgo
ccflags-$(CONFIG_BUILD_CV) += -I$(SVC_CVFLOW_PREFIX)/cv_common/inc/svccvalgo/linux
ccflags-$(CONFIG_BUILD_CV) += -I$(SOC_CVFLOW_PREFIX)/cv_common/inc
ccflags-$(CONFIG_BUILD_CV) += -I$(SOC_CVFLOW_PREFIX)/cv_common/inc/cvapi
ccflags-$(CONFIG_BUILD_CV) += -I$(srctree)/vendors/ambarella/inc/vision
ccflags-$(CONFIG_BUILD_CV) += -I$(CORE_INC_PREFIX)/cv/inc
ccflags-$(CONFIG_BUILD_CV) += -I$(CORE_INC_PREFIX)/cv/transfer/inc
ccflags-$(CONFIG_BUILD_CV) += -I$(CORE_INC_PREFIX)/cv/cvflow/inc
ccflags-$(CONFIG_BUILD_CV) += -I$(srctree)/svc/comsvc/adas/refflow/inc/psd
ccflags-$(CONFIG_BUILD_CV) += -I$(srctree)/svc/comsvc/adas/refflow/inc/rcta
ccflags-$(CONFIG_BUILD_CV) += -I$(srctree)/svc/comsvc/adas/refflow/inc
ccflags-$(CONFIG_BUILD_CV) += -I$(srctree)/svc/comsvc/adas/utils/inc
ccflags-$(CONFIG_ICAM_ENABLE_REGRESSION_FLOW) += -I$(srctree)/svc/comsvc/ambalink/link_user
ccflags-$(CONFIG_BUILD_AMBA_ADAS) += -I$(srctree)/svc/comsvc/imgcal
ccflags-$(CONFIG_BUILD_AMBA_ADAS) += -I$(srctree)/svc/comsvc/imgcal/arch
#ccflags-$(CONFIG_BUILD_AMBA_ADAS) += -I$(srctree)/svc/comsvc/adas/inc
ccflags-$(CONFIG_BUILD_AMBA_ADAS) += -I$(srctree)/svc/comsvc/adas/refflow/inc
ccflags-$(CONFIG_BUILD_AMBA_ADAS) += -I$(srctree)/svc/comsvc/adas/refflow/inc/bsd
ccflags-$(CONFIG_BUILD_AMBA_ADAS) += -I$(srctree)/svc/comsvc/adas/refflow/inc/fc
ccflags-$(CONFIG_BUILD_AMBA_ADAS) += -I$(srctree)/svc/comsvc/adas/refflow/inc/ld
ccflags-$(CONFIG_BUILD_AMBA_ADAS) += -I$(srctree)/svc/comsvc/adas/refflow/inc/autocal
ccflags-$(CONFIG_BUILD_AMBA_ADAS) += -I$(srctree)/svc/comsvc/adas/refflow/inc/rmg
ccflags-$(CONFIG_BUILD_AMBA_ADAS) += -I$(srctree)/svc/comsvc/adas/refflow/inc/sr
ccflags-$(CONFIG_BUILD_AMBA_ADAS) += -I$(CORE_INC_PREFIX)/adas/inc
ccflags-$(CONFIG_ICAM_CV_STEREO) += -I$(srctree)/svc/comsvc/imgcal
ccflags-$(CONFIG_ICAM_CV_STEREO) += -I$(srctree)/svc/comsvc/stereoutility
ccflags-$(CONFIG_ICAM_CV_LOGGER) += -I$(srctree)/svc/comsvc/imgcal
ccflags-$(CONFIG_ICAM_CV_LOGGER) += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-$(CONFIG_ICAM_CV_LOGGER) += -I$(srctree)/svc/comsvc/adas/inc
ccflags-$(CONFIG_ICAM_CV_LOGGER) += -I$(srctree)/svc/comsvc/adas/refflow/inc
ccflags-$(CONFIG_ICAM_CV_LOGGER) += -I$(srctree)/svc/comsvc/adas/refflow/inc/bsd
ccflags-$(CONFIG_ICAM_CV_LOGGER) += -I$(srctree)/svc/comsvc/adas/refflow/inc/fc
ccflags-$(CONFIG_ICAM_PROJECT_SURROUND) += -I$(srctree)/svc/comsvc/anim/inc
ccflags-$(CONFIG_ICAM_PROJECT_SURROUND) += -I$(srctree)/svc/comsvc/tune/ctuner
ccflags-$(CONFIG_ICAM_PROJECT_SURROUND) += -I$(srctree)/svc/comsvc/tune/ctuner/inc
ccflags-$(CONFIG_ICAM_PROJECT_SURROUND) += -I$(srctree)/vendors/foss/lz4
ccflags-$(CONFIG_ICAM_BUILD_ENET) += -I$(srctree)/svc/comsvc/net/app/avdecc/inc
ccflags-$(CONFIG_ICAM_BUILD_ENET) += -I$(srctree)/svc/comsvc/net/app/avdecc
ccflags-$(CONFIG_BUILD_MONFRW_GRAPH) += -I$(CORE_INC_PREFIX)/safe/inc
ccflags-$(CONFIG_BUILD_MONFRW_GRAPH) += -I$(srctree)/svc/comsvc/monfrw/main/inc
ccflags-$(CONFIG_ENABLE_AMBALINK) += -I$(srctree)/vendors/foss/libwrapc
ccflags-$(CONFIG_ENABLE_AMBALINK) += -I$(srctree)/soc/io/src/common/inc/
ccflags-$(CONFIG_ENABLE_AMBALINK) += -I$(srctree)/soc/io/src/common/inc/arm
ccflags-$(CONFIG_ENABLE_AMBALINK) += -I$(srctree)/svc/comsvc/ambalink/core/inc
ccflags-$(CONFIG_ENABLE_AMBALINK) += -I$(srctree)/svc/comsvc/ambalink/inc
ccflags-$(CONFIG_ENABLE_AMBALINK) += -I$(srctree)/svc/comsvc/ambalink/inc/rpcprog
ccflags-$(CONFIG_BUILD_FOSS_LVGL) += -I$(srctree)/vendors/foss/lvgl/src/lvgl/src
ccflags-$(CONFIG_BUILD_COMSVC_DRAMSHMOO) += -I$(srctree)/svc/comsvc/dramshmoo/inc

# include objects
libsvc_icam-objs := $(ENTRY_O_PREFIX)/SvcSysEntry.o
libsvc_icam-objs += $(ENTRY_O_PREFIX)/SvcInitTask.o
libsvc_icam-objs += $(ENTRY_O_PREFIX)/iCamBufPool.o
libsvc_icam-objs += $(ENTRY_O_PREFIX)/iCamRecovery.o
ifeq ($(CONFIG_ICAM_TIMING_LOG),y)
libsvc_icam-objs += $(ENTRY_O_PREFIX)/iCamTimeProfile.o
endif
libsvc_icam-objs += $(CMD_O_PREFIX)/SvcCmdApp.o
libsvc_icam-objs += $(CMD_O_PREFIX)/SvcCmdSys.o
libsvc_icam-objs += $(CMD_O_PREFIX)/SvcCmdDSP.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcSysStatTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcControlTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcVinCtrlTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcVinBufMonitorTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcVoutCtrlTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcLiveviewTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcDisplayTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcDspTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcDataLoader.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcFwUpdateTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcOsdTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcGuiTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcMenuTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcButtonCtrlTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcStgMonitorTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcCanSpeedTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcColorBalance.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcLogoDraw.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcNvmTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcClkSwTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcSysInfoTask.o
libsvc_icam-objs += $(CFG_O_PREFIX)/SvcResCfgTask.o
libsvc_icam-objs += $(CFG_O_PREFIX)/SvcBufMap.o
libsvc_icam-objs += $(CFG_O_PREFIX)/SvcIKCfg.o
libsvc_icam-objs += $(CFG_O_PREFIX)/SvcInfoPack.o
libsvc_icam-objs += $(CFG_O_PREFIX)/SvcUserPref.o

ifeq ($(CONFIG_ICAM_VIEWCTRL_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcEmrTask.o
endif

ifeq ($(CONFIG_ICAM_IMGCAL_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcCalibTask.o
libsvc_icam-objs += $(CFG_O_PREFIX)/SvcCalibCfg.o
ifeq ($(CONFIG_ICAM_IMGCAL_STITCH_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcCalibStitchLdc.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcCalibStitchBlend.o
endif
ifeq ($(CONFIG_ICAM_EEPROM_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcCalibModuleTask.o
libsvc_icam-objs += $(CFG_O_PREFIX)/SvcCalibModuleCfg.o
endif
endif

ifeq ($(CONFIG_ICAM_IMGRAWCAP_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcRawCapTask.o
endif

ifeq ($(CONFIG_ICAM_AUDIO_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcAudioTask.o
libsvc_icam-objs += $(CFG_O_PREFIX)/SvcAudInfoPack.o
libsvc_icam-objs += $(CMD_O_PREFIX)/SvcCmdAudio.o
endif

ifeq ($(CONFIG_ICAM_RECORD_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcRecTask.o
libsvc_icam-objs += $(CFG_O_PREFIX)/SvcRecInfoPack.o
libsvc_icam-objs += $(CMD_O_PREFIX)/SvcCmdRec.o
ifeq ($(CONFIG_ICAM_RAWENC_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcRawEncTask.o
endif
endif

ifeq ($(CONFIG_ICAM_PLAYBACK_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcPlaybackTask.o
libsvc_icam-objs += $(CFG_O_PREFIX)/SvcPbkInfoPack.o
libsvc_icam-objs += $(CMD_O_PREFIX)/SvcCmdPbk.o
endif

ifeq ($(CONFIG_ICAM_STLCAP_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcStillTask.o
endif

ifeq ($(CONFIG_ICAM_PROJECT_ADAS_DVR),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcControlTask_AdasDvr.o
endif

ifeq ($(CONFIG_ICAM_PROJECT_EMIRROR),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcControlTask_Emr.o
endif

ifeq ($(CONFIG_ICAM_PROJECT_DMS), y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcControlTask_Dms.o
endif

ifeq ($(CONFIG_ICAM_PROJECT_SURROUND), y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcControlTask_Surround.o
endif

ifeq ($(CONFIG_ICAM_PROJECT_CNNTESTBED), y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcControlTask_CnnTestbed.o
endif

ifeq ($(CONFIG_ICAM_ENABLE_VOUT_DEF_IMG), y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcVoutDefImgTask.o
endif

ifeq ($(CONFIG_BUILD_CV), y)
libsvc_icam-objs += $(CMD_O_PREFIX)/SvcCmdApp_CV.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcCvMainTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcCvCtrlTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcCvFileInTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcODDrawTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcSegDrawTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcVoutFrmCtrlTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcWarningIconTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcExtOsdDrawTask.o
ifeq ($(CONFIG_ICAM_ENABLE_REGRESSION_FLOW), y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcRegressionTask.o
endif
ifeq ($(CONFIG_BUILD_AMBA_ADAS),y)
ifeq ($(CONFIG_ICAM_PROJECT_ADAS_DVR),y)
# adas_dvr
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcAdasAutoCal.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcLdwsTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcFcwsFcmdTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcAdasNotify.o
endif
ifeq ($(CONFIG_ICAM_PROJECT_EMIRROR),y)
# emr_bsd
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcBsdTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcRmgTask.o
endif
ifeq ($(CONFIG_ICAM_IMGCAL_STITCH_USED),y)
# emr_ema
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcEmrAdaptiveTask.o
endif
endif
ifeq ($(CONFIG_ICAM_CV_STEREO),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcStereoTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcCalibStereo.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcWarp.o
endif
ifeq ($(CONFIG_ICAM_CV_STIXEL),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcStixelTask.o
endif
ifeq ($(CONFIG_ICAM_CV_LOGGER),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcCvLoggerTask.o
endif
endif

ifeq ($(CONFIG_ICAM_PROJECT_SURROUND),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcAnim.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcAnimTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcAnimCalib.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcAnimMemPool.o

ifeq ($(CONFIG_BUILD_CV),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcPsdTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcOwsTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcRctaTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcSurTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcSurDrawTask.o
endif
endif

# dirt detection
ifeq ($(CONFIG_BUILD_CV),y)
ifeq ($(CONFIG_ICAM_USE_AMBA_DIRTDETECT),y)
ifneq ($(wildcard $(srctree)/vendors/ambarella/inc/adas/AmbaDirtDetect.h),)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/adas

libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcDirtDetectTask.o
endif
endif
endif

# defog
ifeq ($(CONFIG_ICAM_USE_AMBA_DEFOG),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcDefogTask.o
endif

ifeq ($(CONFIG_BUILD_IMGFRW_AAA),y)
# include image objects
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcImgTask.o
endif

ifeq ($(CONFIG_BUILD_MONFRW_GRAPH),y)
# include safe objects
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcSafeTask.o
ifeq ($(CONFIG_SVC_VOUT_ERR_CTRL_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcVoutErrTask.o
endif
endif

ifeq ($(CONFIG_SVC_VIN_ERR_CTRL_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcVinErrTask.o
endif

ifeq ($(CONFIG_AMBALINK_BOOT_OS),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcRpmsgApp.o
endif

ifeq ($(CONFIG_ICAM_LV_FEED_EXT_DATA_SUPPORTED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcLvFeedExtRawTask.o
endif

ifeq ($(CONFIG_ICAM_LV_FEED_EXT_YUV_SUPPORTED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcLvFeedExtYuvTask.o
endif

# cnn_testbed entry
ifeq ($(CONFIG_ICAM_PROJECT_CNNTESTBED), y)
include $(CTSYS_INC_PREFIX)/icam_cnntestbed/build/linux/Makefile
endif

install-files := libsvc_icam.a
