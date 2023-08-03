# svc dsp function
DSP_O_PREFIX = ../../src/dsp

libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcDisplay.o            \
                        $(DSP_O_PREFIX)/SvcDSP.o                \
                        $(DSP_O_PREFIX)/SvcIK.o                 \
                        $(DSP_O_PREFIX)/SvcLiveview.o           \
                        $(DSP_O_PREFIX)/SvcUcode.o              \
                        $(DSP_O_PREFIX)/SvcVinSrc.o             \
                        $(DSP_O_PREFIX)/SvcVoutSrc.o            \
                        $(DSP_O_PREFIX)/SvcLivMon.o             \

ccflags-y += -I$(srctree)/bsp/$(AMBA_CHIP_ID)/

ifeq ($(CONFIG_ICAM_IMGCAL_USED), y)
ccflags-y += -I$(srctree)/svc/comsvc/imgcal/arch
endif

# view conrol
ifeq ($(CONFIG_ICAM_VIEWCTRL_USED),y)
ccflags-y += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-y += -I$(srctree)/svc/comsvc/warputility

libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcViewCtrl.o
endif

ccflags-y += -I$(srctree)/svc/apps/icam/cardv

# ifeq ($(CONFIG_ICAM_FIXED_ISO_CFG_USED), y)
ifeq ($(CONFIG_SOC_CV2FS), y)
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcIsoCfg_CV2FS.o
else ifeq ($(CONFIG_SOC_CV5)$(CONFIG_SOC_CV52), y)
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcIsoCfg_CV5.o
else
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcIsoCfg.o
endif
# endif

# still capture
ifeq ($(CONFIG_ICAM_STLCAP_USED), y)
libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcStill.o              \
                        $(DSP_O_PREFIX)/SvcStillCap.o           \
                        $(DSP_O_PREFIX)/SvcStillProc.o          \
                        $(DSP_O_PREFIX)/SvcStillEnc.o           \
                        $(DSP_O_PREFIX)/SvcStillMux.o
endif

# test frame
ifeq ($(CONFIG_ICAM_BIST_UCODE), y)
ccflags-y += -I$(srctree)/vendors/foss/lz4

libsvc_icamcore-objs += $(DSP_O_PREFIX)/SvcUcBIST.o
endif
