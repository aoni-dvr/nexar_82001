ifndef BUILDDIR
export BUILDDIR := $(abspath ../build)
endif

CMNDIR  := $(abspath ../cv_common)

ifdef CV_EXT_BUILD_ENV_INC
include $(CV_EXT_BUILD_ENV_INC)
CCDIR    := $(dir $(shell which $(CROSS_COMPILE)gcc))
TME_FILE := $(BUILDDIR)/diags/.target_make_env
endif

ifeq ("$(BR2_PACKAGE_AMBAMAL)","y")
ifndef CV_COMMON_LFLAGS
CV_COMMON_LFLAGS := -lambamal
endif

export CV_COMMON_LFLAGS
export ENABLE_AMBA_MAL=y
endif

ifndef PREBUILDDIR
PREBUILDDIR := $(CMNDIR)/libs/arm/linux
endif

ifndef CV_OSTYPE
CV_OSTYPE := AMBALINK_LINUX
endif

export CMNDIR
export BUILDDIR
export PREBUILDDIR
export CV_OSTYPE
export AMBACV_KERNEL_SUPPORT=y
export APP_TYPE=linux

ifdef CV_EXT_BUILD_ENV_INC
# flexidag linux sdk
export STATIC_LINK=y
else
# ambalink linux sdk
ifeq ("$(BUILD_AMBACV_DIAGS_SHARED_LIB)","y")
export STATIC_LINK=n
else ifeq ("$(BUILD_AMBACV_SHARED_LIB)","y")
export STATIC_LINK=n
else
export STATIC_LINK=y
endif
endif # CV_EXT_BUILD_ENV_INC

$(info ambacv library build: STATIC_LINK=$(STATIC_LINK))

include cvchip.mk

ifneq ("$(wildcard $(STAGING_DIR)/usr/include/AmbaIPC_*.h)","")
export ENABLE_CVTASK_CAMCTRL_IPC=y
else ifneq ("$(wildcard $(STAGING_DIR)/usr/include/eva*.h)","")
export ENABLE_CVTASK_CAMCTRL_IPC_EVA=y
endif

APPS     = $(subst app/,,$(wildcard app/*))
APPS_ALL = $(addsuffix .all, $(APPS))

INSDIR  = $(DESTDIR)/usr

.PHONY:       scheduler cvtask cvlib framework app $(APPS)
.NOTPARALLEL: app

ifdef BR2_PACKAGE_EVA
UTILS   = $(filter visutil vputils vputils_$(PROJECT), $(APPS))
all: $(UTILS) 
else
UTILS   = $(filter visutil flexidag_app vputils vputils_$(PROJECT) amba_vision_flexi amba_vision, $(APPS))
all: $(UTILS) init_diags
endif

init_diags:
ifdef CV_EXT_BUILD_ENV_INC
ifeq ("$(wildcard $(BUILDDIR)/diags)","")
	@echo "initializing diag directory ....... $(CCDIR)"
	@mkdir -p $(BUILDDIR)/diags
	@echo "PATH=\"$(CCDIR):$(PATH)\" \
	       CC=\"$(CCDIR)$(CROSS_COMPILE)gcc\" \
	       LD=\"$(CCDIR)$(CROSS_COMPILE)ld\" \
	       AR=\"$(CCDIR)$(CROSS_COMPILE)ar\" \
	       NM=\"$(CCDIR)$(CROSS_COMPILE)nm\" \
	       CPP=\"$(CCDIR)$(CROSS_COMPILE)gcc -E\" \
	       OBJCOPY=\"$(CCDIR)$(CROSS_COMPILE)objcopy\" \
	       ranlib=\"$(CCDIR)$(CROSS_COMPILE)ranlib\" \
	       STRIP=\"$(CCDIR)$(CROSS_COMPILE)strip\" \
               CV_OSTYPE=\"FLEXIBLE_LINUX\" \
               AMB_TOPDIR=\"$(AMB_TOPDIR)\" \
               AMB_BOARD_DIR=\"$(AMB_BOARD_DIR)\" \
	" >> $(TME_FILE)
	BASE_DIR=$(BUILDDIR) $(MAKE) -f $(CV_TOP_DIR)/cv_common/build/init_diags.mk
endif
endif

framework: scheduler

scheduler:
	@if [ -d "./scheduler" ]; then $(MAKE) -C ./scheduler CVCHIP=$(CVCHIP) CVARCH=$(CVARCH); fi

cvtask:
	@$(MAKE) -C ./cvtask all CVCHIP=$(CVCHIP) CVARCH=$(CVARCH)

cvlib:
	@$(MAKE) -C ./cvlib all CVCHIP=$(CVCHIP) CVARCH=$(CVARCH)

app: $(APPS)

$(APPS_ALL): framework
	@$(MAKE) -C ./app/$(basename $@) all CVCHIP=$(CVCHIP) CVARCH=$(CVARCH)

$(APPS): framework
	@$(MAKE) -C ./app/$@ app CVCHIP=$(CVCHIP) CVARCH=$(CVARCH)


install:
#	@mkdir -p $(INSDIR)/bin;
#	@cp -rf $(BUILDDIR)/bin/app/* $(INSDIR)/bin
ifneq ("$(wildcard $(BUILDDIR)/bin/app/visutil/*)", "")
	@echo "Copying visorc utilies to target rootfs"
	install -D -m 755 $(BUILDDIR)/bin/app/visutil/* $(INSDIR)/bin
endif
ifneq ("$(wildcard $(BUILDDIR)/bin/app/vputils_$(PROJECT)/*)", "")
	@echo "Copying vp utilies to target rootfs"
	install -D -m 755 $(BUILDDIR)/bin/app/vputils_$(PROJECT)/* $(INSDIR)/bin
else ifneq ("$(wildcard $(BUILDDIR)/bin/app/vputils/*)", "")
	@echo "Copying vp utilies to target rootfs"
	install -D -m 755 $(BUILDDIR)/bin/app/vputils/* $(INSDIR)/bin
endif
ifneq ("$(wildcard $(BUILDDIR)/bin/app/flexidag_app/*)", "")
	@echo "Copying flexidag_app to target rootfs"
	install -D -m 755 $(BUILDDIR)/bin/app/flexidag_app/* $(INSDIR)/bin
endif
ifneq ("$(wildcard $(BUILDDIR)/bin/app/amba_vision_flexi/*)", "")
	@echo "Copying amba_vision_flexi to target rootfs"
	install -D -m 755 $(BUILDDIR)/bin/app/amba_vision_flexi/* $(INSDIR)/bin
endif
ifneq ("$(wildcard $(BUILDDIR)/bin/app/amba_vision/*)", "")
	@echo "Copying amba_vision to target rootfs"
	install -D -m 755 $(BUILDDIR)/bin/app/amba_vision/* $(INSDIR)/bin
endif
ifneq ("$(wildcard $(BUILDDIR)/bin/flexibin/*)", "")
	@echo "Copying flexidag_app's flexibin to target rootfs"
	mkdir -p $(DESTDIR)/lib/firmware/flexibin
	cp -rf $(BUILDDIR)/bin/flexibin/* $(DESTDIR)/lib/firmware/flexibin
endif
ifeq ("$(STATIC_LINK)","n")
	SO_LIBS=`find $(BUILDDIR)/bin/lib -name "*.so"` && echo SO_LIBS=$$SO_LIBS && \
	install -D -m 755 -t $(INSDIR)/lib $$SO_LIBS
endif

install_staging:
#	@mkdir -p $(INSDIR)/lib $(INSDIR)/include/ambadag;
#	@cp -rf $(CMNDIR)/inc/* $(INSDIR)/include/ambadag
	@echo "Copying ambacv header files and scheduler library"
	mkdir -p $(INSDIR)/include/ambacv/cvapi
	install -D -m 644 $(CMNDIR)/inc/schdr_api.h      $(INSDIR)/include/ambacv
	install -D -m 644 $(CMNDIR)/inc/schdr_util.h     $(INSDIR)/include/ambacv
	install -D -m 644 $(CMNDIR)/inc/cvtask_api.h     $(INSDIR)/include/ambacv
	install -D -m 644 $(CMNDIR)/inc/cvtask_coredef.h $(INSDIR)/include/ambacv
	install -D -m 644 $(CMNDIR)/inc/cvtask_errno.h   $(INSDIR)/include/ambacv
	install -D -m 644 $(CMNDIR)/inc/cvsched_errno.h  $(INSDIR)/include/ambacv
	install -D -m 644 $(CMNDIR)/inc/cvsched_drv_errno.h   $(INSDIR)/include/ambacv
	install -D -m 644 $(CMNDIR)/inc/ambint.h         $(INSDIR)/include/ambacv
	install -D -m 644 $(CMNDIR)/inc/cvapi/*.h $(INSDIR)/include/ambacv/cvapi
ifeq ("$(STATIC_LINK)","y")
ifneq ("$(wildcard $(BUILDDIR)/bin/lib/libambadag.a)","")
	install -D -m 644 $(BUILDDIR)/bin/lib/libambadag.a $(INSDIR)/lib
endif
endif

uninstall:
	@for i in $(APPS);   do rm -rf $(INSDIR)/bin/$$i; done
