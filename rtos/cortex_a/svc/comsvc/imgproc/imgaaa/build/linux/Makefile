#### Linux
ifneq ($(OBJ_NAME),)
IMG_AAA_PATH = $(preflix)imgproc/imgaaa/
else
preflix = ../../
OBJ_NAME = libimgaaa
obj-y := $(OBJ_NAME).a
install-files += $(OBJ_NAME).a
IMG_AAA_PATH = $(preflix)
build_imgaaa = IMGAAA
$(info $(build_imgaaa))
endif

IMG_AAA_SRC = $(IMG_AAA_PATH)src/aaa/
IMG_IQ_SRC = $(IMG_AAA_PATH)src/iqparam_handler/
IMG_WBCALIB_SRC = $(IMG_AAA_PATH)src/wbcalib/

IMG_PROC_INC = $(src)/$(IMG_AAA_PATH)../inc
IMG_AAA_INC = $(src)/$(IMG_AAA_PATH)inc/aaa
IMG_IQ_INC = $(src)/$(IMG_AAA_PATH)inc/iqparam
IMG_WBCALIB_INC = $(src)/$(IMG_AAA_PATH)inc/wbcalib

COMSVC_PATH = $(srctree)/svc/comsvc
VENDOR_SSP_INC = $(srctree)/vendors/ambarella/inc

$(OBJ_NAME)-objs += $(IMG_AAA_SRC)Amba_AdjCtrl.o \
                    $(IMG_AAA_SRC)Amba_AeAwbAdj_Control.o \
                    $(IMG_AAA_SRC)Amba_Image.o \
                    $(IMG_AAA_SRC)Amba_ImgProcTest.o \
                    $(IMG_AAA_SRC)Amba_AdjTableSelectSample.o \
                    $(IMG_AAA_SRC)Amba_ExifImg.o

$(OBJ_NAME)-objs += $(IMG_IQ_SRC)AmbaIQParamHandlerSample.o

$(OBJ_NAME)-objs += $(IMG_WBCALIB_SRC)Amba_AwbCalib.o \
                    $(IMG_WBCALIB_SRC)Amba_AwbCalibTskSample.o

ccflags-y += -I$(IMG_PROC_INC)/$(AMBA_CHIP_ID)
ccflags-y += -I$(IMG_AAA_INC)
ccflags-y += -I$(IMG_IQ_INC)
ccflags-y += -I$(IMG_WBCALIB_INC)

ccflags-y += -I$(COMSVC_PATH)/misc \
             -I$(COMSVC_PATH)/print \
             -I$(COMSVC_PATH)/shell/inc

ccflags-y += -I$(VENDOR_SSP_INC) \
             -I$(VENDOR_SSP_INC)/dsp \
             -I$(VENDOR_SSP_INC)/dsp/$(AMBA_CHIP_ID) \
             -I$(VENDOR_SSP_INC)/io \
             -I$(VENDOR_SSP_INC)/io/$(AMBA_CHIP_ID) \
             -I$(VENDOR_SSP_INC)/perif