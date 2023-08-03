# svc dsp function
DSP_O_PREFIX = ../../src/dsp

ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/

ccflags-$(CONFIG_ICAM_IMGCAL_USED) += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-$(CONFIG_ICAM_VIEWCTRL_USED) += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-$(CONFIG_ICAM_VIEWCTRL_USED) += -I$(srctree)/svc/comsvc/warputility
ccflags-$(CONFIG_ICAM_BIST_UCODE) += -I$(srctree)/vendors/foss/lz4

libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcDisplay.o
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcDSP.o
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcIK.o
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcLiveview.o
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcUcode.o
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcVinSrc.o
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcVoutSrc.o
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcLivMon.o
ifeq ($(CONFIG_SOC_CV2FS), y)
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcIsoCfg_CV2FS.o
else ifeq ($(CONFIG_SOC_CV5)$(CONFIG_SOC_CV52), y)
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcIsoCfg_CV5.o
else
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcIsoCfg.o
endif

# view conrol
ifeq ($(CONFIG_ICAM_VIEWCTRL_USED),y)
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcViewCtrl.o
endif

# still capture
ifeq ($(CONFIG_ICAM_STLCAP_USED), y)
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcStill.o
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcStillCap.o
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcStillProc.o
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcStillEnc.o
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcStillMux.o
endif

# test frame
ifeq ($(CONFIG_ICAM_BIST_UCODE), y)
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcUcBIST.o
endif
