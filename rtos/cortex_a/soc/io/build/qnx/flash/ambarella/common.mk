ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

EXTRA_SILENT_VARIANTS+=$(subst -, ,$(SECTION))

include ../../pinfo.mk

include $(MKFILES_ROOT)/qmacros.mk

-include $(PROJECT_ROOT)/roots.mk

ifndef MTDFLASH_ROOT
MTDFLASH_ROOT=$(PRODUCT_ROOT)/mtd-flash
endif

EXTRA_INCVPATH += $(MTDFLASH_ROOT)/public $(PRODUCT_ROOT)
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../vendors/ambarella/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../vendors/ambarella/inc/io
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../../../vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../src/common/inc/csl
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../src/$(AMBA_CHIP_ID)/inc
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../src/$(AMBA_CHIP_ID)/inc/csl
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../src/$(AMBA_CHIP_ID)/inc/rtsl
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../src/$(AMBA_CHIP_ID)/inc/reg
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../src/$(AMBA_CHIP_ID)/inc/reg/debugport
EXTRA_INCVPATH += $(PROJECT_ROOT)/../../../../src/$(AMBA_CHIP_ID)/inc/qnx_public

DEFLIB_VARIANT = $(subst $(space),.,$(strip a $(filter wcc be le v7 spe, $(VARIANTS))))
LIB_VARIANT = $(firstword $(subst ./,,$(dir $(bind))) $(DEFLIB_VARIANT))
EXTRA_LIBVPATH += $(MTDFLASH_ROOT)/$(CPU)/$(LIB_VARIANT)

#NAME = devf-$(SECTION)
NAME = devf-qspi-ambarella
INSTALLDIR=sbin

fs_ver=$(firstword $(F3S_VER) 3)
mtd_ver=$(firstword $(MTD_VER) 2)

LIB_FSFLASH=fs-flash$(fs_ver)

LIBS_COMPRESSION_2=lzo2 ucl

LIBS_PM=

LIBS =\
	$(LIB_DEBUG)\
	$(LIB_FSFLASH)\
	mtd-flash\
	spinor-ambarella \
	comsvc-ambarella-misc \
	io-mw \
	kal-ambarella \
	wrap_stdS m \
	$(LIBS_COMPRESSION_$(fs_ver))\
	$(LIB_$(LIB_SUFF)) cache

-include $(PRODUCT_ROOT)/boards/$(SECTION)/extra_libs.mk

# Check for USEFILE override
ifneq ($(wildcard $(PRODUCT_ROOT)/boards/$(SECTION)/devf-$(SECTION).use),)
USEFILE = $(PRODUCT_ROOT)/boards/$(SECTION)/devf-$(SECTION).use
else
USEFILE = $(MTDFLASH_ROOT)/usagev$(fs_ver).use
endif

CCFLAGS += -DF3S_VER=$(fs_ver) -DMTD_VER=$(mtd_ver)


# If the flash filesystem headers/libraries aren't present on the system, kill
# the compiles.
ifeq ($(call FIND_HDR_DIR,nto,usr/include,fs/f3s_spec.h),)
TARGET_BUILD=@$(ECHO_HOST) Flash filesystem headers/libraries not present on system
TARGET_INSTALL=
SRCS=
endif


#####AUTO-GENERATED by packaging script... do not checkin#####
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../../../../install
   USE_INSTALL_ROOT=1
##############################################################

include $(MKFILES_ROOT)/qtargets.mk
