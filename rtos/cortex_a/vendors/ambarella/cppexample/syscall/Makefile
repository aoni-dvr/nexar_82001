
ifeq ($(CONFIG_THREADX),y)
# For debug
#KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
#KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))
#

obj-y += libsyscall.a

ccflags-y := \
	-I$(srctree)/$(src)/inc \
	-I$(srctree)/svc/comsvc/print \
    -I$(srctree)/vendors/ambarella/inc/io \
	-I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
	-I$(srctree)/vendors/ambarella/inc/threadx \
    -I$(srctree)/vendors/ambarella/inc

# Default libgloss
libsyscall-objs := \
	libgloss/assert.o \
	libgloss/chown.o \
	libgloss/close.o \
	libgloss/environ.o \
	libgloss/errno.o \
	libgloss/execve.o \
	libgloss/_exit.o \
	libgloss/fork.o \
	libgloss/fstat.o \
	libgloss/getpid.o \
	libgloss/gettod.o \
	libgloss/isatty.o \
	libgloss/kill.o \
	libgloss/link.o \
	libgloss/lseek.o \
	libgloss/open.o \
	libgloss/read.o \
	libgloss/readlink.o \
	libgloss/sbrk.o \
	libgloss/stat.o \
	libgloss/symlink.o \
	libgloss/times.o \
	libgloss/unlink.o \
	libgloss/wait.o \
	libgloss/write.o

# Ambarella's file system system call.
libsyscall-objs += \
	fs/sc_fs.o

# IPC
libsyscall-objs += \
	ipc/epoll.o \
	ipc/eventfd.o \
	ipc/pipe.o \
	ipc/poll.o \
	ipc/timerfd.o

libsyscall-objs += AmbaVer.o
CFLAGS_AmbaVer.o = \
	-DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE) \
	-DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM) \
	-DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM) \
	-DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE) \
	-DAMBA_CI_COUNT=$(AMBA_CI_COUNT) \
	-DAMBA_CI_ID=$(AMBA_CI_ID)

ifneq ($(CONFIG_SSP_THREADX_POSIX),)
    TX_POSIX_ccflags += -I$(srctree)/vendors/ExpressLogic/tx_posix -DTC_sys_types_h
endif
CFLAGS_getpid.o = $(TX_POSIX_ccflags)
CFLAGS_sc_fs.o = $(TX_POSIX_ccflags)

CFLAGS_timerfd.o = -DTC_sys_types_h

install-files += libsyscall.a

endif
# end of ifeq ($(CONFIG_THREADX),y)