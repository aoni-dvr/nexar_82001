# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.

include $(objtree)/include/config/auto.conf

EXTRA_SRCVPATH := $(CURDIR)/../../src/

# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
SRCS := AmbaExifParser.c
SRCS += AmbaExifDemuxer.c
SRCS += AmbaExifFile.c
SRCS += AmbaExifMem.c

EXTRA_INCVPATH := $(srctree)/vendors/ambarella/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cfs/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/cfs/src
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print
EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/exif/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/exif
EXTRA_INCVPATH += $(srctree)/svc/comsvc/codec/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/imgproc/inc/$(AMBA_CHIP_ID)

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
