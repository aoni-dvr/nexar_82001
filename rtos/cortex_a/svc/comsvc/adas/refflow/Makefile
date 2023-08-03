obj-y += libadas_refflow.a

ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else
ifeq ($(CONFIG_SOC_CV52),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif
endif

ccflags-y += -I$(srctree)/svc/comsvc/adas/inc
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/bsd
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/fc
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/ldws
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/ld
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/psd
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/rcta
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/sr
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/autocal
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/rmg
ccflags-y += -I$(srctree)/svc/comsvc/imgcal
ccflags-y += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/vendors/ambarella/inc/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)

#libadas_refflow-objs := dummy.o
#libadas_refflow-objs += XXXX.o
ifneq ($(wildcard $(srctree)/$(src)/src/psd),)
libadas_refflow-objs := src/psd/RefFlow_PSD.o
endif
ifneq ($(wildcard $(srctree)/$(src)/src/bsd),)
libadas_refflow-objs += src/bsd/RefFlow_BSD.o
endif
ifneq ($(wildcard $(srctree)/$(src)/src/ldws),)
libadas_refflow-objs += src/ldws/RefFlow_LDWS.o
endif
ifneq ($(wildcard $(srctree)/$(src)/src/ld),)
libadas_refflow-objs += src/ld/RefFlow_LD.o
endif
ifneq ($(wildcard $(srctree)/$(src)/src/fc),)
libadas_refflow-objs += src/fc/RefFlow_FC.o
endif
ifneq ($(wildcard $(srctree)/$(src)/src/rcta),)
libadas_refflow-objs += src/rcta/RefFlow_RCTA.o
endif
ifneq ($(wildcard $(srctree)/$(src)/src/sr),)
libadas_refflow-objs += src/sr/AmbaSRRef_Flow.o
endif
ifneq ($(wildcard $(srctree)/$(src)/src/autocal),)
libadas_refflow-objs += src/autocal/RefFlow_AutoCal.o
endif
ifneq ($(wildcard $(srctree)/$(src)/src/rmg),)
libadas_refflow-objs += src/rmg/RefFlow_RMG.o
endif
install-files := libadas_refflow.a
