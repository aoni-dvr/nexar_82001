# dsp
SRCS += SvcDisplay.c
SRCS += SvcDSP.c
SRCS += SvcIK.c
SRCS += SvcLiveview.c
SRCS += SvcUcode.c
SRCS += SvcVinSrc.c
SRCS += SvcVoutSrc.c
SRCS += SvcLivMon.c
ifeq ($(CONFIG_SOC_CV2FS),y)
SRCS += SvcIsoCfg_CV2FS.c
else
SRCS += SvcIsoCfg.c
endif
ifeq ($(CONFIG_ICAM_VIEWCTRL_USED),y)
SRCS += SvcViewCtrl.c
endif
# still capture
ifeq ($(CONFIG_ICAM_STLCAP_USED),y)
SRCS += SvcStill.c
SRCS += SvcStillCap.c
SRCS += SvcStillProc.c
SRCS += SvcStillEnc.c
SRCS += SvcStillMux.c
endif
# test frame
ifeq ($(CONFIG_ICAM_BIST_UCODE),y)
SRCS += SvcUcBIST.c
endif

ifeq ($(CONFIG_ICAM_IMGCAL_USED)$(CONFIG_ICAM_VIEWCTRL_USED),y)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgcal/arch
endif
# view conrol
ifeq ($(CONFIG_ICAM_VIEWCTRL_USED),y)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/warputility
endif
# test frame
ifeq ($(CONFIG_ICAM_BIST_UCODE),y)
EXTRA_INCVPATH += $(srctree)/vendors/foss/lz4
endif

