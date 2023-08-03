
ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/pmic/inc

PMICPATH=pmic
libperif_$(AMBA_CHIP_ID)-objs += $(PMICPATH)/pmic.o
ifdef CONFIG_PMIC_AMBA_PWC
libperif_$(AMBA_CHIP_ID)-objs += $(PMICPATH)/pmic_amba_pwc.o
endif
ifdef CONFIG_PMIC_CHIPON
libperif_$(AMBA_CHIP_ID)-objs += $(PMICPATH)/pmic_chipon.o
endif