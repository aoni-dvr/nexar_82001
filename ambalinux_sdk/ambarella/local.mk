export AMBARELLA_PKG_DIR	?= $(TOPDIR)/../pkg
export AMBARELLA_KMOD_DIR	?= $(TOPDIR)/../kmod

export BR2_PACKAGE_NETWORK_TURNKEY_STARTUP
export BR2_AMBARELLA_AUTOHIBER
export BR2_AMBARELLA_LINUX_TTY
export BR2_AMBARELLA_LINUX_RESPAWN_SH
export BR2_AMBARELLA_SIMPLE_RCS

# ROOTFS_UBI_PRE_GEN_HOOKS := gen_pref
# gen_pref := cp $(TOPDIR)/ambarella/fs/ubifs/linuxfs.bin.pref.ubifs $(BR2_TARGET_ROOTFS_UBI_DIR)

-include ambarella/package/*/*.mk
-include ambarella/package/*/*/*.mk

LINUX_OVERRIDE_SRCDIR=$(TOPDIR)/../linux-$(strip $(subst ",,$(BR2_AMBARELLA_LINUX_VERSION)))

define LINUX_RSYNC_FIXING_GIT_SYMLINK
	@if [ "$(BR2_LINUX_KERNEL_OOSB)" != "y" ]; then	\
		$(call MESSAGE,"Fixing .git symlinks");	\
		if [ ! -L $(LINUX_DIR) ]; then 		\
			rm -rf $(LINUX_DIR)/.git; 	\
			ln -s $(LINUX_OVERRIDE_SRCDIR)/.git $(LINUX_DIR)/.git; \
		fi	\
	fi
endef

LINUX_POST_RSYNC_HOOKS += LINUX_RSYNC_FIXING_GIT_SYMLINK
LINUX_POST_RSYNC_HOOKS += LINUX_RSYNC_AMBAVFS
