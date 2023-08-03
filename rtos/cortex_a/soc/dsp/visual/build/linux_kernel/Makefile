# SPDX-License-Identifier: MIT

PWD         := $(shell pwd)

LINUXDIR	?= /lib/modules/$(shell uname -r)/build
ARCH		?= x86

ifneq ($(KERNELRELEASE),)

VIS_OS_PATH := linux
export VIS_OS_PATH

ifeq ($(AMBA_CHIP_ID),cv2fs)
VIS_FAMILY_PATH := fs
else
ifeq ($(AMBA_CHIP_ID),cv5x)
VIS_FAMILY_PATH := cv5x
else
VIS_FAMILY_PATH := auto
endif
endif
export VIS_FAMILY_PATH

ifeq ($(AMBA_CHIP_ID),cv2fs)
EXTRA_CFLAGS            += -DCONFIG_SOC_CV2FS
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
else
ifeq ($(AMBA_CHIP_ID),cv22)
EXTRA_CFLAGS            += -DCONFIG_SOC_CV22
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
else
ifeq ($(AMBA_CHIP_ID),cv2)
EXTRA_CFLAGS            += -DCONFIG_SOC_CV2
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
else
ifeq ($(AMBA_CHIP_ID),cv25)
EXTRA_CFLAGS            += -DCONFIG_SOC_CV25
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
else
ifeq ($(AMBA_CHIP_ID),cv28)
EXTRA_CFLAGS            += -DCONFIG_SOC_CV28
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
else
ifeq ($(AMBA_CHIP_ID),h32)
EXTRA_CFLAGS            += -DCONFIG_SOC_H32
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
else
ifeq ($(AMBA_CHIP_ID),cv5x)
EXTRA_CFLAGS            += -DCONFIG_SOC_CV5
AMBA_CHIP_ID_DSP:=cv5
endif
endif
endif
endif
endif
endif
endif

EXTRA_CFLAGS            += -DCONFIG_LINUX
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/osal
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/vendors/ambarella/inc
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/vendors/ambarella/inc/dsp
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/dsp/visual/inc/similar/fs
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/dsp/visual/inc/common
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/dsp/visual/inc/common/$(VIS_OS_PATH)
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/dsp/visual/inc/HL/$(VIS_FAMILY_PATH)
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/dsp/visual/inc/HL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/dsp/visual/inc/LL/$(VIS_FAMILY_PATH)
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/dsp/visual/inc/LL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)
ifeq ($(AMBA_CHIP_ID),cv2fs)
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/soc/dsp/visual/inc/similar/$(VIS_FAMILY_PATH)
endif

KBUILD_EXTRA_SYMBOLS = $(AMBARELLA_PKG_DIR)/soc/osal/build/linux_kernel/Module.symvers

obj-m 	+= ambadsp_sdk.o

# src/common
ambadsp_sdk-y := ../../src/common/$(VIS_OS_PATH)/ambadsp_dev.o
ambadsp_sdk-y += ../../src/common/$(VIS_OS_PATH)/dsp_osal.o
ambadsp_sdk-y += ../../src/common/$(VIS_OS_PATH)/dsp_aaa_stat.o
ambadsp_sdk-y += ../../src/common/$(VIS_OS_PATH)/ambadsp_ioctl.o

# src/HL
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_ApiCheck.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_ApiDump.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_ApiResource.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_ApiTiming.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_BaseCfg.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_Cmds.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_ContextUtility.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_DecodeUtility.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_EncodeAPI.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_EncodeUtility.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_Event.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_IntChk.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_Liveview.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_Main.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_StillCapture.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_StillDec.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_VideoDec.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_VideoEnc.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_VOUT.o
ifeq ($(VIS_FAMILY_PATH),auto)
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_TimeManager.o
else ifeq ($(VIS_FAMILY_PATH),fs)
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_TimeManager.o
else ifeq ($(VIS_FAMILY_PATH),cv5x)
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/AmbaDSP_TimeManager.o
endif

# src/HL/chip
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/AmbaDSP_EncodeAPI_Priv.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/AmbaDSP_Liveview_Priv.o
ambadsp_sdk-y += ../../src/HL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/AmbaDSP_Main_Priv.o

# src/LL
ambadsp_sdk-y += ../../src/LL/$(VIS_FAMILY_PATH)/AmbaDSP_ArmComm.o
ambadsp_sdk-y += ../../src/LL/$(VIS_FAMILY_PATH)/AmbaDSP_Buffers.o
ambadsp_sdk-y += ../../src/LL/$(VIS_FAMILY_PATH)/AmbaDSP_CmdMsgDump.o
ambadsp_sdk-y += ../../src/LL/$(VIS_FAMILY_PATH)/AmbaDSP_CmdsSend.o
ambadsp_sdk-y += ../../src/LL/$(VIS_FAMILY_PATH)/AmbaDSP_CommonAPI.o
ambadsp_sdk-y += ../../src/LL/$(VIS_FAMILY_PATH)/AmbaDSP_EventCtrl.o
ambadsp_sdk-y += ../../src/LL/$(VIS_FAMILY_PATH)/AmbaDSP_Log.o
ambadsp_sdk-y += ../../src/LL/$(VIS_FAMILY_PATH)/AmbaDSP_MsgDispatcher.o

# src/LL/chip
ambadsp_sdk-y += ../../src/LL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/AmbaDSP_ArmComm_Priv.o
ambadsp_sdk-y += ../../src/LL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/AmbaDSP_Buffers_Priv.o
ambadsp_sdk-y += ../../src/LL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/AmbaDSP_CmdMsgDump_Priv.o
ambadsp_sdk-y += ../../src/LL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/AmbaDSP_Int.o
ifeq ($(AMBA_CHIP_ID),cv2fs)
ambadsp_sdk-y += ../../src/LL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID)/ucode_error.o
else ifeq ($(VIS_FAMILY_PATH),cv5x)
ambadsp_sdk-y += ../../src/LL/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID_DSP)/ucode_error.o
endif

# src/similar
ifeq ($(AMBA_CHIP_ID),cv2fs)
ambadsp_sdk-y += ../../src/similar/$(VIS_FAMILY_PATH)/AmbaDSPSimilar_Context.o
ambadsp_sdk-y += ../../src/similar/$(VIS_FAMILY_PATH)/AmbaDSPSimilar_DecodeUtility.o
ambadsp_sdk-y += ../../src/similar/$(VIS_FAMILY_PATH)/AmbaDSPSimilar_EncodeAPI.o
ambadsp_sdk-y += ../../src/similar/$(VIS_FAMILY_PATH)/AmbaDSPSimilar_EncodeUtility.o
ambadsp_sdk-y += ../../src/similar/$(VIS_FAMILY_PATH)/$(AMBA_CHIP_ID)/AmbaDSPSimilar_EncodeAPI_Priv.o
endif

# diag
ifeq ($(CONFIG_ENABLE_DSP_DIAG),y)
ambadsp_sdk-y += ../../diag/AmbaDSP_Diag.o
endif

else # No $(KERNELRELEASE)

all:
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) modules_install

clean:
	@rm -rf *.o *.ko *.mod.c *~ .*.omd *.o.omd .*.o.omd \
		Module.symvers modules.order .tmp_versions modules.builtin
	$(MAKE) ARCH=$(ARCH) -C $(LINUXDIR) M=$(CURDIR) clean

endif # $(KERNELRELEASE)