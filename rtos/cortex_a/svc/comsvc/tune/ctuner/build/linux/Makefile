obj-y += libimg_tuner.a
libimg_tuner-objs += ../../src/common/AmbaCT_AvmRule.o
libimg_tuner-objs += ../../src/common/AmbaCT_AvmTuner.o
libimg_tuner-objs += ../../src/common/AmbaCT_EmirRule.o
libimg_tuner-objs += ../../src/common/AmbaCT_EmirTuner.o

ifeq ($(CONFIG_SOC_CV2), y)
libimg_tuner-objs += ../../src/common/AmbaCT_StereoRule.o
libimg_tuner-objs += ../../src/common/AmbaCT_StereoTuner.o
endif

ifeq ($(CONFIG_SOC_CV2FS), y)
libimg_tuner-objs += ../../src/common/AmbaCT_StereoRule.o
libimg_tuner-objs += ../../src/common/AmbaCT_StereoTuner.o
endif

libimg_tuner-objs += ../../src/common/AmbaCT_1DVigRule.o
libimg_tuner-objs += ../../src/common/AmbaCT_1DVigTuner.o
libimg_tuner-objs += ../../src/common/AmbaCT_LdcRule.o
libimg_tuner-objs += ../../src/common/AmbaCT_LdcTuner.o

libimg_tuner-objs += ../../src/common/AmbaCT_LdccRule.o
libimg_tuner-objs += ../../src/common/AmbaCT_LdccTuner.o

libimg_tuner-objs += ../../src/common/AmbaCT_OcRule.o
libimg_tuner-objs += ../../src/common/AmbaCT_OcTuner.o
libimg_tuner-objs += ../../src/common/AmbaCT_OcCbRule.o
libimg_tuner-objs += ../../src/common/AmbaCT_OcCbTuner.o
libimg_tuner-objs += ../../src/common/AmbaCT_CaRule.o
libimg_tuner-objs += ../../src/common/AmbaCT_CaTuner.o
libimg_tuner-objs += ../../src/common/AmbaCT_BpcRule.o
libimg_tuner-objs += ../../src/common/AmbaCT_BpcTuner.o
libimg_tuner-objs += ../../src/common/AmbaCT_Logging.o
libimg_tuner-objs += ../../src/common/AmbaCT_TextHdlr.o
libimg_tuner-objs += ../../src/common/parser/AmbaCT_Parser.o
libimg_tuner-objs += ../../src/common/parser/AmbaCT_ProcRule.o
libimg_tuner-objs += ../../src/common/parser/AmbaCT_SystemApi.o

ccflags-y := -I$(srctree)/svc/comsvc/imgcal
ccflags-y += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-y += -I$(srctree)/svc/comsvc/tune/ctuner/
ccflags-y += -I$(srctree)/svc/comsvc/tune/ctuner/inc/common
ccflags-y += -I$(srctree)/svc/comsvc/tune/ctuner/inc/common/parser
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)

install-files := libimg_tuner.a
