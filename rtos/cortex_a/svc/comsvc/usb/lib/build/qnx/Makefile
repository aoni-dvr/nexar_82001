# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.

include $(objtree)/include/config/auto.conf

COMSVC_USB_SRC = ../../src/common

EXTRA_SRCVPATH := \
		  $(CURDIR)/$(COMSVC_USB_SRC) \
		  $(CURDIR)/../../src/qnx

# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
SRCS := AmbaSvcUsb.o AmbaSvcUsbCmd.o MSCDevice.o MTPDevice.o PlatformPort.o

EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/usb
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/wrapper/usb/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/wrapper/fs/inc
EXTRA_INCVPATH += $(srctree)/bsp/$(AMBA_CHIP_ID)/$(AMBA_BSP_NAME)
EXTRA_INCVPATH += $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
EXTRA_INCVPATH += $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print
EXTRA_INCVPATH += $(srctree)/svc/comsvc/shell/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/tune/ituner
EXTRA_INCVPATH += $(srctree)/svc/comsvc/usb/inc
EXTRA_INCVPATH += $(src)/$(COMSVC_USB_SRC)

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
