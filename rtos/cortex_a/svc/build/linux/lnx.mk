###############################################################################
#
# This file is to build Linux kernel module
#
###############################################################################

####################################
# Parameters
####################################

# Rsync source of Linux kernel module to remote
MOD_RSYNC := n
# Clean modules after modules_install
#MOD_CLEAN := $(if $(findstring y,$(MOD_RSYNC)),n,y)
MOD_CLEAN := n # Some modules need Module.symvers of others

# Makefile of Linux
MOD_MKNAME := Makefile

# Clean up variables from kbuild-of-amba_camera
override MAKEFLAGS =
override KBUILD_SRC =
export MAKEFLAGS KBUILD_SRC

####################################
# Linux module directory
####################################
if_exist = $(if $(wildcard $(srctree)/$(1)), $(1))
pre_mod_dir :=
pre_mod_dir += soc/osal/build/linux_kernel

ifeq ($(CONFIG_BUILD_MAL),y)
pre_mod_dir += $(call if_exist,vendors/ambarella/wrapper/mal/build/linux_kernel)
endif

mod_dir :=
##hello.ko is for experimental demo
#mod_dir += $(call if_exist,svc/build/linux/mod_hello)

##CONFIG_ARCH_AMBARELLA_CV also works
ifeq ($(CONFIG_BUILD_COMMON_SERVICE_CV),y)
ifeq ($(CONFIG_BUILD_MAL),y)
mod_dir += $(if $(wildcard $(srctree)/soc/vision/arm_framework/cvchip.mk), soc/vision/build/linux_kernel_mal)
else
mod_dir += $(if $(wildcard $(srctree)/soc/vision/arm_framework/cvchip.mk), soc/vision/build/linux_kernel)
endif
endif
ifeq ($(CONFIG_BUILD_DSP_VISUAL),y)
ifeq ($(CONFIG_BUILD_MAL),y)
mod_dir += $(call if_exist,soc/dsp/visual/build/linux_kernel_mal)
else
mod_dir += $(call if_exist,soc/dsp/visual/build/linux_kernel)
endif
endif
ifeq ($(CONFIG_ENABLE_HDMI),y)
mod_dir += $(call if_exist,soc/io/build/linux_kernel/hdmi)
endif
ifeq ($(CONFIG_SVC_APPS_IOUT),y)
mod_dir += $(call if_exist,soc/io/build/linux_kernel/vio)
endif
################################################################################

####################################
# Macros
####################################
abs_mod_dir = $(if $(findstring y,$(MOD_RSYNC)),$(AMBA_CAMERA_DIR)/$1,$(srctree)/$1)
define mk_mod_cmds
	ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		INSTALL_MOD_PATH=$(INSTALL_MOD_PATH) \
		AMBARELLA_PKG_DIR=$(srctree) \
		$(MAKE) \
		-C $(LINUXDIR) \
		M=$(call abs_mod_dir,$1) \
		O=$(LINUXDIR) \
		-f $(MOD_MKNAME)
endef

####################################
# Targets
####################################

pre_mod_alldirs := $(pre_mod_dir)
pre_mod_alldirs_rsync := $(foreach d,$(pre_mod_dir),$(d)-rsync)
pre_mod_alldirs_build := $(foreach d,$(pre_mod_dir),$(d)-build)
pre_mod_alldirs_clean := $(foreach d,$(pre_mod_dir),$(d)-clean)

mod_alldirs := $(mod_dir)
mod_alldirs_rsync := $(foreach d,$(mod_dir),$(d)-rsync)
mod_alldirs_build := $(foreach d,$(mod_dir),$(d)-build)
mod_alldirs_clean := $(foreach d,$(mod_dir),$(d)-clean)

PHONY += all
all: mod_install $(AMBA_CAMERA_DIR)/lnx_mod.txt FORCE
	@:

PHONY += mod_install
mod_install: $(mod_alldirs_build) FORCE
	$(Q)for d in $(pre_mod_dir) $(mod_dir); do \
		$(call mk_mod_cmds,$${d}) \
			modules_install; \
		if [ "$(MOD_CLEAN)" = "y" ]; then \
			$(call mk_mod_cmds,$${d}) \
			clean; \
		fi; \
	done


$(mod_alldirs_build): $(pre_mod_alldirs_build) FORCE


PHONY += clean
clean: $(mod_alldirs_clean) $(pre_mod_alldirs_clean) FORCE
	@:

$(AMBA_CAMERA_DIR)/lnx_mod.txt: FORCE
	$(Q)rm -f $(@)
	+$(Q)for d in $(mod_alldirs) $(pre_mod_alldirs); do \
		echo "$${d}" >> $(@); \
	done

%-rsync: FORCE
	$(Q)mkdir -p $(AMBA_CAMERA_DIR)/$*
	$(Q)rsync -au --chmod=u=rwX,go=rX \
		--copy-links \
		$(srctree)/$*/ $(AMBA_CAMERA_DIR)/$*/

%-build: $(if $(findstring y,$(MOD_RSYNC)),%-rsync) FORCE
	$(Q)$(call mk_mod_cmds,$*) \
		modules


%-clean: FORCE
	$(Q)$(call mk_mod_cmds,$*) \
		clean

%-mk: %-build FORCE
	@:

PHONY += FORCE
FORCE:

.PHONY: $(PHONY)

