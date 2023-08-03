obj-y += libadas_utils.a


ccflags-y += -I$(srctree)/svc/comsvc/adas/inc
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/bsd
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/fc
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/ldws
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/psd
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/rcta
ccflags-y += -I$(srctree)/svc/comsvc/adas/refflow/inc/rmg
ccflags-y += -I$(srctree)/svc/comsvc/adas/utils/inc
ccflags-y += -I$(srctree)/svc/comsvc/imgcal
ccflags-y += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io

#libadas_utils-objs := dummy.o
#libadas_utils-objs += XXXX.o
libadas_utils-objs := src/PrepareAVMCalib.o
libadas_utils-objs += src/Util_BmpHandler.o
libadas_utils-objs += src/Util_CanbusHdlr.o
libadas_utils-objs += src/Util_DrawFunction.o
libadas_utils-objs += src/Util_ImFunction.o
libadas_utils-objs += src/Util_LogFunction.o
libadas_utils-objs += src/Util_PrepareCalib.o
libadas_utils-objs += src/Util_ReadCvoutHdlr.o
libadas_utils-objs += src/UtilVSL_BSD.o
libadas_utils-objs += src/UtilVSL_Common.o
libadas_utils-objs += src/UtilVSL_FC.o
libadas_utils-objs += src/UtilVSL_LDWS.o
libadas_utils-objs += src/UtilVSL_LKA.o
libadas_utils-objs += src/UtilVSL_LCS.o
libadas_utils-objs += src/UtilVSL_PSD.o
libadas_utils-objs += src/UtilVSL_RCTA.o
libadas_utils-objs += src/UtilVSL_RMG.o

install-files := libadas_utils.a
