################################################################
#                             ThreadX                          #
################################################################
CURDIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
APP := $(notdir $(CURDIR))

ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/vision
ccflags-y += -I$(srctree)/soc/io/src/common/inc/arm/
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/$(CV_COMSVC_DIR)/cv_common/inc/cvapi
ccflags-y += -I$(srctree)/$(CV_COMSVC_DIR)/cv_common/inc/svccvalgo
ccflags-y += -I$(srctree)/$(CV_COMSVC_DIR)/cv_common/inc/svccvalgo/threadx
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc/cvapi

ifdef CONFIG_BUILD_AMBA_ADAS_UNITTEST
ccflags-y += -I$(srctree)/svc/comsvc/adas/inc
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/fc
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/ld
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/rcta
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/psd
ccflags-y += -I$(srctree)/svc/comsvc/adas/unittest/inc
ccflags-y += -I$(srctree)/svc/comsvc/adas/utils/inc
endif

ifdef CONFIG_ENABLE_AMBALINK
ccflags-y += -I$(srctree)/svc/comsvc/ambalink/inc
ccflags-y += -I$(srctree)/svc/comsvc/ambalink/inc/rpcprog
endif
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/imgcal
ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/SCAStixel
ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/SCAStixelV2
ccflags-y += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)
ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/FlexidagUt/CtfwRefFlow/common/inc
ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/Ctfw/inc
ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/CtfwTask/inc
ccflags-y += -I$(OUTDIR)
ccflags-y += -O3 -g

obj-y += libcv_$(APP).a

#C_SRCS := $(patsubst $(CURDIR)/%,%,$(wildcard $(CURDIR)/*.c))
#libcv_$(APP)-objs = $(C_SRCS:%.c=%.o)

libcv_$(APP)-objs := RefCV_Accelerator.o          \
                     RefCV_Bisenet.o              \
                     RefCV_Common.o               \
                     RefCV_FetchChipInfo_UT.o     \
                     RefCV_MnetSSD.o              \
                     RefCV_Restart.o              \
                     RefCV_Test.o                 \
                     RefCV_YieldTest.o            \
                     SCA_AmbaOD_UT.o              \
                     SCA_AmbaSeg_UT.o             \
                     SCA_OpenOD_UT.o              \
                     SCA_OpenSeg_UT.o             \
                     SCA_AmbaPerception_UT.o      \
                     RefCV_AmbaSofFex_UT.o        \
                     SCA_Plain_UT.o


ifneq ($(CONFIG_SOC_CV2), y)
libcv_$(APP)-objs +=RefCV_AmbaSWFma_UT.o
endif

ifeq ($(CONFIG_SOC_CV2), y)
libcv_$(APP)-objs += RefCV_AmbaFex_UT.o           \
                     RefCV_AmbaFma_UT.o           \
                     RefCV_AmbaSpuFex_UT.o        \
                     RefCV_AmbaSpuFusion_UT.o     \
                     RefCV_AmbaFmaV1_CV2_UT.o     \
                     RefCV_AmbaFexV1_CV2_UT.o     \
                     RefCV_AmbaSpuFexV1_CV2_UT.o  \
                     SCA_Stixel_UT.o              \
                     RefCV_AmbaSpuFusionV1_CV2_UT.o


ifeq ($(CONFIG_BUILD_CTFW), y)
ifeq ($(CONFIG_CV_FLEXIDAG_STIXEL), y)
libcv_$(APP)-objs += 	CtfwRefFlow/common/src/AmbaCtfw_StixelFlow.o         \
						CtfwRefFlow/common/src/RefCV_AmbaCtfwStixel_UT.o
endif
ifeq ($(CONFIG_CV_FLEXIDAG_STIXEL_V2), y)
libcv_$(APP)-objs += 	CtfwRefFlow/common/src/AmbaCtfw_StixelV2Flow.o         \
						CtfwRefFlow/common/src/RefCV_AmbaCtfwStixelV2_UT.o
endif
endif
                
endif

ifeq ($(CONFIG_SOC_CV2FS), y)
libcv_$(APP)-objs += RefCV_AmbaFex_CV2A_UT.o         \
                     RefCV_AmbaOfFex_CV2A_UT.o         \
                     RefCV_AmbaSpuFex_CV2A_UT.o        \
                     SCA_Stixel_UT.o                   \
                     RefCV_AmbaSpuFusion_CV2A_UT.o
endif

ifeq ($(CONFIG_BUILD_AMBA_ADAS_UNITTEST), y)
libcv_$(APP)-objs += SCA_AmbaADAS_FC_UT.o
endif

install-files += libcv_$(APP).a

