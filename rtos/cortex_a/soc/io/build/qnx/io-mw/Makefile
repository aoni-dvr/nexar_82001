BSP_DIR = $(shell grep "CONFIG_BSP_NAME"  $(CURDIR)/../../../../../output/.config | sed 's/CONFIG_BSP_NAME="//g' | sed 's/"//g')
CHIP_ID = $(shell grep "CONFIG_SOC_NAME"  $(CURDIR)/../../../../../output/.config | sed 's/CONFIG_SOC_NAME="//g' | sed 's/"//g')
include $(objtree)/include/config/auto.conf

EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/io-mw/qnx/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/csl/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/$(AMBA_CHIP_ID)/rtsl/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/io-mw/qnx/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/csl/
EXTRA_SRCVPATH += $(CURDIR)/../../../src/common/rtsl/
EXTRA_SRCVPATH += $(CURDIR)/../../../../../bsp/$(CHIP_ID)
EXTRA_SRCVPATH += $(CURDIR)/../../../../../bsp/$(CHIP_ID)/$(BSP_DIR)

ifeq ($(CONFIG_PIO_MEM_ALLOC),y)
SRCS := AmbaPIO.c
else
SRCS :=
endif

ifeq ($(CONFIG_SOC_CV2),y)

# src
SRCS += AmbaADC.c
SRCS += AmbaCVBS.c
SRCS += AmbaDMA.c
SRCS += AmbaGDMA.c
SRCS += AmbaGPIO.c
SRCS += AmbaHDMI.c
SRCS += AmbaHDMI_CEC.c
SRCS += AmbaHDMI_Info.c
SRCS += AmbaI2C.c
SRCS += AmbaOTP.c
SRCS += AmbaRNG.c
SRCS += AmbaRTC.c
SRCS += AmbaSPI.c
SRCS += AmbaSYS.c
SRCS += AmbaTime.c
SRCS += AmbaTMR.c
SRCS += AmbaVIN.c
SRCS += AmbaVOUT.c
SRCS += AmbaVOUT_CSC.c
SRCS += AmbaWDT.c
SRCS += AmbaPWM.c
SRCS += AmbaSD.c
SRCS += AmbaCAN.c
SRCS += AmbaDramc.c

SRCS += AmbaDiag_Vin.c
SRCS += AmbaDiag_Vout.c

SRCS += AmbaRTSL_HDMI.c
SRCS += AmbaRTSL_HDMI_Audio.c
SRCS += AmbaRTSL_HDMI_Video.c
SRCS += AmbaRTSL_I2S.c
SRCS += AmbaRTSL_VOUT.c

SRCS += AmbaCSL_HDMI.c
SRCS += AmbaCSL_I2S.c
SRCS += AmbaCSL_VOUT.c

SRCS += AmbaDrvEntry.c
SRCS += AmbaMmioBase.c

SRCS += AmbaRTSL_VIN.c

SRCS += AmbaCSL_SKPD.c
SRCS += AmbaCSL_VIN.c

ifeq ($(CONFIG_SVC_APPS_ICAM),y)
SRCS += AmbaiCamPartition.o
else
SRCS += AmbaUserNandPartition.o
endif

else ifeq ($(CONFIG_SOC_CV2FS),y)

#SRCS += AmbaADC.c
SRCS += AmbaDMA.c
SRCS += AmbaGDMA.c
SRCS += AmbaGPIO.c
SRCS += AmbaI2C.c
SRCS += AmbaOTP.c
SRCS += AmbaRNG.c
SRCS += AmbaRTC.c
SRCS += AmbaSPI.c
SRCS += AmbaSYS.c
SRCS += AmbaTime.c
SRCS += AmbaTMR.c
SRCS += AmbaVIN.c
SRCS += AmbaVOUT.c
SRCS += AmbaVOUT_CSC.c
SRCS += AmbaWDT.c
SRCS += AmbaPWM.c
SRCS += AmbaSD.c
SRCS += AmbaCAN.c

SRCS += AmbaDiag_Vin.c
SRCS += AmbaDiag_Vout.c

SRCS += AmbaRTSL_VOUT_LCD.c

SRCS += AmbaCSL_PLL.c

SRCS += AmbaDrvEntry.c
SRCS += AmbaMmioBase.c

SRCS += AmbaRTSL_VIN.c

SRCS += AmbaCSL_SKPD.c
SRCS += AmbaCSL_VIN.c

else

$(error chip not supported for io-mw!)

endif

LIST=LIBIOMW
include recurse.mk
