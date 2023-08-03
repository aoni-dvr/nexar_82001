################################################################################
#
# This file will be included by parent directory Makefile if CONFIG_BUILD_CV_THREADX=y
# The working directory is therefore the parent directory
#
################################################################################
export CMNDIR := $(srctree)/$(CV_CORE_DIR)/cv_common
export BUILDINFO_CMD := $(CMNDIR)/build/create_build_version.sh

.PHONY: force_build_info

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

schdr-dir   = $(srctree)/$(CV_CORE_DIR)/arm_framework/scheduler
CVDIR       = $(srctree)/$(CV_CORE_DIR)/arm_framework/cvtask
# The absolute path, i.e., $(your_output)/soc/cvflow
# Build from buildroot pkg/amba_camera
OUTDIR     := $(O)/$(obj)

ccflags-y += -DAMBACV_KERNEL_SUPPORT -DUSE_AMBA_KAL

ccflags-y += -I$(schdr-dir)/inc                                                 \
	     -I$(schdr-dir)/core/linux                                         		\
	     -I$(OUTDIR)

$(obj)/build_version.h: force_build_info
	@mkdir -p $(OUTDIR)
	@if [ -f $(BUILDINFO_CMD) ] ; then                                      \
	  $(BUILDINFO_CMD) $(schdr-dir) $(OUTDIR) $(CMNDIR);                    \
	fi

################################################################################
#             build library for sub-scheduler
################################################################################
ifneq ("$(wildcard $(schdr-dir))","")
obj-y += libcv_schdr.a
schdr-srcs := $(wildcard $(schdr-dir)/core/*.c)									\
	      $(schdr-dir)/core/linux/os_api.c									    \
	      $(schdr-dir)/core/linux/schdr_resource.c

libcv_schdr-objs += $(schdr-srcs:$(srctree)/$(CV_CORE_DIR)/%.c=%.o)
install-files += libcv_schdr.a
$(foreach m, $(libcv_schdr-objs), $(obj)/$m): $(obj)/build_version.h
else
obj-y += install-schdr-as-bin
SCHDR_BIN_LIBS = $(CV_RELEASE_DIR)/libcv_schdr.a
$(CV_CORE_DIR)/install-schdr-as-bin:
	@mkdir -p $(AMBA_O_LIB)
	install -m 644 $(SCHDR_BIN_LIBS) $(AMBA_O_LIB)/
endif

