
KBUILD_CFLAGS += -Wno-pedantic -Wno-switch-default

ifeq ($(CONFIG_CC_USESTD),y)
obj-y += stdsupp/
endif

# replace libnosys.a
ifeq ($(CONFIG_BUILD_SYSCALL_LIB),y)
obj-y += syscall/
endif

ifeq ($(CONFIG_BUILD_FOSS_OPENCV),y)
obj-y += opencv/
endif
