################################################################
#                             QNX                              #
################################################################
include $(objtree)/include/config/auto.conf

VIS_OS_PATH := qnx
export VIS_OS_PATH

VIS_MOD_ROOT := $(CURDIR)/../../
export VIS_MOD_ROOT

ifdef CONFIG_SOC_CV2FS
VIS_FAMILY_PATH := fs
else
VIS_FAMILY_PATH := auto
endif
export VIS_FAMILY_PATH

EXTRA_SRCVPATH += $(VIS_MOD_ROOT)/src/common/$(VIS_OS_PATH)
EXTRA_SRCVPATH += $(VIS_MOD_ROOT)/src/HL/$(VIS_FAMILY_PATH)
EXTRA_SRCVPATH += $(VIS_MOD_ROOT)/src/HL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID)
EXTRA_SRCVPATH += $(VIS_MOD_ROOT)/src/LL/$(VIS_FAMILY_PATH)
EXTRA_SRCVPATH += $(VIS_MOD_ROOT)/src/LL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID)
EXTRA_SRCVPATH += $(VIS_MOD_ROOT)/src/similar/$(VIS_FAMILY_PATH)
EXTRA_SRCVPATH += $(VIS_MOD_ROOT)/src/similar/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID)
EXTRA_SRCVPATH += $(VIS_MOD_ROOT)/diag/
EXTRA_SRCVPATH += $(VIS_MOD_ROOT)/monitor/

# src/common
SRCS := ambadsp_dev.c
SRCS += ambadsp_ioctl.c
SRCS += dsp_aaa_stat.c
SRCS += dsp_osal.c

# src/HL
SRCS += AmbaDSP_ApiCheck.c
SRCS += AmbaDSP_ApiDump.c
SRCS += AmbaDSP_ApiResource.c
SRCS += AmbaDSP_ApiTiming.c
SRCS += AmbaDSP_BaseCfg.c
SRCS += AmbaDSP_Cmds.c
SRCS += AmbaDSP_ContextUtility.c
SRCS += AmbaDSP_DecodeUtility.c
SRCS += AmbaDSP_EncodeAPI.c
SRCS += AmbaDSP_EncodeUtility.c
SRCS += AmbaDSP_Event.c
SRCS += AmbaDSP_IntChk.c
SRCS += AmbaDSP_Liveview.c
SRCS += AmbaDSP_Main.c
SRCS += AmbaDSP_StillCapture.c
SRCS += AmbaDSP_StillDec.c
SRCS += AmbaDSP_VideoDec.c
SRCS += AmbaDSP_VideoEnc.c
SRCS += AmbaDSP_VOUT.c
ifeq ($(VIS_FAMILY_PATH),auto)
SRCS += AmbaDSP_TimeManager.c
else ifeq ($(VIS_FAMILY_PATH),fs)
SRCS += AmbaDSP_TimeManager.c
else ifeq ($(VIS_FAMILY_PATH),cv5x)
SRCS += AmbaDSP_TimeManager.c
endif

# src/HL/chip
SRCS += AmbaDSP_EncodeAPI_Priv.c
SRCS += AmbaDSP_Liveview_Priv.c
SRCS += AmbaDSP_Main_Priv.c

# src/LL
SRCS += AmbaDSP_ArmComm.c
SRCS += AmbaDSP_Buffers.c
SRCS += AmbaDSP_CmdMsgDump.c
SRCS += AmbaDSP_CmdsSend.c
SRCS += AmbaDSP_CommonAPI.c
SRCS += AmbaDSP_EventCtrl.c
SRCS += AmbaDSP_Log.c
SRCS += AmbaDSP_MsgDispatcher.c

# src/LL/chip
SRCS += AmbaDSP_ArmComm_Priv.c
SRCS += AmbaDSP_Buffers_Priv.c
SRCS += AmbaDSP_CmdMsgDump_Priv.c
SRCS += AmbaDSP_Int.c

ifdef CONFIG_SOC_CV2FS
SRCS += AmbaDSPSimilar_EncodeUtility.c
SRCS += AmbaDSPSimilar_EncodeAPI.c
SRCS += AmbaDSPSimilar_Context.c
SRCS += AmbaDSPSimilar_DecodeUtility.c
SRCS += AmbaDSPSimilar_EncodeAPI_Priv.c

SRCS += ucode_error.c
endif

# diag
ifeq ($(CONFIG_ENABLE_DSP_DIAG),y)
SRCS += AmbaDSP_Diag.c
endif

ifdef CONFIG_ENABLE_DSP_MONITOR
SRCS += AmbaDSP_Monitor.c
endif

EXTRA_INCVPATH += $(VIS_MOD_ROOT)/../../osal/
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/../../osal/qnx/
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/inc/common
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/inc/common/$(VIS_OS_PATH)
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/inc/HL/$(VIS_FAMILY_PATH)
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/inc/HL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/inc/LL/$(VIS_FAMILY_PATH)
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/inc/LL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/inc/similar/$(VIS_FAMILY_PATH)
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/../../../vendors/ambarella/inc/
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/../../../vendors/ambarella/inc/dsp/
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/../../../vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/../../../vendors/ambarella/inc/io/
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/../../../vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/../../../svc/comsvc/print/
EXTRA_INCVPATH += $(VIS_MOD_ROOT)/../../../svc/comsvc/misc


LIST=CPU
include recurse.mk

