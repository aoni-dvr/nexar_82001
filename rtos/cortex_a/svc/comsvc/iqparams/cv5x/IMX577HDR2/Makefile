ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif

IQ_O :=  \
	AmbaIQParamIMX577HDR2_CV5_Adj_PhotoPreview00.o \
	AmbaIQParamIMX577HDR2_CV5_Adj_PhotoPreview01.o \
	AmbaIQParamIMX577HDR2_CV5_Adj_VideoPc00.o \
	AmbaIQParamIMX577HDR2_CV5_Adj_VideoPc01.o \
	AmbaIQParamIMX577HDR2_CV5_Adj_VideoPc02.o \
	AmbaIQParamIMX577HDR2_CV5_Adj_StillLIso00.o \
	AmbaIQParamIMX577HDR2_CV5_Adj_StillLIso01.o \
	AmbaIQParamIMX577HDR2_CV5_Adj_StillHIso00.o \
	AmbaIQParamIMX577HDR2_CV5_Adj_StillHIso01.o \
	AmbaIQParamIMX577HDR2_CV5_AdjTableParam.o \
	AmbaIQParamIMX577HDR2_CV5_DefaultParams.o \
	AmbaIQParamIMX577HDR2_CV5_DeStillParam.o \
	AmbaIQParamIMX577HDR2_CV5_DeVideoParam.o \
	AmbaIQParamIMX577HDR2_CV5_ImageParam.o \
	AmbaIQParamIMX577HDR2_CV5_ScSet01Param.o \
	AmbaIQParamIMX577HDR2_CV5_ScSet02Param.o \
	AmbaIQParamIMX577HDR2_CV5_ScSet03Param.o \
	AmbaIQParamIMX577HDR2_CV5_ScSet04Param.o \
	AmbaIQParamIMX577HDR2_CV5_ScSet05Param.o \
	AmbaIQParamIMX577HDR2_CV5_StillParam.o

# For rename xxxCV5_yyy as yyy
RENAME_PATTERN := "CV5_"

IQ_BIN := $(patsubst %.o,%.bin,$(IQ_O))
IQ_BINFILES := $(addprefix $(obj)/,$(IQ_BIN))
mkfile_this := $(abspath $(lastword $(MAKEFILE_LIST)))
IQ_NAME :=$(shell echo $(dir $(mkfile_this)) | awk -F '/' '{print $$(NF - 1)}')
ifdef CONFIG_LINUX
INSTALL_PATH := $(srctree)/vendors/ambarella/image/iqtable/$(AMBA_CHIP_ID)/linux/$(IQ_NAME)/bins
else
#INSTALL_PATH := $(srctree)/vendors/ambarella/image/iqtable/$(AMBA_CHIP_ID)/$(IQ_NAME)/bins

ifdef CONFIG_ARM64
INSTALL_PATH := $(srctree)/vendors/ambarella/image/iqtable/$(AMBA_CHIP_ID)/tx64/$(IQ_NAME)/bins
else
INSTALL_PATH := $(srctree)/vendors/ambarella/image/iqtable/$(AMBA_CHIP_ID)/$(IQ_NAME)/bins
endif
endif

obj-y += $(IQ_O)
obj-y += .stamp_build


# .bin files depends on .o
$(IQ_BIN): $(IQ_O)

%.bin: %.o
	$(Q)$(OBJCOPY) -O binary $< $@

$(obj)/.stamp_build: $(IQ_BINFILES)
	$(Q)rm -rf $(INSTALL_PATH) && mkdir -p $(INSTALL_PATH)
	$(Q)for f in $(IQ_BINFILES); do \
		install -t $(INSTALL_PATH) $$f; \
		done
	$(Q)cd $(INSTALL_PATH) && \
		for f in `ls`; do \
			mv -f $$f `echo $$f | awk -F "$(RENAME_PATTERN)" '{printf $$NF}'`; \
			done
	@touch $@


ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)


clean-files += $(obj)/.stamp_build

