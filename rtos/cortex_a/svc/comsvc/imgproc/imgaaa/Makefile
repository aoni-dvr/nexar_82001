$(info VAR="$(wildcard $(srctree))")
# "/dump7/wkche/sdk8_ALL_20190925/rtos/cortex_a"
$(OBJ_NAME)-objs += imgproc/imgaaa/src/aaa/Amba_AdjCtrl.o
$(OBJ_NAME)-objs += imgproc/imgaaa/src/aaa/Amba_AeAwbAdj_Control.o
$(OBJ_NAME)-objs += imgproc/imgaaa/src/aaa/Amba_Image.o
$(OBJ_NAME)-objs += imgproc/imgaaa/src/aaa/Amba_ImgProcTest.o
$(OBJ_NAME)-objs += imgproc/imgaaa/src/aaa/Amba_AdjTableSelectSample.o
$(OBJ_NAME)-objs += imgproc/imgaaa/src/aaa/Amba_ExifImg.o
ifeq ($(CONFIG_SOC_CV2FS),y)
ifeq ($(CONFIG_BUILD_IP_MONITOR),y)
$(OBJ_NAME)-objs += imgproc/imgaaa/src/aaa/Amba_IPMonitor.o
endif
endif
$(OBJ_NAME)-objs += imgproc/imgaaa/src/iqparam_handler/AmbaIQParamHandlerSample.o
$(OBJ_NAME)-objs += imgproc/imgaaa/src/wbcalib/Amba_AwbCalib.o
$(OBJ_NAME)-objs += imgproc/imgaaa/src/wbcalib/Amba_AwbCalibTskSample.o

ifeq ($(CONFIG_SOC_CV5),y)
AMBA_CHIP_ID_DSP:=cv5
else
AMBA_CHIP_ID_DSP:=$(AMBA_CHIP_ID)
endif

ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID_DSP)/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/perif
ccflags-y += -I$(srctree)/vendors/ambarella/inc/prfile2
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/aaa
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/iqparam
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/imgaaa/inc/wbcalib
ccflags-y += -I$(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)/

ifeq ($(CONFIG_BUILD_COMMON_SERVICE_AF), y)
ccflags-y += -I$(srctree)/svc/comsvc/af/inc/
endif