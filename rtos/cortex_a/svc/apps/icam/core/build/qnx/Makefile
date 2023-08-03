# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.

include $(objtree)/include/config/auto.conf

ICAM_CORE_SRC = $(CURDIR)/../../src

EXTRA_SRCVPATH := $(ICAM_CORE_SRC)/adas
EXTRA_SRCVPATH += $(ICAM_CORE_SRC)/cal
EXTRA_SRCVPATH += $(ICAM_CORE_SRC)/config
EXTRA_SRCVPATH += $(ICAM_CORE_SRC)/cv
EXTRA_SRCVPATH += $(ICAM_CORE_SRC)/draw
EXTRA_SRCVPATH += $(ICAM_CORE_SRC)/driver
EXTRA_SRCVPATH += $(ICAM_CORE_SRC)/dsp
EXTRA_SRCVPATH += $(ICAM_CORE_SRC)/img
EXTRA_SRCVPATH += $(ICAM_CORE_SRC)/playback
EXTRA_SRCVPATH += $(ICAM_CORE_SRC)/record
EXTRA_SRCVPATH += $(ICAM_CORE_SRC)/safe
EXTRA_SRCVPATH += $(ICAM_CORE_SRC)/sys

# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
SRCS :=

EXTRA_INCVPATH := $(srctree)/vendors/ambarella/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/qnx
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/perif
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/prfile2
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/threadx
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/usb
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/usb/usbx
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/wrapper/usb/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/wrapper/fs/threadx/inc
EXTRA_INCVPATH += $(srctree)/vendors/foss/lvgl/src/lvgl/src
EXTRA_INCVPATH += $(srctree)/bsp/$(AMBA_CHIP_ID)/$(AMBA_BSP_NAME)
EXTRA_INCVPATH += $(srctree)/bsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/soc/io/src/common/inc
EXTRA_INCVPATH += $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
EXTRA_INCVPATH += $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
EXTRA_INCVPATH += $(srctree)/svc/comsvc/exif/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cfs/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/codec/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/dcf/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgproc/imgaaa/inc/aaa
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgproc/imgaaa/inc/iqparam
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgproc/imgaaa/inc/wbcalib
EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print
EXTRA_INCVPATH += $(srctree)/svc/comsvc/shell/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/draw/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/usb/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/net/app
EXTRA_INCVPATH += $(srctree)/svc/comsvc/net/rtsp/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/net/inc
EXTRA_INCVPATH += $(srctree)/output
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/adas/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/cal/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/cal/module/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/config/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/cv/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/cv/cvflow/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/cv/transfer/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/driver/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/dsp/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/draw/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/img/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/playback/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/record/inc
EXTRA_INCVPATH += $(ICAM_CORE_SRC)/sys/inc

include $(CURDIR)/Makefile.adas
include $(CURDIR)/Makefile.cal
include $(CURDIR)/Makefile.config
ifeq ($(CONFIG_BUILD_CV), y)
include $(CURDIR)/Makefile.cv
endif
include $(CURDIR)/Makefile.draw
include $(CURDIR)/Makefile.driver
include $(CURDIR)/Makefile.dsp
include $(CURDIR)/Makefile.img
include $(CURDIR)/Makefile.playback
include $(CURDIR)/Makefile.record
include $(CURDIR)/Makefile.safe
include $(CURDIR)/Makefile.sys

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
