ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(objtree)/include/config/auto.conf

USEFILE=$(PROJECT_ROOT)/comsvc-ambarella-shell.use

NAME=comsvc-ambarella-shell

define PINFO
PINFO DESCRIPTION=Common Service Shell library for the AMBA platform
endef

EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
EXTRA_INCVPATH += $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/
EXTRA_INCVPATH += $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl/
EXTRA_INCVPATH += $(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/qnx_public/
EXTRA_INCVPATH += $(srctree)/svc/comsvc/shell/inc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print/

EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src/common/
EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src/qnx/commands/
EXTRA_SRCVPATH += $(PROJECT_ROOT)/../../src/qnx/

SRCS := AmbaShell.c
SRCS += AmbaShell_CmdHistory.c
SRCS += AmbaShell_KeyCode.c
SRCS += AmbaShell_Utility.c
SRCS += AmbaShell_Cat.c
SRCS += AmbaShell_ChangeWorkDir.c
SRCS += AmbaShell_CopyFile.c
SRCS += AmbaShell_Date.c
SRCS += AmbaShell_Format.c
SRCS += AmbaShell_FsThruput.c
SRCS += AmbaShell_Help.c
SRCS += AmbaShell_ListDir.c
SRCS += AmbaShell_MakeDir.c
SRCS += AmbaShell_MemIO.c
SRCS += AmbaShell_MoveFile.c
SRCS += AmbaShell_Reboot.c
SRCS += AmbaShell_RemoveFile.c
SRCS += AmbaShell_Sleep.c
SRCS += AmbaShell_Ver.c
SRCS += AmbaShell_Volume.c
SRCS += AmbaShell_SaveBin.c
SRCS += AmbaShell_SdShmoo.c
SRCS += AmbaShell_Exec.c
SRCS += AmbaShell_IoStat.c
SRCS += AmbaShell_Eject.c
SRCS += AmbaShell_SysInfo.c

LIBS += comsvc-ambarella-misc io-mw fs-ambarella kal-ambarella sd-ambarella wrap_std m

ifeq ($(CONFIG_BUILD_DSP_VISUAL),y)
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)

SRCS += AmbaShell_DisplayMessage.c

LIBS += comsvc-ambarella-print dsp_wrapper
endif

include $(MKFILES_ROOT)/qmacros.mk

-include $(PROJECT_ROOT)/roots.mk
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../output/install.qnx
   USE_INSTALL_ROOT=1
include $(MKFILES_ROOT)/qtargets.mk
