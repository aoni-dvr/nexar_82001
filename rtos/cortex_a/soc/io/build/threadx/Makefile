obj-$(CONFIG_BUILD_SSP_DRIVER) += common/

ifneq ($(wildcard $(srctree)/$(src)/$(AMBA_CHIP_ID)),)
    obj-$(CONFIG_BUILD_SSP_DRIVER) += $(AMBA_CHIP_ID)/
endif
