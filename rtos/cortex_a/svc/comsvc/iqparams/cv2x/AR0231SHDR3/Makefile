IQ_O :=  \
	AmbaIQParamAR0231SHDR3_CV22_Adj_PhotoPreview00.o \
	AmbaIQParamAR0231SHDR3_CV22_Adj_PhotoPreview01.o \
	AmbaIQParamAR0231SHDR3_CV22_Adj_VideoPc00.o \
	AmbaIQParamAR0231SHDR3_CV22_Adj_VideoPc01.o \
	AmbaIQParamAR0231SHDR3_CV22_Adj_VideoPc02.o \
	AmbaIQParamAR0231SHDR3_CV22_AdjTableParam.o \
	AmbaIQParamAR0231SHDR3_CV22_DefaultParams.o \
	AmbaIQParamAR0231SHDR3_CV22_DeStillParam.o \
	AmbaIQParamAR0231SHDR3_CV22_DeVideoParam.o \
	AmbaIQParamAR0231SHDR3_CV22_ImageParam.o \
	AmbaIQParamAR0231SHDR3_CV22_ScSet01Param.o \
	AmbaIQParamAR0231SHDR3_CV22_ScSet02Param.o \
	AmbaIQParamAR0231SHDR3_CV22_ScSet03Param.o \
	AmbaIQParamAR0231SHDR3_CV22_ScSet04Param.o \
	AmbaIQParamAR0231SHDR3_CV22_ScSet05Param.o \
	AmbaIQParamAR0231SHDR3_CV22_StillParam.o \
    AmbaIQParamAR0231SHDR3_CV22_Adj_StillLIso00.o \
    AmbaIQParamAR0231SHDR3_CV22_Adj_StillLIso01.o \
    AmbaIQParamAR0231SHDR3_CV22_Adj_StillHIso00.o \
    AmbaIQParamAR0231SHDR3_CV22_Adj_StillHIso01.o	

# For rename xxxCV22_yyy as yyy
RENAME_PATTERN := "CV22_"

IQ_BIN := $(patsubst %.o,%.bin,$(IQ_O))
IQ_BINFILES := $(addprefix $(obj)/,$(IQ_BIN))
mkfile_this := $(abspath $(lastword $(MAKEFILE_LIST)))
IQ_NAME :=$(shell echo $(dir $(mkfile_this)) | awk -F '/' '{print $$(NF - 1)}')
ifdef CONFIG_LINUX
INSTALL_PATH := $(srctree)/vendors/ambarella/image/iqtable/$(AMBA_CHIP_ID)/linux/$(IQ_NAME)/bins
else
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


ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)


clean-files += $(obj)/.stamp_build

