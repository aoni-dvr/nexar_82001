# We link the stack with -E so a lot of the undefined
# references get resolved from the stack itself.  If
# you want them listed at link time, turn off
# --allow-shlib-undefined and replace with --warn-once
# if desired.

#LDFLAGS+=-Wl,--warn-once
LDFLAGS+=-Wl,--allow-shlib-undefined


HDR_PATH=$(INSTALL_ROOT_HDR)/
PUBLIC_HDR_PATH=$(QNX_TARGET)/usr/include/

# Check for staging area first
EXTRA_INCVPATH+= $(HDR_PATH) $(HDR_PATH)/sys-nto
# Use headers installed in system if staging area not available
EXTRA_INCVPATH+= $(PUBLIC_HDR_PATH) $(PUBLIC_HDR_PATH)/sys-nto
CCFLAGS += -D_KERNEL -DALTQ --D$(CVCHIP) -DAMBACV_KERNEL_SUPPORT -DLIBRARY_VERSION='"UNVERSIONED"' -DTOOL_VERSION='"UNVERSIONED"'

# gcc sometime after 2.95.3 added a builtin log()
CCFLAGS_gcc += -fno-builtin-log
# currently doesnt handle individual builtins
CCFLAGS_clang += -fno-builtin
CCFLAGS += $(CCFLAGS_$(COMPILER_TYPE))
