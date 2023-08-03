################################################################
#                             Linux                            #
################################################################
CURDIR := $(patsubst %/build/linux/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
APP := $(notdir $(CURDIR))

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
ifdef CONFIG_ENABLE_AMBALINK
ccflags-y += -I$(srctree)/svc/comsvc/ambalink/inc
ccflags-y += -I$(srctree)/svc/comsvc/ambalink/inc/rpcprog
endif
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/imgcal
ccflags-y += -I$(srctree)/svc/comsvc/flexidagio/inc
ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/SCAStixel
ccflags-y += -I$(srctree)/svc/comsvc/cv/arm_framework/app/SCAStixelV2
ccflags-y += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(OUTDIR)
ccflags-y += -O3 -g

obj-y += libcv_$(APP).a

#C_SRCS := $(patsubst $(CURDIR)/%,%,$(wildcard $(CURDIR)/*.c))
#libcv_$(APP)-objs = $(C_SRCS:%.c=%.o)


libcv_$(APP)-objs := ../../RefCV_Bisenet.o              \
                     ../../RefCV_Common.o               \
                     ../../RefCV_FetchChipInfo_UT.o     \
                     ../../RefCV_MnetSSD.o              \
                     ../../RefCV_Restart.o              \
                     ../../SCA_OpenOD_UT.o              \
                     ../../SCA_OpenSeg_UT.o

install-files += libcv_$(APP).a

