
IQ_O :=  \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_Adj_PhotoPreview00.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_Adj_PhotoPreview01.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_Adj_VideoPc00.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_Adj_VideoPc01.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_Adj_VideoPc02.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_AdjTableParam.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_DefaultParams.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_DeStillParam.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_DeVideoParam.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_ImageParam.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_ScSet01Param.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_ScSet02Param.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_ScSet03Param.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_ScSet04Param.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_ScSet05Param.o \
	AmbaIQParamOV2312_SIMULTANEOUS_IR_HJ6074L_CV22_StillParam.o

# For rename xxxCV22_yyy as yyy
RENAME_PATTERN := "CV22_"

IQ_BIN := $(patsubst %.o,%.bin,$(IQ_O))
IQ_BINFILES := $(addprefix $(obj)/,$(IQ_BIN))
mkfile_this := $(abspath $(lastword $(MAKEFILE_LIST)))
IQ_NAME :=$(shell echo $(dir $(mkfile_this)) | awk -F '/' '{print $$(NF - 1)}')
ifdef CONFIG_LINUX
INSTALL_PATH := $(srctree)/vendors/ambarella/image/iqtable/$(AMBA_CHIP_ID)/linux/$(IQ_NAME)/bins
else
INSTALL_PATH := $(srctree)/vendors/ambarella/image/iqtable/$(AMBA_CHIP_ID)/$(IQ_NAME)/bins
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


ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)


clean-files += $(obj)/.stamp_build

