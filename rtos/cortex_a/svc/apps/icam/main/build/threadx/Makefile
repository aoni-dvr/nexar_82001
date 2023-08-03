# icam core
obj-y += ../../../core/build/threadx/

# icam app
obj-y += libsvc_icam.a
obj-y += amba_svc.lds

#
MAIN_O_PREFIX  = ../../src
ENTRY_O_PREFIX = $(MAIN_O_PREFIX)/entry/threadx
CFG_O_PREFIX   = $(MAIN_O_PREFIX)/config
CMD_O_PREFIX   = $(MAIN_O_PREFIX)/cmd

MAIN_INC_PREFIX  = $(src)/$(MAIN_O_PREFIX)
ENTRY_INC_PREFIX = $(src)/$(ENTRY_O_PREFIX)
CFG_INC_PREFIX   = $(src)/$(CFG_O_PREFIX)
CORE_INC_PREFIX  = $(src)/../../../core/src
CTSYS_INC_PREFIX = $(src)/../../../ct_sys

# include path
ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else 
ifeq ($(CONFIG_SOC_CV52),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif
endif

ccflags-y := -I$(srctree)/vendors/ambarella/inc                             \
             -I$(srctree)/vendors/ambarella/inc/vision                      \
             -I$(srctree)/vendors/ambarella/inc/dsp                         \
             -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)     \
             -I$(srctree)/vendors/ambarella/inc/io                          \
             -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)          \
             -I$(srctree)/vendors/ambarella/inc/perif                       \
             -I$(srctree)/vendors/ambarella/inc/prfile2                     \
             -I$(srctree)/vendors/ambarella/inc/threadx                     \
             -I$(srctree)/vendors/ambarella/inc/usb                         \
             -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/display/inc       \
             -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/sensor/inc        \
             -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/yuvinput/inc      \
             -I$(srctree)/bsp/$(AMBA_CHIP_ID)/$(AMBA_BSP_NAME)              \
             -I$(srctree)/bsp/$(AMBA_CHIP_ID)/                              \
             -I$(srctree)/svc/comsvc/adas/inc                               \
             -I$(srctree)/svc/comsvc/cfs/inc                                \
             -I$(srctree)/svc/comsvc/codec/inc                              \
             -I$(srctree)/svc/comsvc/exif/inc                               \
             -I$(srctree)/svc/comsvc/cv/cv_common/inc/cvapi                 \
             -I$(srctree)/svc/comsvc/imgcal                                 \
             -I$(srctree)/svc/comsvc/imgcal/arch                            \
             -I$(srctree)/svc/comsvc/adas/inc                               \
             -I$(srctree)/svc/comsvc/cv/arm_framework/app/CtUtils/inc       \
             -I$(srctree)/svc/comsvc/cv/arm_framework/app/CtSingleFD/inc    \
             -I$(srctree)/svc/comsvc/cv/arm_framework/app/CtSSD/inc         \
             -I$(srctree)/svc/comsvc/cv/arm_framework/app/CtDetCls/inc      \
             -I$(srctree)/svc/comsvc/cv/arm_framework/app/CtSingleFDHeader/inc \
             -I$(srctree)/svc/comsvc/dcf/inc                                \
             -I$(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)            \
             -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/aaa                 \
             -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/iqparam             \
             -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/wbcalib             \
             -I$(srctree)/svc/comsvc/misc                                   \
             -I$(srctree)/svc/comsvc/print                                  \
             -I$(srctree)/svc/comsvc/shell/inc                              \
             -I$(srctree)/svc/comsvc/eis/inc                                \
             -I$(srctree)/svc/comsvc/net/app                                \
             -I$(srctree)/svc/comsvc/net/inc                                \
             -I$(srctree)/svc/comsvc/net/rtsp/inc                           \
             -I$(srctree)/svc/comsvc/usb/inc                                \
             -I$(srctree)/soc/vision/cv_common/inc                          \
             -I$(srctree)/soc/vision/cv_common/inc/cvapi                    \
             -I$(srctree)/soc/io/src/common/inc                             \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc                    \
             -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl               \
             -I$(srctree)/soc/io/src/common/inc/arm                         \
             -I$(O)                                                         \
             -I$(CORE_INC_PREFIX)/config/inc                                \
             -I$(CORE_INC_PREFIX)/cv/inc                                    \
             -I$(CORE_INC_PREFIX)/cv/cvflow/inc                             \
             -I$(CORE_INC_PREFIX)/cv/transfer/inc                           \
             -I$(CORE_INC_PREFIX)/driver/inc                                \
             -I$(CORE_INC_PREFIX)/dsp/inc                                   \
             -I$(CORE_INC_PREFIX)/img/inc                                   \
             -I$(CORE_INC_PREFIX)/draw/inc                                  \
             -I$(CORE_INC_PREFIX)/playback/inc                              \
             -I$(CORE_INC_PREFIX)/record/inc                                \
             -I$(CORE_INC_PREFIX)/sys/inc                                   \
             -I$(CFG_INC_PREFIX)/inc                                        \
             -I$(MAIN_INC_PREFIX)/inc

ifneq ($(wildcard $(srctree)/bsp/$(AMBA_CHIP_ID)/bridge/inc),)
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/bridge/inc
endif

# include objects
libsvc_icam-objs := $(ENTRY_O_PREFIX)/SvcSysEntry.o            \
                    $(ENTRY_O_PREFIX)/SvcInitTask.o            \
                    $(ENTRY_O_PREFIX)/iCamSD.o                 \
                    $(ENTRY_O_PREFIX)/iCamBufPool.o            \
                    $(CMD_O_PREFIX)/SvcCmdApp.o                \
                    $(CMD_O_PREFIX)/SvcCmdSys.o                \
                    $(CMD_O_PREFIX)/SvcCmdDSP.o                \
                    $(MAIN_O_PREFIX)/SvcSysStatTask.o          \
                    $(MAIN_O_PREFIX)/SvcControlTask.o          \
                    $(MAIN_O_PREFIX)/SvcVinCtrlTask.o          \
                    $(MAIN_O_PREFIX)/SvcVinBufMonitorTask.o    \
                    $(MAIN_O_PREFIX)/SvcVoutCtrlTask.o         \
                    $(MAIN_O_PREFIX)/SvcLiveviewTask.o         \
                    $(MAIN_O_PREFIX)/SvcDisplayTask.o          \
                    $(MAIN_O_PREFIX)/SvcDspTask.o              \
                    $(MAIN_O_PREFIX)/SvcDataLoader.o           \
                    $(MAIN_O_PREFIX)/SvcOsdTask.o              \
                    $(MAIN_O_PREFIX)/SvcGuiTask.o              \
                    $(MAIN_O_PREFIX)/SvcMenuTask.o             \
                    $(MAIN_O_PREFIX)/SvcButtonCtrlTask.o       \
                    $(MAIN_O_PREFIX)/SvcStgMonitorTask.o       \
                    $(MAIN_O_PREFIX)/SvcCanSpeedTask.o         \
                    $(MAIN_O_PREFIX)/SvcColorBalance.o         \
                    $(MAIN_O_PREFIX)/SvcLogoDraw.o             \
                    $(MAIN_O_PREFIX)/SvcNvmTask.o              \
                    $(MAIN_O_PREFIX)/SvcClkSwTask.o            \
                    $(MAIN_O_PREFIX)/SvcSysInfoTask.o          \
                    $(CFG_O_PREFIX)/SvcResCfgTask.o            \
                    $(CFG_O_PREFIX)/SvcBufMap.o                \
                    $(CFG_O_PREFIX)/SvcIKCfg.o                 \
                    $(CFG_O_PREFIX)/SvcInfoPack.o              \
                    $(CFG_O_PREFIX)/SvcUserPref.o              \
		    $(CFG_O_PREFIX)/SvcUserPrefCustom.o

ifeq ($(CONFIG_ICAM_FW_UPDATER_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcFwUpdateTask.o
endif
ifeq ($(CONFIG_ICAM_TIMING_LOG),y)
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg

libsvc_icam-objs += $(ENTRY_O_PREFIX)/iCamTimeProfile.o
endif

ifeq ($(CONFIG_THREADX64),y)
libsvc_icam-objs += $(ENTRY_O_PREFIX)/SvcExcept64.o
else
libsvc_icam-objs += $(ENTRY_O_PREFIX)/SvcExcept.o
endif

ifeq ($(CONFIG_ICAM_VIEWCTRL_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcEmrTask.o
endif

ifeq ($(CONFIG_ICAM_IMGCAL_USED),y)

ccflags-y += -I$(srctree)/svc/comsvc/imgcal      \
             -I$(srctree)/svc/comsvc/imgcal/arch \
             -I$(CORE_INC_PREFIX)/cal/inc

ifeq ($(CONFIG_ICAM_IMGCAL_STITCH_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcCalibStitchLdc.o     \
                    $(MAIN_O_PREFIX)/SvcCalibStitchBlend.o
endif

libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcCalibTask.o \
                    $(CFG_O_PREFIX)/SvcCalibCfg.o

ifeq ($(CONFIG_ICAM_EEPROM_USED),y)
ccflags-y += -I$(CORE_INC_PREFIX)/cal/module/inc
ccflags-y += -I$(srctree)/svc/comsvc/camera_module/inc

libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcCalibModuleTask.o \
                    $(CFG_O_PREFIX)/SvcCalibModuleCfg.o

endif

endif

ifeq ($(CONFIG_ICAM_IMGRAWCAP_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcRawCapTask.o
endif

ifeq ($(CONFIG_ICAM_IMGITUNER_USED),y)
ccflags-y += -I$(srctree)/svc/comsvc/tune/ituner
endif

ifeq ($(CONFIG_BUILD_COMMON_TUNE_CTUNER),y)
ccflags-y += -I$(srctree)/svc/comsvc/imgcal         \
             -I$(srctree)/svc/comsvc/imgcal/arch    \
             -I$(srctree)/svc/comsvc/tune/ctuner    \
             -I$(srctree)/svc/comsvc/tune/ctuner/inc/common
endif

ifeq ($(CONFIG_ICAM_AUDIO_USED),y)
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/audio/inc

libsvc_icam-objs += $(ENTRY_O_PREFIX)/iCamAudio.o       \
                    $(MAIN_O_PREFIX)/SvcAudioTask.o     \
                    $(CFG_O_PREFIX)/SvcAudInfoPack.o    \
                    $(CMD_O_PREFIX)/SvcCmdAudio.o
endif

ifeq ($(CONFIG_ICAM_RECORD_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcRecTask.o        \
                    $(CFG_O_PREFIX)/SvcRecInfoPack.o     \
                    $(CMD_O_PREFIX)/SvcCmdRec.o

ifeq ($(CONFIG_ICAM_RAWENC_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcRawEncTask.o
endif
endif

ifeq ($(CONFIG_ICAM_PLAYBACK_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcPlaybackTask.o   \
                    $(CFG_O_PREFIX)/SvcPbkInfoPack.o     \
                    $(CMD_O_PREFIX)/SvcCmdPbk.o
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

ifeq ($(CONFIG_ICAM_PROJECT_SHMOO), y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcControlTask_Shmoo.o
endif

ifeq ($(CONFIG_ICAM_ENABLE_VOUT_DEF_IMG), y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcVoutDefImgTask.o
endif

ifeq ($(CONFIG_BUILD_CV), y)
SOC_CVFLOW_PREFIX = $(srctree)/soc/vision
SVC_CVFLOW_PREFIX = $(srctree)/svc/comsvc/cv

ifeq ($(CONFIG_ICAM_ENABLE_REGRESSION_FLOW), y)
ccflags-y += -I$(srctree)/svc/comsvc/ambalink/link_user

libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcRegressionTask.o
endif

ccflags-y += -I$(SVC_CVFLOW_PREFIX)/cv_common/inc/cvapi/               \
             -I$(SVC_CVFLOW_PREFIX)/cv_common/inc/svccvalgo            \
             -I$(SVC_CVFLOW_PREFIX)/cv_common/inc/svccvalgo/threadx    \
             -I$(SOC_CVFLOW_PREFIX)/cv_common/inc                      \
             -I$(SOC_CVFLOW_PREFIX)/cv_common/inc/cvapi                \
             -I$(srctree)/vendors/ambarella/inc/vision                 \
             -I$(CORE_INC_PREFIX)/cv/inc                               \
             -I$(CORE_INC_PREFIX)/cv/transfer/inc                      \
             -I$(CORE_INC_PREFIX)/cv/cvflow/inc

libsvc_icam-objs += $(CMD_O_PREFIX)/SvcCmdApp_CV.o           \
                    $(MAIN_O_PREFIX)/SvcCvMainTask.o         \
                    $(MAIN_O_PREFIX)/SvcCvCtrlTask.o         \
                    $(MAIN_O_PREFIX)/SvcCvFileInTask.o       \
                    $(MAIN_O_PREFIX)/SvcVoutFrmCtrlTask.o    \
                    $(MAIN_O_PREFIX)/SvcWarningIconTask.o    \
                    $(MAIN_O_PREFIX)/SvcExtOsdDrawTask.o

ifeq ($(CONFIG_ICAM_REBEL_CUSTOMER_NN),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcODDrawTask_Rebel.o  \
                    $(MAIN_O_PREFIX)/SvcSegDrawTask_Rebel.o
else
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcODDrawTask.o        \
                    $(MAIN_O_PREFIX)/SvcSegDrawTask.o
endif


ifeq ($(CONFIG_ICAM_REBEL_USAGE),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcRebelGUI.o
endif

ifeq ($(CONFIG_BUILD_AMBA_ADAS),y)
ccflags-y += -I$(srctree)/svc/comsvc/imgcal                   \
             -I$(srctree)/svc/comsvc/imgcal/arch              \
             -I$(srctree)/svc/comsvc/adas/inc                 \
             -I$(srctree)/svc/comsvc/adas/refflow/inc         \
             -I$(srctree)/svc/comsvc/adas/refflow/inc/bsd     \
             -I$(srctree)/svc/comsvc/adas/refflow/inc/fc      \
             -I$(srctree)/svc/comsvc/adas/refflow/inc/ld      \
             -I$(srctree)/svc/comsvc/adas/refflow/inc/autocal \
             -I$(srctree)/svc/comsvc/adas/refflow/inc/rmg     \
             -I$(srctree)/svc/comsvc/adas/refflow/inc/sr      \
             -I$(CORE_INC_PREFIX)/adas/inc

# adas_dvr
ifeq ($(CONFIG_ICAM_PROJECT_ADAS_DVR),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcAdasAutoCal.o    \
					$(MAIN_O_PREFIX)/SvcLdwsTask.o       \
					$(MAIN_O_PREFIX)/SvcLdwsTaskV2.o       \
                    $(MAIN_O_PREFIX)/SvcFcwsFcmdTask.o   \
                    $(MAIN_O_PREFIX)/SvcFcwsFcmdTaskV2.o \
                    $(MAIN_O_PREFIX)/SvcAdasNotify.o
endif
# emr_bsd
ifeq ($(CONFIG_ICAM_PROJECT_EMIRROR),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcBsdTask.o       \
                    $(MAIN_O_PREFIX)/SvcRmgTask.o
endif
# emr_ema
ifeq ($(CONFIG_ICAM_IMGCAL_STITCH_USED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcEmrAdaptiveTask.o
endif
endif

ifeq ($(CONFIG_ICAM_CV_STEREO),y)
ccflags-y += -I$(srctree)/svc/comsvc/imgcal             \
             -I$(srctree)/svc/comsvc/stereoutility

libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcStereoTask.o    \
                    $(MAIN_O_PREFIX)/SvcCalibStereo.o   \
                    $(MAIN_O_PREFIX)/SvcWarp.o

ifeq ($(CONFIG_ICAM_CV_STEREO_AUTO_CALIB),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcStereoACTask.o
endif

ifeq ($(CONFIG_ICAM_CV_LINUX_STEREO_APP),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcStereoACTask.o
endif
endif

ifeq ($(CONFIG_ICAM_CV_STIXEL),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcStixelTask.o
endif

ifeq ($(CONFIG_ICAM_CV_LINUX_STEREO_APP),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcStixelTask.o
endif

ifeq ($(CONFIG_ICAM_CV_FEX),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcFexTask.o
endif

ifeq ($(CONFIG_ICAM_CV_LOGGER),y)
ccflags-y += -I$(srctree)/svc/comsvc/imgcal                  \
             -I$(srctree)/svc/comsvc/imgcal/arch             \
             -I$(srctree)/svc/comsvc/adas/inc                \
             -I$(srctree)/svc/comsvc/adas/refflow/inc        \
             -I$(srctree)/svc/comsvc/adas/refflow/inc/bsd    \
             -I$(srctree)/svc/comsvc/adas/refflow/inc/fc

libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcCvLoggerTask.o
endif

endif

ifeq ($(CONFIG_ICAM_PROJECT_SURROUND),y)
ccflags-y += -I$(srctree)/svc/comsvc/anim/inc           \
             -I$(srctree)/svc/comsvc/tune/ctuner        \
			 -I$(srctree)/svc/comsvc/tune/ctuner/inc    \
             -I$(srctree)/vendors/foss/lz4

libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcAnim.o          \
                    $(MAIN_O_PREFIX)/SvcAnimTask.o      \
                    $(MAIN_O_PREFIX)/SvcAnimCalib.o     \
                    $(MAIN_O_PREFIX)/SvcAnimMemPool.o
ifeq ($(CONFIG_BUILD_CV),y)
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/psd   \
             -I$(srctree)/svc/comsvc/adas/refflow/inc/rcta  \
             -I$(srctree)/svc/comsvc/adas/refflow/inc       \
             -I$(srctree)/svc/comsvc/adas/utils/inc         \

libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcPsdTask.o  \
                    $(MAIN_O_PREFIX)/SvcOwsTask.o  \
                    $(MAIN_O_PREFIX)/SvcRctaTask.o \
                    $(MAIN_O_PREFIX)/SvcSurTask.o  \
                    $(MAIN_O_PREFIX)/SvcSurDrawTask.o
endif
ifeq ($(CONFIG_ICAM_BUILD_ENET),y)
ccflags-y += -I$(srctree)/svc/comsvc/net/app/avdecc/inc \
             -I$(srctree)/svc/comsvc/net/app/avdecc
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

ifdef CONFIG_BUILD_IMGFRW_AAA
# include image objects
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcImgTask.o
endif

ifdef CONFIG_BUILD_MONFRW_GRAPH
# include safe objects
ccflags-y += -I$(CORE_INC_PREFIX)/safe/inc
ccflags-y += -I$(srctree)/svc/comsvc/monfrw/main/inc
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcSafeTask.o
ifdef CONFIG_SVC_VOUT_ERR_CTRL_USED
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcVoutErrTask.o
endif
endif
ifdef CONFIG_SVC_VIN_ERR_CTRL_USED
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcVinErrTask.o
endif

ifdef CONFIG_AMBALINK_BOOT_OS
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcRpmsgApp.o
endif

ifdef CONFIG_ICAM_LV_FEED_EXT_DATA_SUPPORTED
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcLvFeedExtRawTask.o
endif

ifdef CONFIG_ICAM_LV_FEED_EXT_YUV_SUPPORTED
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcLvFeedExtYuvTask.o
endif

ifdef CONFIG_ENABLE_AMBALINK
ccflags-y += -I$(srctree)/vendors/foss/libwrapc
ccflags-y += -I$(srctree)/soc/io/src/common/inc/
ccflags-y += -I$(srctree)/soc/io/src/common/inc/arm
ccflags-y += -I$(srctree)/svc/comsvc/ambalink/core/inc
ccflags-y += -I$(srctree)/svc/comsvc/ambalink/inc
ccflags-y += -I$(srctree)/svc/comsvc/ambalink/inc/rpcprog
endif

ifdef CONFIG_BUILD_FOSS_LVGL
ccflags-y += -I$(srctree)/vendors/foss/lvgl/src/lvgl/src
endif

ifdef CONFIG_ICAM_SENSOR_ASIL_ENABLED
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcSensorCrcCheckTask.o
endif

# cnn_testbed entry
ifeq ($(CONFIG_ICAM_PROJECT_CNNTESTBED), y)
include $(CTSYS_INC_PREFIX)/icam_cnntestbed/build/threadx/Makefile
endif

ccflags-y += -I$(srctree)/svc/comsvc/dramshmoo/inc
ifdef CONFIG_ICAM_PROJECT_SHMOO
ifeq ($(CONFIG_BUILD_CV), y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcShmooCvTask.o
endif
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcAdvancedShmooTask.o
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcShmooRecTask.o
endif

ifeq ($(CONFIG_ICAM_YUVFILE_FEED),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcLvFeedFileYuvTask.o
endif

ifeq ($(CONFIG_ICAM_AVE_RAW_CAP),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcAveRawCapTask.o
endif

ifeq ($(CONFIG_ICAM_USE_DIAG_COMMANDS),y)
include $(src)/Makefile.diag
endif

ccflags-y += -I$(srctree)/svc/comsvc/dramshmoo/inc
ifeq ($(CONFIG_BUILD_COMSVC_DRAMSHMOO),y)
ifneq ($(findstring $(CONFIG_SOC_CV5)$(CONFIG_SOC_CV52), y y),)
ccflags-y += -I$(srctree)/svc/comsvc/dramshmoo/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg
endif
ccflags-y += -I$(srctree)/svc/comsvc/dramshmoo/common
endif


ifeq ($(CONFIG_ICAM_CVBS_ADC_FINE_TUNE),y)
libsvc_icam-objs += $(MAIN_O_PREFIX)/SvcNtscAdjust.o
ccflags-y += -I$(srctree)/soc/io/src/common/inc/reg
endif

ifdef CONFIG_APP_FLOW_CARDV_AONI
ifdef CONFIG_BUILD_FOSS_LIBJPEG_TURBO
ccflags-y += -I$(srctree)/vendors/foss/libjpeg-turbo
endif
ifdef CONFIG_BUILD_FOSS_WOLFSSL
ccflags-y += -I$(srctree)/vendors/foss/wolfssl
endif
ccflags-y += -I$(srctree)/vendors/ambarella/wrapper/fs/threadx/inc
ccflags-y += -I$(srctree)/vendors/ambarella/wrapper/usb/inc
ccflags-y += -I$(srctree)/svc/comsvc/usb/inc
ccflags-y += -I$(srctree)/svc/comsvc/usb/lib/src/common
ccflags-y += -I$(srctree)/svc/apps/icam/cardv
ccflags-y += -I$(srctree)/svc/apps/icam/cardv/cardv_modules
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/pmic/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/imu/inc
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/thermal/inc
ccflags-y += -I$(srctree)/svc/comsvc/codec/lib/src/common/dataflow/container/inc
ccflags-y += -I$(srctree)/svc/comsvc/codec/lib/src/common/dataflow/container/inc/format
ccflags-y += -I$(srctree)/svc/comsvc/codec/lib/src/common/dataflow/fifo/inc
#ccflags-y += -I$(srctree)/vendors/foss/libjpeg-turbo
ccflags-y += -I$(srctree)/svc/apps/icam/core/src/sys/inc
ccflags-y += -I$(srctree)/soc/io/src/common/inc/rtsl

CARDV_O_PREFIX  = ../../..
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/system/handler.o     			\
                   $(CARDV_O_PREFIX)/cardv/rec/rec_dvr_func.o     		\
                   $(CARDV_O_PREFIX)/cardv/user_setting/user_setting.o  	\
                   $(CARDV_O_PREFIX)/cardv/user_setting/ini_helper.o  	\
                   $(CARDV_O_PREFIX)/cardv/msg_queue/msg_queue.o			\
                   $(CARDV_O_PREFIX)/cardv/timer/timer.o					\
                   $(CARDV_O_PREFIX)/cardv/app_helper.o					\
                   $(CARDV_O_PREFIX)/cardv/system/t_app.o             	\
                   $(CARDV_O_PREFIX)/cardv/system/wifi.o              	\
                   $(CARDV_O_PREFIX)/cardv/system/metadata.o          	\
                   $(CARDV_O_PREFIX)/cardv/system/beep.o              	\
                   $(CARDV_O_PREFIX)/cardv/system/linux_api_service.o 	\
                   $(CARDV_O_PREFIX)/cardv/system/bt_stream_share.o     \
                   $(CARDV_O_PREFIX)/cardv/system/stream_share.o     \
                   $(CARDV_O_PREFIX)/cardv/system/rtos_log_share.o     \
                   $(CARDV_O_PREFIX)/cardv/system/sha256_helper.o      	\
                   $(CARDV_O_PREFIX)/cardv/system/key_manager.o      		\
                   $(CARDV_O_PREFIX)/cardv/system/async_task.o			\
                   $(CARDV_O_PREFIX)/cardv/system/offline_log.o			\
                   $(CARDV_O_PREFIX)/cardv/system/app_util.o			\
                   $(CARDV_O_PREFIX)/cardv/system/scan_file_info_task.o	\
                   $(CARDV_O_PREFIX)/cardv/record/imu_record.o			\
                   $(CARDV_O_PREFIX)/cardv/record/gnss_record.o			\
                   $(CARDV_O_PREFIX)/cardv/record/pcm_queue.o            	\
                   $(CARDV_O_PREFIX)/cardv/record/pcm_record.o             	\
                   $(CARDV_O_PREFIX)/cardv/record/aac_queue.o            	\
                   $(CARDV_O_PREFIX)/cardv/record/aac_record.o
ifdef CONFIG_ENABLE_IMU
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/peripheral_task/imu_task.o
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/peripheral_task/imu_queue.o
endif
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/peripheral_task/gnss_task.o
ifdef CONFIG_GNSS_PPS_TIME_SYNC_ENABLE
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/peripheral_task/gnss_pps_task.o
endif
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/peripheral_task/agnss_task.o
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/peripheral_task/ir_task.o
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/peripheral_task/button_task.o
ifdef CONFIG_BSP_CV25_NEXAR_D161
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/peripheral_task/battery_task.o
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/system/mcu_update.o
else
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/peripheral_task/battery_task_new.o
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/system/mcu_update_new.o
endif
ifdef CONFIG_BSP_CV25_NEXAR_D161
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/peripheral_task/usb_device_task.o
endif
ifdef CONFIG_BSP_CV25_NEXAR_D161V2
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/peripheral_task/usb_device_task.o
endif
ifdef CONFIG_APP_FLOW_CARDV_UVC
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/uvc/uvc.o
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/uvc/uvc_impl.o
libsvc_icam-objs += $(CARDV_O_PREFIX)/cardv/uvc/yuv_queue.o
endif
# cardv modules start
export CARDV_MODULES_ROOT_PATH=$(srctree)/svc/apps/icam/cardv/cardv_modules
export CARDV_MODULES_PREFIX_PATH=$(CARDV_O_PREFIX)/cardv/cardv_modules
include $(srctree)/svc/apps/icam/cardv/cardv_modules/Makefile.cardv_modules
ccflags-y += -I$(srctree)/svc/apps/icam/cardv/cardv_modules
libsvc_icam-objs += ${cardv_module_src}
# cardv modules end
endif

install-files := libsvc_icam.a
