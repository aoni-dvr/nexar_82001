$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/AmbaIOUTDiag_CmdDiag.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/AmbaIOUTDiag_CmdTempSen.o

ifeq ($(CONFIG_BUILD_SSP_PCIE_LIB),y)
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/pcie/AmbaIOUTDiag_CmdPcie.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/pcie/AppPCIE_Boot.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/pcie/AppPCIE_Endpoint.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/pcie/AppPCIE_Root.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/pcie/AppPCIE_RegSeq.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/nvme/AmbaIOUTDiag_CmdNvme.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/nvme/AppNVME.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/nvme/AppNVME_Utility.o
$(TARGET_APP_LIB)-objs += $(TARGET_APP_SRC_PATH)/diag/$(AMBA_CHIP_ID)/nvme/AppNVME_PrFile.o

endif