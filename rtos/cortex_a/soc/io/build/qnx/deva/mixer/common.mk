ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

ifndef CLASS
CLASS=mixer
endif

NAME=deva-$(CLASS)-$(SECTION)
USEFILE=$(PROJECT_ROOT)/$(SECTION)/$(NAME).use
EXTRA_SILENT_VARIANTS+=$(SECTION)

ifneq ($(DEBUG),)
ifneq ($(origin DEBUG),undefined)
CCFLAGS += -DADO_DEBUG=1
endif
endif

EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../vendors/ambarella/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../vendors/ambarella/inc/io
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../vendors/ambarella/inc/perif
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../svc/comsvc/misc/
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../src/$(AMBA_CHIP_ID)/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../src/$(AMBA_CHIP_ID)/inc/csl
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../src/$(AMBA_CHIP_ID)/inc/rtsl
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../src/$(AMBA_CHIP_ID)/inc/reg
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../src/$(AMBA_CHIP_ID)/inc/reg/debugport
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../src/$(AMBA_CHIP_ID)/inc/qnx_public


LIBS += cache kal-ambarella io-mw

define PINFO
PINFO DESCRIPTION=
endef

include $(MKFILES_ROOT)/qmacros.mk
include $(PROJECT_ROOT)/$(SECTION)/pinfo.mk
LDVFLAG_dll=	#Removes the default -Wl,-Bsymbolic from the link commandline

#####AUTO-GENERATED by packaging script... do not checkin#####
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../../install
   USE_INSTALL_ROOT=1
##############################################################

include $(MKFILES_ROOT)/qtargets.mk


ifeq ($(filter dll, $(VARIANTS)),)
CCFLAGS += -Dversion=version_$(SECTION)
CCFLAGS += -Dmixer_dll_init=mixer_dll_init_$(SECTION)
endif
