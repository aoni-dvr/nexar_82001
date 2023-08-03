
EXTRA_SRCVPATH += $(CURDIR)/../../src

ifeq ($(AMBA_CHIP_ID),cv2fs)
SRCS := AmbaTUNE_ImgCalibItunerImpl_cv2fs.o \
		AmbaTUNE_Rule_cv2fs.o \
		AmbaTUNE_TextHdlr_cv2fs.o \
		AmbaTUNE_USBHdlr_cv2fs.o \
		AmbaTUNE_Parser_cv2fs.o \
		AmbaTUNE_SystemApi_cv2fs.o \
		AmbaTUNE_HdlrManager_cv2fs.o \
		AmbaTUNE_USBCtrl_cv2fs.o
else
SRCS := AmbaTUNE_HdlrManager_cv2x.o \
		AmbaTUNE_ImgCalibItunerImpl_cv2x.o \
		AmbaTUNE_Parser_cv2x.o \
		AmbaTUNE_Rule_cv2x.o \
		AmbaTUNE_SystemApi_cv2x.o \
		AmbaTUNE_TextHdlr_cv2x.o \
		AmbaTUNE_USBHdlr_cv2x.o \
		AmbaTUNE_USBCtrl_cv2x.o
endif

EXTRA_INCVPATH += $(CURDIR)/../.. \
				$(CURDIR)/../../inc \
				$(CURDIR)/../../../../../../vendors/ambarella/inc \
				$(CURDIR)/../../../../../../vendors/ambarella/inc/dsp \
				$(CURDIR)/../../../../../../vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID) \
				$(CURDIR)/../../../../../../svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID) \
				$(CURDIR)/../../../../../../vendors/ambarella/inc/io \
				$(CURDIR)/../../../../../../vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
				$(CURDIR)/../../../../../../vendors/ambarella/wrapper/kal/qnx/public \
				$(CURDIR)/../../../../../../svc/comsvc/misc \
				$(CURDIR)/../../../../../../svc/comsvc/print/

LIST=CPU
include recurse.mk
